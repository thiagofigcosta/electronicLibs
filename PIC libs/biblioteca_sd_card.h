/***************************************************************************************************************************************************
***************************************************************BIBLIOTECA - SD Card*****************************************************************
**                                         					   				                                                                       *
**                                       Esta biblioteca contém as funções de controle da memória SD Card.   					                   *
**                                                            								 													   *
** Livro: Microcontroladores PIC18 - Aprenda e Programe em Linguagem C.                                                               			   *               
** Autor: Alberto Noboru Miyadaira            				   				 																	   *
***************************************************************************************************************************************************/

unsigned int cont_sd, cont_bloco; //Contadores dos laços.


#define CMD0 0x40	//Reinicia o SD_CARD e aguarda entrar no modo de espera.
#define CMD12 0x4C	//Interrompe a leitura sequencial.
#define CMD16 0x50	//Determina o tamanho do bloco. 
#define CMD17 0x51	//Lê um bloco de dados.
#define CMD18 0x52	//Lê uma sequência de blocos de dados.
#define CMD24 0x58	//Escreve em um bloco.  
#define CMD25 0x59	//Escreve em uma sequência de blocos.  
#define CMD55 0x77	//Notifica o SD Card que o próximo comando será para uma aplicação específica.
#define CMD58 0x7A	//Lê o registrador OCR.
#define ACMD41 0x69	//Ativa o processo de inicialização do SD_CARD/MMC .

// O tamanho do bloco pode variar de acordo com a necessidade da aplicação. Por exemplo, ele pode ser definido 
// como 32, 64, 128, 256bytes ... Porém, não são todos os cartões que suportam estes comprimentos, o que pode ser verificado no 
// Datasheet do cartão ou experimentalmente.
#ifndef TAMANHO_BLOCO 
#define TAMANHO_BLOCO 512 //Define o tamanho do bloco em bytes. DESTACAR
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Este pino pode ser qualquer pino I/O digital, e deve ser configurado como saída.
#ifndef ENABLE_SD
#define ENABLE_SD PORTDbits.RD4  //Define o pino destinado a habilitar o SD Card. DESTACAR
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Retorna o endereço do bloco solicitado.
unsigned long bloco_SD(unsigned long n_bloco)
{
return n_bloco*TAMANHO_BLOCO;
}

unsigned char resposta_8bits (unsigned char v_r1, unsigned char mascara, unsigned char t_r1)
{
unsigned char cont_r1;

      for (cont_r1=0;cont_r1<t_r1;cont_r1++)
      {
WriteSPI(0xff);//Coloca o pino SDO em nível alto e lê o dado recebido.
if((SSPBUF&mascara)!=v_r1 && cont_r1<(t_r1-1))
continue; //Força a próxima iteração.
   
if(cont_r1<(t_r1-1))
return 1; //Resposata R1 OK.
   
ENABLE_SD = 1; //Desabilita o SD Card   
return 0; //Erro na resposta R1.
      }
}

//Envia o comando para o SD Card.
unsigned char envia_comando ( unsigned char byte_1, unsigned char byte_2, unsigned char byte_3, unsigned char byte_4, unsigned char byte_5, unsigned char byte_6, unsigned char valor_r1, unsigned char tentativas_r1 )
{
WriteSPI(0xff);
WriteSPI(byte_1);
WriteSPI(byte_2);                
WriteSPI(byte_3);
WriteSPI(byte_4);
WriteSPI(byte_5);
WriteSPI(byte_6);  

return resposta_8bits( valor_r1, 0xff, tentativas_r1);
}

