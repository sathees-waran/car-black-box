/* 
 * File:   main.h
 * Author: 25014
 *
 * Created on 26 November, 2025, 5:53 PM
 */

#ifndef MAIN_H
#define	MAIN_H
#include "adc.h"
#include "timers.h"
#include "digital_keypad.h"
#include "clcd.h"
#include "i2c.h"
#include "ds1307.h"
#include "external_eeprom.h"
#include "uart.h"
#include <string.h>


void dashboard(unsigned char key);
void password(unsigned char key);
void menu(unsigned char key);


void clear_clcd(void);
extern unsigned char key, operation;
extern volatile unsigned int sec;
extern volatile unsigned char allow;

void init_event_log(void);
void init_password(void);
void set_time(unsigned char key);

void store_event(unsigned char *clock, char *event,char *speed);
void bcd_to_ascii(unsigned char value, char *buffer);

void view_log(unsigned char key);
void read_log(unsigned char index);
void display_log(unsigned char index);
void download_log(void);
void change_password(unsigned char key);
void clear_log(void);

#endif	/* MAIN_H */

