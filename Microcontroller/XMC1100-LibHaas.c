// Bibliothek:       XMC1100-LibHaas.c Vers 0.82 vom 19.09.2021
// Controller:       XMC1100 Board XMC1100 Boot Kit
// Adapterboard:	 XMC-Learnboard Version0_9
// Ausgangsversion:  Version 1.8 Birk, Bubbers
// Autor			 Haas
#include <xmc1100-LibHaas.h>

// Global Variablen
// RS232
char rs232Receivedata[256];
uint8_t rs232ReceivedataPointer=0;
uint8_t rs232EnterReceive=0;
// UART
char uartReceivedata[256];
uint8_t uartReceivedataPointer=0;
uint8_t uartEnterReceive=0;

// Definition der lokalen Funktionen:
uint16_t bit_set_clr(uint16_t wert, uint16_t set ,uint16_t bitnr);
void clock_init(void);

//***************************************************************
// ab hier Funktionen für Port-Ein-/Ausgabe
//***************************************************************

// --------------------------------------------------------------
// Einzelnes Bit für Ein- oder Ausgabe initialisieren
// port: P0,P1,P2 bitnr: 0..15 direction: INP 0, OUTP 1
//---------------------------------------------------------------
void bit_init(uint8_t port, uint8_t bitnr, uint8_t direction)
{
	switch (port)
	{
		case 0 :
			{if (bitnr < 4 )
				{ if (direction == 1) PORT0->IOCR0 |=1UL << ((bitnr * 8)+7);
				  else PORT0->IOCR0 &= ~(1UL << ((bitnr * 8 ) + 7 ));
				}
			 if ( (bitnr <8) && (bitnr > 3))
					 { if (direction == 1) PORT0->IOCR4 |= 1UL<< (((bitnr-4) * 8)+7);
					   else PORT0->IOCR4 &= ~(1UL << (((bitnr-4) * 8)+7) );
					 }
			 if ( (bitnr <12) && (bitnr > 7))
			 		 { if (direction == 1) PORT0->IOCR8 |= 1UL << (((bitnr-8) * 8)+7);
			 		   else PORT0->IOCR8 &= ~(1UL << (((bitnr-8) * 8)+7) );
			 		 }
			 if ( (bitnr <16) && (bitnr > 11))
			 		{ if (direction == 1) PORT0->IOCR12 |= 1UL << (((bitnr-12) * 8)+7);
			 		  else PORT0->IOCR12 &= ~(1UL << (((bitnr-12) * 8)+7) );
			 		}
			}
			break;
	  case 1 :
			{
				{if (bitnr < 4 )
					{ if (direction == 1) PORT1->IOCR0 |=1UL << ((bitnr * 8)+7);
						else PORT1->IOCR0 &= ~(1UL << ((bitnr * 8 ) + 7 ));
					}
			if ((bitnr < 8) && (bitnr > 3)) {
				if (direction == 1) PORT1->IOCR4 |= 1UL << (((bitnr - 4) * 8) + 7);
				else
					PORT1->IOCR4 &= ~(1UL << (((bitnr - 4) * 8) + 7));
			}

				}
			}
			break;

	  case 2 : //Achtung!! Nur Pins P2.0, P2.1, P2.10 und P2.11 sind als Out Pin einstellbar
	  	{
	  		if (bitnr < 2 ) {
	  			CLR_BIT(PORT2->PDISC,bitnr); // digital Pad aktiv
	  			if (direction == 1) PORT2->IOCR0 |=1UL << ((bitnr * 8)+7);
					else PORT2->IOCR0 &= ~(1UL << ((bitnr * 8 ) + 7 ));
	  		}
				if ((bitnr > 1)&&(bitnr<4)) {
					CLR_BIT(PORT2->PDISC,bitnr); // digital Pad aktiv
					PORT2->IOCR0 &= ~(1UL << ((bitnr * 8) + 7));
				}
	  		if ((bitnr < 8) && (bitnr > 3)) {
	  			    CLR_BIT(PORT2->PDISC,bitnr); // digital Pad aktiv
					PORT2->IOCR4 &= ~(1UL << (((bitnr-4) * 8 ) + 7 ));
				}
	  		if ((bitnr < 10) && (bitnr > 7)) {
	  			PORT2->IOCR8 &= ~(1UL << (((bitnr-8) * 8 ) + 7 ));
	  		}
	  		if (bitnr > 9 ) {
	  			CLR_BIT(PORT2->PDISC,bitnr); // digital Pad aktiv
	  			if (direction == 1) PORT2->IOCR8 |=1UL << (((bitnr-8) * 8)+7);
	  			else PORT2->IOCR8 &= ~(1UL << (((bitnr-8) * 8 ) + 7 ));
	  		}
  			break;
	  	}
	  }
}

/* --------------------------------------------------------------
* Einzelnes Bit für Ein- oder Ausgabe initialisieren
* port: P0,P1,P2 bitnr: 0..15 direction: INP 0, OUTP 1
* mode: NO_PULL PULLUP PULLDOWN
*/
void bit_init_mode (uint8_t port, uint8_t bitnr, uint8_t direction, uint8_t mode)
{
	switch (port)
	{
		case 0 :
		{
			if (bitnr < 4 ) {
				if (direction == 1) {
					PORT0->IOCR0 |=1UL << ((bitnr * 8)+7);
					PORT0->IOCR0 &= ~(0x0000000F << ((bitnr * 8)+3));
				}
				else {
					PORT0->IOCR0 &= ~(1UL << ((bitnr * 8 ) + 7 ));
					if (mode == PULLUP) {
						PORT0->IOCR0 &= ~(0x0000001F << ((bitnr * 8)+3) );
						PORT0->IOCR0 |= (1UL << ((bitnr * 8)+4) );
					} else if (mode == PULLDOWN) {
						PORT0->IOCR0 &= ~(0x0000001F << ((bitnr * 8)+3) );
						PORT0->IOCR0 |= (1UL << ((bitnr * 8)+3) );
					}
				}
			}
			if ( (bitnr <8) && (bitnr > 3)) {
				if (direction == 1) {
					PORT0->IOCR4 |= 1UL<< (((bitnr-4) * 8)+7);
					PORT0->IOCR4 &= ~(0x0000000F << (((bitnr-4) * 8)+3));
				}
				else {
					PORT0->IOCR4 &= ~(1UL << (((bitnr-4) * 8)+7) );
					if (mode == PULLUP) {
						PORT0->IOCR4 &= ~(0x0000001F << (((bitnr-4) * 8)+3) );
						PORT0->IOCR4 |= (1UL << (((bitnr-4) * 8)+4) );
					}else if (mode == PULLDOWN) {
						PORT0->IOCR4 &= ~(0x0000001F << ((bitnr * 8)+3) );
						PORT0->IOCR4 |= (1UL << ((bitnr * 8)+3) );
					}
				}
			}
			if ( (bitnr <12) && (bitnr > 7)) {
				if (direction == 1) {
					PORT0->IOCR8 |= 1UL << (((bitnr-8) * 8)+7);
					PORT0->IOCR8 &= ~(0x0000000F << (((bitnr-8) * 8)+3));
				}
				else {
					PORT0->IOCR8 &= ~(1UL << (((bitnr-8) * 8)+7) );
					if (mode == PULLUP) {
						PORT0->IOCR8 &= ~(0x0000001F << (((bitnr-8) * 8)+3) );
						PORT0->IOCR8 |= (1UL << (((bitnr-8) * 8)+4) );
					} else if (mode == PULLDOWN) {
						PORT0->IOCR8 &= ~(0x0000001F << ((bitnr * 8)+3) );
						PORT0->IOCR8 |= (1UL << ((bitnr * 8)+3) );
					}
				}
			}
			if ( (bitnr <16) && (bitnr > 11)) {
				if (direction == OUTP){
					PORT0->IOCR12 |= 1UL << (((bitnr-12) * 8)+7);
					PORT0->IOCR12 &= ~(0x0000000F << (((bitnr-12) * 8)+3));
				}
				else {
					PORT0->IOCR12 &= ~(1UL << (((bitnr-12) * 8)+7) );
					if (mode == PULLUP) {
						PORT0->IOCR12 &= ~(0x0000001F << (((bitnr-12) * 8)+3) );
						PORT0->IOCR12 |= (1UL << (((bitnr-12) * 8)+4) );
					}else if (mode == PULLDOWN) {
						PORT0->IOCR12 &= ~(0x0000001F << ((bitnr * 8)+3) );
						PORT0->IOCR12 |= (1UL << ((bitnr * 8)+3) );
					}
				}
			}
		}
		break;
		case 1 :
		{
			if (bitnr < 4 ) {
				if (direction == 1) {
					PORT1->IOCR0 |=1UL << ((bitnr * 8)+7);
					PORT1->IOCR0 &= ~(0x0000000F << ((bitnr * 8)+3));
				}
				else {
					PORT1->IOCR0 &= ~(1UL << ((bitnr * 8 ) + 7 ));
					if (mode == PULLUP) {
						PORT1->IOCR0 &= ~(0x0000001F << ((bitnr * 8)+3) );
						PORT1->IOCR0 |= (1UL << ((bitnr * 8)+4) );
					} else if (mode == PULLDOWN) {
						PORT1->IOCR0 &= ~(0x0000001F << ((bitnr * 8)+3) );
						PORT1->IOCR0 |= (1UL << ((bitnr * 8)+3) );
					}
				}
			}
			if ( (bitnr <8) && (bitnr > 3)) {
				if (direction == 1) {
					PORT1->IOCR4 |= 1UL<< (((bitnr-4) * 8)+7);
					PORT1->IOCR4 &= ~(0x0000000F << (((bitnr-4) * 8)+3));
				}
				else {
					PORT1->IOCR4 &= ~(1UL << (((bitnr-4) * 8)+7) );
					if (mode == PULLUP) {
						PORT1->IOCR4 &= ~(0x0000001F << (((bitnr-4) * 8)+3) );
						PORT1->IOCR4 |= (1UL << (((bitnr-4) * 8)+4) );
					}else if (mode == PULLDOWN) {
						PORT1->IOCR4 &= ~(0x0000001F << ((bitnr * 8)+3) );
						PORT1->IOCR4 |= (1UL << ((bitnr * 8)+3) );
					}
				}
			}
		}
		break;

		case 2 :
			if (bitnr == 0) { //P2.0
			 if (direction == OUTP) {
				 CLR_BIT(PORT2->PDISC,0); // digital Pad aktiv
				 PORT2->IOCR0 &= 0xffffff00;
				 PORT2->IOCR0 |= 0x00000080;
			 } else if (direction == INP) {
				 CLR_BIT(PORT2->PDISC,0); // digital Pad aktiv
				 if (mode==PULLUP) {
					 PORT2->IOCR0 &= 0xFFFFFF00;
					 PORT2->IOCR0 |= 0x00000010;
				 } else if (mode==PULLDOWN) {
					 PORT2->IOCR0 &= 0xFFFFFF00;
					 PORT2->IOCR0 |= 0x00000008;
				 } else if (mode == ANALOG) {
					SET_BIT(PORT2->PDISC,0); //Analog Modus aktiv
				 }
			 }
		 }
		 if (bitnr == 1) { //P2.1
			 if (direction == OUTP) {
				 CLR_BIT(PORT2->PDISC,1); // digital Pad aktiv
				 PORT2->IOCR0 &= 0xffff00ff;
				 PORT2->IOCR0 |= 0x00008000;
			 } else if (direction == INP) {
				 CLR_BIT(PORT2->PDISC,1); // digital Pad aktiv
				 if (mode==PULLUP) {
					 PORT2->IOCR0 &= 0xFFFF00ff;
					 PORT2->IOCR0 |= 0x00001000;
				 } else if (mode==PULLDOWN) {
					 PORT2->IOCR0 &= 0xFFFF00ff;
					 PORT2->IOCR0 |= 0x00000800;
				 } else if (mode == ANALOG) {
					SET_BIT(PORT2->PDISC,1); //Analog Modus aktiv
				 }
			 }
		 }
		 if (bitnr == 2) { //P2.2
			 if (direction == OUTP) {
				 // nicht moeglich
			 } else if (direction == INP) {
				 CLR_BIT(PORT2->PDISC,2); // digital Pad aktiv
				 if (mode==PULLUP) {
					 PORT2->IOCR0 &= 0xFF00ffff;
					 PORT2->IOCR0 |= 0x00100000;
				 } else if (mode==PULLDOWN) {
					 PORT2->IOCR0 &= 0xFF00ffff;
					 PORT2->IOCR0 |= 0x00080000;
				 } else if (mode == ANALOG) {
					SET_BIT(PORT2->PDISC,2); //Analog Modus aktiv
				 }
			 }
		 }
		 if (bitnr == 3) { //P2.3
			 if (direction == OUTP) {
				 // nicht moeglich
			 } else if (direction == INP) {
				 CLR_BIT(PORT2->PDISC,3); // digital Pad aktiv
				 if (mode==PULLUP) {
					 PORT2->IOCR0 &= 0x00ffffff;
					 PORT2->IOCR0 |= 0x10000000;
				 } else if (mode==PULLDOWN) {
					 PORT2->IOCR0 &= 0x00ffffff;
					 PORT2->IOCR0 |= 0x08000000;
				 } else if (mode == ANALOG) {
					SET_BIT(PORT2->PDISC,3); //Analog Modus aktiv
				 }
			 }
		 }
		 if ((bitnr > 3) && (bitnr < 8)) { //P2.4 bis P2.7
			 if (direction == OUTP) {
				 // nicht moeglich
			 } else if (direction == INP) {
				 CLR_BIT(PORT2->PDISC,bitnr); // digital Pad aktiv
				 if (mode==PULLUP) {
					 PORT2->IOCR4 &= ~(0xff<<((bitnr-4)*8)); //PORT2->IOCR1 &= 0xffffff00;
					 PORT2->IOCR4 |= 0x10<<((bitnr-4)*8);    //PORT2->IOCR4 |= 0x00000010;
				 } else if (mode==PULLDOWN) {
					 PORT2->IOCR4 &= ~(0xff<<((bitnr-4)*8)); //PORT2->IOCR1 &= 0xffffff00;
					 PORT2->IOCR4 |= 0x08<<((bitnr-4)*8);    //PORT2->IOCR4 |= 0x00000010;
				 } else if (mode == ANALOG) {
					 SET_BIT(PORT2->PDISC,bitnr); //Analog Modus aktiv
				 }
			 }
		 }
		 if (bitnr == 8) { //P2.8
			if (direction == OUTP) {
				CLR_BIT(PORT2->PDISC,8); // digital Pad aktiv
				PORT2->IOCR8 &= 0xffffff00;
				PORT2->IOCR8 |= 0x00000080;
			} else if (direction == INP) {
				CLR_BIT(PORT2->PDISC,8); // digital Pad aktiv
				if (mode==PULLUP) {
					PORT2->IOCR8 &= 0xFFFFFF00;
					PORT2->IOCR8 |= 0x00000010;
				} else if (mode==PULLDOWN) {
					PORT2->IOCR8 &= 0xFFFFFF00;
					PORT2->IOCR8 |= 0x00000008;
				} else if (mode == ANALOG) {
					SET_BIT(PORT2->PDISC,8); //Analog Modus aktiv
				}
			}
		 }
		 if (bitnr == 9) { //P2.9
			if (direction == OUTP) {
				CLR_BIT(PORT2->PDISC,9); // digital Pad aktiv
				PORT2->IOCR8 &= 0xffff00ff;
				PORT2->IOCR8 |= 0x00008000;
			} else if (direction == INP) {
				CLR_BIT(PORT2->PDISC,9); // digital Pad aktiv
				if (mode==PULLUP) {
					PORT2->IOCR8 &= 0xFFFF00FF;
					PORT2->IOCR8 |= 0x00001000;
				} else if (mode==PULLDOWN) {
					PORT2->IOCR8 &= 0xFFFF00FF;
					PORT2->IOCR8 |= 0x00000800;
				} else if (mode == ANALOG) {
					SET_BIT(PORT2->PDISC,9); //Analog Modus aktiv
				}
			}
		 }
		 if (bitnr == 10) { //P2.10
			 if (direction == OUTP) {
				 CLR_BIT(PORT2->PDISC,10); // digital Pad aktiv
				 PORT2->IOCR8 &= 0xff00ffff;
				 PORT2->IOCR8 |= 0x00800000;
			 } else if (direction == INP) {
				 CLR_BIT(PORT2->PDISC,10); // digital Pad aktiv
				 if (mode==PULLUP) {
					 PORT2->IOCR8 &= 0xFF00FFFF;
					 PORT2->IOCR8 |= 0x00100000;
				 } else if (mode==PULLDOWN) {
					 PORT2->IOCR8 &= 0xFF00FFFF;
					 PORT2->IOCR8 |= 0x00080000;
				 } else if (mode == ANALOG) {
					 SET_BIT(PORT2->PDISC,10); //Analog Modus aktiv
				 }
			 }
		 }
		 if (bitnr == 11) { //P2.11
			if (direction == OUTP) {
				CLR_BIT(PORT2->PDISC,11); // digital Pad aktiv
				PORT2->IOCR8 &= 0x00ffffff;
				PORT2->IOCR8 |= 0x80000000;
			} else if (direction == INP) {
				CLR_BIT(PORT2->PDISC,11); // digital Pad aktiv
				if (mode==PULLUP) {
					PORT2->IOCR8 &= 0x00FFFFFF;
					PORT2->IOCR8 |= 0x10000000;
				} else if (mode==PULLDOWN) {
					PORT2->IOCR8 &= 0x00FFFFFF;
					PORT2->IOCR8 |= 0x08000000;
				} else if (mode == ANALOG) {
					SET_BIT(PORT2->PDISC,11); //Analog Modus aktiv
				}
			}
		 }
		 break;
	}
}

