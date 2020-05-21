
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

//definiçes de tipos
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long microcode;

//estrutura para guardar uma microinstruçao decodificada
struct decoded_microcode
{
    word nadd = 0;
    byte jam;
    byte sft;
    byte alu;
    word reg_w;
    byte mem;
    byte reg_r;
};

//Funções utilitárias ======================
void write_microcode(microcode w) //Dado uma microinstrucao, exibe na tela devidamente espaçado pelas suas partes.
{
   unsigned int v[36];
   for(int i = 35; i >= 0; i--)
   {
       v[i] = (w & 1);
       w = w >> 1;
   }

   for(int i = 0; i < 36; i++)
   {
       cout << v[i];
       if(i == 8 || i == 11 || i == 13 || i == 19 || i == 28 || i == 31) cout << " ";
   }
}

void write_word(word w) //Dada uma palavra (valor de 32 bits / 4 bytes), exibe o valor binário correspondente.
{
   unsigned int v[32];
   for(int i = 31; i >= 0; i--)
   {
       v[i] = (w & 1);
       w = w >> 1;
   }

   for(int i = 0; i < 32; i++)
       cout << v[i];
}

void write_byte(byte b) //Dado um byte (valor de 8 bits), exibe o valor binário correspondente na tela.
{
   unsigned int v[8];
   for(int i = 7; i >= 0; i--)
   {
       v[i] = (b & 1);
       b = b >> 1;
   }

   for(int i = 0; i < 8; i++)
       cout << v[i];
}

void write_dec(word d) //Dada uma palavra (valor de 32 bits / 4 bytes), exibe o valor decimal correspondente.
{
   cout << (int)d << endl;
}
//=========================================

//sinalizador para desligar máquina
bool halt = false;

//memoria principal
#define MEM_SIZE 0xFFFF+1 //0xFFFF + 0x1; // 64 KBytes = 64 x 1024 Bytes = 65536 (0xFFFF+1) x 1 Byte;
byte memory[MEM_SIZE]; //0x0000 a 0xFFFF (0 a 65535)

//registradores
word mar=0, mdr=0, pc=0, sp=0, lv=0, cpp=0, tos=0, opc=0, h=0;
byte mbr=0;

//barramentos
word bus_a=0, bus_b=0, bus_c=0, alu_out=0;

//estado da ALU para salto condicional
byte n=0, z=1;

//registradores de microprograma
word mpc;

//memória de microprograma: 512 x 64 bits = 512 x 8 bytes = 4096 bytes = 4 KBytes.
//Cada microinstrução é armazenada em 8 bytes (64 bits), mas apenas os 4,5 bytes (36 bits) de ordem mais baixa são de fato decodificados.
//Os 28 bits restantes em cada posição da memória são ignorados, mas podem ser utilizados para futuras melhorias nas microinstruções para controlar microarquiteturas mais complexas.
microcode microprog[512];

//carrega microprograma
//Escreve um microprograma de controle na memória de controle (array microprog, declarado logo acima)
void load_microprog()
{
    FILE *microprograma;
	microprograma = fopen("microprog.rom", "rb");

	if (microprograma != NULL) {
		fread(microprog, sizeof(microprog), 512, microprograma);
		fclose(microprograma);
	}
}


//carrega programa na memória principal para ser executado pelo emulador.
//programa escrito em linguagem de máquina (binário) direto na memória principal (array memory declarado mais acima).
void load_prog()
{
    //Início carregamento IJVM

    byte num_of_vars = 0;
    memory[1] = 0x73; //init (bytes 2 e 3 são descartados por conveniência de implementação)

    memory[4] = 0x0006; //(CPP inicia com o valor 0x0006 guardado na palavra 1 – bytes 4 a 7.)

    word tmp = 0x1001; //LV 

    memcpy(&(memory[8]), &tmp, 4); //(LV inicia com o valor de tmp guardado na palavra 2 – bytes 8 a 11)

    tmp = 0x0400; //PC

    memcpy(&(memory[12]), &tmp, 4); //(PC inicia com o valor de tmp guardado na palavra 3 – bytes 12 a 15)

    tmp = 0x1001 + num_of_vars; //SP
    //SP (Stack Pointer) é o ponteiro para o topo da pilha. 
    //A base da pilha é LV e ela já começa com algumas variáveis empilhadas (dependendo do programa).
    //Cada variável gasta uma palavra de memória. Por isso a soma de LV com num_of_vars.

    memcpy(&(memory[16]), &tmp, 4); //(SP inicia com o valor de tmp guardado na palavra 4 – bytes 16 a 19)
    //Fim carregamento IJVM

    //Resolução do exercício
    memory[1025] = 0x19;
    memory[1026] = 0b00000101;
    memory[1027] = 0x19;
    memory[1028] = 0b00000011;
    memory[1029] = 0x02;
}

