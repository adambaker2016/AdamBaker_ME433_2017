/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
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
    OC3RS = 4500; // should set the motor to 90 degrees (0.5ms to 2.5ms is 1500 to 7500 for 0 to 180 degrees)
    OC3R = 4500; // read-only
    T3CONbits.ON = 1;
    OC3CONbits.ON = 1;
    
    __builtin_enable_interrupts();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            // somewhere in APP_Tasks(), probably in case APP_STATE_SCHEDULE_READ
                // when you read data from the host
                LATAbits.LATA1 = 1; // direction
                OC1RS = 700; // velocity, OC1RS=600 -> 50%
                LATBbits.LATB3 = 1; // direction
                OC4RS = 700; // velocity, OC4RS=600 -> 50%
                
                // set servo to 90 degrees
                OC3RS = 2000; // should set the motor to 60 degrees (0.5ms to 2.5ms is 1500 to 7500 for 0 to 180 degrees)
    
        
            break;
        }

        /* TODO: implement your application state machine.*/
        
	    

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
