#include <xc.h>
#include "timers.h"

void init_timer0(void)
{
    TMR0 = 6; // to have 250 in overflow to avoid decimal point
    
    T0CS = 0;// Selecting internal clock
    
    /* Config INTCON */
    TMR0IF = 0;
    
    TMR0IE = 1;
    
}
void init_timer2(void)
{
    /* Pre load*/
    PR2 = 250;
    
    /* Config INTCON */
    TMR2IF = 0;
    
    TMR2IE = 1;
   
    
    /*pre scaler for 1:4 ratio*/
    T2CKPS1 = 0;   
    T2CKPS0 = 1;
    
    PEIE = 1;
    GIE = 1;
    
}