//exibe estado da máquina
void debug(bool clr = true)
{
    if(clr) system("clear");

    cout << "Microinstrução: ";
    write_microcode(microprog[mpc]);

    cout << "\n\nMemória principal: \nPilha: \n";
    for(int i = lv*4; i <= sp*4; i+=4)
    {
        write_byte(memory[i+3]);
        cout << " ";
        write_byte(memory[i+2]);
        cout << " ";
        write_byte(memory[i+1]);
        cout << " ";
        write_byte(memory[i]);
        cout << " : ";
        if(i < 10) cout << " ";
        cout << i << " | " << memory[i+3] << " " << memory[i+2] << " " << memory[i+1] << " " << memory[i];
        word w;
        memcpy(&w, &memory[i], 4);
        cout << " | " << i/4 << " : " << w << endl;
    }

    cout << "\n\nPC: \n";
    for(int i = (pc-1); i <= pc+20; i+=4)
    {
        write_byte(memory[i+3]);
        cout << " ";
        write_byte(memory[i+2]);
        cout << " ";
        write_byte(memory[i+1]);
        cout << " ";
        write_byte(memory[i]);
        cout << " : ";
        if(i < 10) cout << " ";
        cout << i << " | " << memory[i+3] << " " << memory[i+2] << " " << memory[i+1] << " " << memory[i];
        word w;
        memcpy(&w, &memory[i], 4);
        cout << " | " << i/4 << " : " << w << endl;
    }

    cout << "\nRegistradores - \nMAR: " << mar << " ("; write_word(mar);
    cout << ") \nMDR: " << mdr << " ("; write_word(mdr);
    cout << ") \nPC : " << pc << " ("; write_word(pc);
    cout << ") \nMBR: " << (int) mbr << " ("; write_byte(mbr);
    cout << ") \nSP : " << sp << " (";  write_word(sp);
    cout << ") \nLV : " << lv << " ("; write_word(lv);
    cout << ") \nCPP: " << cpp << " ("; write_word(cpp);
    cout << ") \nTOS: " << tos << " ("; write_word(tos);
    cout << ") \nOPC: " << opc << " ("; write_word(opc);
    cout << ") \nH  : " << h << " ("; write_word(h);
    cout << ")" << endl;
}

decoded_microcode decode_microcode(microcode code) //Recebe uma microinstrução binária e separa suas partes preenchendo uma estrutura de microinstrucao decodificada, retornando-a.
{
    decoded_microcode dec;
    dec = *(decoded_microcode*)malloc(sizeof(decoded_microcode));
    dec.reg_r = code & 0b1111;
    dec.mem = (code >> 4) & 0b111;
    dec.reg_w = (code >> 7) & 0b111111111;
    dec.alu = (code >> 16) & 0b111111;
    dec.sft = (code >> 22) & 0b11;
    dec.jam = (code >> 24) & 0b111;
    dec.nadd = (code >> 27) & 0b111111111;
    return dec;
}

