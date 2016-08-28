unsigned char escreve_mem_EEPROM(unsigned char endereco_reg, unsigned char *dados_EEPROM, unsigned char quant_dado_escreve)
/**********************************************************************************************************************************
*                              Escreve no endereço da memória EEPROM apontado por endereco_reg                                    *
*                                                 				      					 										  *
*   Sendo:                                              			      					 									  *
*   endereco_reg: Endereço da célula da memória EEPROM.                       													  *
*   dado_EEPROM: Ponteiro para o vetor contendo os dados.                      					 							      *
*   quant_dado_escreve: Quantidade de bytes que devem ser escritos.            													  *
*   Observação: A proteção contra escrita na EEPROM deve estar desabilitada.   				 									  *
*                                          				              					 								 		  *
*   Livro: Microcontroladores PIC18 - Aprenda e Programe em Linguagem C.                                                          *
*   Autor: Alberto Noboru Miyadaira.                             		       					 								  *
**********************************************************************************************************************************/
{
	unsigned char cont_dado_escrito;
	
	EEADR  = endereco_reg; // Carrega o endereço da célula da memória EEPROM.
	
	for (cont_dado_escrito=0 ; cont_dado_escrito<quant_dado_escreve ; cont_dado_escrito++)
	{
	//Carrega o byte do buffer, para o registro de dado da memória EEPROM.
	EEDATA = dados_EEPROM [cont_dado_escrito]; 
	
	EECON1bits.EEPGD = 0;	//Seleciona a memória EEPROM.
	EECON1bits.CFGS = 0;    //Acesso a memória EEPROM.
	EECON1bits.WREN = 1;    //Habilita a escrita na memória.
	INTCONbits.GIE = 0;	    //Desabilita as interrupções.
	EECON2 = 0x55;	        //Carrega o valor 0x55 no registrador EECON2. (sequência obrigatória)
	EECON2 = 0xAA;    	    //Carrega o valor 0xAA no registrador EECON2. (sequência obrigatório)
	EECON1bits.WR = 1;	    //Inicia o ciclo de escrita na memória EEPROM.

	while(EECON1bits.WR); //Aguarda a finalização da operação de escrita.

	INTCONbits.GIE = 1;	 //Habilita as interrupções.
	EECON1bits.WREN = 0; //Desabilita a escrita na memória.
	
	if (EECON1bits.WRERR) 
		return 1; //Operação terminada prematuramente.
	
	EEADR++;//Incrementa o endereço apontado.
	}
	return 0; //Operação concluída
}

unsigned char le_mem_EEPROM( unsigned char endereco_reg )
/**************************************************************************************************************************************
*                                  Lê o conteúdo da memória EEPROM apontado por endereco_reg                                          *
*                                                 				                                                                      *
*   Sendo:                                           			                                                                      *
*   endereco_reg: Endereço da célula da memória EEPROM.                                                                               *
*   Observação: A proteção contra leitura de tabela deve estar desabilitada.                                                          *
*                                                				                                                                      *
*   Autor: Alberto Noboru Miyadaira.                         		                                                                  *
**************************************************************************************************************************************/
{
	EEADR = endereco_reg;	//Carrega o endereço da célula da memória EEPROM.
	EECON1bits.EEPGD = 0;	//Seleciona a memória EEPROM.
	EECON1bits.CFGS = 0;	//Acesso à memória EEPROM.
	EECON1bits.RD = 1;   	//Inicia o ciclo de leitura da memória EEPROM.
	return EEDATA;    	    //Retorna o conteúdo da memória EEPROM.
}