//Inicia o SD Card.
unsigned char inicia_sd( )
{
//--------------------------------------------------------INÍCIO DA ROTINA DE INICIALIZAÇÃO DO SD Card--------------------------------------------
ENABLE_SD = 1; //Desabilita o SD Card

OpenSPI (SPI_FOSC_64, MODE_00 ,SMPMID);
//Configura o modo de operação do módulo SPI interno do PIC.
//Módulo configurado como master, clock = Fosc/prescaler (deve estar entre 100KHz a 400KHz).
//CKE = 1 - CKP = 0 - SMP = 0.            

//--------------------------------------------------------------------------INÍCIO 80 CLOCKS------------------------------------------------------------------------
ENABLE_SD = 0;//Habilita o SD Card.
for(cont_sd=0;cont_sd<10;cont_sd++) // Envia 80 clocks para o barramento SPI.    
{
WriteSPI(0xff); //Envia 8 pulsos de clock.
}
ENABLE_SD = 1; //Desabilita o SD Card
//---------------------------------------------------------------------------FIM 80 CLOCKS---------------------------------------------------------------------------

//------------------------------------------------------------------INÍCIO DO COMANDO CMD0------------------------------------------------------------------
//imprime_string_RS232("INÍCIO - CMD0\n"); //Imprime a string.

ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando CMD0: Reinicia todos os cartões e os coloca em modo de espera.
if(!envia_comando ( CMD0, 0, 0, 0, 0, 0x95, 0x01, 10 ))
return 0;//Ocorrência de erro na inicialização.

ENABLE_SD = 1; //Desabilita o SD Card

//imprime_string_RS232("FIM - CMD0\n"); //Imprime a string.
//-------------------------------------------------------------------FIM DO COMANDO CMD0---------------------------------------------------------------------

//-----------------------------------------------------------------INÍCIO DO COMANDO CMD55-----------------------------------------------------------------
for ( cont_sd = 0 ; cont_sd < 20 ; cont_sd++ )
{
//imprime_string_RS232("INÍCIO - CMD55\n"); //Imprime a string.
  
ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando CMD55: Notifica o SD Card que o próximo comando será para uma aplicação específica.
if(!envia_comando ( CMD55, 0, 0, 0, 0, 0xFF, 0x01, 10 )&& cont_sd <19) 
continue; //Volta ao início do laço.

ENABLE_SD = 1; //Desabilita o SD Card

//imprime_string_RS232("FIM - CMD55\n"); //Imprime a string.
//---------------------------------------------------------------------FIM DO COMANDO CMD55-----------------------------------------------------------------

//-----------------------------------------------------------------INÍCIO DO COMANDO ACMD41---------------------------------------------------------------
//imprime_string_RS232("INÍCIO - ACMD41\n"); //Imprime a string.

ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando ACMD41: Ativa o processo de inicialização do SD_CARD.
if(!envia_comando ( ACMD41, 0, 0, 0, 0, 0xFF, 0x00, 10 )&& cont_sd <19) 
continue; //Volta ao início do laço.

if (cont_sd < 19)
{
ENABLE_SD = 1; //Desabilita o SD Card
break;//Força a saída do laço.
}
else
return 0;//Ocorrência de erro na inicialização.   
}

//imprime_string_RS232("FIM - ACMD41\n"); //Imprime a string.
//------------------------------------------------------------------------FIM DO COMANDO ACMD41------------------------------------------------------------

//-----------------------------------------------------------------------INÍCIO DO COMANDO CMD58-----------------------------------------------------------
//imprime_string_RS232("INÍCIO - CMD58\n"); //Imprime a string.

ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando CMD16: Determina o tamanho do bloco.  
if(!envia_comando ( CMD58, 0, 0, 0, 0, 0xFF, 0x00, 10 )) 
return 0;//Ocorrência de erro na inicialização.

//Verifica se o procedimento de alimentação está finalizado. (Resposta R3) - 80FF8000.
WriteSPI(0xff);//Coloca o pino SDO em nível alto e lê o dado recebido.
if((SSPBUF&0x80)==0x80) //Byte 4 do registrador OCR
{
WriteSPI(0xff); //Coloca o pino SDO em nível alto e lê o dado recebido. (Byte 3 do OCR)
WriteSPI(0xff); //Coloca o pino SDO em nível alto e lê o dado recebido. (Byte 2 do OCR)
WriteSPI(0xff); //Coloca o pino SDO em nível alto e lê o dado recebido. (Byte 1 do OCR)
}
else
{
WriteSPI(0xff); //Coloca o pino SDO em nível alto e lê o dado recebido. (Byte 3 do OCR)
WriteSPI(0xff); //Coloca o pino SDO em nível alto e lê o dado recebido. (Byte 2 do OCR)
WriteSPI(0xff); //Coloca o pino SDO em nível alto e lê o dado recebido. (Byte 1 do OCR)
return 0;//Ocorrência de erro na inicialização.   
}

ENABLE_SD = 1; //Desabilita o SD Card

//imprime_string_RS232("FIM - CMD58\n"); //Imprime a string.
//------------------------------------------------------------------------FIM DO COMANDO CMD58--------------------------------------------------------------

//-------------------------------------------------------FIM DA ROTINA DE INICIALIZAÇÃO DO SD Card-------------------------------------------------
//imprime_string_RS232("CARTÃO INICIADO COM SUCESSO!!\n"); //Imprime a string.

OpenSPI (SPI_FOSC_4, MODE_00 ,SMPMID);
//Configura o modo de operação do módulo SPI erno do PIC.
//Módulo configurado como master, clock_max = Fosc/prescaler. Não ultrapassar o limite de velocidade do cartão.
//CKE = 1 - CKP = 0 - SMP = 0.   

//--------------------------------------------------------------------INÍCIO DO COMANDO CMD16--------------------------------------------------------------
//imprime_string_RS232("INÍCIO - CMD16\n"); //Imprime a string.

ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando CMD16: Determina o tamanho do bloco.  
if(!envia_comando ( CMD16, 0, 0, TAMANHO_BLOCO >> 8, TAMANHO_BLOCO, 0xFF, 0x00, 10 )) 
   return 0;//Ocorrência de erro na inicialização.
     
ENABLE_SD = 1; //Desabilita o SD Card

//imprime_string_RS232("FIM - CMD16\n"); //Imprime a string.
//-------------------------------------------------------------------FIM DO COMANDO CMD16-------------------------------------------------------------------
                     
return 1; //SD Card pronto para operar.
}

