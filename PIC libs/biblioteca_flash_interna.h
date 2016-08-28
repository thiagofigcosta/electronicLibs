void le_mem_flash ( unsigned short long endereco_reg, unsigned char * dado_rom, unsigned char quant_dado_le )
/*************************************************************************************************************************************
*                                   L� o conte�do da mem�ria FLASH apontado por endereco_reg                                         *
*                                                                           													     *
* Sendo:                                                                    							 							 *
* endereco_reg: Endere�o do registrador.                                     						 								 *
* dado_rom: Ponteiro para o vetor contendo os dados.                         					 									 *
* quant_dado_le: Quantidade de bytes que devem ser lidos.                      					 									 *
* Observa��o: A prote��o contra leitura de tabela deve estar desabilitada.   					 									 *
*                                                                         							 								 *
* Livro: Microcontroladores PIC18 - Aprenda e Programe em Linguagem C.                                                               *
* Autor: Alberto Noboru Miyadaira.                                           														 *
*************************************************************************************************************************************/
{
	unsigned char cont_dado_lido;
	TBLPTR = endereco_reg; //Carrega o endere�o inicial da mem�ria de programa.
	
	EECON1bits.EEPGD = 1; //Seleciona a mem�ria de programa FLASH.
	EECON1bits.CFGS = 0;   //Acesso � mem�ria de programa FLASH.
	INTCONbits.GIE = 0;        //Desabilita as interrup��es.
	
	for (cont_dado_lido=0 ; cont_dado_lido<quant_dado_le ; cont_dado_lido++)
	{
		_asm
			TBLRDPOSTINC //L� a tabela apontada pelo ponteiro TBLPTR e a incrementa ap�s a leitura.
		_endasm
		
		//Carrega o conte�do da mem�ria de programa FLASH para o buffer.
		dado_rom [cont_dado_lido] = TABLAT; 
	}
	INTCONbits.GIE = 1; //Habilita as interrup��es.
}

unsigned short long escreve_mem_flash(unsigned short long endereco_reg, unsigned char *dados_rom, unsigned char quant_dado_escreve)
/****************************************************************************************************************************************
*                                       Escreve no endere�o da mem�ria FLASH apontado por endereco_reg                                  *
*                                                                         					  		 								    *
* Sendo:                                                                    							 								*
* endereco_reg: Endere�o do registrador.                                    														    *
* dado_rom: Ponteiro para o vetor contendo os dados.                        					 										*
* quant_dado_escreve: Quantidade de bytes que devem ser escritos.           					 										*
* Observa��o: A prote��o contra escrita de tabela deve estar desabilitada.   					 										*
*                                                                            							 								*
* Livro: Microcontroladores PIC18 - Aprenda e Programe em Linguagem C.                                                                  *
* Autor: Alberto Noboru Miyadaira.                                           						 									*
****************************************************************************************************************************************/
{
	unsigned char cont_dado_escrito;
	unsigned char rep_laco;
	unsigned char quant_dado_escreve_b[2];
	
	//Divide o valor referente ao tamanho do buffer (dados_rom) em duas vari�veis cujo valor m�ximo � 32.
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

	TBLPTR = endereco_reg; //Carrega o endere�o inicial da mem�ria de programa.
	EECON1bits.EEPGD = 1;	//Seleciona a mem�ria de programa FLASH.
	EECON1bits.CFGS = 0;	//Acesso � mem�ria FLASH.
	EECON1bits.WREN = 1;	//Habilita a escrita na mem�ria.
	EECON1bits.FREE = 1;	//Habilita apagar a fila da mem�ria FLASH (64bytes) apontada por TBLPTR.
	INTCONbits.GIE = 0;	//Desabilita as interrup��es.
	EECON2 = 0x55;	//Carrega o valor 0x55 no registrador EECON2. (sequ�ncia obrigat�ria)
	EECON2 = 0xAA;	//Carrega o valor 0xAA no registrador EECON2. (sequ�ncia obrigat�ria)
	EECON1bits.WR = 1;	//Apaga a fila da mem�ria FLASH (64bytes).
	INTCONbits.GIE = 1;	//Habilita as interrup��es.

	for (rep_laco=0 ; rep_laco<2 ; rep_laco++)
	{
		for (cont_dado_escrito=0 ; cont_dado_escrito<quant_dado_escreve_b[rep_laco]; cont_dado_escrito++)
		{
			//Carrega o byte do buffer para o registro da mem�ria de programa.
			TABLAT = dados_rom[rep_laco*32+cont_dado_escrito]; 
			_asm
				//Escreve na tabela indicada pelo ponteiro TBLPTR e a incrementa ap�s a escrita.
				TBLWTPOSTINC 
			_endasm
		}

	/*Quando o comando de escrita � efetuado, o ponteiro deve estar dentro da regi�o onde os dados foram carregados.*/
	if(quant_dado_escreve_b[rep_laco] == 32)
		TBLPTR--; //Decrementa o ponteiro
	
	EECON1bits.EEPGD = 1;	//Seleciona a mem�ria de programa FLASH.
	EECON1bits.CFGS = 0;	//Acesso a mem�ria FLASH.
	EECON1bits.WREN = 1;	//Habilita a escrita na mem�ria.
	INTCONbits.GIE = 0;	//Desabilita as interrup��es.
	EECON2 = 0x55;	//Carrega o valor 0x55 no registrador EECON2. (sequ�ncia obrigat�ria)
	EECON2 = 0xAA;	//Carrega o valor 0xAA no registrador EECON2. (sequ�ncia obrigat�ria)
	EECON1bits.WR = 1;	//Inicia o ciclo para escrever os 32bytes na mem�ria FLASH.
	INTCONbits.GIE = 1;	//Habilita as interrup��es.
	EECON1bits.WREN = 0;	//Desabilita a escrita na mem�ria.
	/*For�a o ponteiro TBLPTR para a posi��o do pr�ximo bloco*/
	TBLPTR=endereco_reg+(rep_laco+1)*32; 
	}
	return TBLPTR; //Retorna o valor do ponteiro para uma poss�vel sequ�ncia de escrita. (endereco+64)
}
