/* Bibliothek:       XMC1100-LibHaas.c Vers 0.82 vom 19.09.2021
 * Controller:       XMC1100 Board XMC1100 Boot Kit
 * Adapterboard:	 XMC-Learnboard
 * Version:          0.82
 * Ausgangsversion:  Version 0.63 und 1.8 Birk, Bubbers
 * Autor			 Haas
*/
#ifndef XMC1100_LIBHAAS_H_
#define XMC1100_LIBHAAS_H_

#include "xmc1100.h"
//*******************************************************************************
// Globale Schalter und Symbole
// ... fuer Bit- und Byte-I/O
//#define NOPULLUPS
#define INP 0   // Input
#define OUTP 1
#define NO_PULL 0
#define PULLUP 1
#define PULLDOWN 2
#define ANALOG 3
// Namen (Aliase) fuer Ports
enum ports {P0,P1,P2};  // PORT0 = 0, PORT1 = 1, ....
// ext.Interrupt
#define RE 1 // flanke=RE=1 Rising Edge ansteigende Flanke
#define FE 0 // flanke=FE=0 Falling Edge abfallende Flanke
// ... fuer PWM (XMC1100)
//#define PWM_PIN 0           //      P0.6 ist PWM Ausgang1
// defines fuer UART
#define RS232 0
#define UART 1
#define UARTCHANNEL_DCE 0
#define UARTCHANNEL_DTE 1
#define UARTCHANNEL_1 2
#define UARTCHANNEL_0 3

//defines fuer Interrupts
#define IRQ1 1
#define IRQ2 2
//*******************************************************************************
//Globale Variablen
extern char rs232Receivedata[256];
extern uint8_t rs232ReceivedataPointer;
extern uint8_t rs232EnterReceive;
extern char uartReceivedata[256];
extern uint8_t uartReceivedataPointer;
extern uint8_t uartEnterReceive;

//*******************************************************************************
// Funktionsprototypen fuer Bit- und Byte Ein-/Ausgabe
extern void    bit_init   ( uint8_t port, uint8_t bitnr, uint8_t direction);
extern void    bit_init_mode ( uint8_t port, uint8_t bitnr, uint8_t direction, uint8_t mode);
extern uint8_t bit_read   ( uint8_t port, uint8_t bitnr);
extern void    bit_write  ( uint8_t port, uint8_t bitnr, uint8_t value);

extern void    port_init  ( uint8_t port, uint8_t direction);
extern uint16_t port_read  ( uint8_t port );
extern void    port_write ( uint8_t port, uint16_t value);
extern void    port0_init_maske(uint16_t io_maske);

// Funktionsprototypen fuer PWM (8 Bit und 16 Bit)
extern void    pwm1_init_8   ( void );	// 8-BitPWM an P0.6
extern void    pwm1_init     ( void );	// 16-BitPWM an P0.6
extern void    pwm1_start_interrupt(void); //mit Interrupt bei jeder Periode
extern void    pwm1_start  ( void );
extern void    pwm1_stop   ( void );
extern void    pwm1_duty_cycle_8 ( uint8_t compare); // 8-Bit-Comparewert einstellen
extern void    pwm1_duty_cycle   ( uint16_t compare);// 16-Bit-Comparewert einstellen
extern void    pwm1_duty_cycle_period_8 (uint8_t compare, uint8_t period);
extern void    pwm1_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode);
extern void    pwm2_init_8   ( void );	// 8-Bit-PWM an P0.7
extern void    pwm2_init     ( void );	// 16-Bit-PWM an P0.7
extern void    pwm2_start_interrupt(void); //mit Interrupt bei jeder Periode
extern void    pwm2_start  ( void );
extern void    pwm2_stop   ( void );
extern void    pwm2_duty_cycle_8 ( uint8_t value);
extern void    pwm2_duty_cycle (uint16_t compare);
extern void    pwm2_duty_cycle_period_8 (uint8_t compare, uint8_t period);
extern void    pwm2_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode);
extern void    pwm3_init_8   ( void );	// 8-Bit-PWM an P0.8
extern void    pwm3_init     ( void );  // 16-Bit-PWM an P0.8
extern void    pwm3_start_interrupt(void); //mit Interrupt bei jeder Periode
extern void    pwm3_start  ( void );
extern void    pwm3_stop   ( void );
extern void    pwm3_duty_cycle_8 ( uint8_t value);
extern void    pwm3_duty_cycle (uint16_t compare);
extern void    pwm3_duty_cycle_period_8 (uint8_t compare, uint8_t period);
extern void    pwm3_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode);
extern void    pwm4_init     ( void );  // 16-Bit-PWM an P0.9
extern void    pwm4_start_interrupt(void); //mit Interrupt bei jeder Periode
extern void    pwm4_start  ( void );
extern void    pwm4_stop   ( void );
extern void    pwm4_duty_cycle (uint16_t compare);
extern void    pwm4_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode);