//Escreve dados no bloco do SD Card.
unsigned char escreve_bloco(unsigned long bloco, unsigned char *pont_dados)
{
bloco = bloco_SD(bloco); //Obtém o endereço do bloco.

//-------------------------------------------------------------------INÍCIO DO COMANDO CMD24---------------------------------------------------------------
//imprime_string_RS232("INÍCIO - CMD24\n"); //Imprime a string.

ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando CMD24: Escreve dados em um bloco.         
if(!envia_comando ( CMD24, bloco>>24, bloco>>16, bloco>>8, bloco, 0xFF, 0x00, 10 )) 
return 0;//Erro no comando de escrita.
         
WriteSPI(0xFE);  // Início do bloco.

for( cont_sd = 0 ; cont_sd < TAMANHO_BLOCO ; cont_sd++ )
{
//Este bloco deve ser modificado pelo usuário, de acordo com a sua necessidade.   DESTACAR

WriteSPI(*pont_dados);
pont_dados++;

}

WriteSPI(0xFF);  //CRC.
WriteSPI(0xFF);  //CRC.
   
if(!resposta_8bits (0x05,0x05,10))
return 0;//Erro na escrita.      
 
ENABLE_SD = 1; //Desabilita o SD Card

WriteSPI (0xFF); //Envia 8 pulsos de clock para finalizar a operação.

//imprime_string_RS232("FIM - CMD24\n"); //Imprime a string.
//------------------------------------------------------------------FIM DO COMANDO CMD24--------------------------------------------------------------------

return 1;//Escrita realizada.
}

//Escreve dados em uma sequência de blocos.
unsigned char escreve_seq_bloco(unsigned long bloco, unsigned long quanti_blocos)
{
bloco = bloco_SD(bloco); //Obtém o endereço do bloco.

//-----------------------------------------------------------------INÍCIO DO COMANDO CMD25-----------------------------------------------------------------
//imprime_string_RS232("INÍCIO - CMD25\n"); //Imprime a string.

ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando CMD25: Escreve dados em uma sequência de blocos.         
if(!envia_comando ( CMD25, bloco>>24, bloco>>16, bloco>>8, bloco, 0xFF, 0, 10 )) 
return 0;//Erro no comando de escrita.
   
for( cont_bloco=0 ; cont_bloco < quanti_blocos ; cont_bloco++ )
{
WriteSPI(0xFC); //Início do bloco.
   
for( cont_sd = 0 ; cont_sd < TAMANHO_BLOCO ; cont_sd++)
{
//Esta instrução deve ser modificada pelo usuário, de acordo com a sua necessidade.
WriteSPI('A');//Envia o dado para o SD Card.   DESTACAR
}
   
WriteSPI(0xFF); //CRC.
WriteSPI(0xFF); //CRC.
   
if(!resposta_8bits (0x05,0x05,10))
return 0;//Erro no comando de escrita.   

WriteSPI(0xFF); //Envia 8 pulsos de clock.
}

 WriteSPI(0xFD); // Fim do bloco

ENABLE_SD = 1; //Desabilita o SD Card

 WriteSPI (0xFF); //Envia 8 pulsos de clock para finalizar a operação.

//imprime_string_RS232("FIM - CMD25\n"); //Imprime a string.
//---------------------------------------------------------------------FIM DO COMANDO CMD25-----------------------------------------------------------------

return 1;//Escrita realizada.
}

