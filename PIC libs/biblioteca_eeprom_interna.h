unsigned char escreve_mem_EEPROM(unsigned char endereco_reg, unsigned char *dados_EEPROM, unsigned char quant_dado_escreve)
/**********************************************************************************************************************************
*                              Escreve no endere�o da mem�ria EEPROM apontado por endereco_reg                                    *
*                                                 				      					 										  *
*   Sendo:                                              			      					 									  *
*   endereco_reg: Endere�o da c�lula da mem�ria EEPROM.                       													  *
*   dado_EEPROM: Ponteiro para o vetor contendo os dados.                      					 							      *
*   quant_dado_escreve: Quantidade de bytes que devem ser escritos.            													  *
*   Observa��o: A prote��o contra escrita na EEPROM deve estar desabilitada.   				 									  *
*                                          				              					 								 		  *
*   Livro: Microcontroladores PIC18 - Aprenda e Programe em Linguagem C.                                                          *
*   Autor: Alberto Noboru Miyadaira.                             		       					 								  *
**********************************************************************************************************************************/
{
	unsigned char cont_dado_escrito;
	
	EEADR  = endereco_reg; // Carrega o endere�o da c�lula da mem�ria EEPROM.
	
	for (cont_dado_escrito=0 ; cont_dado_escrito<quant_dado_escreve ; cont_dado_escrito++)
	{
	//Carrega o byte do buffer, para o registro de dado da mem�ria EEPROM.
	EEDATA = dados_EEPROM [cont_dado_escrito]; 
	
	EECON1bits.EEPGD = 0;	//Seleciona a mem�ria EEPROM.
	EECON1bits.CFGS = 0;    //Acesso a mem�ria EEPROM.
	EECON1bits.WREN = 1;    //Habilita a escrita na mem�ria.
	INTCONbits.GIE = 0;	    //Desabilita as interrup��es.
	EECON2 = 0x55;	        //Carrega o valor 0x55 no registrador EECON2. (sequ�ncia obrigat�ria)
	EECON2 = 0xAA;    	    //Carrega o valor 0xAA no registrador EECON2. (sequ�ncia obrigat�rio)
	EECON1bits.WR = 1;	    //Inicia o ciclo de escrita na mem�ria EEPROM.

	while(EECON1bits.WR); //Aguarda a finaliza��o da opera��o de escrita.

	INTCONbits.GIE = 1;	 //Habilita as interrup��es.
	EECON1bits.WREN = 0; //Desabilita a escrita na mem�ria.
	
	if (EECON1bits.WRERR) 
		return 1; //Opera��o terminada prematuramente.
	
	EEADR++;//Incrementa o endere�o apontado.
	}
	return 0; //Opera��o conclu�da
}

unsigned char le_mem_EEPROM( unsigned char endereco_reg )
/**************************************************************************************************************************************
*                                  L� o conte�do da mem�ria EEPROM apontado por endereco_reg                                          *
*                                                 				                                                                      *
*   Sendo:                                           			                                                                      *
*   endereco_reg: Endere�o da c�lula da mem�ria EEPROM.                                                                               *
*   Observa��o: A prote��o contra leitura de tabela deve estar desabilitada.                                                          *
*                                                				                                                                      *
*   Autor: Alberto Noboru Miyadaira.                         		                                                                  *
**************************************************************************************************************************************/
{
	EEADR = endereco_reg;	//Carrega o endere�o da c�lula da mem�ria EEPROM.
	EECON1bits.EEPGD = 0;	//Seleciona a mem�ria EEPROM.
	EECON1bits.CFGS = 0;	//Acesso � mem�ria EEPROM.
	EECON1bits.RD = 1;   	//Inicia o ciclo de leitura da mem�ria EEPROM.
	return EEDATA;    	    //Retorna o conte�do da mem�ria EEPROM.
}
