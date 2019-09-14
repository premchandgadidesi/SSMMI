#include <AT89X52.H>
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#define LCD_DATA			P1
//#define DAC_DATA			P0	
#define		KEYBOARD    	P2
#define		LOW				0

#define		HIGH			1
#define		PHYSICALADDRESS_WRITE	0xA0
#define		ACK				0	
#define		NOOFCHARACTERS	0x10
#define		PAGE1			0x20
#define		PAGE2			0x30
#define		PAGE3			0x40
#define		PAGE4			0x50
#define		PAGE5			0x60


#define  	      MAX_COLUMNS									20
#define		UNDER_LINE_CURSOR_ON						74
#define		UNDER_LINE_CURSOR_OFF						75
#define	 	CLEAR_DISPLAY								88
#define	 	BLINKING_CURSOR_ON							83
#define	 	BLINKING_CURSOR_OFF							84

#define	 	SET_CURSOR_HOME								72
#define	 	SET_CURSOR_POSITION							71
#define		SET_CONTRAST								80
#define		SET_BACKLIGHT_BRIGHTNESS					153
#define		GENERALPURPOSE_OUTPUT_ON					87
#define		GENERALPURPOSE_OUTPUT_OFF					86

#define		BACKLIGHT_ON								66
#define		BACKLIGHT_OFF								70
#define		CURSOR_LEFT									76
#define		CURSOR_RIGHT								77
#define		LOAD_STARTUP_SCREEN							64
#define		READ_VERSION_NO								54
#define		SET_CONTRAST_AND_SAVE 						145


#define  	TAG_BYTE									01
#define	 	COLUMN_BYTE									02
#define  	ROW_BYTE									03
#define  	NO_BYTE										00		
#define		CONTRAST_BYTE								04
#define		GENERAL_PURPOSE_OUTPUT_ON_SELECTION			05

#define		GENERAL_PURPOSE_OUTPUT_OFF_SELECTION		06
#define		BACK_LIGHT_ON_DURATION_MINUTES				07
#define		LOAD_STARTUP_SCREEN_BIT						11
#define		READ_VERSION_NUMBER							12
#define		SET_CONTRAST_AND_SAVE_VALUE			   		13

//lcd commands
#define		LCD_CLEAR_DISPLAY							0X01
#define		LCD_CURSOR_ON								0x0E
#define		LCD_CURSOR_OFF								0x0C
#define		LCD_BLINKING_ON								0x0F
#define		LCD_BLINKING_OFF							0x0C
#define		LCD_CURSOR_HOME								0x02

#define		LCD_CURSOR_LEFT								0x10
#define		LCD_CURSOR_RIGHT							0x14


#define     BUFFERSIZE									0X62	  

//*********************************************************************
//FUNCTION PROTOTYPES DECLARATIONS

//*********************************************************************
void SignOnMessage();
ScanKey();
void delay(unsigned int);
void UartInit(void);
void UartIsr(void);
void LcdInit();
void LCDInstWrite(unsigned char instruction);

void LCDDataWrite(unsigned char character);
void LCDGotoXY(unsigned char X, unsigned char Y);
void LCDPutText(char *datastr);
LCDPutChar(char datachar);
void LCDWriteLine(unsigned char LineNo, char *datastr, unsigned char

 Alignment);
void LCDClearLine(unsigned char LineNo);
void LCDCursorON();
void LCDCursorOFF();
void LCDClearLine(unsigned char LineNo);
void CheckBuffer();
void DecodeTagKey();
void DatalatchGPO();

void DatalatchDAC();
void BigDelay(unsigned char);
void I2cstart ();
void Send(unsigned char Write);
void I2cwrite(unsigned char);
void I2cstop();
void I2cReadData();
void I2cack(void);
void I2cwritepage();

void I2cReadPage();
/***********************************************************************************************************/

sbit	ROW1			=	P2^0;                                      
sbit	ROW2   			=	P2^1;

sbit	ROW3			=	P2^2;
sbit    ROW4			=	P2^3;
sbit    COLUMN1			=	P2^4;
sbit    COLUMN2			=	P2^5;
sbit    COLUMN3			=	P2^6;
//P2.7 IS NO CONNECTION
//P3.0 AND P3.1  FOR RXD AND TXD
sbit    RS				=   P3^2;