uint16_t bit_set_clr(uint16_t wert, uint16_t set ,uint16_t bitnr)
{
	if ( set == 1 ) return  wert |= (0x01 << bitnr);
	else return wert &= ~(0x01 << bitnr);
}

//---------------------------------------------------------------
//Gesamtes Port für Ein- oder Ausgabe initialisieren
// port: P0,P1 direction: INP 0, OUTP 1
//---------------------------------------------------------------
 void port_init(uint8_t port, uint8_t direction)

{
	switch(port)
	{
		case 0:	if (direction == OUTP) {
					PORT0->IOCR0 = 0x80808080;  // auf Ausgang setzen
					PORT0->IOCR4 = 0x80808080;
					PORT0->IOCR8 = 0x80808080;
					PORT0->IOCR12 = 0x80808080;
					}
				else {
					PORT0->IOCR0 = 0x00;
					PORT0->IOCR4 = 0x00;
					PORT0->IOCR8 = 0x00;
					PORT0->IOCR12 = 0x00;
					}
					break;
		case 1:	 if (direction == OUTP) {
					PORT1->IOCR0 = 0x80808080;  // auf Ausgang setzen
					PORT1->IOCR4 = 0x80808080;
								}
				else {
					PORT1->IOCR0 = 0x00;
					PORT1->IOCR4 = 0x00;
			}
				break;

		case 2:	 if (direction == OUTP) {
					PORT2->IOCR0 = 0x80808080;  // auf Ausgang setzen geht aber nur für ein paar Pins an P2
					PORT2->IOCR4 = 0x80808080;
					PORT2->IOCR8 = 0x80808080;
					PORT2->PDISC = 0; // digital Pad aktiv
				}
				else {
					PORT2->IOCR0 = 0x00;
					PORT2->IOCR4 = 0x00;
					PORT2->IOCR8 = 0x00;
					PORT2->PDISC = 0; // digital Pad aktiv
				}
				break;

	}
}

//---------------------------------------------------------------
// Einzelnes Portbit einlesen
// port: P0,P1,P2 bitnr: 0..15
//---------------------------------------------------------------
uint8_t bit_read(uint8_t port, uint8_t bitnr)
{
  uint16_t temp;

  temp = port_read(port);
  return ((temp>>bitnr) & 0x01);
}
//---------------------------------------------------------------
// Einzelnes Portbit ausgeben
// port: P0,P1 bitnr: 0..15 P2=>bitnr 9..11
//---------------------------------------------------------------
void bit_write(uint8_t port, uint8_t bitnr, uint8_t value)
{
	//uint32_t temp;
	value &= 0x01;       // value darf nur 0 oder 1 sein!
	switch (port)
	{
		case 0 :
			if (value == 1) {
				//PORT0->OUT |= 1UL << bitnr ;
				PORT0->OMR &= ~(1UL << (bitnr + 16)); // loeschen des Bits
				PORT0->OMR |= 1UL << bitnr;      // setzen
				//temp=PORT0->OMR;
				//temp &= ~(1UL << (bitnr + 16)); // loeschen des Bits
				//temp |= 1UL << bitnr;      // setzen
				//PORT0->OMR = temp;
			}
			else {
				//PORT0->OUT &= ~( 1UL << bitnr);
				PORT0->OMR &= ~(1UL << bitnr);      // Set = 0
				PORT0->OMR |= 1UL << (bitnr + 16); // Reset = 0
				//temp = PORT0->OMR;
				//temp &= ~(1UL << bitnr);      // Set = 0
				//temp |= 1UL << (bitnr + 16); // Reset = 0
				//PORT0->OMR = temp;
			}
			break;
	    case 1 :
	  	  	  {if (value == 1) PORT1->OUT |= 1UL << bitnr ;
	  		   else PORT1->OUT &= ~( 1UL << bitnr);
	  		  }
			break;

	    case 2 :
			  {if (value == 1) PORT2->OUT |= 1UL << bitnr ;
			   else PORT2->OUT &= ~( 1UL << bitnr);
			  }
			break;
		}
 }

//---------------------------------------------------------------
// Gesamtes Port einlesen
// port: P0,P1,P2
//---------------------------------------------------------------
uint16_t port_read (uint8_t port)
{
	uint16_t temp;
  switch (port)
  {
    case 0: temp = PORT0->IN; return temp; break;
    case 1: temp = PORT1->IN; return temp; break;
    case 2: temp = PORT2->IN; return temp; break;
    default: return 0; break;
  }
  //return 0;
}

//----------------------------------------------------------------
//Gesamtes Port P0 initialisieren mit IO-Bitmaske
// Bit der Maske 0 -> Input, 1 -> Output
// z.B.0x00ff rechte 8 Bit Ausgabe, linke 8 Bit Eingabe (nur 6 sichtbar)
// 0b0000000011111111 = 0x00ff
//----------------------------------------------------------------
void port0_init_maske(uint16_t io_maske)
{
	uint8_t z;
	for (z = 0; z<16;z++)				// 16 Bits (0..15)
	{
		bit_init(P0,z,io_maske&0b1);	// einzeln initialisieren
		io_maske >>= 1;					// nächste Bit der Maske
	}
}

//---------------------------------------------------------------
// Gesamtes Port ausgeben
// port: P0,P1
//---------------------------------------------------------------
void port_write(uint8_t port, uint16_t value)
{
  switch (port)
  {
    case 0: PORT0->OUT  = value; break;
    case 1: PORT1->OUT = value; break;
    case 2: PORT1->OUT = value; break;
  }
}
// ***************************************************************
// ab hier PWM-Funktionen
// ***************************************************************

#define periode_pwm_8 2550 // 8-Bit-Periode fuer ca. 1.5 kHz
#define periode_pwm 50000 // 16-Bit-Periode 50000 für leichtere Berechnung, ca. 640 Hz
//#define periode_pwm 0xffff // max 16-Bit-Periode


