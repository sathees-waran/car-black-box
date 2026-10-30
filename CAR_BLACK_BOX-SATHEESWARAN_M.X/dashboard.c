/*
 * File:   dashboard.c
 * Author: SATHEESWARAN M
 * Created on 25 October, 2026
 
 * KEY CONFIGURATIONS
 
 * 1. Dashboard Screen
    Key             Function
    DKP1 (SW1)      Gear Up
    DKP2 (SW2)      Gear Down
    DKP3 (SW3)      Collision Detection
    DKP4 (SW4)      Enter Password Screen
    
    Potentiometer1  SPEED

* 2. Password Screen
    Key	        Function
    DKP1 (SW1)	Enter 1
    DKP2 (SW2)	Enter 0
    DKP4 (SW4)	-
    DKP5 (SW5)	Exit Password Screen -> Dashboard

* 3. Menu Screen
    Key	        Function
    DKP1 (SW1)	Enter Selected Menu Option
    DKP2 (SW2)	Exit Menu -> Dashboard
    DKP4 (SW4)	Scroll Up
    DKP5 (SW5)	Scroll Down

* 4. View Log Screen
    Key	        Function
    DKP1 (SW1)	Scroll Up (Previous Log)
    DKP2 (SW2)	Scroll Down (Next Log)
    DKP5 (SW5)	Exit View Log -> Menu

* 5. Set Time Screen
    Key	        Function
    DKP1 (SW1)	Increment Selected Digit
    DKP2 (SW2)	Change Selected Field
    DKP4 (SW4)	Save Time & Exit -> Menu
    DKP5 (SW5)	Exit Without Saving -> Menu

* 6. Change Password Screen
    Key	        Function
    DKP1 (SW1)	Enter 1
    DKP2 (SW2)	Enter 0
    DKP4 (SW4)	Save New Password & Exit
    DKP5 (SW5)	Discard Changes & Exit
 * 
 */
#include <xc.h>
#include <string.h>
#include "main.h"
#include "uart.h"
#include "timers.h"


unsigned char clock_reg[3], speed[3] = {0};
char  *event[ ] = { "ON",  "GN", "G1", "G2", "G3", "G4", "G5","GR", "C "};
int pos = 0;
unsigned int adc_val;
volatile unsigned int sec = 60;
volatile unsigned char allow = 0;
volatile unsigned int eventcount = 0;
unsigned char event_changed = 0;
char log[17];
    
unsigned char original_password[5] = "1100";


/*=================== DASHBOARD SCREEN ========================================*/
void dashboard(unsigned char key) {
    
        clcd_print(" TIME     EV  SP", LINE1(0));
        
        // display time
        get_time(clock_reg);
        display_time(clock_reg);
        
        // event display ( Gear pos and speed )

        // check swl is pressed if yes increment the pos + 1
        if(key == SW1)
        {
            if(pos == 8)
            {
                pos = 1;
                eventcount++;
                event_changed = 1;
            }
            else if(pos < 7)
            {
                 eventcount++;
                 event_changed = 1;
                  pos++;
            }
               
           
        }
        // if sw2 is pressed then decrement the pos - 1
        else if(key == SW2)
        {
             if(pos == 8)
             {
                pos = 1;
                eventcount++;
                 event_changed = 1;
             }
            else if(pos > 1)
            {
                pos--;
                eventcount++;
                event_changed = 1;
            }
            
        }
        else if(key == SW3)
        {
            pos = 8;
            eventcount++;
            event_changed = 1;
        }
        

        
        clcd_print(event[pos] , LINE2(10));
        
        
       //display speed
        adc_val = read_adc(CHANNEL0);// 0 - 1023
        
        // compress the value into the range 0 - 99
        adc_val = adc_val / 10.25; // 00 - 99
        
        //Convert the speed into string
        speed[0] = (adc_val / 10) + '0';
        speed[1] = (adc_val % 10) + '0';
        speed[2] = '\0';
        
        if (event_changed)
        {
            store_event(clock_reg, event[pos], speed);

            event_changed = 0;
        }
        
        //display in clcd
        clcd_print(speed, LINE2(14));
        
        
}
/*================================ PASSWORD SCREEN ========================================*/

