/* ************************************************************************** */
/** Hello World!

  @HW2
    main.c

  @Summary
 SPI communication to get variable output
  @Description
 creates a sin wave and a triangle wave
 */
/* ************************************************************************** */

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include"spi.h"


unsigned char sin_list[100];
unsigned char triangle_list[100];

// DEVCFG0
#pragma config DEBUG = 11 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 11 // use PGED1 and PGEC1
#pragma config PWP = 111111111 // no write protect
#pragma config BWP = 1 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // disable secondary osc
#pragma config FPBDIV = 00 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = 10 // do not enable clock switch
#pragma config WDTPS = 00000 // use slowest wdt
#pragma config WINDIS = 1 // wdt no window mode
#pragma config FWDTEN = 0 // wdt disabled
#pragma config FWDTWINSZ = 11 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = 001 //2x divider ->4// divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = 111 //24x multiplier ->96// multiply clock after FPLLIDIV
#pragma config FPLLODIV = 001 //2x divider ->48// divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = 001 //2x divider// divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0000001100000000 // 768 in binary //some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module

void make_sin() {
    int i = 0;
    for(i; i<= 99; i = i + 1){
        float temp = 127*sin(((float)i)*2*3.141592/100)+127;
        sin_list[i] = (unsigned char)((int)temp);
    }
}

void make_triangle() {
    int i = 0;
    for(i; i<= 99; i = i + 1){
        float temp = 255*((float)i)/100;
        triangle_list[i] = (unsigned char)((int)temp);
    }
} 

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // set led as output
    LATAbits.LATA4 = 1; // turn led on
    
    TRISBbits.TRISB4 = 1; // set button as an input
    
    __builtin_enable_interrupts();
    make_triangle();
    make_sin();
    int list_count = 0;
    initSPI1();
    _CP0_SET_COUNT(0);
    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		  // remember the core timer runs at half the sysclk
        if(_CP0_GET_COUNT()>48000){
            _CP0_SET_COUNT(0);
             
            
            // cycling through the voltage lists
            setVoltage(1, sin_list[list_count]);
            setVoltage(0, triangle_list[list_count]);            
            if(PORTBbits.RB4) { //if the button is on let the light flicker
                LATAINV = 0b10000; // turn LED on and off at 1 khz
            }
            else {
                LATAbits.LATA4 = 0; // turn led off
            }       
            if(list_count = 99) {
                list_count = 0;
            }
            else {
                list_count = list_count + 1;
            }
        }
              
        
        //*/
        
    }
}