//------- Takt für 8-Bit- und 16-Bit-Funktionen PWM --------------------------------
void clock_init(void)
{
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CLKCR = 0x3FF10100;			// Config SCU Clock = 32MHz , PCLK = 64 MHz
	while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 |= 0x04;				// Disable CCU4 Gating
	while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;
	SCU_CLK->CGATSET0 |= 0xfff;				// Modul  Clock enable , all Modul's
}
//-----------------------------------------------------------------------
// 8-Bit-PWM1-Ausgang P0.6 initialisieren,
// Hinweis: anschließend starten mit pwm1_start oder pwm1_start_interrupt
//-----------------------------------------------------------------------
void pwm1_init_8(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;				// Enable CCU40
	// CCU40 Init:
	 CCU40->GIDLC = 0x10F; 					// Vorteiler enable,CCU4x enable
	 CCU40_CC40->TC= 4;  					// Shadow transfer enable on clear
	 CCU40_CC40->PSC = 0x4;					// Vorteiler , hier 64Mhz/16
	 CCU40_CC40->PRS = periode_pwm_8; 		// Period Register set , hier ca. 1.5 kHz

		// CCU40 Compare:  ( Ausganspin P0.6 )
	 CCU40_CC40->CRS = 1270;				// Compare Wert,hier 1:1 Imp
	 CCU40->GCSS = 1; 						// Shadow transfer request slice 0
	 //PORT0->IOCR0 = 0xA0;					// Alt4=>OUT bei P0.0
	 PORT0->IOCR4 |= 0xA0<<16;				// Alt4=>OUT bei P0.6
	 //CCU40_CC40->TCSET = 0x1; //starts counting
}
//---------------------------------------------------------------
// PWM1-Ausgang P0.6 initialisieren (16-Bit),
// Hinweis: anschließend starten mit pwm1_start oder pwm1_start_interrupt
// eingestellte Periodendauer bei #define periode_pwm 50000
//----------------------------------------------------------------
void pwm1_init(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;				// Enable CCU40
	// CCU40 Init:
	CCU40->GIDLC = 0x10F; 					// Vorteiler enable,CCU4x enable
	CCU40_CC40->TC = 4;  					// Shadow transfer enable on clear
	CCU40_CC40->PSC = 0x1;					// Vorteiler , hier 64Mhz/2
	CCU40_CC40->PRS = periode_pwm; 			// Period Register set, hier ca. 640 Hz

	// CCU40 Compare:  (Ausganspin P0.6)
	CCU40_CC40->CRS = 25000;				// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1; 						// Shadow transfer request slice 0
	//PORT0->IOCR0 = 0xA0;					// Alt4=>OUT bei P0.0
	PORT0->IOCR4 |= 0xA0<<16;				// Alt4=>OUT bei P0.6
	//CCU40_CC40->TCSET = 0x1; //starts counting
}
//---------------------------------------------------------------
// 8-Bit-PWM2-Ausgang P0.7 initialisieren
//---------------------------------------------------------------
void pwm2_init_8(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;				// Enable CCU40
		// CCU41 Init:
	CCU40->GIDLC = 0x10F; 					// Vorteiler enable,CCU4x enable
	CCU40_CC41->TC= 4;  					// Shadow transfer enable on clear
	CCU40_CC41->PSC = 0x4;					// Vorteiler , hier 64Mhz/16
	CCU40_CC41->PRS = periode_pwm_8; 			// Period Register set , hier ca. 1.5 kHz

		// CCU40 Compare:  ( Ausganspin P0.0 )
	CCU40_CC41->CRS = 1270;				// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1<<4; 				// Shadow transfer request slice 1
	//PORT0->IOCR0 |= 0xA0 <<8;			// Alt4=>OUT bei P0.1
	PORT0->IOCR4 |= 0xA0<<24;			// Alt4=>OUT bei P0.7
}
//---------------------------------------------------------------
// PWM2-Ausgang P0.7 initialisieren (16-Bit)
// Hinweis: anschließend starten mit pwm2_start oder pwm2_start_interrupt
// eingestellte Periodendauer bei #define periode_pwm 50000
//---------------------------------------------------------------
void pwm2_init(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;				// Enable CCU40
	// CCU41 Init:
	CCU40->GIDLC = 0x10F; 					// Vorteiler enable,CCU4x enable
	CCU40_CC41->TC = 4;  					// Shadow transfer enable on clear
	CCU40_CC41->PSC = 0x1;					// Vorteiler , hier 64Mhz/2
	CCU40_CC41->PRS = periode_pwm; 			// Period Register set, hier ca. 640 Hz

	// CCU40 Compare:  (Ausganspin P0.0)
	CCU40_CC41->CRS = 25000;			// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1<<4; 				// Shadow transfer request slice 1
	//PORT0->IOCR0 |= 0xA0 <<8;			// Alt4=>OUT bei P0.1
	PORT0->IOCR4 |= 0xA0<<24;			// Alt4=>OUT bei P0.7
}
//---------------------------------------------------------------
// 8-Bit-PWM3-Ausgang P0.8 initialisieren
//---------------------------------------------------------------
void pwm3_init_8(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;			// Enable CCU40
		// CCU42 Init:
	CCU40->GIDLC = 0x10F; 				// Vorteiler enable,CCU4x enable
	CCU40_CC42->TC= 4;  				// Shadow transfer enable on clear
	CCU40_CC42->PSC = 0x4;				// Vorteiler , hier 64Mhz/16
	CCU40_CC42->PRS = periode_pwm_8; 		// Period Register set , hier ca. 1.5 kHz

		// CCU40 Compare:  ( Ausganspin P0.8 )
	CCU40_CC42->CRS = 1270;				// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1<<8; 				// Shadow transfer request slice 2
	//PORT0->IOCR0 |= 0xA0 << 16;		// Alt4=>OUT bei P0.2
	PORT0->IOCR8 |= 0xA0;				// Alt4=>OUT bei P0.8
}
//---------------------------------------------------------------
// PWM3-Ausgang P0.8 initialisieren (16-Bit)
// Hinweis: anschließend starten mit pwm3_start oder pwm3_start_interrupt
// eingestellte Periodendauer bei #define periode_pwm 50000
//---------------------------------------------------------------
void pwm3_init(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;			// Enable CCU40
		// CCU42 Init:
	CCU40->GIDLC = 0x10F; 				// Vorteiler enable,CCU4x enable
	CCU40_CC42->TC = 4;  				// Shadow transfer enable on clear
	CCU40_CC42->PSC = 0x1;				// Vorteiler , hier 64Mhz/2
	CCU40_CC42->PRS = periode_pwm; 		// Period Register set , hier ca. 640 Hz

	// CCU40 Compare:  (Ausganspin P0.8)
	CCU40_CC42->CRS = 25000;			// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1<<8; 				// Shadow transfer request slice 2
	//PORT0->IOCR0 |= 0xA0 << 16;		// Alt4=>OUT bei P0.2
	PORT0->IOCR8 |= 0xA0;				// Alt4=>OUT bei P0.8
}
//---------------------------------------------------------------
// PWM4-Ausgang P0.9 initialisieren (16-Bit)
// Hinweis: anschließend starten mit pwm4_start oder pwm4_start_interrupt
// eingestellte Periodendauer bei #define periode_pwm 50000
//---------------------------------------------------------------
void pwm4_init(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;			// Enable CCU40
		// CCU42 Init:
	CCU40->GIDLC = 0x10F; 				// Vorteiler enable,CCU4x enable
	CCU40_CC43->TC = 4;  				// Shadow transfer enable on clear
	CCU40_CC43->PSC = 0x1;				// Vorteiler , hier 64Mhz/2
	CCU40_CC43->PRS = periode_pwm; 		// Period Register set , hier ca. 640 Hz

	// CCU40 Compare:  (Ausganspin P0.8)
	CCU40_CC43->CRS = 25000;			// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1<<12; 				// Shadow transfer request slice 3
	//PORT0->IOCR0 |= 0xA0 << 16;		// Alt4=>OUT bei P0.2
	//PORT0->IOCR8 &= !(0xF8 << 8);
	PORT0->IOCR8 |= 0xA0 << 8;				// Alt4=>OUT bei P0.9
}
//---------------------------------------------------------------
// 8-Bit-Comparewert einstellen, der den Tastgrad des 8-Bit-PWM-Singals verändert
// compare = 0...255, Tastgrad 50% bei compare = 127
//---------------------------------------------------------------
void pwm1_duty_cycle_8 (uint8_t compare)
{
	CCU40_CC40->CRS = periode_pwm_8 - (compare * 10);// Compare Wert,hier 1:1 Imp
	CCU40->GCSS |= 1; 					//Shadow transfer request slice 0
}
//---------------------------------------------------------------
// 16-Bit-Comparewert einstellen, der den Tastgrad des PWM-Singals verändert
// eingestellte Periodendauer bei #define periode_pwm 50000
// compare = 0...50000, Tastgrad 50% bei compare = 25000
//---------------------------------------------------------------
void pwm1_duty_cycle (uint16_t compare)
{
	CCU40_CC40->CRS = compare; 	// Compare Wert, hier 1:1 Imp
	CCU40->GCSS |= 1; 					// Shadow transfer request slice 0
}
//---------------------------------------------------------------
// 8-Bit-Comparewert (für Tastgrad) und Periodendauer der 8-Bit-PWM1 P0.6 einstellen
// compare = 0...period, period = 0...255
// Tastgrad 50% bei compare = period/2
//---------------------------------------------------------------
void pwm1_duty_cycle_period_8 (uint8_t compare, uint8_t periode)
{
	CCU40_CC40->PRS = periode*10; 			// Period Register set , ca. 1.5 kHz bei periode = 255
	CCU40_CC40->CRS = (periode - compare)*10;// Compare Wert
	CCU40->GCSS |= 1; 						//Shadow transfer request slice 0
}
//---------------------------------------------------------------
// 16-Bit-Comparewert (für Tastgrad) und Periodendauer der PWM1 P0.6 einstellen
// Vorteiler max 15
// eingestellte Periodendauer bei #define periode_pwm 50000
// compare = 0...period, period = 0...50000
// Tastgrad 50% bei compare = period/2
//---------------------------------------------------------------
void pwm1_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode)
{
	vorteiler %= 0x10;				// max. 15
	CCU40_CC40->PSC = vorteiler;
	CCU40_CC40->PRS = periode; 		// Period Register set, ca. 640 Hz bei periode = 50000
	CCU40_CC40->CRS = compare;		// Compare Wert
	CCU40->GCSS |= 1; 				// Shadow transfer request slice 0
}
//---------------------------------------------------------------
// 8-Bit-Comparewert einstellen, der den Tastgrad des 8-Bit-PWM-Singals verändert
// compare = 0...255, Tastgrad 50% bei compare = 127
//---------------------------------------------------------------
void pwm2_duty_cycle_8 (uint8_t compare)
{
	CCU40_CC41->CRS = periode_pwm_8 - (compare * 10);// Compare Wert,hier 1:1 Imp
	CCU40->GCSS |= 1<<4; 				//Shadow transfer request slice 1
}
//---------------------------------------------------------------
// 16-Bit-Comparewert einstellen, der den Tastgrad des PWM-Singals verändert
// eingestellte Periodendauer bei #define periode_pwm 50000
// compare = 0...50000, Tastgrad 50% bei compare = 25000
//---------------------------------------------------------------
void pwm2_duty_cycle (uint16_t compare)
{
	CCU40_CC41->CRS = compare;	// Compare Wert,hier 1:1 Imp
	CCU40->GCSS |= 1<<4; 				// Shadow transfer request slice 1
}
//---------------------------------------------------------------
// 8-Bit-Comparewert (für Tastgrad) und Periodendauer der 8-Bit-PWM2 P0.7 einstellen
// compare = 0...periode, periode = 0...255
// Tastgrad 50% bei compare = periode/2
//---------------------------------------------------------------
void pwm2_duty_cycle_period_8 (uint8_t compare, uint8_t periode)
{
	CCU40_CC41->PRS = periode*10; 			// Period Register set , ca. 1.5 kHz bei periode = 255
	CCU40_CC41->CRS = (periode - compare)*10;// Compare Wert
	CCU40->GCSS |= 1<<4; 				//Shadow transfer request slice 1
}
//---------------------------------------------------------------
// 16-Bit-Comparewert (für Tastgrad) und Periodendauer der PWM2 P0.7 einstellen
// Vorteiler max 15
// eingestellte Periodendauer bei #define periode_pwm 50000
// compare = 0...period, period = 0...50000
// Tastgrad 50% bei compare = periode/2
//---------------------------------------------------------------
void pwm2_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode)
{
	vorteiler %= 0x10;					// max. 15
	CCU40_CC41->PSC = vorteiler;
	CCU40_CC41->PRS = periode; 			// Period Register set, ca. 640 Hz bei periode = 50000
	CCU40_CC41->CRS = compare;			// Compare Wert
	CCU40->GCSS |= 1<<4; 				// Shadow transfer request slice 1
}
//---------------------------------------------------------------
// 8-Bit-Comparewert einstellen, der den Tastgrad des 8-Bit-PWM-Singals verändert
// compare = 0...255, Tastgrad 50% bei compare = 127
//---------------------------------------------------------------
void pwm3_duty_cycle_8 (uint8_t compare)
{
	CCU40_CC42->CRS = periode_pwm_8 - (compare * 10);// Compare Wert,hier 1:1 Imp
	CCU40->GCSS |= 1<<8; 				//Shadow transfer request slice 2
}
//---------------------------------------------------------------
// 16-Bit-Comparewert einstellen, der den Tastgrad des PWM-Singals verändert
// eingestellte Periodendauer bei #define periode_pwm 50000
// compare = 0...50000, Tastgrad 50% bei compare = 25000
//---------------------------------------------------------------
void pwm3_duty_cycle (uint16_t compare)
{
	CCU40_CC42->CRS = compare;	// Compare Wert,hier 1:1 Imp
	CCU40->GCSS |= 1<<8; 				// Shadow transfer request slice 2
}
//---------------------------------------------------------------
// 8-Bit-Comparewert (für Tastgrad) und Periodendauer der 8-Bit-PWM3 P0.8 einstellen
// compare = 0...periode, periode = 0...255
// Tastgrad 50% bei compare = periode/2
//---------------------------------------------------------------
void pwm3_duty_cycle_period_8 (uint8_t compare, uint8_t periode)
{
	CCU40_CC42->PRS = periode*10; 			// Period Register set , ca. 1.5 kHz bei periode = 255
	CCU40_CC42->CRS = (periode - compare)*10;// Compare Wert
	CCU40->GCSS |= 1<<8; 				//Shadow transfer request slice 2
}
//---------------------------------------------------------------
// 16-Bit-Comparewert (für Tastgrad) und Periodendauer der PWM3 P0.8 einstellen
// Vorteiler max 15
// eingestellte Periodendauer bei #define periode_pwm 50000
// compare = 0...period, period = 0...50000
// Tastgrad 50% bei compare = periode/2
//---------------------------------------------------------------
void pwm3_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode)
{
	vorteiler %= 0x10;					// max. 15
	CCU40_CC42->PSC = vorteiler;
	CCU40_CC42->PRS = periode; 			// Period Register set, ca. 640 Hz bei periode = 50000
	CCU40_CC42->CRS = compare;			// Compare Wert
	CCU40->GCSS |= 1<<8; 				// Shadow transfer request slice 2
}
//---------------------------------------------------------------
// 16-Bit-Comparewert einstellen, der den Tastgrad des PWM-Singals verändert
// eingestellte Periodendauer bei #define periode_pwm 50000
// compare = 0...50000, Tastgrad 50% bei compare = 25000
//---------------------------------------------------------------
void pwm4_duty_cycle (uint16_t compare)
{
	CCU40_CC43->CRS = compare;	// Compare Wert,hier 1:1 Imp
	CCU40->GCSS |= 1<<12; 				// Shadow transfer request slice 2
}
//---------------------------------------------------------------
// 16-Bit-Comparewert (für Tastgrad) und Periodendauer der PWM3 P0.9 einstellen
// Vorteiler max 15
// eingestellte Periodendauer bei #define periode_pwm 50000
// compare = 0...period, period = 0...50000
// Tastgrad 50% bei compare = periode/2
//---------------------------------------------------------------
void pwm4_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode)
{
	vorteiler %= 0x10;					// max. 15
	CCU40_CC43->PSC = vorteiler;
	CCU40_CC43->PRS = periode; 			// Period Register set, ca. 640 Hz bei periode = 50000
	CCU40_CC43->CRS = compare;			// Compare Wert
	CCU40->GCSS |= 1<<12; 				// Shadow transfer request slice 2
}
//----------------------------------------------------------------
// PWM1-Ausgang P0.6 Ausgabe starten ohne Interrupt starten, 8- oder 16-Bit
//----------------------------------------------------------------
void pwm1_start(void)
{
	 CCU40_CC40->TCSET = 0x1; //starts counting
}
/*------------------------------------------------------------------
* PWM1-Ausgang P0.6 Ausgabe starten mit Interrupt nach jeder Periode
* Diese Timer-ISR ins eigene Programm kopieren
void CCU40_0_IRQHandler (void)
{
	pwm1_duty_cycle (uint8_t compare); // neuen Comparewert laden
}
*/
void pwm1_start_interrupt(void)
{
	CCU40_CC40->TCSET = 0x1; 		//starts counting
	// Int bei Periode CCU42_CC40 :
	CCU40_CC40->INTE |=1 ;			//Int bei Period Match_
	NVIC_EnableIRQ(CCU40_0_IRQn); 	// Int Freigabe für CCU40 => CC40
}
//----------------------------------------------------------------
// PWM2-Ausgang P0.7 Ausgabe starten ohne Interrupt starten
//----------------------------------------------------------------
void pwm2_start(void)
{
	 CCU40_CC41->TCSET = 0x1; //starts counting
}
/*------------------------------------------------------------------
* PWM2-Ausgang P0.7 Ausgabe starten mit Interrupt nach jeder Periode
* Diese Timer-ISR ins eigene Programm kopieren
void CCU40_1_IRQHandler (void)
{
	pwm2_duty_cycle (uint8_t compare); // neuen Comparewert laden
}
*/
void pwm2_start_interrupt(void)
{
	CCU40_CC41->TCSET = 0x1; 		//starts counting
	// Int bei Periode CCU42_CC41 :
	CCU40_CC41->INTE |=1 ;			//Int bei Period Match_
	NVIC_EnableIRQ(CCU40_1_IRQn); 	// Int Freigabe für CCU40 => CC41
}
//----------------------------------------------------------------
// PWM3-Ausgang P0.8 Ausgabe starten ohne Interrupt starten
//----------------------------------------------------------------
void pwm3_start(void)
{
	 CCU40_CC42->TCSET = 0x1; //starts counting
}
/*------------------------------------------------------------------
* PWM3-Ausgang P0.8 Ausgabe starten mit Interrupt nach jeder Periode
* Diese Timer-ISR ins eigene Programm kopieren
void CCU40_2_IRQHandler (void)
{
	pwm3_duty_cycle (uint8_t compare); // neuen Comparewert laden
}
*/
void pwm3_start_interrupt(void)
{
	CCU40_CC42->TCSET = 0x1; 		//starts counting
	// Int bei Periode CCU42_CC42 :
	CCU40_CC42->INTE |=1 ;			//Int bei Period Match_
	NVIC_EnableIRQ(CCU40_2_IRQn); 	// Int Freigabe für CCU40 => CC42
}
//----------------------------------------------------------------
// PWM4-Ausgang P0.9 Ausgabe starten ohne Interrupt starten
//----------------------------------------------------------------
void pwm4_start(void)
{
	 CCU40_CC43->TCSET = 0x1; //starts counting
}
/*------------------------------------------------------------------
* PWM4-Ausgang P0.9 Ausgabe starten mit Interrupt nach jeder Periode
* Diese Timer-ISR ins eigene Programm kopieren
void CCU40_3_IRQHandler (void)
{
	pwm4_duty_cycle (uint8_t compare); // neuen Comparewert laden
}
*/
void pwm4_start_interrupt(void)
{
	CCU40_CC43->TCSET = 0x1; 		//starts counting
	// Int bei Periode CCU40_CC43 :
	CCU40_CC43->INTE |=1 ;			//Int bei Period Match_
	NVIC_EnableIRQ(CCU40_3_IRQn); 	// Int Freigabe für CCU40 => CC43
}
// PWM1 anhalten -----------------------------------------------------
void pwm1_stop(void)
{
	CCU40_CC40->TCCLR = 0x01;
	NVIC_DisableIRQ(CCU40_0_IRQn); 	// Int sperren für CCU40 => CC40
}
// PWM2 anhalten -----------------------------------------------------
void pwm2_stop(void)
{
	CCU40_CC41->TCCLR = 0x01;
	NVIC_DisableIRQ(CCU40_1_IRQn); 	// Int sperren für CCU40 => CC41
}
// PWM3 anhalten -----------------------------------------------------
void pwm3_stop(void)
{
	CCU40_CC42->TCCLR = 0x01;
	NVIC_DisableIRQ(CCU40_2_IRQn); 	// Int sperren für CCU40 => CC42
}
// PWM4 anhalten -----------------------------------------------------
void pwm4_stop(void)
{
	CCU40_CC43->TCCLR = 0x01;
	NVIC_DisableIRQ(CCU40_3_IRQn); 	// Int sperren für CCU40 => CC43
}


