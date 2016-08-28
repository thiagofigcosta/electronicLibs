// Fosc = 20MHz
// Tciclo = 4/Fosc = 0,2us
#pragma config FOSC = HS
#pragma config CPUDIV = OSC1_PLL2

#pragma config WDT = OFF 	//Desabilita o Watchdog Timer (WDT).
#pragma config PWRT = ON	//Habilita o Power-up Timer (PWRT).
#pragma config BOR = ON	    //Brown-out Reset (BOR) habilitado somente no hardware.
#pragma config BORV = 1 	//Voltagem do BOR é 4,33V.
#pragma config PBADEN = OFF	//RB0,1,2,3 e 4 configurado como I/O digital.
#pragma config LVP = OFF	//Desabilita o Low Voltage Program.