//Lê um bloco de dados.
unsigned char le_bloco(unsigned long bloco, unsigned char *pont_dados)
{
bloco = bloco_SD(bloco); //Obtém o endereço do bloco.

//------------------------------------------------------------------INÍCIO DO COMANDO CMD17----------------------------------------------------------------
//imprime_string_RS232("INÍCIO - CMD17\n"); //Imprime a string.

ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando CMD17: Lê um bloco de dados.       
if(!envia_comando ( CMD17, bloco>>24, bloco>>16, bloco>>8, bloco, 0xFF, 0x00, 10 )) 
return 0;//Erro no comando de leitura.
  
if(!resposta_8bits (0xFE,0xFF,100)) //Aguarda o sinal de início do bloco.
return 0;//Erro no comando de leitura.   

for( cont_sd =0 ; cont_sd < TAMANHO_BLOCO ; cont_sd++)
{
//Esta instrução deve ser modificada pelo usuário, de acordo com a sua necessidade.
//imprime_char_RS232(getcSPI ( )); //Envia o dado contido no SD Card para a RS-232.    DESTACAR     
*pont_dados = getcSPI ( );
pont_dados++;  

}

WriteSPI(0xFF);  //CRC.
WriteSPI(0xFF);  //CRC.

ENABLE_SD = 1; //Desabilita o SD Card

 WriteSPI (0xFF); //Envia 8 pulsos de clock para finalizar a operação.

//imprime_string_RS232("FIM - CMD17\n"); //Imprime a string.
//-----------------------------------------------------------------FIM DO COMANDO CMD17---------------------------------------------------------------------

return 1;//Leitura realizada.
} 

//Lê uma sequência de blocos de dados.
unsigned char le_seq_bloco(unsigned long bloco, unsigned long quanti_blocos)
{
unsigned long verificador = 1;

bloco = bloco_SD(bloco); //Obtém o endereço do bloco.

//--------------------------------------------------------------INÍCIO DO COMANDO CMD18--------------------------------------------------------------------
//imprime_string_RS232("INÍCIO - CMD18\n"); //Imprime a string.

ENABLE_SD = 0; //Habilita o SD Card.

//Envia o comando CMD18: Lê uma sequência de blocos de dados.       
if(!envia_comando ( CMD18, bloco>>24, bloco>>16, bloco>>8, bloco, 0xFF, 0x00, 10 )) 
return 0;//Erro no comando de leitura.

if(!resposta_8bits (0xFE,0xFF,100)) //Aguarda o sinal de início do bloco.
return 0;//Erro no comando de leitura.   
    
for( cont_sd = 1 ; cont_sd <= (TAMANHO_BLOCO*quanti_blocos) ; cont_sd++ )
{
//Esta instrução deve ser modificada pelo usuário, de acordo com a sua necessidade.
imprime_char_RS232(getcSPI ( )); //Envia pela comunicação RS232, o dado contido no bloco.  DESTACAR

// Leitura de 5 caracteres antes de continuar a leitura do proximo bloco, caso contrário ocorre erro.
if( cont_sd == TAMANHO_BLOCO*verificador )
{
ReadSPI();  //CRC.
ReadSPI();  //CRC.   

if(!resposta_8bits (0xFE,0xFF,100)) //Aguarda o sinal de início do bloco.
return 0;//Erro no comando de escrita.   

verificador++;    
}     
}
//Envia o comando CMD12: Interrompe a leitura sequencial.
if(!envia_comando ( CMD12, bloco>>24, bloco>>16, bloco>>8, bloco, 0xFF, 0x00, 10 ))
return 0;//Erro no comando.

ENABLE_SD = 1; //Desabilita o SD Card

 WriteSPI (0xFF); //Envia 8 pulsos de clock para finalizar a operação.

//imprime_string_RS232("FIM - CMD18\n"); //Imprime a string.
//----------------------------FIM DO COMANDO CMD18------------------------------
return 1;//Leitura realizada.
}