// ab hier ADC-Funktionen ****************************************

//---------------------------------------------------------------
// Analog-Digital-Converter initialisieren
//---------------------------------------------------------------
void adc_init(void)
{
	PORT2->IOCR4 = PORT2->IOCR4 & 0x00000000;	// P2 => INPUT
	SCU_GENERAL->PASSWD = 0x000000C0UL;			// Modul write enable
	SCU_CLK->CGATCLR0 |= 0x00000001; 			// disable  VADC gating
	while (( SCU_CLK->CLKCR)& 0x40000000);		// wait for stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;
	CLR_BIT(VADC->CLC,0); 						// Clock Enable

	SHS0->SHSCFG = 0x00008000 ; 				// SCWC Bit = 1
	//SET_BIT (SHS0->SHSCFG,15); => geht nicht !!
	CLR_BIT (SHS0->SHSCFG,12);  				// ANOFF = 0
	SET_BIT (SHS0->SHSCFG,11); 					// int. Uref
	*((int*)0x40010500)= 0x01;					// Toggle on => workarond
	//VADC_Start = 0x00000001;					// Zusatz Bit für Start
	while ( (SHS0->SHSCFG & 0x00004000) == 0);	// Warte auf ready
    //--------------Kalibrierung
	 // VADC->GLOBCFG &= ~VADC_GLOBCFG_DPCAL0_Msk;    // Enable post-calibration.
	  // workaround for calibration
	  VADC->GLOBCFG |= VADC_GLOBCFG_DPCAL0_Msk;
	  VADC->GLOBCFG |= VADC_GLOBCFG_DPCAL1_Msk;
	  VADC->GLOBCFG &= ~VADC_GLOBCFG_SUCAL_Msk;      // Raising edge on SUCAL to initiate the calibration.
	  VADC->GLOBCFG |= VADC_GLOBCFG_SUCAL_Msk;
	  // workaround for calibration
	  VADC->GLOBCFG |= VADC_GLOBCFG_DPCAL0_Msk;
	  VADC->GLOBCFG |= VADC_GLOBCFG_DPCAL1_Msk;
	  while ((SHS0->SHSCFG & SHS_SHSCFG_STATE_Msk) != 0);  // Wait until calibration is finished.
	  *((int*)0x480340E0) = 0x80008000;
	  *((int*)0x480340E4) = 0x80008000;
	//--------------Ende Kalibrierung
	SET_BIT (VADC->BRSMR,0);					// Conv Request => pending Bit
	VADC->BRSCTRL = 0x00000000;					// kein ext. Trigger
	SET_BIT(VADC->BRSSEL[0],0)  ;				// Kanal 0
}

//---------------------------------------------------------------
// 12-Bit-Wert vom Analogeingang einlesen
// Kanal 0: Poti links, 1: Poti rechts, 2: P2.9, 3: P2.10, 4: P2.11, 7: P2.2
//---------------------------------------------------------------
uint16_t adc_in(uint8_t kanal)

{
	kanal %=7;									// max Kanal 7
	VADC->BRSPND[0]  = 1 << kanal;				// Pending Kanal 0.. 8
	while ((VADC->GLOBRES & 0x80000000) == 0 );	// Abfrage VF Bit
	CLR_BIT(VADC->GLOBRES,31);					// Clear VF Bit
	return (VADC->GLOBRES & 0x00000fff);		// 12 Bit Ergebnis
}
// Delays ********************************************************
//---------------------------------------------------------------
void delay_10us (uint8_t n)
{
	volatile uint16_t z;
	for (;n != 0;--n)
			for (z = 10; 	z!= 0; 	--z); // 10 zu lang, 9 zu kurz
}
//---------------------------------------------------------------
// Zeitverzögerung n mal 100µs
//---------------------------------------------------------------
void delay_100us (uint8_t n)
{
	volatile uint16_t z;
	for (;n != 0;--n)
		for (z = _T_CONST_; 	z!= 0; 	--z);
}

//---------------------------------------------------------------
// Zeitverzögerung Millisekunden
//---------------------------------------------------------------
void delay_ms( uint16_t millisekunden)
{
	volatile uint16_t n = 0;

	while(n < millisekunden)
	{
		delay_100us(10);
		n++;
	}
}

//void delay_s(uint16_t sekunden)
//{
//	volatile uint16_t n = 0;
//
//	while(n < sekunden)
//	{
//		delay_ms(1000);
//		n++;
//	}
//}

// ****************************************************************************
// I2C- Routinen zur Ansteuerung eins I2C-Slaves
// I2C-Bus-Funktionen i2c_init, i2c_start, i2c_stop, i2c_write, i2c_read
// ****************************************************************************

//---------------------------------------------------------------
// **** Zeitverzoegerung zur Verlangsamung der Datenuebertragungsrate ***********
// **** i=2 bis i=100 waehlen je nach I2C-IC und Pull-Up-Widerstand
//---------------------------------------------------------------
void i2c_delay(void)
//---------------------------------------------------------------
{
  volatile uint8_t i;	         // i nicht rausoptimieren (volatile)!!
  for (i=10;i!=0;i--);
}

//---------------------------------------------------------------
// Initialiserung I2C-Bus
// SDA P1.0, SCL P1.1
//---------------------------------------------------------------
void i2c_init (void)
{
  I2C_DIR |= SDA_OUT; 			// SDA => Pn.0
  I2C_DIR |= SCL_OUT;			//SCL => OUT
  I2C_PORT_OUT |= (1<<SDA);   	// Leitungen in den Grundzustand High
  i2c_delay();
  I2C_PORT_OUT |= (1<<SCL);
  i2c_delay();
}

//---------------------------------------------------------------
// Startbedingung I2C-Bus
//---------------------------------------------------------------
void i2c_start (void)
{
  I2C_DIR |= SDA_OUT;			// Richtung => OUT
  I2C_DIR |= SCL_OUT;			// Richtung => OUT
  I2C_PORT_OUT |= (1<<SDA);     // Grundzustand
  I2C_PORT_OUT |= (1<<SCL);
  i2c_delay();
  I2C_PORT_OUT &= ~(1<<SDA);    // Wechsel SDA von 1 -> 0 w�hrend SCL = 1
  i2c_delay();
  //I2C_PORT_OUT &= ~(1<<SCL);
  //i2c_delay();
}

//---------------------------------------------------------------
// Stoppbedingung I2C-Bus
//---------------------------------------------------------------
void i2c_stop (void)
{
	I2C_DIR |= SDA_OUT;	// Richtung => OUT
	I2C_DIR |= SCL_OUT;	// Richtung => OUT
	I2C_PORT_OUT &= ~(1<<SDA);                 // SDA = 4;
	I2C_PORT_OUT |= (1<<SCL);                  // SCL = 5;
	i2c_delay();
  // Wechsel von SDA 0 -> 1 w�hrend SCL = 1
	I2C_PORT_OUT |= (1<<SDA);                  // SDA = 4;
	i2c_delay();
}

//---------------------------------------------------------------
// Byte ausgeben an I2C-Bus , Rueckgabewert = ack = ACK/NACK
//---------------------------------------------------------------
uint8_t i2c_write (uint8_t value)
{
  uint8_t z;                             // Zaehler
  uint8_t ack;                           // Acknowledge-Bit

    I2C_DIR |= SDA_OUT;					// Richtung => OUT
    I2C_DIR |= SCL_OUT;					// Richtung => OUT

  for (z = 8; z != 0; z --)              // Zaehler: serielle Ausgabe von 8 Bit
  {
	  I2C_PORT_OUT &= ~(1<<SCL);         // SCL = 0  Clockausgabe
	  // Daten:
	  if ((value & 0x80) == 0x80)        // Ausgabe: MSB first
		  I2C_PORT_OUT |= (1<<SDA);      // SDA = 1;
	  else
		  I2C_PORT_OUT &= ~(1<<SDA);     // SDA = 0;
	  value <<=1;                  // naechstes Bit!

	  i2c_delay();
	  I2C_PORT_OUT |= (1<<SCL);          // SCL = 1
    i2c_delay();
  }
  //I2C_DIR &= SDA_IN;					// Data auf Input hier ist es zu früh, wenn LSB 0 war
  I2C_PORT_OUT &= ~(1<<SCL);    // SCL = 0 Datenausabe
  I2C_DIR &= SDA_IN;						// Data auf Input

  i2c_delay();
  I2C_PORT_OUT |= (1<<SCL);              // SCL = 1 Slave kann best�tigen
  i2c_delay();                           // warten

  ack = (I2C_PORT_IN & (1<<SDA))>>SDA;

  i2c_delay();
  I2C_PORT_OUT &= ~(1<<SCL);             // SCL = 0 Slave soll Ackn-Ausgabe beenden
  i2c_delay();

  return ack;                            // Acknoledge-Bit ist R�ckgabewert der Funktion
                                         // ack = 0 bedeutet Slave hat "verstanden" !!!!!!
}
//---------------------------------------------------------------
// Byte einlesen vom I2C-Bus.
//---------------------------------------------------------------
uint8_t i2c_read (uint8_t ack)
{
  uint8_t z, value = 0;
  I2C_DIR &= SDA_IN;			// Data auf Input


  for (z = 8; z != 0; z--)      // Z�hler: serielles Einlesen von 8 Bit
  {
    I2C_PORT_OUT |= (1<<SCL);       // SCL = 1  Daten sind g�ltig
    i2c_delay();

    value <<= 1;
    value |= (uint8_t) ((I2C_PORT_IN & (1<<SDA))>>SDA);    //SDA   Datenbit in Puffer

    i2c_delay();
    I2C_PORT_OUT &= ~(1<<SCL);                // SCL = 0  Daten lesen beendet
    i2c_delay();
  }
  I2C_DIR |= SDA_OUT;	// Richtung => OUT
  if (ack == ACK)  I2C_PORT_OUT &= ~(1<<SDA); // SDA = 0  (= Acknowledge)
  else             I2C_PORT_OUT |= (1<<SDA);  // SDA = 1  (= Not Acknowledge)

  i2c_delay();
  I2C_PORT_OUT |= (1<<SCL);                   // SCL = 1  Ackn. g�ltig
  i2c_delay();
  I2C_PORT_OUT &= ~(1<<SCL);                  // SCL = 0  Ackn. beendet
  i2c_delay();
  I2C_PORT_OUT &= ~(1<<SDA);                  // SDA = 0  Leitung SDA vorbereiten f�r Stoppbed.
  i2c_delay();

  return ( value );                       // eingelesenes Byte = R�ckgabewert
}

// ****************************************************************************
// RS232-Routinen zur Kommunikation mit PC-Terminal
// ****************************************************************************
// RxD auf P1.3 , XMC1100 TxD auf P1.2
// values calculated for 9.6 kbaud @ 32 MHz
// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
//#define FDR_STEP	25	// fuer 9.6 kBaud
//#define BRG_PDIV	4
//#define BRG_DCTQ	15
//#define BRG_PCTQ	0
#define FDR_STEP	118
#define BRG_PDIV	23  //9600  11=>19,2k  5=>38,4k  3=>57,6k  1=>115,2k
#define BRG_DCTQ	15
#define BRG_PCTQ	0

/* USIC : FIFO DPTR & SIZE MASK and POS Values */
#define   USIC_CH_TBCTR_DPTRSIZE_Pos  	(0U)
#define   USIC_CH_TBCTR_DPTRSIZE_Msk  	(0x0700003FU << USIC_CH_TBCTR_DPTRSIZE_Pos)

#define   USIC_CH_RBCTR_DPTRSIZE_Pos  	(0U)
#define   USIC_CH_RBCTR_DPTRSIZE_Msk  	(0x0700003FU << USIC_CH_RBCTR_DPTRSIZE_Pos)

//---------------------------------------------------------------
// Serielle Schnittstelle initialisieren
// RxD auf P1.3 , XMC1100 TxD auf P1.2, 9600 Baud, 8N1
// über USB als virtuelle Schnittstelle COM x am PC verwendbar
//---------------------------------------------------------------
void rs232_init ( void )
{	//USIC0 Cannel 1 wird verwendet:
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 |= 0x00000008;		// stop gating USIC
		while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;

	USIC0_CH1->KSCFG |= 3UL ;	//  Mode enable + Freigabe write
	//WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 2);
	USIC0_CH1->FDR |= 1UL << 15 ;	//Fractional divider mode selected
		//Step value
	USIC0_CH1->FDR|= FDR_STEP ;
		//Vorteiler:
		// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
	USIC0_CH1->BRG |= (BRG_PDIV << 16) ;
	USIC0_CH1->BRG |= BRG_DCTQ << 10 ;
	USIC0_CH1->BRG |= BRG_PCTQ << 8 ;

		//Configuration of USIC Shift Control
		//Transmit/Receive LSB first is selected, Transmission Mode (TRM) = 1, Passive Data Level (PDL) = 1
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);
		//Set Word Length (WLE) & Frame Length (FLE)
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);

		//Configuration of USIC Transmit Control/Status Register
		//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);

		//Configuration of Protocol Control Register
		//Sample Mode (SMD) = 1, 1 Stop bit is selected, Sample Point (SP) = 2, Pulse Length (PL) = 0
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,
				USIC_CH_PCR_ASCMode_SMD_Pos, 1);
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0);
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 9);
		//TBIF is set to simplify polling
	WR_REG(USIC0_CH1->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);
		//Configuration of Transmitter Buffer Control Register
	WR_REG(USIC0_CH1->TBCTR, USIC_CH_TBCTR_LIMIT_Msk, USIC_CH_TBCTR_LIMIT_Pos, 0);

		//Configuration of Channel Control Register
		//parity generation is disabled
	USIC0_CH1->CCR |=  0x02;	// ASC Mode

		//Data Pointer & Buffer Size for Transmitter Buffer Control - DPTR = 64,  SIZE = 6
	WR_REG(USIC0_CH1->TBCTR, USIC_CH_TBCTR_DPTRSIZE_Msk, USIC_CH_TBCTR_DPTRSIZE_Pos, 0x06000040);
		// Rx Pin:
	USIC0_CH1->DX0CR = 0x00;	// Kanal: USIC0_CH1 => DXnB auf P1.3 ( siehe Pin Mapping)
	PORT1->IOCR0  |= 0b10111 << 19 ; // ALT 7 fuer P1.2 => TxD d.h. XMC1100 sendet auf P1.2
	PORT1->IOCR0 &= 0x00ffffff;	// P1.3 => INPUT =>  RxD
}