//alu
//recebe uma operação de alu binária representada em um byte (ignora-se os 2 bits de mais alta ordem, pois a operação é representada em 6 bits)
//e duas palavras (as duas entradas da alu), carregando no barramento alu_out o resultado da respectiva operação aplicada às duas palavras.
void alu(byte func, word a, word b)
{
    switch(func){
        case 0b011000: alu_out = a;break;
        case 0b010100: alu_out = b;break;
        case 0b011010: alu_out = ~a;break;
        case 0b101100: alu_out = ~b;break;
        case 0b111100: alu_out = a+b;break;
        case 0b111101: alu_out = a+b+1;break;
        case 0b111001: alu_out = a+1;break;
        case 0b110101: alu_out = b+1;break;
        case 0b111111: alu_out = b-a;break;
        case 0b110110: alu_out = b-1;break;
        case 0b111011: alu_out = -a;break;
        case 0b001100: alu_out = a & b;break;
        case 0b011100: alu_out = a | b;break;
        case 0b010000: alu_out = 0;break;
        case 0b110001: alu_out = 1;break;
        case 0b110010: alu_out = -1;break;
        default: break;
    }
    if(alu_out){
        z = 0;
    }
    else{
        z = 1;
    }
    n = (alu_out>>31) & 0b1;
}

//Deslocamento. Recebe a instrução binária de deslocamento representada em um byte (ignora-se os 6 bits de mais alta ordem, pois o deslocador eh controlado por 2 bits apenas)
//e uma palavra (a entrada do deslocador) e coloca o resultado no barramento bus_c.
void shift(byte s, word w)
{
    if(s & 0b01){
        bus_c = w >> 1;
    }
    if(s & 0b10){
        bus_c = w << 8;
    }
}

//Leitura de registradores. Recebe o número do registrador a ser lido (0 = mdr, 1 = pc, 2 = mbr, 3 = mbru, ..., 8 = opc) representado em um byte,
//carregando o barramento bus_b (entrada b da ALU) com o valor do respectivo registrador e o barramento bus_a (entrada A da ALU) com o valor do registrador h.
void read_registers(byte reg_end)
{
    switch(reg_end){
        case 0b0000: bus_b = mdr; break;
        case 0b0001: bus_b = pc; break;
        case 0b0010: bus_b = mbr;break;
        case 0b0011: bus_b = mbr;
        if(mbr & (0b10000000)){
            bus_b = bus_b | (0b111111111111111111111111 << 8);
        }break;
        case 0b0100: bus_b = sp; break;
        case 0b0101: bus_b = lv; break;
        case 0b0110: bus_b = cpp; break;
        case 0b0111: bus_b = tos; break;
        case 0b1000: bus_b = opc; break;
        default: bus_b = -1; break;//Sinal de erro
    }
    bus_a = h;
}

//Escrita de registradores. Recebe uma palavra (valor de 32 bits) cujos 9 bits de ordem mais baixa indicam quais dos 9 registradores que
//podem ser escritos receberao o valor que está no barramento bus_c (saída do deslocador).
void write_register(word reg_end)
{
    if(reg_end & 0b000000001){
        mar = bus_c;
    }
    if(reg_end & 0b000000010){
        mdr = bus_c;
    }
    if(reg_end & 0b000000100){
        pc = bus_c;
    }
    if(reg_end & 0b000001000){
        sp = bus_c;
    }
    if(reg_end & 0b00010000){
        lv = bus_c;
    }
    if(reg_end & 0b000100000){
        cpp = bus_c;
    }
    if(reg_end & 0b001000000){
        tos = bus_c;
    }
    if(reg_end & 0b010000000){
        opc = bus_c;
    }
    if(reg_end & 0b100000000){
        h = bus_c;
    }
}

