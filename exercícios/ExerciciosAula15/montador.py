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
        conteudo = linha.lower().split()
        #Checar se a palavra é um marcador válido
        if(marcador(conteudo[0])):
            del conteudo[0]
        #Checar se a palavra é uma instrução válida
        if(conteudo[0] in dicionario_instrucoes.keys()):
            constantes = dicionario_instrucoes[conteudo[0]][1]  #Tipo de constantes
            operando = conteudo[1: len(constantes)+1] #Valores das constantes
            print(conteudo)
            print(constantes, operando)
            #Checar se é um operador válido
            if(operador(constantes, len(constantes), operando)):
                #Adicionar instrução
                byte_execucao +=1
                byte.append(dicionario_instrucoes[conteudo[0]][0])
            else:
                print("Operador inválido")
                return 0
        else:
            print("Instrução inválida")
            return 0
    txt.close()
    print(marcadores, variaveis, byte)
    #escrever() #Se não tiver erros, escreve o arquivo
        
def marcador(marcador):
    global dicionario_instrucoes, variaveis, byte_execucao 
    if(marcador in dicionario_instrucoes.keys() or marcador in variaveis.keys()): #checar se é uma variável  
        print("Marcador inválido")
        return 0
    else:
        #Adiciona o marcador 
        marcadores[marcador] = byte_execucao
        return 1


def operador(constantes, num_constantes, operando):
    global variaveis, byte, byte_execucao, quant_variaveis
    if(num_constantes == len(operando)):
        #Se num_constantes = 0, obviamente o comando estará certo
        if(num_constantes == 0):
            condicao = True
        condicao = True
        for i in range(0,num_constantes): 
            if(constantes[i] == 'varnum'):
                if(operando[i] not in variaveis):
                    condicao = False
                elif(constantes[i] == 'new_varnum'):
                    if(not operando[0] in marcadores.keys()):
                        condicao = False
                elif(constantes[i] == 'byte') or (constantes[i] == 'const') or (constantes[i] == 'disp') or (constantes[i] == 'index'):
                    if(not operando[i].isnumeric()):
                        condicao = False
                else: #offset
                    if(not marcador(operando[i])):
                        condicao = False
    else:
        condicao = False
        return 0
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
            #???????????
        else: #offset
            byte.append([operando[i], byte_execucao])
            byte_execucao +=2
    return 1

def variavel(variavel):
    global dicionario_instrucoes, variaveis
    if(variavel in dicionario_instrucoes.keys() or variavel in variaveis.keys()):
        print("Variável com nome inválido")
        return 0
    else:
        return 1

main()