//---------------------------------------------------------------
// Serielle Schnittstelle initialisieren
// RxD auf P1.3 , XMC1100 TxD auf P1.2, 9600 Baud, 8N1
// über USB als virtuelle Schnittstelle COM x am PC verwendbar
//---------------------------------------------------------------
void rs232_init_bd (int bd)
{	//USIC0 Cannel 1 wird verwendet:
	uint8_t brgPdivUART=1;

	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 |= 0x00000008;		// stop gating USIC
		while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;

	USIC0_CH1->KSCFG |= 3UL ;	//  Mode enable + Freigabe write
	//WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 2);
	USIC0_CH1->FDR |= 1UL << 15 ;	//Fractional divider mode selected
		//Step value
	USIC0_CH1->FDR|= FDR_STEP ;
		//Vorteiler:
		// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
	USIC0_CH1->BRG &=0xFC00FFFF;  //Pdiv loeschen
	if (bd == 115200)  brgPdivUART=1;
	else if (bd == 57600) brgPdivUART=3;
	else if (bd == 38400) brgPdivUART=5;
	else if (bd == 19200) brgPdivUART=11;
	else brgPdivUART=23; //if (bd == 9600)
	USIC0_CH1->BRG |= (brgPdivUART << 16) ;
	USIC0_CH1->BRG |= BRG_DCTQ << 10 ;
	USIC0_CH1->BRG |= BRG_PCTQ << 8 ;

		//Configuration of USIC Shift Control
		//Transmit/Receive LSB first is selected, Transmission Mode (TRM) = 1, Passive Data Level (PDL) = 1
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);
		//Set Word Length (WLE) & Frame Length (FLE)
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);

		//Configuration of USIC Transmit Control/Status Register
		//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);

		//Configuration of Protocol Control Register
		//Sample Mode (SMD) = 1, 1 Stop bit is selected, Sample Point (SP) = 2, Pulse Length (PL) = 0
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,
				USIC_CH_PCR_ASCMode_SMD_Pos, 1);
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0);
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 9);
		//TBIF is set to simplify polling
	WR_REG(USIC0_CH1->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);
		//Configuration of Transmitter Buffer Control Register
	WR_REG(USIC0_CH1->TBCTR, USIC_CH_TBCTR_LIMIT_Msk, USIC_CH_TBCTR_LIMIT_Pos, 0);

		//Configuration of Channel Control Register
		//parity generation is disabled
	USIC0_CH1->CCR |=  0x02;	// ASC Mode

		//Data Pointer & Buffer Size for Transmitter Buffer Control - DPTR = 64,  SIZE = 6
	WR_REG(USIC0_CH1->TBCTR, USIC_CH_TBCTR_DPTRSIZE_Msk, USIC_CH_TBCTR_DPTRSIZE_Pos, 0x06000040);
		// Rx Pin:
	USIC0_CH1->DX0CR = 0x00;	// Kanal: USIC0_CH1 => DXnB auf P1.3 ( siehe Pin Mapping)
	PORT1->IOCR0  |= 0b10111 << 19 ; // ALT 7 fuer P1.2 => TxD d.h. XMC1100 sendet auf P1.2
	PORT1->IOCR0 &= 0x00ffffff;	// P1.3 => INPUT =>  RxD
}

void rs232_interrupt_enable()
{
	USIC0_CH1->CCR  |=0x00004000;
	USIC0_CH1->INPR &=0xFFFFF8FF;
	USIC0_CH1->PSCR |=0x00000400;
	//NVIC_EnableIRQ(USIC0_0_IRQn); 	// Int Freigabe  SR0 für empfange Zeichen
	NVIC_EnableIRQ(9); 	// Int Freigabe  SR0 für empfange Zeichen
}
/*
void USIC0_0_IRQHandler(void)
{
	empfangsdaten[0]=USIC0_CH0->RBUF & 0xff ;	// RX Buffer auslesen
}
*/

//---------------------------------------------------------------
// liest ein Byte (1..255) von COM, liefert 0 wenn kein Zeichen vorhanden
//---------------------------------------------------------------
char rs232_get ( void )
{
  uint8_t buffer;
	buffer = 0;
	if ((USIC0_CH1->PSR &(1UL <<14)) != 0)  // wenn Zeichen vorhanden
	 {
		buffer = USIC0_CH1->RBUF & 0xff ;	// RX Buffer auslesen
		USIC0_CH1->PSCR |= 1UL <<14 ;		// Receive Flag löschen
	 }
	return buffer;
}
//-----------------------------------------------------------------
// liefert 1 wenn Zeichen empfangen und 0 wenn kein Zeichen empfangen
//----------------------------------------------------------------
uint8_t rs232_char_received (void)
{
	if ((USIC0_CH1->PSR &(1UL <<14)) != 0) return 1; // wenn Zeichen vorhanden
		else return 0;
}

//---------------------------------------------------------------
// wartet auf ein Byte von COM und liest es ein (0..255)
// Achtung! Programm bleibt solange in dieser Funktion, bis ein Zeichen eintrifft!
//---------------------------------------------------------------
char rs232_wait_get ( void )
{
  uint8_t buffer;
	while ((USIC0_CH1->PSR &(1UL <<14)) == 0); 	// auf Zeichen warten
	buffer = USIC0_CH1->RBUF & 0xff ;			// RX Buffer auslesen
	USIC0_CH1->PSCR |= 1UL <<14 ;				// Receive Flag löschen
	return buffer;
}
//---------------------------------------------------------------
// schreibt ein Byte auf COM
//---------------------------------------------------------------
void rs232_put ( char value )
{
	USIC0_CH1->IN[0] = value;
	while((USIC0_CH1->TRBSR & (1U<< 11))  == 0);  // Warten , falls Transmit Buffer empty

}
//---------------------------------------------------------------
// schreibt eine nullterminierte Zeichenfolge auf COM
//---------------------------------------------------------------
void rs232_print ( char *text )
{
  while (*text != '\0')
    rs232_put(*text++);
}

/*------------------------------------------------------------------
* Bei jedem empfangenen Zeichen wird diese Interrupt Service Routine aktiviert
* Diese USIC-ISR kann ins eigene Programm kopiert werden dann muss diese
* Variante auskommentiert werden.
* Diese ISR arbeitet mit folgende Funktionen:
* rs232_wait_of_enter(), rs232_clear_receivedata()
* Es werden folgende globale Variablen benötigt:
* rs232Receivedata, rs232ReceivedataPointer, rs232EnterReceive
*/
void USIC0_0_IRQHandler(void)
{
	char temp=USIC0_CH1->RBUF & 0xff;	// Rx-Buffer auslesen
	if (temp == '\r') {					// Enterzeichen empfangen?
		rs232Receivedata[rs232ReceivedataPointer]='\0';
		rs232EnterReceive = 1;
	} else {
		rs232Receivedata[rs232ReceivedataPointer]=temp;
	}
	rs232ReceivedataPointer++;
	return;
}
void rs232_wait_of_enter()
{
	while (rs232EnterReceive==0) {}
	rs232EnterReceive=0;
}
void rs232_clear_receivedata()
{
	rs232Receivedata[0]='\0';
	rs232ReceivedataPointer=0;
}

uint8_t rs232_read_number()
{
	uint8_t stellen, zahl=0;

	rs232_wait_of_enter();
	for (stellen=0; rs232Receivedata[stellen] != '\0'; stellen++);

	for(uint8_t i=0;stellen >0; stellen--,i++) {
			zahl=rs232Receivedata[i] - 0x30 + zahl * 10;
	}
	return zahl;
}
int rs232_read_int_number()
{
	uint8_t stellen;
	int zahl=0;

	rs232_wait_of_enter();
	for (stellen=0; rs232Receivedata[stellen] != '\0'; stellen++);

	for(uint8_t i=0;stellen >0; stellen--,i++) {
			zahl=rs232Receivedata[i] - 0x30 + zahl * 10;
	}
	return zahl;
}
void rs232_print_number(uint8_t zahl)
{
	uint8_t rest, stellen=1;
	char string[20];

	for (rest=zahl; (rest=rest/10) > 0; stellen++);

	string[stellen] = '\0';

	for (; stellen > 0; stellen--) {
		rest=zahl % 10;
		zahl=zahl / 10;
		string[stellen-1]=rest + 0x30;
	}
	rs232_print(string);
}
void rs232_print_int_number(int zahl)
{
	uint8_t stellen=1;
	int rest;
	char string[20];

	for (rest=zahl; (rest=rest/10) > 0; stellen++);

	string[stellen] = '\0';

	for (; stellen > 0; stellen--) {
		rest=zahl % 10;
		zahl=zahl / 10;
		string[stellen-1]=rest + 0x30;
	}
	rs232_print(string);
}
// ****************************************************************************
// UART-Routinen zur Kommunikation mit PC-Terminal
// ****************************************************************************
// RxD auf P0.6 , XMC1100 TxD auf P0.7
// values calculated for 9.6 kbaud @ 32 MHz
// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
//#define FDR_STEP_UART	25	// fuer 9.6 kBaud
//#define BRG_PDIV_UART	4
//#define BRG_DCTQ_UART	15
//#define BRG_PCTQ_UART	0

#define FDR_STEP_UART	118
#define BRG_PDIV_UART	23 //9600  11=>19,2k  5=>38,4k  3=>57,6k  1=>115,2k
#define BRG_DCTQ_UART	15
#define BRG_PCTQ_UART	0

// USIC : FIFO DPTR & SIZE MASK and POS Values
#define   USIC_CH_TBCTR_DPTRSIZE_Pos_UART  	(0U)
#define   USIC_CH_TBCTR_DPTRSIZE_Msk_UART  	(0x0700003FU << USIC_CH_TBCTR_DPTRSIZE_Pos_UART)

#define   USIC_CH_RBCTR_DPTRSIZE_Pos_UART  	(0U)
#define   USIC_CH_RBCTR_DPTRSIZE_Msk_UART  	(0x0700003FU << USIC_CH_RBCTR_DPTRSIZE_Pos_UART)

//---------------------------------------------------------------
// Serielle Schnittstelle initialisieren
// RxD auf P1.1 0.6, TxD auf P1.0 0.7, 9600 Baud, 8N1
//---------------------------------------------------------------
void uart_init ( void )
{	//USIC0 Cannel 0 wird verwendet:
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 |= 0x00000008;		// stop gating USIC
		while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;

	USIC0_CH0->KSCFG |= 3UL ;	//  Mode enable + Freigabe write
	//WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 2);
	USIC0_CH0->FDR |= 1UL << 15 ;	//Fractional divider mode selected
		//Step value
	USIC0_CH0->FDR|= FDR_STEP_UART;
		//Vorteiler:
		// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
	USIC0_CH0->BRG |= (BRG_PDIV_UART << 16) ;
	USIC0_CH0->BRG |= BRG_DCTQ_UART << 10 ;
	USIC0_CH0->BRG |= BRG_PCTQ_UART << 8 ;

		//Configuration of USIC Shift Control
		//Transmit/Receive LSB first is selected, Transmission Mode (TRM) = 1, Passive Data Level (PDL) = 1
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);
		//Set Word Length (WLE) & Frame Length (FLE)
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);

		//Configuration of USIC Transmit Control/Status Register
		//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
	WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);
	WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);

		//Configuration of Protocol Control Register
		//Sample Mode (SMD) = 1, 1 Stop bit is selected, Sample Point (SP) = 2, Pulse Length (PL) = 0
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,
				USIC_CH_PCR_ASCMode_SMD_Pos, 1);
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0);
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 9);
		//TBIF is set to simplify polling
	WR_REG(USIC0_CH0->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);
		//Configuration of Transmitter Buffer Control Register
	WR_REG(USIC0_CH0->TBCTR, USIC_CH_TBCTR_LIMIT_Msk, USIC_CH_TBCTR_LIMIT_Pos, 0);

		//Configuration of Channel Control Register
		//parity generation is disabled
	USIC0_CH0->CCR |=  0x02;	// ASC Mode

		//Data Pointer & Buffer Size for Transmitter Buffer Control - DPTR = 64,  SIZE = 6
	WR_REG(USIC0_CH0->TBCTR, USIC_CH_TBCTR_DPTRSIZE_Msk_UART, USIC_CH_TBCTR_DPTRSIZE_Pos_UART, 0x06000040);

	// Rx Pin: P1.1
	bit_init(P1,1,INP); //P1.1 als Eingang definieren
	USIC0_CH0->DX0CR &= ~(0x7);	// Kanal: USIC0_CH0 => DX0D auf P1.1 ( siehe Pin Mapping)
	USIC0_CH0->DX0CR |= 3;

	//Tx Pin P1.0
	PORT1->IOCR0 &= 0xffffff07;	// P1.0
	PORT1->IOCR0  |= 0x000000B8; // ALT 7 fuer P1.0 => TxD d.h. XMC1100 sendet auf P1.0

	//UART Channel Auswahl auf DCE
	bit_init(0,12,OUTP);
	bit_write(0,12,1);
	bit_init(0,13,OUTP);
	bit_write(0,13,1);
}

//Initialisierung mit waehlbare Baudrate
void uart_init_bd(int bd)
{	//USIC0 Cannel 0 wird verwendet:
	uint8_t brgPdivUART=1;

	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 |= 0x00000008;		// stop gating USIC
		while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;

	USIC0_CH0->KSCFG |= 3UL ;	//  Mode enable + Freigabe write
	//WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 2);
	USIC0_CH0->FDR |= 1UL << 15 ;	//Fractional divider mode selected
		//Step value
	USIC0_CH0->FDR|= FDR_STEP_UART;
		//Vorteiler:
		// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
	USIC0_CH0->BRG &=0xFC00FFFF;  //Pdiv loeschen
	if (bd == 115200)  brgPdivUART=1;
	else if (bd == 57600) brgPdivUART=3;
	else if (bd == 38400) brgPdivUART=5;
	else if (bd == 19200) brgPdivUART=11;
	else brgPdivUART=23; //if (bd == 9600)
	USIC0_CH0->BRG |= (brgPdivUART << 16) ;
	USIC0_CH0->BRG |= BRG_DCTQ_UART << 10 ;
	USIC0_CH0->BRG |= BRG_PCTQ_UART << 8 ;

		//Configuration of USIC Shift Control
		//Transmit/Receive LSB first is selected, Transmission Mode (TRM) = 1, Passive Data Level (PDL) = 1
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);
		//Set Word Length (WLE) & Frame Length (FLE)
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);

		//Configuration of USIC Transmit Control/Status Register
		//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
	WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);
	WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);

		//Configuration of Protocol Control Register
		//Sample Mode (SMD) = 1, 1 Stop bit is selected, Sample Point (SP) = 2, Pulse Length (PL) = 0
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,
				USIC_CH_PCR_ASCMode_SMD_Pos, 1);
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0);
	WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 9);
		//TBIF is set to simplify polling
	WR_REG(USIC0_CH0->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);
		//Configuration of Transmitter Buffer Control Register
	WR_REG(USIC0_CH0->TBCTR, USIC_CH_TBCTR_LIMIT_Msk, USIC_CH_TBCTR_LIMIT_Pos, 0);

		//Configuration of Channel Control Register
		//parity generation is disabled
	USIC0_CH0->CCR |=  0x02;	// ASC Mode

		//Data Pointer & Buffer Size for Transmitter Buffer Control - DPTR = 64,  SIZE = 6
	WR_REG(USIC0_CH0->TBCTR, USIC_CH_TBCTR_DPTRSIZE_Msk_UART, USIC_CH_TBCTR_DPTRSIZE_Pos_UART, 0x06000040);

	// Rx Pin: P1.1
	bit_init(P1,1,INP); //P1.1 als Eingang definieren
	USIC0_CH0->DX0CR &= ~(0x7);	// Kanal: USIC0_CH0 => DX0D auf P1.1 ( siehe Pin Mapping)
	USIC0_CH0->DX0CR |= 3;

	//Tx Pin P1.0
	PORT1->IOCR0 &= 0xffffff07;	// P1.0
	PORT1->IOCR0  |= 0x000000B8; // ALT 7 fuer P1.0 => TxD d.h. XMC1100 sendet auf P1.0

	//UART Channel Auswahl auf DCE
	bit_init(0,12,OUTP);
	bit_write(0,12,1);
	bit_init(0,13,OUTP);
	bit_write(0,13,1);
}


