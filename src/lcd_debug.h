#ifndef F_CPU
#define F_CPU 16000000UL  
#endif

#include <avr/io.h>   
#include <stdio.h>
#include <util/delay.h> 

#define LCD_DATA1 PORTC //PORTC (C0 - C4) lcd data pins
#define LCD_DATA2 PORTD //PORTD (D5 - D7) lcd data pins

#define DATA_DIR1 DDRC //Data direction register for pins on PortC
#define DATA_DIR2 DDRD //Data direction register for pins on PortD

#define LCD_CMD PORTB //PORTB (B0 - B2) lcd command pins RS, R/W, E
#define CMD_DIR DDRB //Data direction register for PORTB

void lcd_cmd(uint8_t reg, unsigned char cmd)
{   

    LCD_DATA1 = (0x1F & cmd); //masking the 3 upper bits 
    LCD_DATA2 = (0xE0 & cmd); //masking the 5 lower bits
    LCD_CMD &= ~(1 << PORTB0); //clearing the B0 bit
    LCD_CMD |= (reg << PORTB0); //setting lcd register select pin

    LCD_CMD &= ~(1 << PORTB1); 

    LCD_CMD |= (1 << PORTB2); //pulsing enable pin for executing instruction
    _delay_ms(1);
    LCD_CMD &= ~(1 << PORTB2);
    _delay_ms(1);

}

void lcd_init(void) //LCD initialization function
{   
    CMD_DIR |= (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2);
    DATA_DIR1 |= 0x1F; //configuring data and command registers for output for lcd data1
    DATA_DIR2 |= 0xE0; //configuring data and command registers for output for lcd data2
    

    
    _delay_ms(20);

    lcd_cmd(0,0x38); //setting lcd display to 8-bit mode

    lcd_cmd(0,0x0C); // // Display ON, Cursor OFF

    lcd_cmd(0,0x01); // clear display
    _delay_ms(2);

    lcd_cmd(0,0x06); // entry mode set : increment cursor

}

void lcd_display_int(uint8_t value)
{   
    char disp[9];
    sprintf(disp, "%u", value);
    char *str = disp;

    lcd_cmd(0,0x01); // clear display
    _delay_ms(2);

    while (*str)
    {
        lcd_cmd(1,*str);   
        _delay_ms(1);
        str++;
    }
    _delay_ms(100);
}

void lcd_display_char(uint8_t value)
{
    lcd_cmd(0,0x01); // clear display
    _delay_ms(2);
    lcd_cmd(1,value);   
    _delay_ms(300);
}


void lcd_display_str(char *str)
{   
    lcd_cmd(0,0x01); // clear display
    _delay_ms(2);
    while (*str)
    {
        lcd_cmd(1,*str);  
        _delay_ms(1);
        str++;
    }
    _delay_ms(500);
    
}


void lcd_display_bin(uint8_t value)
{   
    char disp_str[9];  // 8 bits + null terminator
    lcd_cmd(0, 0x01);  // Clear display
    _delay_ms(2);
    int index = 0;
    
    if (value == 0)
    {
        while (index < 8)
        {
            disp_str[index++] = '0';
        }
    }
    else
    {
        while(value > 0)
        {
            disp_str[index++] = (value % 2) ? '1' : '0';
            value /= 2;
        }
            
        while (index < 8)
        {
            disp_str[index++] = '0';
        }
            
    }

    disp_str[index] = '\0';

    // Reverse the string
    for (int i = 0; i < index / 2; i++) 
    {
        char temp = disp_str[i];
        disp_str[i] = disp_str[index - i - 1];
        disp_str[index - i - 1] = temp;
    }
    
    // Display the binary string on the LCD
    char *ptr = disp_str; 
    while (*ptr)
    {
        lcd_cmd(1, *ptr);   
        _delay_ms(1);
        ptr++;
    }
    
    _delay_ms(1000);
}