/*Password Screen it allows only 3 attempts, On failure it holds user for 1 minute */
void password(unsigned char key)
{
    //Static varibles preserves value between function calls
    static unsigned char screen_started = 0;
    static unsigned char i = 0;
    static unsigned char attempts = 3;
    static unsigned char blink_state = 0;
    static unsigned int blink_count = 0;
    static char user_password[5];


    if (!screen_started)
    {
        clear_clcd();
        clcd_print("ENTER PASSWORD", LINE1(0));

        for (i = 0; i < 4; i++)
        {
            user_password[i] = '\0';
            clcd_putch('_', LINE2(3 + i));
        }

        i = 0;
        screen_started = 1;
        blink_state = 0;
        blink_count = 0;
    }
    /* EXIT from PASSWORD SCREEN */
    if(key == SW5)
    {
        clear_clcd();
        screen_started = 0;
        operation = 0;
        return;
    }
    /* Password input: SW1 = 1, SW2 = 0 */
    if ((key == SW1 || key == SW2) && i < 4)
    {
        user_password[i] = (key == SW1) ? '1' : '0';

        clcd_putch('*', LINE2(3 + i));
        i++;

        blink_count = 0;
        blink_state = 0;
    }

    /* Validate after four key presses */
    if (i == 4)
    {
        user_password[4] = '\0';

        /* upon success It stops the function and enables menu_screen operation */
        if (strcmp(user_password, original_password) == 0)
        {
            clear_clcd();
            clcd_print("LOGIN SUCCESS", LINE1(0));

            operation = 2;
            screen_started = 0;
            i = 0;
            return;
        }
        else // Wrong attempt detection
        {
            attempts--;

            clear_clcd();

            if (attempts > 0)
            {
                clcd_print("WRONG PASSWORD", LINE1(0));
                clcd_print("ATTEMPTS LEFT:", LINE2(0));
                clcd_putch(attempts + '0', LINE2(15));

                __delay_ms(1500);

                clear_clcd();
                clcd_print("ENTER PASSWORD", LINE1(0));

                for (i = 0; i < 4; i++)
                {
                    clcd_putch('_', LINE2(3 + i));
                }

                i = 0;
                blink_count = 0;
                blink_state = 0;
            }
            else /*using Timer2 for holding the screen for 1 minute*/
            {   
                sec = 60;
                allow = 0;

                TMR2 = 0;
                TMR2IF = 0;
                TMR2ON = 1;
                
                while(!allow)
                {
                clcd_print("ATTEMPTS OVER", LINE1(0));
                clcd_print("Try after : ", LINE2(0));
                clcd_putch((sec / 10) + '0', LINE2(11));
                clcd_putch((sec %10) + '0', LINE2(12));
                clcd_putch('s', LINE2(13));
                }
                
                clear_clcd();
                operation = 0;
                screen_started = 0;
                attempts = 3;
                i = 0;
                return;
            }
        }
    }

    /* Blink underscore at the next input position */
    if (i < 4)
    {
        if (++blink_count >= 500)
        {
            blink_count = 0;
            blink_state = !blink_state;

            clcd_putch(blink_state ? '_' : ' ', LINE2(3 + i));
        }
    }

    /* SW3 cancels login */
    if (key == SW3)
    {
        clear_clcd();
        operation = 0;
        screen_started = 0;
        i = 0;
        blink_count = 0;
    }
}
/*=========================  MENU SCREEN  ===================================*/


