#include <xc.h>
/* ISR Function */
__interrupt() void isr(void)
{
    static unsigned int count = 0;

     if(TMR2IF)
    {
    
        if(++count >= 5000)
        {
            count = 0;
            if(--sec == 0)
            {
                allow = 1;
            }
        }
        TMR2IF = 0;
    }
}