sbit    E				=   P3^3;
sbit	SCL				=	P2^7;
sbit    RELAY_CONTROL	=   P3^4;
sbit    LATCHENABLE_DAC	=	P3^6;
sbit    SDA				=	P3^5;
sbit    LATCHENABLE_GPO	=	P3^7;
bit		Address;
bit		RowPosition	=	0;

bit		ColumPosition	=	0;
bit		Temp1;
bit		STARTUP_SCREEN	=	HIGH;
bit		First;	

/****************************************************************************
                     GLOBAL VARIABLE DECLARATION

****************************************************************************/


unsigned char	LCD_CurrentX=1;
unsigned char	LCD_CurrentY=1;
unsigned char idata CurrentData[BUFFERSIZE];
unsigned char	Buffer=0x00; 

unsigned char	Received=0x00;
unsigned int	NoOfElements;
unsigned char	NextByte;
unsigned char	Column;
unsigned char	Row;
unsigned int	Counter	=	0;
unsigned int 	DelayTime;
unsigned int	OneMinute = 0x04B0;

unsigned int	InternalCounter	=	0;
unsigned char 	Gpo_data;
unsigned char   Temp=0x00;
unsigned char   Temp2;
unsigned char 	Characters	=	0x00;





/****************************************************************************

                LCD INTIALISATIONS
*****************************************************************************/
void LcdInit()
{
	
	LCDInstWrite(0X30);
	delay(0xff);
	LCDInstWrite(0x30);
	delay(0xff);

	LCDInstWrite(0x30);
	delay(0xff);
	LCDInstWrite(0x38);
	LCDInstWrite(0x0C);
	LCDInstWrite(0x01);
	LCDInstWrite(0x06);  // Auto increment cursor position
}

/***************************************************************************************************

								LCD INSTRUCTION WRITE
****************************************************************************************************/

void LCDInstWrite(unsigned char instruction)
{
	 
	RS = 0;
	E  = 0;

	LCD_DATA = instruction;
	E  = 1;
	_nop_();
	_nop_();
	_nop_();
	E  = 0;
	delay(0xff);
}

/***************************************************************************************************

								LCD CHARACTER DISPLAY
****************************************************************************************************/

void LCDDataWrite(unsigned char character)
{
	RS = 0;
	E  = 0;
	LCD_DATA = character;

	E  = 1;
	RS = 1;
	_nop_();
	_nop_();
	_nop_();
	E  = 0;
	delay(0x1ff);
}

void LCDGotoXY(unsigned char X, unsigned char Y)
{
	
	if(X < 1 || X > MAX_COLUMNS)
		return;
	switch(Y)

	{
	case 1:
		LCDInstWrite(0x80+(X-1));
		break;
	case 2:
		LCDInstWrite(0xC0+(X-1));
		break;
	case 3:
		LCDInstWrite(0x94+(X-1));
		break;
	case 4:
		LCDInstWrite(0xD4+(X-1));
		break;

	default:
		return;
	}
	LCD_CurrentX=X;
   	LCD_CurrentY=Y;
}


void LCDPutText(char *datastr)
{
	unsigned char i;
	for(i=0; i<strlen(datastr);i++)
	{
   		LCD_CurrentX++;
   		LCDDataWrite(datastr[i]);

	}
}


LCDPutChar(unsigned char datachar)
{
	
	LCD_CurrentX++;
   	LCDDataWrite(datachar);
}


void LCDWriteLine(unsigned char LineNo, char *datastr, unsigned char
 Alignment)
{

	LCDGotoXY(1,LineNo);

	if (Alignment == 0) 		// Left
	{
		LCDPutText(datastr);
	}

	if (Alignment == 1)   // Right 
	{
		LCDGotoXY(20-strlen(datastr)+1,LineNo);
		LCDPutText(datastr);
    }


	if (Alignment == 2)   // Center
	{
		LCDGotoXY((20-strlen(datastr))/2 +1,LineNo);
		LCDPutText(datastr);
	}
}


//**********************************************************************************

// DELAY SUBROUTINE STARTS HERE 
//*******************************************************************************
void delay(unsigned int  count)
{
	while((count--)!= 0);
}


void SignOnMessage()