void menu(unsigned char key)
{
    static unsigned char menu_started = 0;
    static unsigned char selected = 0;

    char *menu_items[] =
    {
        "View Log",
        "Download Log",
        "Clear Log",
        "Set Time",
        "Change passwrd"
    };

    unsigned char top;
    unsigned char redraw = 0;

    char line1[17] = "                ";
    char line2[17] = "                ";

    /* Initialize menu */
    if (!menu_started)
    {
        clear_clcd();
        menu_started = 1;
        selected = 0;
        redraw = 1;
    }

    /* Scroll down */
    if (key == SW5 && selected < 4)
    {
        selected++;
        redraw = 1;
    }

    /* Scroll up */
    else if (key == SW4 && selected > 0)
    {
        selected--;
        redraw = 1;
    }

    /* Exit menu */
    else if (key == SW2)
    {
        clear_clcd();
        operation = 0;
        menu_started = 0;
        return;
    }

    /* Select menu item */
    else if (key == SW1)
    {
        switch (selected)
        {
            case 0: /* === VIEW LOG === */
                       menu_started = 0;
                       operation = 3;
                        break;

            case 1: /* === DOWNLOAD LOG === */
                   menu_started = 0;
                   operation = 4;
                   break;

            case 2: /* === CLEAR LOG === */
                      menu_started = 0;
                      operation = 5;
                      break;
            case 3:
                menu_started = 0;
                operation = 6;
                break;

            case 4:/*===  CHANGE PASSWORD ===*/
                menu_started = 0;
                operation = 7;
                break;
        }

        return;
    }

    /* Update LCD only when required */
    if (redraw)
    {
        top = (selected == 0) ? 0 : selected - 1;
        
        
          clear_clcd();

        /* Prepare LCD lines */
        line1[0] = (selected == top) ? '*' : ' ';
        line2[0] = (selected == top + 1) ? '*' : ' ';

        strncpy(line1 + 2, menu_items[top], 15);
        strncpy(line2 + 2, menu_items[top + 1], 15);

        clcd_print(line1, LINE1(0));
        clcd_print(line2, LINE2(0));
    }
}

/*=================== EEPROM INITIALIZATION =======================*/
void init_event_log(void)
{
    log_head = read_external_eeprom(LOG_HEAD_ADDR);
    log_count = read_external_eeprom(LOG_COUNT_ADDR);

    if (log_head >= MAX_LOGS || log_count > MAX_LOGS)
    {
        log_head = 0;
        log_count = 0;

        write_external_eeprom(LOG_HEAD_ADDR, log_head);
        write_external_eeprom(LOG_COUNT_ADDR, log_count);
    }
}
/*=== INITIALIZE PASSWORD ===*/
void init_password(void)
{
    unsigned char i;
    unsigned char marker;

    marker = read_external_eeprom(PASSWORD_MARKER_ADDR);

    if (marker == PASSWORD_MARKER)
    {
        /* Load previously saved password */
        for (i = 0; i < 4; i++)
        {
            original_password[i] = read_external_eeprom(PASSWORD_ADDR + i);
        }

        original_password[4] = '\0';
    }
    else
    {
        /* Set default password for first-time initialization */
        original_password[0] = '1';
        original_password[1] = '1';
        original_password[2] = '0';
        original_password[3] = '0';
        original_password[4] = '\0';

        /* Save default password */
        for (i = 0; i < 4; i++)
        {
            write_external_eeprom(PASSWORD_ADDR + i,
                                  original_password[i]);
        }

        /* Write marker only after saving the password */
        write_external_eeprom(PASSWORD_MARKER_ADDR,
                              PASSWORD_MARKER);
    }
}

/* ============== BCD TO ASCII ==================*/
void bcd_to_ascii(unsigned char value, char *buffer)
{
    buffer[0] = ((value >> 4) & 0x0F) + '0';
    buffer[1] = (value & 0x0F) + '0';
}

/*=============== STORE EVENT IN EEPROM ======================*/
void store_event(unsigned char *clock, char *event, char *speed)
{
    unsigned char address;
    unsigned char i;

    char log[LOG_SIZE];

    /* Store time */
    bcd_to_ascii(clock[0], &log[0]);
    log[2] = ':';

    bcd_to_ascii(clock[1], &log[3]);
    log[5] = ':';

    bcd_to_ascii(clock[2], &log[6]);

    /* Store event */
    log[8] = event[0];
    log[9] = event[1];

    /* Store speed */
    log[10] = speed[0];
    log[11] = speed[1];

    /* Calculate EEPROM address */
    address = log_head * LOG_SIZE;

    /* Write record */
    for (i = 0; i < LOG_SIZE; i++)
    {
        write_external_eeprom(address + i, log[i]);
    }

    /* Update circular buffer */
    log_head = (log_head + 1) % MAX_LOGS;

    if (log_count < MAX_LOGS)
    {
        log_count++;
    }

    /* Update EEPROM metadata */
    write_external_eeprom(LOG_HEAD_ADDR, log_head);
    write_external_eeprom(LOG_COUNT_ADDR, log_count);
}

