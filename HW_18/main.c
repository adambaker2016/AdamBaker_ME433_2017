/* ************************************************************************** */
/** Hello World!

  @HW1
    main.c

  @Summary
 Provides a template file and blinks a light on and off
  @Description
 Prove that it is possible to load a file with the PicKit3
 */
/* ************************************************************************** */

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

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

        // put these initializations in APP_Initialize()
    RPA0Rbits.RPA0R = 0b0101; // A0 is OC1
    TRISAbits.TRISA1 = 0;
    LATAbits.LATA1 = 0; // A1 is the direction pin to go along with OC1

    RPB2Rbits.RPB2R = 0b0101; // B2 is OC4
    TRISBbits.TRISB3 = 0;
    LATBbits.LATB3 = 0; // B3 is the direction pin to go along with OC4
        // also put these in APP_Initialize()
    T2CONbits.TCKPS = 2; // prescaler N=4 
    PR2 = 1200 - 1; // 10kHz
    TMR2 = 0;
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC4CONbits.OCM = 0b110;
    OC1RS = 0; // max allowed value is 1119
    OC1R = 0; // read-only initial value
    OC4RS = 0; // max allowed value is 1119
    OC4R = 0; // read-only initial value
    T2CONbits.ON = 1;
    OC1CONbits.ON = 1;
    OC4CONbits.ON = 1;
    
    //Servo motor
        // put these initializations in APP_Initialize()
    RPB14Rbits.RPB14R =0b0101; // set pin B14 as OC3
            
    T3CONbits.TCKPS = 4; // prescaler N=16
    PR3 = 60000 - 1; // 50Hz
    TMR3 = 0;
    OC3CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC3CONbits.OCTSEL = 1; // use timer3
    OC3RS = 4250; // should set the motor to 90 degrees (0.5ms to 2.5ms is 1500 to 7500 for 0 to 180 degrees)
    OC3R = 4250; // read-only
    T3CONbits.ON = 1;
    OC3CONbits.ON = 1;
    
    __builtin_enable_interrupts();
    
    _CP0_SET_COUNT(0);
    int count = 0;
    int direction = 0;
    while(1) {
	    // somewhere in APP_Tasks(), probably in case APP_STATE_SCHEDULE_READ
                // when you read data from the host
                LATAbits.LATA1 = 1; // direction
                OC1RS = 300; // velocity, OC1RS=600 -> 50%
                LATBbits.LATB3 = 1; // direction
                OC4RS = 300; // velocity, OC4RS=600 -> 50%
                //if(direction==1){OC3RS = 4600;} //OC3R = 6500;}
                //else{OC3RS = 4400;} //OC3R = 2500;}
                // set servo to 90 degrees
                if(_CP0_GET_COUNT()>48000){
                    count = count+1;
                    //OC3RS = (7500-1500)/2+1500+direction; //uld set the motor to 60 degrees (0.5ms to 2.5ms is 1500 to 7500 for 0 to 180 degrees)//
                    if(count > 3600) {
                        count = 0;
                        if(direction==1){direction=0;}
                        else{direction = 1;}
                    }
                    _CP0_SET_COUNT(0);
                }
    }
}
