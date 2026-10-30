#include <xc.h>
#include "main.h"

void init_adc(void)
{
    /* Selecting Right Justification */
    ADFM = 1;
    
    /* Starting the ADC Module */
    ADON = 1;
}

unsigned short read_adc(unsigned char channel)
{
    unsigned short adc_reg_val;
    
    /* Selecting the required channel */
    ADCON0 = (unsigned char)(ADCON0 & 0xC7) | (unsigned char)(channel << 3);
    
    /* Start the ADC conversion */
    GO = 1;
    /* Wait for the conversion to complete */
    while (nDONE);
    
    adc_reg_val = (unsigned short)(((unsigned short)ADRESH << 8) |(unsigned short)ADRESL);// 10bits
    
    return adc_reg_val;
}