/*============= VIEW LOG ======================*/
void read_log(unsigned char index)
{
    unsigned char slot;
    unsigned char address;
    unsigned char j;

    slot = (log_head + MAX_LOGS - log_count + index) % MAX_LOGS;

    address = slot * LOG_SIZE;

    for (j = 0; j < LOG_SIZE; j++)
    {
        log[j] = read_external_eeprom(address + j);
    }

    log[LOG_SIZE] = '\0';
}

void display_log(unsigned char index)
{
    char line1[] = "LOG TIME  EV SP";
    char line2[] = "01 12:00:00 ON00";

    unsigned char number = index + 1;

    line2[0] = (number / 10) + '0';
    line2[1] = (number % 10) + '0';

    line2[3]  = log[0];
    line2[4]  = log[1];
    line2[5]  = log[2];
    line2[6]  = log[3];
    line2[7]  = log[4];
    line2[8]  = log[5];
    line2[9]  = log[6];
    line2[10] = log[7];

    line2[12] = log[8];
    line2[13] = log[9];

    line2[14] = log[10];
    line2[15] = log[11];

    clcd_print(line1, LINE1(0));
    clcd_print(line2, LINE2(0));
}

void view_log(unsigned char key)
{
    static unsigned char screen_started = 0;
    static unsigned char index = 0;

    if (screen_started == 0)
    {
        clear_clcd();

        index = 0;
        screen_started = 1;

      if (log_count == 0)
      {
        clcd_print("NO LOGS FOUND", LINE1(1));
        clcd_print("SW5: BACK", LINE2(3));
        return;
      }

        read_log(index);
        display_log(index);
    }

    if (log_count == 0)
    {
        if (key == SW5)
        {
            clear_clcd();
            screen_started = 0;
            operation = 2;
        }

        return;
    }

    if (key == SW1)
    {
        if (index > 0)
        {
            index--;
            read_log(index);
            display_log(index);
        }
    }
    else if (key == SW2)
    {
        if (index < log_count - 1)
        {
            index++;
            read_log(index);
            display_log(index);
        }
    }
    else if (key == SW5)
    {
        clear_clcd();
        screen_started = 0;
        operation = 2;
    }
}

/*================ DOWNLOAD LOG =================*/

void download_log(void)
{
    unsigned char i;

    static unsigned char download_started = 0;

    (void)key;

    if (download_started == 0)
    {
        download_started = 1;

        clear_clcd();

        /* Check whether logs are available */
        if (log_count == 0)
        {
            clcd_print("NO LOGS FOUND", LINE1(1));
            clcd_print("RETURNING MENU", LINE2(1));

            __delay_ms(1500);

            clear_clcd();

            download_started = 0;
            operation = 2;

            return;
        }

        /* Display download status */
        clcd_print("DOWNLOADING...", LINE1(0));

        /* Send heading to Tera Term */
        uart_puts("\r\n");
        uart_puts("LOG  TIME      EV  SP\r\n");
        uart_puts("----------------------\r\n");

        /* Read and transmit all EEPROM records */
        for (i = 0; i < log_count; i++)
        {
            read_log(i);

            /* Log number */
            uart_putc((i + 1) / 10 + '0');
            uart_putc((i + 1) % 10 + '0');
            uart_putc(' ');

            /* Time: HH:MM:SS */
            uart_putc(log[0]);
            uart_putc(log[1]);
            uart_putc(':');
            uart_putc(log[3]);
            uart_putc(log[4]);
            uart_putc(':');
            uart_putc(log[6]);
            uart_putc(log[7]);

            uart_puts("  ");

            /* Event */
            uart_putc(log[8]);
            uart_putc(log[9]);

            uart_puts("  ");

            /* Speed */
            uart_putc(log[10]);
            uart_putc(log[11]);

            uart_puts("\r\n");
        }
              
        /* Download completion message */
        uart_puts("----------------------\r\n");
        
        uart_puts("Download Completed\r\n");

        /* Show success on CLCD */
        clear_clcd();

        clcd_print("DOWNLOAD", LINE1(3));
        clcd_print("SUCCESS", LINE2(4));

        __delay_ms(2000);

        /* Return to menu */
        clear_clcd();

        download_started = 0;
        operation = 2;
    }
}

