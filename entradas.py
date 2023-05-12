import random

# Abrir o arquivo para escrita
with open('numeros.txt', 'w') as arquivo:
    # Gerar 100 listas com tamanhos crescentes
    for tamanho in range(1, 101):
        #fazer varias listas. a primeira 1.  a segunda 1 2. a terceira 1 2 3
        numeros = list(range(1, tamanho + 1))
        # Escrever cada número em uma linha separada
        for numero in numeros:
            arquivo.write(str(numero) + ' ')
        arquivo.write('-1')
        arquivo.write('\n')