{
LCDWriteLine(1,"********************",2);
LCDWriteLine(2,"TUX DIGITAL RECORDER",2);
LCDWriteLine(3,"R08I",2);
LCDWriteLine(4,"********************",2);
}



/*******************************************************************/

ScanKey()
{
/********ROW IS ZERO
 **************************************************************/
	
		ROW1	=	0;
		ROW2	=	1;

		ROW3	=	1;
		ROW4    =   1;
		if(COLUMN1	==	0)
		{
			delay(0x01ff);
			while(COLUMN1	!=	1);
				SBUF	=	'1';				
				TI	=	1;
				return 0;
		}
		if(COLUMN2	==	0)
		{
			delay(0x01ff);

			while(COLUMN2	!=	1);
			SBUF	=	'2';
			TI	=	1;
			return 0;
		}
		if(COLUMN3	==	0)
		{
			delay(0x01ff);
			while(COLUMN3	!=	1);
			SBUF	=	'3';
			TI	=	1;
			return 0;

		}

/****************************************************************************	
        ROW ONE SELECTION
****************************************************************************/
		ROW1	=	1;
		ROW2	=	0;

		ROW3	=	1;
		ROW4    =   1;
		if(COLUMN1	==	0)
		{
			delay(0x01ff);
			while(COLUMN1	!=	1);
		
				SBUF	=	'4';
				TI	=	1;
				return 0;
		}
		if(COLUMN2	==	0)
		{
			delay(0x01ff);

			while(COLUMN2	!=	1);
			
			SBUF	=	'5';
			TI	=	1;
			return 0;
		}
		if(COLUMN3	==	0)
		{
			delay(0x01ff);
			while(COLUMN3	!=	1);
			
			SBUF	=	'6';
			TI	=	1;

			return 0;
		}
/****************************************************************************	
        ROW TWO SELECTION
****************************************************************************/
		ROW1	=	1;

		ROW2	=	1;
		ROW3	=	0;
		ROW4    =   1;
		if(COLUMN1	==	0)
		{
			delay(0x01ff);
			while(COLUMN1	!=	1);
			SBUF	=	'7';
			TI	=	1;
			return 0;
		}
		if(COLUMN2	==	0)
		{
			delay(0x01ff);

			while(COLUMN2	!=	1);
			SBUF	=	'8';
			TI	=	1;
			return 0;
		}
		if(COLUMN3	==	0)
		{
			delay(0x01ff);
			while(COLUMN3	!=	1);
			SBUF	=	'9';
			TI	=	1;
			return 0;

		}

/****************************************************************************	
        ROW THREE SELECTION
****************************************************************************/
		ROW1	=	1;
		ROW2	=	1;

		ROW3	=	1;
		ROW4    =   0;
		if(COLUMN1	==	0)
		{
			delay(0x01ff);
			while(COLUMN1	!=	1);
			SBUF	=	'A';
			TI	=	1;
			return 0;
		}
		if(COLUMN2	==	0)
		{
			delay(0x01ff);

			while(COLUMN2	!=	1);
			SBUF	=	'B';
			TI	=	1;
			return 0;
		}
		if(COLUMN3	==	0)
		{
			delay(0x01ff);
			while(COLUMN3	!=	1);
			SBUF	=	'C';
			TI	=	1;
			return 0;

		}
}

//
 *******************************************************************************
// UART INTIALISATION STARTS HERE
//********************************************************************************

void UartInit()
{
	TCON	=	0X00;		// 0000 0000
	SCON	=	0X50;		// MODE1 FOR SERIAL TRANSMISSION 
	TMOD	=	0X21;		// mode 2 ( 8 bit Auto Reload),TIMER0 IN MODE O
 OPERATION
	PCON	=	0X80;       //  19200 bps

	TH1		=	0XFD;		//	
	TL1		=	0XFD;		
	TH0		=	0X4C;       //0.05 second delay
	TL0		=	0X00;
	TR1		=	1;	
	ET0		=	1;
	ES		=	1;	
	EA		=	1;	
}

//********************************************************************************

// UART INTERRUPT LOCATION STARTS HERE
//*****************************************************************************
void UartIsr() interrupt 4 
{
	if(TI	!=1)
	{
		RI		=	0;
		CurrentData[Buffer] = SBUF;

		Buffer++;
		NoOfElements++;
		if(Buffer == BUFFERSIZE)
		{
			Buffer = 0x00;
		}
	}
	else
	{
		TI	=	0;
		
	}						
	
}