//Leitura e escrita de memória. Recebe em um byte o comando de memória codificado nos 3 bits de mais baixa ordem (fetch, read e write, podendo executar qualquer conjunto dessas três operações ao
//mesmo tempo, sempre nessa ordem) e executa a respectiva operação na memória principal.
//fetch: lê um byte da memória principal no endereço constando em PC para o registrador MBR. Endereçamento por byte.
//write e read: escreve e lê uma PALAVRA na memória principal (ou seja, 4 bytes em sequência) no endereço constando em MAR com valor no registrador MDR. Nesse caso, o endereçamento é dado em palavras.
//Mas, como a memoria principal eh um array de bytes, deve-se fazer a conversão do endereçamento de palavra para byte (por exemplo, a palavra com endereço 4 corresponde aos bytes 16, 17, 18 e 19).
//Lembrando que esta é uma máquina "little endian", isto é, os bits menos significativos são os de posições mais baixas.
//No exemplo dado, suponha os bytes:
//16 = 00110011
//17 = 11100011
//18 = 10101010
//19 = 01010101
//Tais bytes correspondem à palavra 01010101101010101110001100110011
void mainmemory_io(byte control)
{
    //fetch
    if(control & 0b001){
        mbr = memory[pc];
    }
    //read
    if(control & 0b010){
        //Palavras de 4 bytes lida "pedaço" a "pedaço"
        word aux;
        aux = memory[mar*4+3];
        aux = aux << 8;
        aux += memory[mar*4+2];
        aux = aux << 8;
        aux += memory[mar*4+1];
        aux = aux << 8;
        aux += memory[mar*4];
        mdr = aux;
    }
    //write
    if(control & 0b100){
        word aux = mdr;
        memory[mar*4] = aux;
        aux = aux >> 8;
        memory[mar*4+1] = aux;
        aux = aux >> 8;
        memory[mar*4+2] = aux;
        aux = aux >> 8;
        memory[mar*4+3] = aux;
    }
}

//Define próxima microinstrução a ser executada. Recebe o endereço da próxima instrução a ser executada codificado em uma palavra (considera-se, portanto, apenas os 9 bits menos significativos)
//e um modificador (regra de salto) codificado em um byte (considera-se, portanto, apenas os 3 bits menos significativos, ou seja JAMZ (bit 0), JAMN (bit 1) e JMPC (bit 2)), construindo e
//retornando o endereço definitivo (codificado em uma word - desconsidera-se os 21 bits mais significativos (são zerados)) da próxima microinstrução.
word next_address(word next, byte jam)
{
    //se 0 -> ocorre o pulo de 256 bits;
    if(jam & 0b001){
        next = next | (z << 8);
    }
    //se != 0 -> ocorre o pulo de 256 bits;
    if(jam & 0b010){
        next = next | (n << 8);
    }
    //pula para a posição mbr (pegar microinstrução da memoria);
    if(jam & 0b100){
        next = next | mbr;
    }
    return next;
}

int main(int argc, char* argv[])
{
    load_microprog(); //carrega microprograma de controle

    load_prog(); //carrega programa na memória principal a ser executado pelo emulador. Já que não temos entrada e saída, jogamos o programa direto na memória ao executar o emulador.

    decoded_microcode decmcode;

    //laço principal de execução do emulador. Cada passo no laço corresponde a um pulso do clock.
    //o debug mostra o estado interno do processador, exibindo valores dos registradores e da memória principal.
    //o getchar serve para controlar a execução de cada pulso pelo clique de uma tecla no teclado, para podermos visualizar a execução passo a passo.
    //Substitua os comentários pelos devidos comandos (na ordem dos comentários) para ter a implementação do laço.
    while(!halt)
    {
        debug();
        //implementar! Pega uma microinstrução no armazenamento de controle no endereço determinado pelo registrador contador de microinstrução e decodifica (gera a estrutura de microinstrução, ou seja, separa suas partes). Cada parte é devidamente passada como parâmetro para as funções que vêm a seguir.
        decmcode = decode_microcode(microprog[mpc]); //decodifica mpc;
        
        //implementar! Ler registradores
        read_registers(decmcode.reg_r);

        //implementar! Executa alu
        alu(decmcode.alu, h, bus_b);

        //implementar! Executa deslocamento
        shift(decmcode.sft, alu_out);
        bus_c = alu_out;
        //implementar! Escreve registradores
        write_register(decmcode.reg_w);

        //implementar! Manipula memória principal
        mainmemory_io(decmcode.mem);

        //implementar! Determina endereço da microinstrução (mpc) a ser executada no próximo pulso de clock
        mpc =next_address(decmcode.nadd, decmcode.jam);
	    getchar();
        
    }

    debug(false);

    return 0;
}
