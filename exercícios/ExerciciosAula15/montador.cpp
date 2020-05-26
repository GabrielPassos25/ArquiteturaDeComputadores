#include <iostream>
#include <fstream>
#include <string>
using namespace std;
struct Operadores{
    string nome;
    int byte;
    int constante;
    int constante1;
};
struct Noh{
  Operadores instrucao;
  Noh *prox;
};
Noh *fim;
//instruções que não utilizam operadores ou possuem somente um operador
Operadores nop = {"nop",0x01};
Operadores iadd = {"iadd",0x02};
Operadores isub = {"isub",0x05};
Operadores iand = {"iand",0x08};
Operadores ior = {"ior",0x0b};
Operadores dup = {"dup",0x0e};
Operadores pop = {"pop",0x10};
Operadores swap = {"swap",0x13};
Operadores wide = {"wide",0x28};
Operadores ireturn = {"ireturn",0x6b};
Operadores go_to = {"goto",0x3c};
Operadores iflt = {"iflt",0x43};
Operadores ifeq = {"ifeq", 0x47};
Operadores if_icmpeq = {"if_icmpeq",0x4b};
Operadores ldc_w = {"ldc_w", 0x32};
Operadores invokevirtual = {"invokevirtual",0x55};
Operadores bipush = {"bipush",0x19};
Operadores iload = {"iload",0x1c};
Operadores istore = {"istore",0x22};
//instruções que utilizam mais de um operador
Operadores iinc = {"iinc",0x36};

//armazenamento de dados
string marcadores = {};
string constantes = {};
int bytes = {};
//variáveis globais
int num_byte = 0;
int num_const = 0;

int main (int argc,char *argv[]) {
  string linha;
  FILE *arquivo;
  arquivo = fopen(argv[1],"r");
  while(!feof(arquivo)){
    ifstream i(argv[1]);
    getline (i,linha);
    linha = linha.split();
    
    if(marcador()){

    }
  }
  return 0;
}
//Colocar na lista encadeada;
bool enfilar(Operadores e){
  Noh* n= new(nothrow)Noh;
  if(n==nullptr){
    return true;
  }
  n->instrucao = e;
  n->prox = nullptr;
  if(vazia()){
    fim=n;
  }
  else{
    fim->prox=n;
    fim=n;
  }
  return false;
}
int marcador(string a){
  if(a.substr(0," "));
}