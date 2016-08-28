void le_mem_flash ( unsigned short long endereco_reg, unsigned char * dado_rom, unsigned char quant_dado_le )
/*************************************************************************************************************************************
*                                   Lê o conteúdo da memória FLASH apontado por endereco_reg                                         *
*                                                                           													     *
* Sendo:                                                                    							 							 *
* endereco_reg: Endereço do registrador.                                     						 								 *
* dado_rom: Ponteiro para o vetor contendo os dados.                         					 									 *
* quant_dado_le: Quantidade de bytes que devem ser lidos.                      					 									 *
* Observação: A proteção contra leitura de tabela deve estar desabilitada.   					 									 *
*                                                                         							 								 *
* Livro: Microcontroladores PIC18 - Aprenda e Programe em Linguagem C.                                                               *
* Autor: Alberto Noboru Miyadaira.                                           														 *
*************************************************************************************************************************************/
{
	unsigned char cont_dado_lido;
	TBLPTR = endereco_reg; //Carrega o endereço inicial da memória de programa.
	
	EECON1bits.EEPGD = 1; //Seleciona a memória de programa FLASH.
	EECON1bits.CFGS = 0;   //Acesso à memória de programa FLASH.
	INTCONbits.GIE = 0;        //Desabilita as interrupções.
	
	for (cont_dado_lido=0 ; cont_dado_lido<quant_dado_le ; cont_dado_lido++)
	{
		_asm
			TBLRDPOSTINC //Lê a tabela apontada pelo ponteiro TBLPTR e a incrementa após a leitura.
		_endasm
		
		//Carrega o conteúdo da memória de programa FLASH para o buffer.
		dado_rom [cont_dado_lido] = TABLAT; 
	}
	INTCONbits.GIE = 1; //Habilita as interrupções.
}

unsigned short long escreve_mem_flash(unsigned short long endereco_reg, unsigned char *dados_rom, unsigned char quant_dado_escreve)
/****************************************************************************************************************************************
*                                       Escreve no endereço da memória FLASH apontado por endereco_reg                                  *
*                                                                         					  		 								    *
* Sendo:                                                                    							 								*
* endereco_reg: Endereço do registrador.                                    														    *
* dado_rom: Ponteiro para o vetor contendo os dados.                        					 										*
* quant_dado_escreve: Quantidade de bytes que devem ser escritos.           					 										*
* Observação: A proteção contra escrita de tabela deve estar desabilitada.   					 										*
*                                                                            							 								*
* Livro: Microcontroladores PIC18 - Aprenda e Programe em Linguagem C.                                                                  *
* Autor: Alberto Noboru Miyadaira.                                           						 									*
****************************************************************************************************************************************/
{
	unsigned char cont_dado_escrito;
	unsigned char rep_laco;
	unsigned char quant_dado_escreve_b[2];
	
	//Divide o valor referente ao tamanho do buffer (dados_rom) em duas variáveis cujo valor máximo é 32.
	if(quant_dado_escreve > 31)
	{
		quant_dado_escreve_b[0]=32;
		quant_dado_escreve_b[1]=quant_dado_escreve-32;
	}
	else
	{
		quant_dado_escreve_b[0]=quant_dado_escreve;
		quant_dado_escreve_b[1]=0;
	}

	TBLPTR = endereco_reg; //Carrega o endereço inicial da memória de programa.
	EECON1bits.EEPGD = 1;	//Seleciona a memória de programa FLASH.
	EECON1bits.CFGS = 0;	//Acesso à memória FLASH.
	EECON1bits.WREN = 1;	//Habilita a escrita na memória.
	EECON1bits.FREE = 1;	//Habilita apagar a fila da memória FLASH (64bytes) apontada por TBLPTR.
	INTCONbits.GIE = 0;	//Desabilita as interrupções.
	EECON2 = 0x55;	//Carrega o valor 0x55 no registrador EECON2. (sequência obrigatória)
	EECON2 = 0xAA;	//Carrega o valor 0xAA no registrador EECON2. (sequência obrigatória)
	EECON1bits.WR = 1;	//Apaga a fila da memória FLASH (64bytes).
	INTCONbits.GIE = 1;	//Habilita as interrupções.

	for (rep_laco=0 ; rep_laco<2 ; rep_laco++)
	{
		for (cont_dado_escrito=0 ; cont_dado_escrito<quant_dado_escreve_b[rep_laco]; cont_dado_escrito++)
		{
			//Carrega o byte do buffer para o registro da memória de programa.
			TABLAT = dados_rom[rep_laco*32+cont_dado_escrito]; 
			_asm
				//Escreve na tabela indicada pelo ponteiro TBLPTR e a incrementa após a escrita.
				TBLWTPOSTINC 
			_endasm
		}

	/*Quando o comando de escrita é efetuado, o ponteiro deve estar dentro da região onde os dados foram carregados.*/
	if(quant_dado_escreve_b[rep_laco] == 32)
		TBLPTR--; //Decrementa o ponteiro
	
	EECON1bits.EEPGD = 1;	//Seleciona a memória de programa FLASH.
	EECON1bits.CFGS = 0;	//Acesso a memória FLASH.
	EECON1bits.WREN = 1;	//Habilita a escrita na memória.
	INTCONbits.GIE = 0;	//Desabilita as interrupções.
	EECON2 = 0x55;	//Carrega o valor 0x55 no registrador EECON2. (sequência obrigatória)
	EECON2 = 0xAA;	//Carrega o valor 0xAA no registrador EECON2. (sequência obrigatória)
	EECON1bits.WR = 1;	//Inicia o ciclo para escrever os 32bytes na memória FLASH.
	INTCONbits.GIE = 1;	//Habilita as interrupções.
	EECON1bits.WREN = 0;	//Desabilita a escrita na memória.
	/*Força o ponteiro TBLPTR para a posição do próximo bloco*/
	TBLPTR=endereco_reg+(rep_laco+1)*32; 
	}
	return TBLPTR; //Retorna o valor do ponteiro para uma possível sequência de escrita. (endereco+64)
}