/*****************************************************************************************************/


main()
{
	UartInit();
	LcdInit();
	RELAY_CONTROL = 0;		//TO SET BACKLIGHT Off BY DEFAULT CONDITION
	LATCHENABLE_GPO	=	0;
    P0	=	0x00;
	DatalatchGPO();
	SignOnMessage();
	LATCHENABLE_DAC	=	0;

	I2cReadData();
	P0 =	Temp2;
	DatalatchDAC();
	I2cReadPage();
	while(1)
	{
		ScanKey();
		if(NoOfElements != 0)
		{
			if(CurrentData[Received] == 0xfe)
			{
				CheckBuffer();
				Address	=	1;

				NextByte	= TAG_BYTE;
				continue;
			}
			if(Address	==	0)
			{
			  if(STARTUP_SCREEN	==	LOW)
			  {
	    		//LCDPutChar(CurrentData[Received]);
				I2cwritepage();
				//CheckBuffer();

				continue;
		   	  }

			  else
			  {
			    LCDPutChar(CurrentData[Received]);
				CheckBuffer();
				STARTUP_SCREEN	=	HIGH;
				continue;
			  }
			}
			else
			if(Address	==	1)

			{
				switch(NextByte)
			 	{
				case TAG_BYTE:
					DecodeTagKey();
					break;
				case COLUMN_BYTE:
					Column = CurrentData[Received];
					NextByte = ROW_BYTE;
					CheckBuffer();
					break;

				case ROW_BYTE:
					Row = CurrentData[Received];
					LCDGotoXY(Column, Row);
					CheckBuffer();
					NextByte = NO_BYTE;
					Address = 0;
					break;
			   case CONTRAST_BYTE:
			   		
				 	P0	= CurrentData[Received]; 

					DatalatchDAC();
					CheckBuffer();
					Address	=	0;
					
					break;
 				case SET_CONTRAST_AND_SAVE_VALUE:
					P0= CurrentData[Received];
					DatalatchDAC();
					I2cwrite(0x00);
					CheckBuffer();

					Address	=	0;
					
					break;
               case BACK_LIGHT_ON_DURATION_MINUTES:
					DelayTime	=	CurrentData[Received];
					RELAY_CONTROL = 1;
					CheckBuffer();
					if(DelayTime	>	0)

					{
						TR0	=	1;
					}
					
					Address	=	0;
					break;
				case READ_VERSION_NUMBER:
					LCDDataWrite(CurrentData[Received]);
					CheckBuffer();
					Address	=	0;
					break;
				case GENERAL_PURPOSE_OUTPUT_ON_SELECTION:

					Gpo_data	=CurrentData[Received];
					CheckBuffer();
					Address	=	0;
				switch(Gpo_data)
				{
					case 1:
						P0 = Temp | 0x01;
						Temp  = Temp | 0x01;
						DatalatchGPO();						

						break;
					case 2:
						P0 = Temp | 0x02;
						Temp  = Temp | 0x02;
						DatalatchGPO();						
						break;
					case 3:
						P0 = Temp | 0x04;
						Temp  = Temp | 0x04;
						DatalatchGPO();						

						break;
					case 4:
						P0 = Temp | 0x08;
						Temp  = Temp | 0x08;
						DatalatchGPO();						
						break;
					case 5:
						P0 = Temp | 0x10;
						Temp  = Temp | 0x10;
						DatalatchGPO();						

						break;
					case 6:
						P0 = Temp | 0x20;
						Temp  = Temp | 0x20;
						DatalatchGPO();						
						break;
					case 7:
						P0 = Temp | 0x40;
						Temp  = Temp | 0x40;
						DatalatchGPO();						

						break;
					case 8:
						P0 = Temp | 0x80;
						Temp  = Temp | 0x80;
						DatalatchGPO();						
						break;
				}	
				break;
				case GENERAL_PURPOSE_OUTPUT_OFF_SELECTION:
					Gpo_data	=CurrentData[Received];

					CheckBuffer();
					Address	=	0;
					
					switch(Gpo_data)
					{
						case 1:
							P0 = Temp & 0xfe;
							Temp  = Temp & 0xfe;
							DatalatchGPO();
						break;
						case 2:

							P0= Temp & 0xfd;
							Temp  = Temp & 0xfd;
							DatalatchGPO();
						break;
						case 3:
							P0= Temp & 0xfb;
							Temp  = Temp & 0xfb;
							DatalatchGPO();
						break;

						case 4:
							P0	= Temp & 0xf7;
							Temp  = Temp & 0xf7;
							DatalatchGPO();
						break;
						case 5:
							P0	= Temp & 0xef;
							Temp  = Temp & 0xef;
							DatalatchGPO();

						break;
						case 6:
							P0	= Temp & 0xdf;
							Temp  = Temp & 0xdf;
							DatalatchGPO();
						break;
						case 7:
							P0 = Temp & 0xbf;
							Temp  = Temp & 0xbf;

							DatalatchGPO();
						break;
						case 8:
							P0	= Temp & 0x7f;
							Temp  = Temp & 0x7f;
							DatalatchGPO();
						break;

					}
				break;					
				
				 default:

				 	Address	=	0;
	
				} //End of Switch
			}	// IF - Address == 0
		} //NoOfElements IF
	}//while
}//main



