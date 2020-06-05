import sys
#operandos byte, const e varnum são de 1 byte. Os operandos disp, index e offset são de 2 bytes.
dicionario_instrucoes = {'nop':[0x01,[]],'iadd':[0x02, []],'isub':[0x05, []],'iand':[0x08,[]],'ior':[0x0b, []],'dup':[0x0e, []],'pop':[0x10, []],'swap':[0x13, []],'wide':[0x28, []],'ireturn':[0x6b, []], #variáveis sem constante
                     'bipush':[0x19,['byte']],'iload':[0x1c, ['varnum']],'istore':[0x22, ['new_varnum']],'ldc_w':[0x32, ['index']],'goto':[0x3c, ['offset']], #Variáveis que utilizam uma constante
                     'iflt':[0x43, ['offset']],'ifeq':[0x47, ['offset']],'if_icmpeq':[0x4b, ['offset']],'invokevirtual':[0x55, ['disp']],
                     'iinc':[0x36,['varnum', 'const']]} #Instruções que utilizam duas constantes

#Dicionários para marcar as principais coisas
marcadores = {}
variaveis = {}
byte = []
programa_assembly = []
byte_execucao = 0
linhas = 0
quant_variaveis = 0

def main():
    #Abrir o programa txt
    global linhas, byte_execucao, dicionario_instrucoes
    try:
        txt = open(sys.argv[1],'r')
    except:
        print("Arquivo inválido")
        return 0
    for linha in txt:
        linhas +=1
        programa_assembly.append(linha)
        conteudo = linha.lower().split()
        #Checar se a palavra é um marcador válido
        if(marcador(conteudo[0])):
            del conteudo[0]
        #Checar se a palavra é uma instrução válida
        if(conteudo[0] in dicionario_instrucoes.keys()):
            #Assume que a instrução está correta
            byte_execucao +=1
            byte.append(dicionario_instrucoes[conteudo[0]][0])
            #constantes para checagem operadores
            constantes = dicionario_instrucoes[conteudo[0]][1]  #Tipo de constantes
            operando = conteudo[1: len(constantes)+1] #Valores das constantes
            #Checar se é um operador válido
            if(not operador(constantes, len(constantes), operando)):
                print("Instrução inválida")
                return 0
    txt.close()
    escrever() #Se não tiver erros, escreve o arquivo
        
def marcador(marcador):
    global dicionario_instrucoes, variaveis, byte_execucao 
    if(marcador in dicionario_instrucoes.keys() or marcador in variaveis.keys()): #checar se é uma variável  
        return 0
    else:
        #Adiciona o marcador 
        marcadores[marcador] = byte_execucao
        return 1


def operador(constantes, num_constantes, operando):
    global variaveis, byte, byte_execucao, quant_variaveis, marcadores
    if(num_constantes == 0):
        condicao = True
    else:
        condicao = True
        for i in range(0,num_constantes): 
            if(constantes[i] == 'varnum'):
                if(operando[i] not in variaveis):
                    condicao = False
            elif(constantes[i] == 'new_varnum'):
                if(operando[0] in marcadores.keys()):
                    condicao = False
            elif(constantes[i] == 'byte') or (constantes[i] == 'const') or (constantes[i] == 'disp') or (constantes[i] == 'index'):
                if(not operando[i].isnumeric()):
                    condicao = False
            #Checagem feita no fim
            #else:
                #if(not operando[i] in marcadores):
                    #print(marcadores)
                    #print("offset invalido")
                    #condicao = False
    if(condicao == False):
        return condicao
    #Adiciona o operador
    for i in range(0,num_constantes):
        if(constantes[i] == 'varnum'):
            byte.append(variaveis[operando[i]])
            byte_execucao +=1
        elif(constantes[i] == 'new_varnum'):
            if(operando[i] not in variaveis.keys()):
                variaveis[operando[i]] = quant_variaveis
                quant_variaveis += 1
            byte.append(variaveis[operando[i]])
            byte_execucao +=1
        elif(constantes[i] == 'byte') or (constantes[i] == 'const'):
            byte.append(int(operando[i]))
            byte_execucao +=1
        elif(constantes[i] == 'disp') or (constantes[i] == 'index'):
            byte_list.append(int(operand[i]) & 0xff) #Unsigned int 8 bits
            byte_list.append(int(operand[i]) >> 8)
            byte_counter += 2
        else: #offset
            byte.append([operando[i], byte_execucao-1])
            byte_execucao +=2
    return 1

def variavel(variavel):
    global dicionario_instrucoes, variaveis
    if(variavel in dicionario_instrucoes.keys() or variavel in variaveis.keys()):
        print("Variável com nome inválido")
        return 0
    else:
        return 1

def inicializar(byte, quant_variaveis, output):
    registradores = [0x7300,0x0006,0x1001,0x0400,0x1001 + quant_variaveis]
    for r in registradores:
        output += r.to_bytes(4,byteorder = 'little')

def cabecalho(byte_execucao, output):
    q = (20 + byte_execucao).to_bytes(4, byteorder = 'little')
    output += q

def programa(byte,output,marcadores):
    for i in byte:
        if type(i) is list:
            #Checar se os offsets existem
            if(not i[0] in marcadores):
                    print("offset", "\"",i[0], "\"", "inválido")
                    return 0
            byte_marcador = (marcadores[i[0]] - i[1]) & 0xffff #Unsigned int 16 bits offset = 2bytes 
            output += byte_marcador.to_bytes(2,byteorder = 'big')
        else:
            output.append(i)

def escrita(output):
    program = open(sys.argv[1] + '.exe', 'wb')
    program.write(output)
    program.close()

def informacoes(output):
    global marcadores, variaveis,programa_assembly
    print("Marcadores:", marcadores)
    print("Variáveis:", variaveis)
    print("Tamanho do programa:", int(output[0]), "bytes")
    print("Array de bytes:", end="[")
    for i in range(0,len(output)-1):
        print(hex(output[i]),end = ",")
    print(i,end ="]\n")
    print("Programa em assembly:", programa_assembly)
    print("Vetor de bytes:", byte)

def escrever():
    global byte_execucao, byte, linhas, marcadores
    output = bytearray()
    #cabeçalho
    cabecalho(byte_execucao, output)
    #inicialização
    inicializar(byte, quant_variaveis, output)
    #programa
    if(programa(byte, output, marcadores) == 0):
        return 0
    #escrita
    escrita(output)
    #informações
    informacoes(output)

#Iniciaçização do programa
main()