void uart_interrupt_enable()
{
	USIC0_CH0->CCR  |=0x00004000;
	USIC0_CH0->INPR &=0xFFFFF8FF;
	USIC0_CH0->INPR |=0x00000100; // Interrupt wird auf SR1 gelegt
	USIC0_CH0->PSCR |=0x00000400;
	NVIC_EnableIRQ(USIC0_1_IRQn); 	// Int Freigabe  SR1 für empfange Zeichen
	//NVIC_EnableIRQ(10); 	// Int Freigabe  SR0 für empfange Zeichen
}
/*
void USIC0_1_IRQHandler(void)
{
	empfangsdaten[0]=USIC0_CH0->RBUF & 0xff ;	// RX Buffer auslesen
}
*/
//---------------------------------------------------------------
// liest ein Byte (1..255) von COM, liefert 0 wenn kein Zeichen vorhanden
//---------------------------------------------------------------
char uart_get ( void )
{
  uint8_t buffer;
	buffer = 0;
	if ((USIC0_CH0->PSR &(1UL <<14)) != 0)  // wenn Zeichen vorhanden
	 {
		buffer = USIC0_CH0->RBUF & 0xff ;	// RX Buffer auslesen
		USIC0_CH0->PSCR |= 1UL <<14 ;		// Receive Flag löschen
	 }
	return buffer;
}
//-----------------------------------------------------------------
// liefert 1 wenn Zeichen empfangen und 0 wenn kein Zeichen empfangen
//----------------------------------------------------------------
uint8_t uart_char_received (void)
{
	if ((USIC0_CH0->PSR &(1UL <<14)) != 0) return 1; // wenn Zeichen vorhanden
		else return 0;
}
//---------------------------------------------------------------
// wartet auf ein Byte von COM und liest es ein (0..255)
// Achtung! Programm bleibt solange in dieser Funktion, bis ein Zeichen eintrifft!
//---------------------------------------------------------------
char uart_wait_get ( void )
{
  uint8_t buffer;
	while ((USIC0_CH0->PSR &(1UL <<14)) == 0); 	// auf Zeichen warten
	buffer = USIC0_CH0->RBUF & 0xff ;			// RX Buffer auslesen
	USIC0_CH0->PSCR |= 1UL <<14 ;				// Receive Flag löschen
	return buffer;
}
//---------------------------------------------------------------
// schreibt ein Byte auf COM
//---------------------------------------------------------------
void uart_put ( char value )
{
	USIC0_CH0->IN[0] = value;
	while((USIC0_CH0->TRBSR & (1U<< 11))  == 0);  // Warten , bis Transmit Buffer empty

}
//---------------------------------------------------------------
// schreibt eine nullterminierte Zeichenfolge auf COM
//---------------------------------------------------------------
void uart_print ( char *text )
{
  while (*text != '\0')
    uart_put(*text++);
}

/*------------------------------------------------------------------
* Bei jedem empfangenen Zeichen wird diese Interrupt Service Routine aktiviert
* Diese USIC-ISR kann ins eigene Programm kopiert werden dann muss diese
* Variante auskommentiert werden.
* Diese ISR arbeitet mit folgende Funktionen:
* rs232_wait_of_enter(), rs232_clear_receivedata()
* Es werden folgende globale Variablen benötigt:
* rs232Receivedata, rs232ReceivedataPointer, rs232EnterReceive
*/
void USIC0_1_IRQHandler(void)
{
	char temp=USIC0_CH0->RBUF & 0xff;	// Rx-Buffer auslesen
	if (temp == '\r') {					// Enterzeichen empfangen?
		uartReceivedata[uartReceivedataPointer]='\0';
		uartEnterReceive = 1;
	} else {
		uartReceivedata[uartReceivedataPointer]=temp;
	}
	uartReceivedataPointer++;
	//USIC0_CH0->PSCR |=0x00000400;
	return;
}
void uart_wait_of_enter()
{
	while (uartEnterReceive==0) {}
	uartEnterReceive=0;
}
void uart_clear_receivedata()
{
	uartReceivedata[0]='\0';
	uartReceivedataPointer=0;
}

/*********uart_select_channel ***************************
 * Die Kanäle der UART Schnittstelle werden umgeschaltet
 * UARTCHANNEL_DCE : P0.12=1 und P0.13=1 => Stecker-Ausgang
 * UARTCHANNEL_DTE : P0.12=0 und P0.13=1 => Buchse Ausgang
 * UARTCHANNEL_0 : P0.12=1 und P0.13=0 => TTL Pegel Kanal 0
 * UARTCHANNEL_1 : P0.12=0 und P0.13=0 => TTL Pegel Kanal 1
 */
void uart_select_channel(uint8_t select) {
	while((USIC0_CH0->TRBSR & (1U<< 11))  == 0);  // Warten , bis Transmit Buffer empty
	//delay_ms(2);
	switch (select) {
	  case UARTCHANNEL_DCE:
	    bit_write(0,12,1);
	    bit_write(0,13,1);
	    break;
	  case UARTCHANNEL_DTE:
		bit_write(0,12,0);
		bit_write(0,13,1);
		break;
	  case UARTCHANNEL_1:
	    bit_write(0,12,1);
	    bit_write(0,13,0);
	    break;
	  case UARTCHANNEL_0:
		bit_write(0,12,0);
		bit_write(0,13,0);
		break;
	}
}

uint8_t uart_read_number()
{
	uint8_t stellen, zahl=0;
	uart_wait_of_enter();
	for (stellen=0; uartReceivedata[stellen] != '\0'; stellen++);

	for(uint8_t i=0;stellen >0; stellen--,i++) {
			zahl=uartReceivedata[i] - 0x30 + zahl * 10;
	}
	return zahl;
}

void uart_print_number(uint8_t zahl)
{
	uint8_t rest, stellen=1;
	char string[20];

	for (rest=zahl; (rest=rest/10) > 0; stellen++);

	string[stellen] = '\0';

	for (; stellen > 0; stellen--) {
		rest=zahl % 10;
		zahl=zahl / 10;
		string[stellen-1]=rest + 0x30;
	}
	uart_print(string);
}

uint8_t warte_auf_eingabe(){
  uint8_t eingabeErfolgt=0;

  while (eingabeErfolgt==0) {
	if (uartEnterReceive==1) {
	  eingabeErfolgt=1;
	  uartEnterReceive=0;
	  return UART;
	}
	if (rs232EnterReceive==1) {
	  eingabeErfolgt=1;
	  rs232EnterReceive=0;
	  return RS232;
	}
  }
  return RS232;
}
// ***********************************************************************
// LCD-Funktionen

// Deklaration lokale Funktionen
void send_byte(unsigned char,unsigned char);
uint8_t lcd_lookup(uint8_t ascii);
void lcd_befehl(uint8_t befehl);
void lcd_daten(uint8_t daten);
void lcd_waitwhilebusy(void);
void init_wait(uint16_t n);
void wait_bsy2(void);
void wait_bsy1(void);

// Anschluss des Displays XMC1100  Board an:
#define sclk 0 // sclk = P2.0 => Clock Anschluss LCD
#define sid 1  // sid  = P2.1 => seriell Input LCD
#define sod 0  // sod  =

// Wartezeit-Konstante für Initialisierung und Busy-Abfrage berechnen
#define LCD_WAIT 3 // ca. 7 uSek Takt

//---------------------------------------------------------------
// Sende ein Byte zum Display, senddata ist Bytewert
// rs => Command = 0 und Data = 1
//---------------------------------------------------------------
void send_byte(unsigned char senddata, unsigned char rs)
{
	unsigned char i;
	unsigned long sendwert, zw;
	zw = (unsigned long)senddata;   // Typconvertierung
	sendwert = 0x001f + ((zw & 0xf0)<<12) + ((zw&0x0f)<<8);
	if (rs == 1) sendwert = sendwert + 0x40;
	for (i = 0; i<24;i++)	// serielle Ausgabe von 24Bit
	{  	CLR_BIT (PORT2->OUT,sclk)  ; // clk = 0;
		if ((sendwert & 0x00000001) == 0)  CLR_BIT(PORT2->OUT,sid);  //sid = 0;
		else SET_BIT(PORT2->OUT,sid);
		wait_bsy1();
		SET_BIT ( PORT2->OUT,sclk);	//sclk = 1;
		wait_bsy1();
		sendwert = sendwert >> 1;
	}
}
void wait_bsy2(void)	  // ca. 1.6 ms warten
{
	unsigned int zaehler ;
	for (zaehler = 0; zaehler < 0xaff; zaehler++);
}
void wait_bsy1(void)	// ca. 50 us warten
{
	unsigned int zaehler ;
	for (zaehler = 0; zaehler < LCD_WAIT; zaehler++);
}
//---------------------------------------------------------------
// 	Gibt das Byte 'befehl' als Befehl ans LCD-Display
//  Entweder Busy-Flag des LCD abfragen oder Zeitverzögerung verwenden
//---------------------------------------------------------------
void lcd_befehl (uint8_t befehl)
{
  send_byte(befehl,0);
  wait_bsy1();
}
//---------------------------------------------------------------
// 	Gibt das Byte 'daten' ans LCD-Display im SPI Mode aus.
//---------------------------------------------------------------
void lcd_daten(unsigned char daten)
{
  send_byte(daten,1);
	wait_bsy1();
}
//---------------------------------------------------------------
// Initialisierung des LCD- Displays für den 4-Bit-Modus
//---------------------------------------------------------------
void lcd_init (void)
{	delay_ms(500);				// warten bis LCD-Controller bereit
	PORT2->PDISC &=0xfffffffc;	// P2.0 / P2.1 dig. Fkt
	PORT2->IOCR0 |= 0x8080;	// P2.0 / P2.1 = OUT
	PORT2->IOCR0 &= 0xffff8080;

	wait_bsy2();
	wait_bsy2();
	// LCD Init
	send_byte(0x34,0); 			// 8 Bit Mode
	wait_bsy2();
	send_byte(0x09,0); 			// 4 Zeilen Mode
	wait_bsy2();
	send_byte(0x30,0); 			// 8 Bit Datenläge
	wait_bsy2();
	send_byte(0x0f,0); 			// display ein , Cursor ein
	wait_bsy2();
	send_byte(0x01,0); 			// clear Display
	wait_bsy2();
	send_byte(0x07,0); 			// Cursor autoincremet
	wait_bsy2();
}
//---------------------------------------------------------------
// Display löschen
//---------------------------------------------------------------
void lcd_clear (void)
	{
		lcd_befehl (0x01);
		wait_bsy2();
	  lcd_befehl( 0x0e); // Cursor on
		wait_bsy1();
	}
//---------------------------------------------------------------
// LCD-Cursor auf eine Position setzen: Zeile 1..4, Spalte 1..16 (Displayabhängig)
//---------------------------------------------------------------
void lcd_setcursor (uint8_t zeile, uint8_t spalte)
{
  uint8_t position;
  switch(zeile)
  {
    case 0:  position = ZEILE1; break;
    case 1:  position = ZEILE2; break;
    case 2:  position = ZEILE3; break;
    case 3:  position = ZEILE4; break;
    default: position = ZEILE1; break;
  }

  position += (spalte);

  lcd_befehl ( position | 0x80 );         // 0x80 = Kennung für DD RAM address set
}
//---------------------------------------------------------------
//  Ausgabe eines Zeichens an das LCD-Display
//  Entweder Busy-Flag des LCD abfragen oder Zeitverzögerung verwenden
//---------------------------------------------------------------
void lcd_char (uint8_t zeichen)
{
  zeichen = lcd_lookup(zeichen);    // Umlaute und Sonderzeichen ersetzen!
  if (zeichen != 0xC3)							// ASCII Zeichen 0xC3 wird nicht ausgegeben.
  	lcd_daten(zeichen);
}

//---------------------------------------------------------------
// \0-terminierten Text an das LCD-Display ausgeben.
 //---------------------------------------------------------------
void lcd_print (char *text)
{
  while (*text != '\0')                 // Text
    lcd_char (*text++);                // zeichenweise ausgeben
}

//---------------------------------------------------------------
// Gibt das Byte n als 3 stelligen Dez-Wert aufs  Display.
// Führende Nullen werden zu blank.
//---------------------------------------------------------------
void lcd_byte(uint8_t val)
{
  uint8_t buffer[3];
  uint8_t n = 0;

  do
  {
    buffer[n++] = val%10 + '0';
  } while ((val /= 10) > 0);

  while (n<3)                              // Rest von buffer mit blank füllen
  {
    buffer[n++] = ' ';
  }

  while (n > 0)                            // Ausgabe auf das Display (umgekehrt)
  {
    n--;
    lcd_char(buffer[n]);
  }
}
//---------------------------------------------------------------
// Gibt den Integer n als 5 stelligen Dez-Wert aufs Display.
// Führende Nullen werden zu blank.
//---------------------------------------------------------------
void lcd_int(uint16_t val)
{
  uint8_t buffer[5];
  uint8_t n = 0;

  do
  {
    buffer[n++] = val%10 + '0';
  } while ((val /= 10) > 0);

  while (n<5)                              // Rest von buffer mit blank füllen
  {
    buffer[n++] = ' ';
  }

  while (n > 0)                            // Ausgabe auf das Display (umgekehrt)
  {
    n--;
    lcd_char(buffer[n]);
  }
}

//---------------------------------------------------------------
// Ersetzt ein ASCII-Umlaut durch den entsprechenden Display-Code.
// Da die Umlaute durch 16 Bit Werte übersetzt werden, können ein
// paar ASCII Zeichen vertauscht werden.
// ASCII Zeichen 0xC3 wird nicht ausgegeben.
//---------------------------------------------------------------
uint8_t lcd_lookup(uint8_t ascii)
{
  switch (ascii)
  {
    case 0xb0: return 0xdf; // '°'

    case 0xa4:  return 0x7b;
    case 0xb6:  return 0x7C;
    case 0xbc:  return 0x7E;
    /*
    case 'Ä':  return 0xe1;
    case 'Ö':  return 0xef;
    case 'Ü':  return 0xf5;
    case 'ß':  return 0xe2;
    case 'µ':  return 0xe4;
    case '\\': return 0xa4;
    case '€':  return 0xd3;
*/
  }
  return ascii;
}