void DecodeTagKey()
{
	switch (CurrentData[Received])
	{
/* FE 4A	*/

	case UNDER_LINE_CURSOR_ON:  
   	 	LCDInstWrite(LCD_CURSOR_ON);	   //CURSOR ON
		Address	=	0;
		break;
/* FE 4B	*/
	case UNDER_LINE_CURSOR_OFF:
		LCDInstWrite(LCD_CURSOR_OFF);	   //CURSOR Off	
		Address	=	0;

		break;
/* FE 58	*/
	case CLEAR_DISPLAY:
		LCDInstWrite(LCD_CLEAR_DISPLAY);
		Address	=	0;
		break;
/* FE 42 <Minutes>	*/
 	case BLINKING_CURSOR_ON:
		LCDInstWrite(LCD_BLINKING_ON);
		Address = 0;	

		break;
/* FE 46	*/
	case BLINKING_CURSOR_OFF:
		LCDInstWrite(LCD_BLINKING_OFF);
		Address	=	0;
		break;
/*	FE 48	*/
	case SET_CURSOR_HOME:
		LCDInstWrite(LCD_CURSOR_HOME);
		Address = 0;
		break;

/* FE 47 <Colunmn><Row>	*/
	case SET_CURSOR_POSITION:
		NextByte = COLUMN_BYTE;
		break;
/* FE 50 <Contrast>	*/
	 case SET_CONTRAST:
	 	NextByte = CONTRAST_BYTE;
		LATCHENABLE_DAC	=	0;

		break;
	 case BACKLIGHT_ON:
		NextByte = BACK_LIGHT_ON_DURATION_MINUTES;
		break;
	 case BACKLIGHT_OFF:
	 	RELAY_CONTROL = 0;
		TR0	=	0;
		TH0		=	0X4C;       //0.05 second delay
		TL0		=	0X00;

		Address = 0;
		break;
    case CURSOR_LEFT:
    	LCDInstWrite(LCD_CURSOR_LEFT);
		Address = 0;
		break;
	case CURSOR_RIGHT:
    	LCDInstWrite(LCD_CURSOR_RIGHT);
		Address = 0;
		break;
	case GENERALPURPOSE_OUTPUT_ON:

		NextByte = GENERAL_PURPOSE_OUTPUT_ON_SELECTION;
		LATCHENABLE_GPO	=	0;
		break;
	case GENERALPURPOSE_OUTPUT_OFF:
		NextByte = GENERAL_PURPOSE_OUTPUT_OFF_SELECTION;
		LATCHENABLE_GPO	=	0;
		break;
	case LOAD_STARTUP_SCREEN:

		STARTUP_SCREEN	= LOW;
		Address	=	0;
		//NextByte = LOAD_STARTUP_SCREEN_BIT;
		break;
	case READ_VERSION_NO:
		NextByte = READ_VERSION_NUMBER;
		break;
	case SET_CONTRAST_AND_SAVE:
    	NextByte=SET_CONTRAST_AND_SAVE_VALUE;

		break;
	default:
		Address	=	0;
		break;
	
	}//switch
	CheckBuffer();		
}

