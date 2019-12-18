#include "EUSART.h"

/*
 @brief: initializes the EUSART module
 */
char EUSART_Initialize(const long int baudrate)
{
 unsigned int x;
	x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);
	if(x>255)
	{
		x = (_XTAL_FREQ - baudrate*16)/(baudrate*16);
		BRGH = 1;
	}
	if(x<256)
	{
	  SPBRG = x;
	  SYNC = 0;
	  SPEN = 1;
          TRISC7 = 1;
          TRISC6 = 1;
          CREN = 1;
          TXEN = 1;
	  return 1;
	}
	return 0;

}

/*
 @brief: Reads the EUSART module
 */
uint8_t EUSART_Read(void)
{

   RC1STAbits.SREN = 1;
    while(!PIR1bits.RCIF)
    {
    }

    
    if(1 == RC1STAbits.OERR)
    {
        // EUSART error - restart

        RC1STAbits.SPEN = 0; 
        RC1STAbits.SPEN = 1; 
    }

    return RC1REG;
}

/*
 @brief: Writes to the EUSART module
 */
void EUSART_Write(uint8_t txData)
{
    while(0 == PIR1bits.TXIF)
    {
    }

    TX1REG = txData;    // Write the data byte to the USART.
}

void EUSART_Read_Text(char *Output, unsigned int length)
{
	int i;
	for(int i=0;i<length;i++)
		Output[i] = EUSART_Read();
}

/*
 @brief: Writes text the EUSART module
 */
void EUSART_Write_Text(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
	  EUSART_Write(text[i]);
}