// Funktionsprototypen fuer AD-Wandler
extern void    adc_init   ( void );
extern uint16_t adc_in	(uint8_t kanal);

// DELAY:
#define _T_CONST_ 109  // zuletzt 104, geändert 109 Bub Konstante fuer XMC1100 mit Default Frequ 32 MHz
					  // fuer 100 uSek
extern void delay_10us (uint8_t mikrosekunden10);
extern void delay_100us( uint8_t mikrosekunden100);
extern void delay_ms( uint16_t millisekunden);
//*************************************************************
// Timer Funktionen 1ms: mit CCU40_CC43
extern void timer1ms_init ( void );
extern void timer1ms_enable ( void );
extern void timer1ms_disable ( void );
//extern void timer1ms_isr(void);
//*************************
// Timer und Zeitmessfunktion mit CCU40_CC43 :
extern void timer_init(uint8_t vorteiler, uint16_t periode);
extern void timer_start(void);
extern void timer_start_interrupt(void);
extern void timer_stop(void);
extern uint16_t timer_value(void);  // Rückgabe => Timer Wert
extern void timer_reset(void); // Timer rücksetzen, früher in timer_value enthalten

// Timer und Zeitmessfunktion mit CCU40_CC42 :
extern void timer2_init(uint8_t vorteiler, uint16_t periode);
extern void timer2_start(void);
extern void timer2_start_interrupt(void);
extern void timer2_stop(void);
extern uint16_t timer2_value(void);  // Rückgabe => Timer Wert
extern void timer2_reset(void); // Timer rücksetzen, früher in timer_value enthalten

// Timer und Zeitmessfunktion mit CCU40_CC41 :
extern void timer1_init(uint8_t vorteiler, uint16_t periode);
extern void timer1_start(void);
extern void timer1_start_interrupt(void);
extern void timer1_stop(void);
extern uint16_t timer1_value(void);  // Rückgabe => Timer Wert
extern void timer1_reset(void); // Timer rücksetzen, früher in timer_value enthalten

// Timer und Zeitmessfunktion mit CCU40_CC40 :
extern void timer0_init(uint8_t vorteiler, uint16_t periode);
extern void timer0_start(void);
extern void timer0_start_interrupt(void);
extern void timer0_stop(void);
extern uint16_t timer0_value(void);  // Rückgabe => Timer Wert
extern void timer0_reset(void); // Timer rücksetzen, früher in timer_value enthalten

//Timer durch externe Signale starten und stoppen
extern void timer0_extern_controll(uint8_t PIN, uint8_t Start, uint8_t Stopp);  //Funktion der Timer 0 kontrolle mit den Bedingungen
extern void timer1_extern_controll(uint8_t PIN, uint8_t Start, uint8_t Stopp);  //Funktion der Timer 1 kontrolle mit den Bedingungen
extern void timer2_extern_controll(uint8_t PIN, uint8_t Start, uint8_t Stopp);  //Funktion der Timer 2 kontrolle mit den Bedingungen
extern void timer3_extern_controll(uint8_t PIN, uint8_t Start, uint8_t Stopp);  //Funktion der Timer 3 kontrolle mit den Bedingungen

//*************************************************************
//Globale Definitionen
// ... fuer I2C-Funktionen
// Variable  Hier : P1.0 => SDA
//				    P1.1 =>  SCL

#define I2C_PORT_OUT PORT0->OUT
#define I2C_PORT_IN  PORT0->IN
#define I2C_DIR  PORT0->IOCR4

#define SDA  4       // Bit P0.4
#define SCL  5       // Bit P0.5
#define SDA_OUT 0x000000C0	// Out mit OPEN DRAIN
#define SDA_IN  0xffffff00
#define SCL_OUT 0x00008000	// Out push/pull, da kein Multi Master System

#define ACK  0
#define NACK 1

//*************************************************************
// Funktionsprototypen fuer I2C-Busschnittstelle (Master-Mode)
extern void    i2c_init    ( void );
extern void    i2c_start   ( void );
extern void    i2c_stop    ( void );
extern uint8_t i2c_write   ( uint8_t value);     // R�ckgabewert ACK / NACK
extern uint8_t i2c_read    ( uint8_t ack );      // R�ckgabewert Value