/////////////////////////////////////////////////////////////////////////////////
// Ab hier sind ergänzende Funktionen
/////////////////////////////////////////////////////////////////////////////////
#ifdef LCD_EXTENSION
/***********************************************************************************
 * Pixelweise Definition eigener LCD-Zeichen, Ablegen von max 7 Zeichen im CG-RAM des LCD
 ***********************************************************************************/
void lcd_defchar (uint8_t *PixelTabelle, uint8_t ZeichenNr)
{
  uint8_t z;

  if (ZeichenNr<8)                  // Adr 0 nicht innerhalb eines Textstrings nutzbar!
  lcd_befehl (((ZeichenNr*8) & 0x7F) | 0x40);
  // Adresse der ersten Pixelzeile = 8 x Adresse des fertigen Zeichens im DD Ram
  // Zugriff auf Adresse im CG-RAM (Character Generator) mit  Bit7 = 0, Bit6 = 1
  for (z=0;z<8;z++)
  lcd_char (*(PixelTabelle+z));     // 8 Pixelzeilen ins CR-Ram
  lcd_setcursor (0,0);              // wieder auf DD-Ram-Zugriff umschalten
}

/*************************************************************************
Löschen der Zeilen 1 bis 4 des Displays
Es wird ein Leerstring (16 Byte) auf die Zeile geschrieben
***************************************************************************/
void lcd_clearline(uint8_t zeile)
{
  uint8_t n;

  lcd_setcursor(zeile,0);
  for (n=0; n < LCD_LEN; n++)  lcd_char(' ');
}


#endif

/*************************************************************************
Löschen der Zeilen 1 bis 4 des Displays
Es wird ein Leerstring (16 Byte) auf die Zeile geschrieben
***************************************************************************/
void lcd_clearline(uint8_t zeile)
{
  uint8_t n;

  lcd_setcursor(zeile,0);
  for (n=0; n < LCD_LEN; n++)  lcd_char(' ');
  lcd_setcursor(zeile, 0);
}

//**************************************************************************
// Ab hier Int - Funktionen :
//lokale Fkt :
void ext_interrupt1_init(uint8_t flanke);
void ext_interrupt2_init(uint8_t flanke);


/* --------------------------------------------------------------------------
   Externen Interrupt1 initialisieren, ERU0 Kanal 2B0 -> Eingang P2.10,
   flanke=RE=1 Rising Edge ansteigende Flanke
   flanke=FE=0 Falling Edge abfallende Flanke
   Interrupt-Handler ins eigene Programm kopieren:
void ERU0_2_IRQHandler(void)	// ext. Int an P2.10
	{
								// Interrupt-Servie-Routine
	}
//--------------------------------------------------------------------------*/
void ext_interrupt1_init(uint8_t flanke)
{
	ERU0->EXISEL = 0x00000000;			//Eingang 2B0 auswählen (P2.10)
	if (flanke == FE)
		 ERU0->EXICON[2]=0x00000129;	// PE=1 (Trigger Impuls erzeugen) , FE (auf fallende Flanke reagieren)
	else ERU0->EXICON[2]=0x00000125;	// PE=1 (Trigger Impuls erzeugen) , RE (auf ansteigende Flanke reagieren)
	ERU0->EXOCON[2]= 0x00000010;		// GP=1 , OGU2 wird mit NVIC Request Line verbunden
	//CLR_BIT (PORT2->PDISC,10);			// Port 2.10 auf dig. Input
	bit_init_mode(P2, 10, INP, PULLUP);  // Port 2.10 digital, In, Pullup
	ext_clear_IRQ(IRQ1);						 // ISR Anforderungsbit geloescht
	//NVIC_EnableIRQ(ERU0_2_IRQn); // Interrupt wird freigegeben Kanal 2 (CMSIS Bib)
}


/* --------------------------------------------------------------------------
   Externen Interrupt2 initialisieren,  ERU0 Kanal 3B0 -> Eingang P2.9,
   flanke=RE=1 Rising Edge ansteigende Flanke
   flanke=FE=0 Falling Edge abfallende Flanke
   Interrupt-Service-Routinen ins eigene Programm kopieren:
void ERU0_3_IRQHandler(void)	// ext. Int an P2.9
	{
								// Interrupt-Servie-Routine
	}
//--------------------------------------------------------------------------*/
void ext_interrupt2_init(uint8_t flanke)
{
	ERU0->EXISEL |= 0x00000000; 		//Eingang 3B0 auswaehlen (P2.9)
	if (flanke == FE)
		 ERU0->EXICON[3]=0x00000139; 	// PE=1 , FE (auf fallende Flanke reagieren) , Trigger an OGU3 ,SS=01b fuer B
	else ERU0->EXICON[3]=0x00000135; 	// PE=1 , RE (steigende Flanke Kanal 2) , Trigger an OGU3 ,SS=01b fuer B
	ERU0->EXOCON[3]=0x00000010; 		// GP=1 , OGU3 wird mit NVIC Request Line verbunden
	//CLR_BIT (PORT2->PDISC,9);			// Port 2.9 auf dig. Input
	bit_init_mode(P2, 9, INP, PULLUP);  // Port 2.10 digital, In, Pullup
	ext_clear_IRQ(IRQ2);  						// ISR Anforderungsbit geloescht
	//NVIC_EnableIRQ(ERU0_3_IRQn); 		// Freigabe Interrupt Kanal 3 (CMSIS Bib)
}

/* Externen Interrupt1 initialisieren --------------------------------------
   ERU0 Kanal 2B0 -> Eingang P2.10, abfallende Flanke
   Interrupt-Handler ins eigene Programm kopieren:
void ERU0_2_IRQHandler(void)	// ext. Int bei fallender Flanke an P2.10
	{							// Interrupt-Servie-Routine
	}
   Externen Interrupt2 initialisieren -------------------------------------
   ERU0 Kanal 3B0 -> Eingang P2.9, steigende Flanke
   Interrupt-Service-Routinen ins eigene Programm kopieren:
void ERU0_3_IRQHandler(void)	// ext. Int bei steigender Flanke an P2.9
	{							// Interrupt-Servie-Routine
	}*/
void ext_interrupt_init ( void )	// beide Int Eingänge Init
{
	ext_interrupt1_init(FE);
	ext_interrupt2_init(RE);
}
/*Externen Interrupt1 freigeben --------------------------------------
   ERU0 Kanal 2B0 -> Eingang P2.10, abfallende Flanke
   Interrupt-Handler ins eigene Programm kopieren:
void ERU0_2_IRQHandler(void)	// ext. Int bei an P2.10
	{							// Interrupt-Servie-Routine
	}*/
void ext_interrupt1_enable ( void )
{
	NVIC_EnableIRQ(ERU0_2_IRQn);
}
/* --------------------------------------------------------------------------
   Externen Interrupt2 freigeben
   ERU0 Kanal 3B0 -> Eingang P2.9, steigende Flanke
   Interrupt-Service-Routinen ins eigene Programm kopieren:
void ERU0_3_IRQHandler(void)	// ext. Int an P2.9
	{							// Interrupt-Servie-Routine
	}*/
void ext_interrupt2_enable ( void )
{
	NVIC_EnableIRQ(ERU0_3_IRQn);
}
void ext_interrupt1_disable( void )
{
	NVIC_DisableIRQ(ERU0_2_IRQn);
}
void ext_interrupt2_disable( void )
{
	NVIC_DisableIRQ(ERU0_3_IRQn);
}

void ext_interrupt1_prioritaet(uint8_t prio) {
	if (prio < 4) NVIC_SetPriority(ERU0_2_IRQn,prio);
}

void ext_interrupt2_prioritaet(uint8_t prio) {
	if (prio < 4) NVIC_SetPriority(ERU0_3_IRQn,prio);
}
//NVIC_SetPriority(ERU0_2_IRQn,0);
//***********************************************
/*Interrupt-Service-Routinen*/
// void ERU0_2_IRQHandler(void)// P2.10
//{
			// Interrupt-Servie-Routine
//}
//**********************************************
//void ERU0_3_IRQHandler(void) // P2.9
//{
			// Interrupt-Servie-Routine
//}
//neu
/***********************************************
 * löscht Interrupt-Anforderungsbit
 * IRQ1 für Interrupt 1
 * IRQ2 für Interrupt 2
 */
void ext_clear_IRQ(uint8_t interruptquelle)
{
  if (interruptquelle== IRQ1) {
	  NVIC_ClearPendingIRQ(ERU0_2_IRQn);
  } else {
	  NVIC_ClearPendingIRQ(ERU0_3_IRQn);
  }
}




/*///////////////////// Achtung!!!!! //////////////////////
Bei der gleichzeitigen Verwendung von Timer und PWM muss darauf
geachtet werden, dass nicht die gleiche CCU40-CCx benutzt wird.
///////////////////////////////////////////////////////////*/

/* --------------------------------------------------------
   Timer-Interrupt 1ms initialisieren
   Timer-Interrupt-Handler ins eigene Programm kopieren:
void CCU40_3_IRQHandler (void)  // Timer-Interrupt jede 1 ms
	{
								// Interrupt-Servie-Routine
    }
 */
void timer1ms_init ( void )
{
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
			// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	//Vorteiler enable für alle Timer , IDLE clear
	CCU40_CC43->TC	= 0x08;		//Shadow transfer enable
	CCU40_CC43->PSC = 0x1;		// Vorteiler , hier 64Mhz/2 => 32 MHzclock
	CCU40_CC43->PRS	= 32000;	//Period Register für 1ms
	CCU40->GCSS = 0b001 << 12; 	//Shadow transfer request  Timer CCU40->CC43
	CCU40_CC43->TCSET = 1; 		//starts counting
	// Interrupt Freigaben:
	CCU40_CC43->SRS |= 0b11;	// Service Req. Line 3
	CCU40_CC43->INTE |= 1;		// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_3_IRQn);  // Freigabe CCU40SR3
}
/* wie timer_start_interrupt();
   Timer-Interrupt-Handler ins eigene Programm kopieren:
	void CCU40_3_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
		{
								// Interrupt-Servie-Routine
    	}*/
/* Timer-Interrupt freigeben */
void timer1ms_enable ( void )
{
	CCU40_CC43->TCSET = 1; 			//starts counting
		// Interrupt Freigaben:
	CCU40_CC43->INTE |= 1;			// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_3_IRQn);  // Freigabe CCU40SR0
}

void timer1ms_disable ( void )
{
	CCU40_CC43->TCCLR = 1; 			//stop counting
}
//***********  Timer ISR : *******************************
//void CCU40_3_IRQHandler (void)
//{
									// Interrupt-Servie-Routine
// }

/* ---------------------------------------alte Funktion
Zeitmessfunktionen mit Timer CCU40_CC43
kann anstelle der 1ms Sek Funktion benutzt werden
Hinweis: zusätzlich verwenden: timer_start();timer_stop();timer_value();
void timer_init(uint8_t teiler)
{	teiler %= 0x10;				// max. 15
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
				// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	// Vorteiler enable für alle Timer , IDLE clear
	CCU40_CC43->TC	= 0x08;		// Shadow transfer enable
	CCU40_CC43->PSC = teiler;	// Vorteiler , hier 64Mhz/2 => 32 MHzclock
	CCU40_CC43->PRS	= 0xffff;	// Period Register für 1ms
	CCU40->GCSS = 0b001 <<12 ; 	// Shadow transfer request  Timer
}*/
/* ---------------------------------------------------------------------
Initialisierung Timer CCU40_CC43, vorteiler 0...15 ergibt Teilung mit 2^vorteiler
                                  periode 1...65536, 64MHz-Takt
Hinweise: Für Zeitmessungen zusätzlich verwenden:
timer_start();timer_stop();timer_value(); timer_reset();
Für timergesteuerte Interrupts zusätzlich verwenden:
timer_start_interrupt();
Timer-Interrupt-Handler ins eigene Programm kopieren:
	void CCU40_3_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
		{							// Interrupt-Servie-Routine
    	} */
void timer_init(uint8_t vorteiler, uint16_t periode)
{	vorteiler %= 0x10;				// max. 15
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
				// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	// Vorteiler enable für alle Timer , IDLE clear
	CCU40_CC43->TC	= 0x08;		// Shadow transfer enable
	CCU40_CC43->PSC = vorteiler;	// Vorteiler , hier 64Mhz/2 hoch Vorteiler
	CCU40_CC43->PRS	= periode;	// Period Register
	CCU40->GCSS = 0b001 <<12 ; 	// Shadow transfer request  Timer
}


// Timer für Zeitmessung starten
void timer_start(void)
{
	CCU40_CC43->TCSET = 1; 		//starts counting
}
/* Timer starten mit Interruptfreigabe
   Timer-Interrupt-Handler ins eigene Programm kopieren:
void CCU40_3_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
{
						// Interrupt-Servie-Routine
} */
void timer_start_interrupt(void)
{
	CCU40_CC43->TCSET = 1; 		//starts counting
	// Interrupt Freigaben:
	CCU40_CC43->SRS |= 0b11;	// Service Req. Line 3
	CCU40_CC43->INTE |= 1;		// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_3_IRQn);  // Freigabe CCU40SR3
}

// Timer stoppen
void timer_stop(void)
{
	CCU40_CC43->TCCLR = 0x1;	// Timer stop
}
// Timerwert lesen
uint16_t timer_value(void)
{
	uint16_t wert;
	wert = CCU40_CC43->TIMER;
	return (wert);
}
// Timer zurücksetzen
void timer_reset(void) // Timer rücksetzen, früher in timer_value enthalten
{
	CCU40_CC43->TCCLR = 0x3;	// Timer stop und clear
}

//Timer 2
void timer2_init(uint8_t vorteiler, uint16_t periode)
{	vorteiler %= 0x10;				// max. 15
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
				// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	// Vorteiler enable für alle Timer , IDLE clear
	CCU40_CC42->TC	= 0x08;		// Shadow transfer enable
	CCU40_CC42->PSC = vorteiler;	// Vorteiler , hier 64Mhz/2 => 32 MHzclock
	CCU40_CC42->PRS	= periode;	// Period Register
	CCU40->GCSS = 0b001 <<8 ; 	// Shadow transfer request  Timer
}


// Timer für Zeitmessung starten
void timer2_start(void)
{
	CCU40_CC42->TCSET = 1; 		//starts counting
}
/* Timer starten mit Interruptfreigabe
   Timer-Interrupt-Handler ins eigene Programm kopieren:
void CCU40_2_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
{
							// Interrupt-Servie-Routine
} */
void timer2_start_interrupt(void)
{
	CCU40_CC42->TCSET = 1; 		//starts counting
	// Interrupt Freigaben:
	CCU40_CC42->SRS |= 0b10;	// Service Req. Line 3
	CCU40_CC42->INTE |= 1;		// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_2_IRQn);  // Freigabe CCU40SR3
}

