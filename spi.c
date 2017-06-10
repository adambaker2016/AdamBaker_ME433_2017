#include "spi.h"
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

char SPI1_IO(char write) {
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF) {
        ;
    }
    return SPI1BUF;
}

void initSPI1() {
    
    // SET PINS for PIC32
    
    RPB2Rbits.RPB2R = 0b0011; // SD01 to RPB2 pin
    SDI1Rbits.SDI1R = 0b0000; // SDI1 to RPA1 pin
    
    
    TRISBbits.TRISB9 = 0; // Sets Pin 18 as output for CS (select) pin
    LATBbits.LATB9 = 1; // Turns Pin on because only one SPI
    
    // setup SPI
    
    SPI1CON = 0;
    SPI1BUF;
    SPI1BRG = 0x3;
    SPI1STATbits.SPIROV = 0;
    SPI1CONbits.CKE = 1;
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.ON = 1;
    
    LATBbits.LATB9 = 0; // enable the device
    SPI1_IO(0x01);  // write status
    SPI1_IO(0x41);  // sequential mode, hold disabled
    LATBbits.LATB9 = 1; // send the command
}

void setVoltage(char channel, char voltage) {
    LATBbits.LATB9 = 0; // enable the device
    if( channel = 1) { // For VoutA
        SPI1_IO(1);
        SPI1_IO(voltage);
        
    }
    else { // channel = 0, for VoutB
        SPI1_IO(0);
        SPI1_IO(voltage);
        
    }
    LATBbits.LATB9 = 1; // send the command
}