// Funktionsprototypen fuer RS232-Schnittstelle
// ... fuer RS232-Funktionen
#define BAUD 9600
extern void rs232_init  ( void );
extern void rs232_init_bd (int);
extern char rs232_get   ( void );
extern uint8_t rs232_char_received (void);
extern char rs232_wait_get ( void );
extern void rs232_put   ( char value );
extern void rs232_print ( char text[] );

extern void 	rs232_interrupt_enable(void);
extern void 	rs232_wait_of_enter();
extern void 	rs232_clear_receivedata();
extern uint8_t rs232_read_number();
extern int rs232_read_int_number();
extern void rs232_print_number(uint8_t );
extern void rs232_print_int_number(int);

// ... fuer UART-Funktionen
#define BAUD_UART 9600
extern void    	uart_init  ( void );
extern void uart_init_bd(int);
extern char 	uart_get   ( void );
extern uint8_t  uart_char_received (void);
extern char     uart_wait_get ( void );
extern void    	uart_put   ( char value );
extern void    	uart_print ( char text[] );

extern void 	uart_interrupt_enable(void);
extern void 	uart_wait_of_enter();
extern void 	uart_clear_receivedata();
extern void 	uart_select_channel(uint8_t);
extern uint8_t uart_read_number();
extern void uart_print_number(uint8_t );

extern uint8_t warte_auf_eingabe();

//  LCD :
#define LCD_LEN 20              // Display-Zeilenlaenge 20 oder 16 (default)
//#define LCD_EXTENSION           // schaltet Zusatzfunktionen frei
//**********************************************************************
//Anzeige Einstellungen
#define CURSOR      0x0c        // 0b00001100  ->  Display on, Cursor off, Blink off
#define DISPLAY     0x14        // 0b00010100  ->  Cursor shift right

#define ZEILE1      0x00        // Startadresse f�r erste Displayzeile
#define ZEILE2      0x20

#if (LCD_LEN == 16)
 #define ZEILE3     0x10        // F�r 16 Zeichen LCDs Zeile3 = 0x10
 #define ZEILE4     0x50        // "       "       "   Zeile4 = 0x50
#elif (LCD_LEN == 20)
 #define ZEILE3     0x40        // F�r 20 Zeichen LCDs Zeile3 =
 #define ZEILE4     0x60        // "       "       "   Zeile4 =
#endif
extern void lcd_init      ( void );                            // Initialisierung des Displays
extern void lcd_clear     ( void );
extern void lcd_setcursor ( uint8_t row, uint8_t column );     // Setzen der Cursorposition
                                                               // row (Zeile) = 1,2, ...
                                                               // column (Spalte) = 1,2,3, ...
extern void lcd_print     ( char  text[] );                 // Textausgabe ab Cursorposition, bis '\0'
extern void lcd_char      ( uint8_t  value  );                 // Ausgabe eines Zeichens an der aktuellen Cursorposition
extern void lcd_byte      ( uint8_t  value  );                 // 0 ... 255
extern void lcd_int       ( uint16_t value  );                 // 0 ... 65535
extern void lcd_clearline (uint8_t zeile);

//erweiterte Funktionen nicht Teil der Technischen Richtlinie FA205
#ifdef LCD_EXTENSION
extern void lcd_defchar   ( uint8_t *PixelTabelle, uint8_t ZeichenNr );
 // Definition von 7 eigenen ZeichenNr: 1 bis 7
 // ZeichenNr = 0 funnktioniert nur, wenn das Zeichen nicht in Zeichenketten ('\0' = 0) verwendet wird.
extern void lcd_clearline ( uint8_t ZeilenNr );
#endif
// Interrupt Einstellungen
extern void ext_interrupt_init ( void ); //bei fallender Flanke an P2.10, steigende Flanke an P2.9
extern void ext_interrupt1_init ( uint8_t flanke );
extern void ext_interrupt2_init ( uint8_t flanke );
extern void ext_interrupt1_enable ( void );
extern void ext_interrupt2_enable ( void );
//extern void ext_interrupt1_isr (void );
//extern void ext_interrupt2_isr (void );
extern void ext_interrupt1_disable( void );
extern void ext_interrupt2_disable( void );
extern void ext_clear_IRQ(uint8_t interruptquelle);
extern void ext_interrupt1_prioritaet(uint8_t prio);
extern void ext_interrupt2_prioritaet(uint8_t prio);

extern void deepSleep_init(void);
#endif /* XMC1100_LIBHAAS_H_ */
