Giuliano Augusto Faulin Belinassi - 8517272

Exercício Programa de MAC0336

1. Compilação:
	1.1 Sistemas UNIX-like. 
	
	Basta abrir um shell na pasta que contem este arquvo e executar:
	  $make
	Isto compilará para a plataforma linux.

	1.2 Windows.
	1.2.1: MinGW
	
	Tenha instalado o MinGW (https://mingw-w64.org/doku.php). Com o MSYS
	executando na pasta contendo este arquivo, execute:
	  $make
	Isto compilará para a plataforma Windows.
	É possível compilar sem usar o MSYS. Para tanto, tenha as variáveis
	de ambiente devidamente configuradas. Veja a documentação do Mingw.

	1.2.2: Microsoft Visual C++
	
	Este não foi testado, porém o código foi escrito na intercecção entre
	C e C++ e portanto um compilador C++ deve ser capaz de compilá-lo. 

	1.3 DOS.
	
	Tenha instalado o DJGPP (http://www.delorie.com/djgpp/). Com este
	devidamente instalado e as variáveis de ambiente devidamente
	configuradas, execute na pasta que contém este arquivo:
	  >make
	
	Note que tal executável não funcionará no DOSBox pois o csdpmi não
	é compatível com este emulador.

2. Testes automatizados:
	
	Foram utilizados testes automatizados durante o desenvolvimento. Isto
	facilitou os testes com o EP e tornou o seu desenvolvimento mais rápido.

	Para executar os testes (Compatível com sistemas UNIX-like e talvez MSYS)
	  $make tests
	

3. Do desenvolvimento do código:
	3.1 Da codificação do processador.
	
	Foi assumido que a plataforma alvo deste programa é um computador cujo
	tamanho de um char tenha 1 byte e 8 bits. Toda a cautela necessária foi
	inserida para manter a compatibilidade entre processadores que trabalham
	com a ordem dos bytes de forma diferente. Por exemplo, considere o
	número em base 16:
		A = 0x11223344
	Note que A tem 4 bytes. Em um processador Little Endian (como o x86), A
	seria representado na memória da seguinte forma
		A ~ 0x44 0x33 0x22 0x11
	E em um computador Big Endian (Como os PowerPC, antigos Macs)
		A ~ 0x11 0x22 0x33 0x44
	Claramente isto representa um problema se durante a comunicação do arquivo 
	criptografado os computadores têm processadores que codificam os bytes 
	distintamente, conforme descrito acima.

	Para resolver este problema, todo cast de um inteiro para bytes leva
	em consideração o fato acima, adotando como o alvo a simular um
	processador big-endian mesmo estando em, por exemplo, um x86. Veja
	a implementação em endian.c

	3.2 Dos arquivos tais que 8 não divide o seu comprimento
	
	Como o K128 considera que o arquivo é múltiplo de 8, então ao escrever o
	arquivo criptografado no dispositivo de saída, o primeiro byte contém
	quantos bytes foram adicionados para que o tamanho do arquivo se tornasse 
	múltiplo de 8. Ao descriptografar, esta informação é crucial para recuperar
	o tamanho original do arquivo.

	3.3 Do modo CBC

	O modo CBC foi implementado conforme descrito em aula. Veja as linhas 
	175-176 de k128.c

4. Máquinas de Teste:
	O Código foi testado apenas em x86:
	  - Intel Core-i5 (x86_64)
      - Intel Core2-Duo (i686)
	  - AMD A10 (64-bits) (x86_64)
	  - AMD K6-2 (i586)

	O sistema operacional usado no desenvolivimento foi o Linux, porém o
	programa se comportou muito bem no Windows e DOS.

EOF