void CheckBuffer()
{
	Received++;
	NoOfElements--;
	if(Received == BUFFERSIZE)
	{
		Received = 0x00;

	}
}


void Timer0Isr() interrupt 1
{
	TF0		=	0;
	Counter++;
	if(Counter	>= OneMinute)
	{
		InternalCounter++;
		Counter	=	0;
		if(InternalCounter >= DelayTime)
		{
           	RELAY_CONTROL = 0;

			InternalCounter = 0;
			TR0		=	0;
		}
	}
	TH0		=	0X4C;       //0.05 second delay
	TL0		=	0X00;
}

void DatalatchGPO()
{
	LATCHENABLE_GPO	=	1;
	BigDelay(0x40);
	I2cwrite(0x60);	
	LATCHENABLE_GPO	=	0;

}

void BigDelay(unsigned char count)
{
while((count--)!=0);
}

void DatalatchDAC()
{
	LATCHENABLE_DAC	=	1;
	BigDelay(0x40);
	LATCHENABLE_DAC	=	0;
}


//i2c protocol program starts here



void I2cstart(void)
{
	SCL	=	LOW;
	SDA	=	HIGH;
	SCL	=	HIGH;
	SDA	=	LOW;
	SCL	=	LOW;
}
void Send(unsigned char Write)
{	unsigned char  i;	

	for(i=0; i<8; i++)
   {
      if(Write&0x80)

      {
         SDA	=	HIGH;
      }
      else
      {
         SDA	=	LOW;
      }
      SCL	=	HIGH;
	  SCL	=	LOW;
	  Write = Write << 1;
   }
	  SDA	=	HIGH;
      SCL	=	HIGH;

	  SCL	=	LOW;

}


void I2cwrite(unsigned char No)
{
	I2cstart ();
	Send(PHYSICALADDRESS_WRITE);
	Send(No);   //device address
	Send(CurrentData[Received]);
	I2cstop();

}



void I2cstop()
{
	SCL	=	LOW;
    SDA	=	LOW;
    SCL	=	HIGH;
    SDA =	HIGH;
}


I2cReadbyte(void)
{
   unsigned char  i_byte=0x00, n;
   SDA	=	HIGH;
   for (n=0; n<8; n++)
   {

   	SCL	=	HIGH;
      if (SDA)
      {
         i_byte = (i_byte << 1) | 0x01; // msbit first
      }
      else
      {
         i_byte = i_byte << 1;
      }
      SCL	=	LOW;
   }

   return(i_byte);
}
// read result
void I2cReadData()  // Random read operation
{
	I2cstart();
	Send(0xA0);//PHYSICAL ADDRESS AND WRITE OPERATION
	Send(0x00);
	I2cstart();
	Send(0xA1);// READ OPERATION

   	Temp2=I2cReadbyte();  // throw the first away
    I2cstop();
}

void I2cack(void)
{
	SDA	=	LOW;
	SCL	=	HIGH;
	SCL	=	LOW;
}

void I2cwritepage()
{
	unsigned char PageNo= 0x10;

	while(PageNo	<=	0x50)
	{
		I2cstart ();
		Send(0xa0);  //10100010
		Send(PageNo);   //device address
		for(Characters=0;Characters<NOOFCHARACTERS;Characters++)
		{
		Send(CurrentData[Received]);

		LCDPutChar(CurrentData[Received]);
		PageNo++;
		CheckBuffer();
		}
		I2cstop();
		BigDelay(500);
	}
   	
}


void I2cReadPage()  // Random read operation
{
	unsigned char ReadpageNo	=	0x10;

	while(ReadpageNo <=	0x50)
	{
		I2cstart();
		Send(0xA0);//PHYSICAL ADDRESS AND WRITE OPERATION
		Send(ReadpageNo);
		I2cstart();
		Send(0xA1);// READ OPERATION page 
			for(Characters=0;Characters<NOOFCHARACTERS-1;Characters++)

			{
   			Temp2=I2cReadbyte();  // throw the first away
			LCDPutChar(Temp2);
			ReadpageNo++;
			I2cack();
			}
		Temp2	=	I2cReadbyte();
		LCDPutChar(Temp2);
		I2cstop();
	}
	
}