/*================ CLEAR LOG =================*/

void clear_log(void)
{
    /* Reset log variables */
    log_head = 0;
    log_count = 0;
    eventcount = 0;

    /* Update EEPROM metadata */
    write_external_eeprom(LOG_HEAD_ADDR, 0);
    write_external_eeprom(LOG_COUNT_ADDR, 0);

    /* Display success message */
    clear_clcd();

    clcd_print("CLEAR LOG", LINE1(3));
    clcd_print("SUCCESSFUL", LINE2(3));

    __delay_ms(2000);

    /* Return to menu */
    clear_clcd();
    operation = 2;
}


/* =================== SET TIME ========================= */


void set_time(unsigned char key)
{
    static unsigned char screen_started = 0;
    static unsigned char selected = 0;
    static unsigned char digits[6];
    static unsigned char blink_state = 0;

    unsigned char clock_reg[3];
    unsigned char i;
    unsigned char max_value;

    unsigned char display_pos[6] = {0, 1, 4, 5, 8, 9};

    char line1[] = "HH: MM: SS";
    char line2[] = "00: 00: 00";

    /* Initialize screen and read current RTC time */
    if (!screen_started)
    {
        get_time(clock_reg);

        /* Convert BCD to individual decimal digits */

        digits[0] = (clock_reg[0] >> 4) & 0x03;
        digits[1] = clock_reg[0] & 0x0F;

        digits[2] = (clock_reg[1] >> 4) & 0x07;
        digits[3] = clock_reg[1] & 0x0F;

        digits[4] = (clock_reg[2] >> 4) & 0x07;
        digits[5] = clock_reg[2] & 0x0F;

        selected = 0;
        blink_state = 0;
        screen_started = 1;

        clear_clcd();
    }

    /* SW5: Exit without saving */
    if (key == SW5)
    {
        clear_clcd();

        screen_started = 0;
        selected = 0;

        operation = 2;
        return;
    }

    /* SW4: Save time to RTC */
    if (key == SW4)
    {
        write_ds1307(HOUR_ADDR,(digits[0] << 4) | digits[1]);

        write_ds1307(MIN_ADDR,(digits[2] << 4) | digits[3]);

        write_ds1307(SEC_ADDR,(digits[4] << 4) | digits[5]);

        clear_clcd();

        clcd_print("TIME UPDATED", LINE1(2));
        clcd_print("SUCCESSFULLY", LINE2(2));

        __delay_ms(1500);

        clear_clcd();

        screen_started = 0;
        selected = 0;
        operation = 2;

        return;
    }

    /* SW1: Increment selected digit */
    if (key == SW1)
    {
        switch (selected)
        {
            case 0:
                max_value = 2;
                break;

            case 1:
                max_value = (digits[0] == 2) ? 3 : 9;
                break;

            case 2:
            case 4:
                max_value = 5;
                break;

            case 3:
            case 5:
                max_value = 9;
                break;
        }

        if (digits[selected] < max_value)
        {
            digits[selected]++;
        }
        else
        {
            digits[selected] = 0;
        }

        /* Maintain valid 24-hour format */
        if (selected == 0 && digits[0] == 2 && digits[1] > 3)
        {
            digits[1] = 0;
        }
    }

    /* SW2: Move to next digit */
    else if (key == SW2)
    {
        selected++;

        if (selected >= 6)
        {
            selected = 0;
        }
    }

    /* Prepare LCD display */
    for (i = 0; i < 6; i++)
    {
        line2[display_pos[i]] = digits[i] + '0';
    }

    /* Blink selected digit */
    blink_state = !blink_state;

    if (blink_state)
    {
        line2[display_pos[selected]] = ' ';
    }

    clcd_print(line1, LINE1(0));
    clcd_print(line2, LINE2(0));

    __delay_ms(500);
}



/* ================= CHANGE PASSWORD ====================*/

