/*
 * File:   main.c
 * Author: SATHEESWARAN M
 * @brief: CAR BLACK BOX project - A microcontroller-based vehicle monitoring system using the PIC16F877A.
                                    It monitors vehicle speed, displays real-time clock information, records
                                    driving events with timestamps, and provides secure access to stored data.
 *         
 * @note: view dashboard.c file for key configuration
 * Created on 25 October, 2026
 */

#pragma config WDTE = OFF

#include <xc.h>
#include "main.h"


void init_config()
{
    init_adc();
    init_clcd();
    init_digital_keypad();
    init_i2c(100000); 
    init_uart(9600);
    init_ds1307();
    init_timer2();
    init_event_log();
    init_password();
    
}
unsigned char key;
unsigned char operation = 0;
void main(void) {
    init_config();

    while(1)
    {
        key = read_digital_keypad(STATE);
        
        if(operation == 0)
        {
            if(key == SW4)
            {
                operation = 1;
            }
            else
            {
                dashboard(key);
            }
        }
        else if(operation == 1)
        {
            password(key);
        }
        else if(operation == 2)
        {
            menu(key);
        }
        else if(operation == 3)
        {
            view_log(key);
        }
        else if(operation == 4)
        {
             download_log();
        }
        else if(operation == 5)
        {
            clear_log();
        }
        else if(operation == 6)
        {
            set_time(key);
        }
        else if(operation == 7)
        {
            change_password(key);
        }
        
           
    }
}