// Timer stoppen
void timer2_stop(void)
{
	CCU40_CC42->TCCLR = 0x1;	// Timer stop
}
// Timerwert lesen
uint16_t timer2_value(void)
{
	uint16_t wert;
	wert = CCU40_CC42->TIMER;
	return (wert);
}
// Timer zurücksetzen
void timer2_reset(void) // Timer rücksetzen, früher in timer_value enthalten
{
	CCU40_CC42->TCCLR = 0x3;	// Timer stop und clear
}


//Timer 1
void timer1_init(uint8_t vorteiler, uint16_t periode)
{	vorteiler %= 0x10;				// max. 15
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
				// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	// Vorteiler enable für alle Timer , IDLE clear
	CCU40_CC41->TC	= 0x08;		// Shadow transfer enable
	CCU40_CC41->PSC = vorteiler;	// Vorteiler , hier 64Mhz/2 => 32 MHzclock
	CCU40_CC41->PRS	= periode;	// Period Register
	CCU40->GCSS = 0b001 <<4 ; 	// Shadow transfer request  Timer
}


// Timer für Zeitmessung starten
void timer1_start(void)
{
	CCU40_CC41->TCSET = 1; 		//starts counting
}
/* Timer starten mit Interruptfreigabe
   Timer-Interrupt-Handler ins eigene Programm kopieren:
void CCU40_1_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
{
							// Interrupt-Servie-Routine
} */
void timer1_start_interrupt(void)
{
	CCU40_CC41->TCSET = 1; 		//starts counting
	// Interrupt Freigaben:
	CCU40_CC41->SRS |= 0b01;	// Service Req. Line 3
	CCU40_CC41->INTE |= 1;		// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_1_IRQn);  // Freigabe CCU40SR3
}

// Timer stoppen
void timer1_stop(void)
{
	CCU40_CC41->TCCLR = 0x1;	// Timer stop
}
// Timerwert lesen
uint16_t timer1_value(void)
{
	uint16_t wert;
	wert = CCU40_CC41->TIMER;
	return (wert);
}
// Timer zurücksetzen
void timer1_reset(void) // Timer rücksetzen, früher in timer_value enthalten
{
	CCU40_CC41->TCCLR = 0x3;	// Timer stop und clear
}


//Timer 0
void timer0_init(uint8_t vorteiler, uint16_t periode)
{	vorteiler %= 0x10;				// max. 15
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
				// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	// Vorteiler enable für alle Timer , IDLE clear
	CCU40_CC40->TC	= 0x08;		// Shadow transfer enable
	CCU40_CC40->PSC = vorteiler;	// Vorteiler , hier 64Mhz/2 => 32 MHzclock
	CCU40_CC40->PRS	= periode;	// Period Register
	CCU40->GCSS = 0b001 <<0 ; 	// Shadow transfer request  Timer
}


// Timer für Zeitmessung starten
void timer0_start(void)
{
	CCU40_CC40->TCSET = 1; 		//starts counting
}
/* Timer starten mit Interruptfreigabe
   Timer-Interrupt-Handler ins eigene Programm kopieren:
void CCU40_0_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
{
				// Interrupt-Servie-Routine
} */
void timer0_start_interrupt(void)
{
	CCU40_CC40->TCSET = 1; 		//starts counting
	// Interrupt Freigaben:
	CCU40_CC40->SRS |= 0b00;	// Service Req. Line 3
	CCU40_CC40->INTE |= 1;		// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_0_IRQn);  // Freigabe CCU40SR3
}

// Timer stoppen
void timer0_stop(void)
{
	CCU40_CC40->TCCLR = 0x1;	// Timer stop
}
// Timerwert lesen
uint16_t timer0_value(void)
{
	uint16_t wert;
	wert = CCU40_CC40->TIMER;
	return (wert);
}
// Timer zurücksetzen
void timer0_reset(void) // Timer rücksetzen, früher in timer_value enthalten
{
	CCU40_CC40->TCCLR = 0x3;	// Timer stop und clear
}

//Extrene kontrolle des Timer 0
//Auswahl zwischen Pin 0, Pin 6 und Pin 12
//Auswahl bei Start zwischen RE=Steigende Flanke und FE=Fallende Flanke
//Auswahl bei Stop zwischen RE=Steigende Flanke und FE=Fallende Flanke

void timer0_extern_controll(uint8_t PIN, uint8_t Start, uint8_t Stopp)
{
	switch (PIN)
	{

	case 0 :  //Pin 0 wurde Ausgewählt
		bit_init(P0, 0, INP);
		if (Start == RE)   //PIN 0 und steigende Flanke als Start
		{
			CCU40_CC40->CMC &= 0xFFFFFFF9;
			CCU40_CC40->CMC |= 0x00000009;

			CCU40_CC40->INS &= 0xFF39FF22;
			CCU40_CC40->INS |= 0x1E090022;
		}

		if (Start == FE)  //PIN 0 und fallende Flanke als Start
		{
			CCU40_CC40->CMC &= 0xFFFFFFF9;
			CCU40_CC40->CMC |= 0x00000009;

			CCU40_CC40->INS &= 0xFF36FF22;
			CCU40_CC40->INS |= 0x1E060022;
		}
		break;


	case 6:  //Pin 6 wurde Ausgewählt
		bit_init(P0, 6, INP);
		if  (Start == RE)   //PIN 6 und steigende Flanke als Start
		{
			CCU40_CC40->CMC &= 0xFFFFFFF9;
			CCU40_CC40->CMC |= 0x00000009;

			CCU40_CC40->INS &= 0xFF39FF11;
			CCU40_CC40->INS |= 0x1E090011;
		}

		if (Start == FE)   //PIN 6 und fallende Flanke als Start
		{
			CCU40_CC40->CMC &= 0xFFFFFFF9;
			CCU40_CC40->CMC |= 0x00000009;

			CCU40_CC40->INS &= 0xFF36FF11;
			CCU40_CC40->INS |= 0x1E060011;
		}
		break;

	case 12:  //Pin 12 wurde Ausgewählt
		bit_init(P0, 12, INP);
		if  (Start == RE)   //PIN 12 und steigende Flanke als Start
		{
			CCU40_CC40->CMC &= 0xFFFFFFF9;
			CCU40_CC40->CMC |= 0x00000009;

			CCU40_CC40->INS &= 0xFF39FF00;
			CCU40_CC40->INS |= 0x1E090000;
		}

		if (Start == FE)   //PIN 12 und fallende Flanke als Start
		{
			CCU40_CC40->CMC &= 0xFFFFFFF9;
			CCU40_CC40->CMC |= 0x00000009;

			CCU40_CC40->INS &= 0xFF36FF00;
			CCU40_CC40->INS |= 0x1E060000;
		}
		break;
	} // Ende switch
}

//Extrene kontrolle des Timer 1
//Auswahl zwischen Pin 1, Pin 7 und Pin 12
//Auswahl bei Start zwischen RE=Steigende Flanke und FE=Fallende Flanke
//Auswahl bei Stop zwischen RE=Steigende Flanke und FE=Fallende Flanke
void timer1_extern_controll(uint8_t PIN, uint8_t Start, uint8_t Stopp)
{
	switch (PIN)
	{

	case 1 :  //Pin 1 wurde Ausgewählt
		bit_init(P0, 1, INP);
		if (Start == RE)   //PIN 1 und steigende Flanke als Start
		{
			CCU40_CC41->CMC &= 0xFFFFFFF9;
			CCU40_CC41->CMC |= 0x00000009;

			CCU40_CC41->INS &= 0xFF39FF22;
			CCU40_CC41->INS |= 0x1E090022;
		}

		if (Start == FE)  //PIN 1 und fallende Flanke als Start
		{
			CCU40_CC41->CMC &= 0xFFFFFFF9;
			CCU40_CC41->CMC |= 0x00000009;

			CCU40_CC41->INS &= 0xFF36FF22;
			CCU40_CC41->INS |= 0x1E060022;
		}
		break;


	case 7:  //Pin 1 wurde Ausgewählt

		bit_init(P0, 7, INP);
		if  (Start == RE)   //PIN 7 und steigende Flanke als Start
		{
			CCU40_CC41->CMC &= 0xFFFFFFF9;
			CCU40_CC41->CMC |= 0x00000009;

			CCU40_CC41->INS &= 0xFF39FF11;
			CCU40_CC41->INS |= 0x1E090011;
		}

		if (Start == FE)   //PIN 7 und fallende Flanke als Start
		{
			CCU40_CC41->CMC &= 0xFFFFFFF9;
			CCU40_CC41->CMC |= 0x00000009;

			CCU40_CC41->INS &= 0xFF36FF11;
			CCU40_CC41->INS |= 0x1E060011;
		}
		break;

	case 12:  //Pin 12 wurde Ausgewählt
		bit_init(P0, 12, INP);
		if  (Start == RE)   //PIN 12 und steigende Flanke als Start
		{
			CCU40_CC41->CMC &= 0xFFFFFFF9;
			CCU40_CC41->CMC |= 0x00000009;

			CCU40_CC41->INS &= 0xFF39FF00;
			CCU40_CC41->INS |= 0x1E090000;
		}

		if (Start == FE)   //PIN 12 und fallende Flanke als Start
		{
			CCU40_CC41->CMC &= 0xFFFFFFF9;
			CCU40_CC41->CMC |= 0x00000009;

			CCU40_CC41->INS &= 0xFF36FF00;
			CCU40_CC41->INS |= 0x1E060000;
		}
		break;
	}
}

//Extrene kontrolle des Timer 2
//Auswahl zwischen Pin 2, Pin 8 und Pin 12
//Auswahl bei Start zwischen RE=Steigende Flanke und FE=Fallende Flanke
//Auswahl bei Stop zwischen RE=Steigende Flanke und FE=Fallende Flanke
void timer2_extern_controll(uint8_t PIN, uint8_t Start, uint8_t Stopp)
{
	switch (PIN)
	{
	case 2 :  //Pin 2 wurde Ausgewählt
		bit_init(P0, 2, INP);
		if (Start == RE)   //PIN 2 und steigende Flanke als Start
		{
			CCU40_CC42->CMC &= 0xFFFFFFF9;
			CCU40_CC42->CMC |= 0x00000009;

			CCU40_CC42->INS &= 0xFF39FF22;
			CCU40_CC42->INS |= 0x1E090022;
		}

		if (Start == FE)  //PIN 2 und fallende Flanke als Start
		{
			CCU40_CC42->CMC &= 0xFFFFFFF9;
			CCU40_CC42->CMC |= 0x00000009;

			CCU40_CC42->INS &= 0xFF36FF22;
			CCU40_CC42->INS |= 0x1E060022;
		}
		break;

	case 8:  //Pin 8 wurde Ausgewählt
		bit_init(P0, 8, INP);
		if  (Start == RE)   //PIN 8 und steigende Flanke als Start
		{
			CCU40_CC42->CMC &= 0xFFFFFFF9;
			CCU40_CC42->CMC |= 0x00000009;

			CCU40_CC42->INS &= 0xFF39FF11;
			CCU40_CC42->INS |= 0x1E090011;
		}

		if (Start == FE)   //PIN 8 und fallende Flanke als Start
		{
			CCU40_CC42->CMC &= 0xFFFFFFF9;
			CCU40_CC42->CMC |= 0x00000009;

			CCU40_CC42->INS &= 0xFF36FF11;
			CCU40_CC42->INS |= 0x1E060011;
		}
		break;

	case 12:  //Pin 12 wurde Ausgewählt
		bit_init(P0, 12, INP);
		if  (Start == RE)   //PIN 12 und steigende Flanke als Start
		{
			CCU40_CC42->CMC &= 0xFFFFFFF9;
			CCU40_CC42->CMC |= 0x00000009;

			CCU40_CC42->INS &= 0xFF39FF00;
			CCU40_CC42->INS |= 0x1E090000;
		}

		if (Start == FE)   //PIN 12 und fallende Flanke als Start
		{
			CCU40_CC42->CMC &= 0xFFFFFFF9;
			CCU40_CC42->CMC |= 0x00000009;

			CCU40_CC42->INS &= 0xFF36FF00;
			CCU40_CC42->INS |= 0x1E060000;
		}
		break;
	}
}

//Extrene kontrolle des Timer 3
//Auswahl zwischen Pin 3, Pin 9 und Pin 12
//Auswahl bei Start zwischen RE=Steigende Flanke und FE=Fallende Flanke
//Auswahl bei Stop zwischen RE=Steigende Flanke und FE=Fallende Flanke

void timer3_extern_controll(uint8_t PIN, uint8_t Start, uint8_t Stopp)
{
	switch (PIN)
	{
	case 3 :  //Pin 3 wurde Ausgewählt
		bit_init(P0, 3, INP);
		if (Start == RE)   //PIN 3 und steigende Flanke als Start
		{
			CCU40_CC43->CMC &= 0xFFFFFFF9;
			CCU40_CC43->CMC |= 0x00000009;

			CCU40_CC43->INS &= 0xFF39FF22;
			CCU40_CC43->INS |= 0x1E090022;
		}

		if (Start == FE)  //PIN 3 und fallende Flanke als Start
		{
			CCU40_CC43->CMC &= 0xFFFFFFF9;
			CCU40_CC43->CMC |= 0x00000009;

			CCU40_CC43->INS &= 0xFF36FF22;
			CCU40_CC43->INS |= 0x1E060022;
		}
		break;

	case 9:  //Pin 9 wurde Ausgewählt
		bit_init(P0, 9, INP);
		if  (Start == RE)   //PIN 9 und steigende Flanke als Start
		{
			CCU40_CC43->CMC &= 0xFFFFFFF9;
			CCU40_CC43->CMC |= 0x00000009;

			CCU40_CC43->INS &= 0xFF39FF11;
			CCU40_CC43->INS |= 0x1E090011;
		}

		if (Start == FE)   //PIN 9 und fallende Flanke als Start
		{
			CCU40_CC43->CMC &= 0xFFFFFFF9;
			CCU40_CC43->CMC |= 0x00000009;

			CCU40_CC43->INS &= 0xFF36FF11;
			CCU40_CC43->INS |= 0x1E060011;
		}
		break;

	case 12:  //Pin 12 wurde Ausgewählt
		bit_init(P0, 12, INP);
		if  (Start == RE)   //PIN 12 und steigende Flanke als Start
		{
			CCU40_CC43->CMC &= 0xFFFFFFF9;
			CCU40_CC43->CMC |= 0x00000009;

			CCU40_CC43->INS &= 0xFF39FF00;
			CCU40_CC43->INS |= 0x1E090000;
		}

		if (Start == FE)   //PIN 12 und fallende Flanke als Start
		{
			CCU40_CC43->CMC &= 0xFFFFFFF9;
			CCU40_CC43->CMC |= 0x00000009;

			CCU40_CC43->INS &= 0xFF36FF00;
			CCU40_CC43->INS |= 0x1E060000;
		}
		break;
	}
}


void deepSleep_init(void)
{
	PPB->SCR |= 0x00000004;
}