void change_password(unsigned char key)
{
    static unsigned char stage = 0;
    static unsigned char i = 0;
    static unsigned char screen_started = 0;

    static char current_password[5];
    static char new_password[5];
    static char confirm_password[5];

    unsigned char j;

    /* Initialize screen */
    if (!screen_started)
    {
        clear_clcd();

        stage = 0;
        i = 0;
        screen_started = 1;

        clcd_print("CURRENT PASSWORD", LINE1(0));

        for (j = 0; j < 4; j++)
            clcd_putch('_', LINE2(3 + j));
    }

    /* SW5: Exit without saving */
    if (key == SW5)
    {
        clear_clcd();

        screen_started = 0;
        stage = 0;
        i = 0;

        operation = 2;
        return;
    }

    /* Stage 0: Enter current password */
    if (stage == 0)
    {
        if ((key == SW1 || key == SW2) && i < 4)
        {
            current_password[i] = (key == SW1) ? '1' : '0';

            clcd_putch('*', LINE2(3 + i));
            i++;
        }

        if (i == 4)
        {
            current_password[4] = '\0';

            if (strcmp(current_password, original_password) == 0)
            {
                stage = 1;
                i = 0;

                clear_clcd();
                clcd_print("NEW PASSWORD", LINE1(2));

                for (j = 0; j < 4; j++)
                    clcd_putch('_', LINE2(3 + j));
            }
            else
            {
                clear_clcd();
                clcd_print("WRONG PASSWORD", LINE1(1));
                clcd_print("TRY AGAIN", LINE2(3));

                __delay_ms(1500);

                stage = 0;
                i = 0;

                clear_clcd();
                clcd_print("CURRENT PASSWORD", LINE1(0));

                for (j = 0; j < 4; j++)
                    clcd_putch('_', LINE2(3 + j));
            }
        }
    }

    /* Enter new password */
    else if (stage == 1)
    {
        if ((key == SW1 || key == SW2) && i < 4)
        {
            new_password[i] = (key == SW1) ? '1' : '0';

            clcd_putch('*', LINE2(3 + i));
            i++;
        }

        if (i == 4)
        {
            new_password[4] = '\0';

            stage = 2;
            i = 0;

            clear_clcd();
            clcd_print("CONFIRM PASSWORD", LINE1(0));

            for (j = 0; j < 4; j++)
                clcd_putch('_', LINE2(3 + j));
        }
    }

    /* Confirm new password */
    else if (stage == 2)
    {
        if ((key == SW1 || key == SW2) && i < 4)
        {
            confirm_password[i] = (key == SW1) ? '1' : '0';

            clcd_putch('*', LINE2(3 + i));
            i++;
        }

        if (i == 4)
        {
            confirm_password[4] = '\0';

            if (strcmp(new_password, confirm_password) == 0)
            {
                stage = 3;
                i = 0;

                clear_clcd();
                clcd_print("SW4: SAVE", LINE1(2));
                clcd_print("SW5: DISCARD", LINE2(1));
            }
            else
            {
                clear_clcd();
                clcd_print("PASSWORD", LINE1(3));
                clcd_print("NOT MATCHED", LINE2(2));

                __delay_ms(1500);

                stage = 1;
                i = 0;

                clear_clcd();
                clcd_print("NEW PASSWORD", LINE1(2));

                for (j = 0; j < 4; j++)
                    clcd_putch('_', LINE2(3 + j));
            }
        }
    }

    /* Save or discard */
    else if (stage == 3)
    {
        /* SW4: Save password */
        if (key == SW4)
        {
            strcpy(original_password, new_password);

            /* Save to external EEPROM */
            for (j = 0; j < 4; j++)
            {
                write_external_eeprom(PASSWORD_ADDR + j, original_password[j]);
            }

            write_external_eeprom(PASSWORD_MARKER_ADDR, PASSWORD_MARKER);

            clear_clcd();
            clcd_print("PASSWORD", LINE1(3));
            clcd_print("SAVED SUCCESS", LINE2(2));

            __delay_ms(1500);

            clear_clcd();

            screen_started = 0;
            stage = 0;
            i = 0;

            operation = 2;
            return;
        }
    }
}
