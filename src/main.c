#define F_CPU 16000000UL //defining clock speed
#define BAUD 9600 //defining baud rate (bits per second)

#include <avr/io.h>
#include <util/delay.h>
#include "lcd_debug.h"
#include <avr/interrupt.h>
#include <stdbool.h>
#include "rgb.h"

#define BAUD_PRESCALER ((F_CPU / (16UL *BAUD)) - 1) //calculating baud prescaler value to define the baudrate value in the UBRR register


volatile uint8_t colorList[3][3];
volatile uint8_t recieve_index;
volatile uint8_t ele_index;
uint8_t data;
uint8_t ele;
bool disp = false;

void init() //function to initialize USART communication
{   
    //The Baudrate prescaler is a 12-bit value with 4 upper bits in the UBBR0H register and the remaining 8 lower bits 
    // stored in the UBBR0L register.
    UBRR0H = BAUD_PRESCALER >> 8;
    UBRR0L =  BAUD_PRESCALER;


    UCSR0C = (0<<UMSEL00) | (0<<UPM00) | (0<<USBS0) | (3<<UCSZ00);

    //enabling reciever and transmiter for in UCSR0B register.
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

    sei();
}


uint8_t recieve()
{
    return UDR0;
}

ISR(USART_RX_vect) 
{   
    data = recieve();
    if(data == '-'){
        // lcd_display_str("Rx");
        ele_index++;
        // lcd_display_int(recieve_index);
        if(ele_index >= 3)
        {
            ele_index = 0;
            recieve_index++;
            if (recieve_index >= 3)
            {
                recieve_index = 0;
                disp = true;
            }

        }
    }
    else{
        ele = data - 48;
        colorList[recieve_index][ele_index] = colorList[recieve_index][ele_index]*10+ele;
        // lcd_display_int(ele);
    }
}

int sensor_read()
{ 
  int ADC5_U;
  int ADC5_L;
  int sensor_val;
  while (1)
  {
    if ((ADCSRA & 0x40) == 0)
    {
        ADC5_L = ADCL;
        ADC5_U = ADCH;
        sensor_val = (ADC5_U << 11) | (ADC5_L);
        ADCSRA |= 0x40;
        // lcd_display_str("sensor val : ");
        // lcd_display_int(sensor_val );
        
        return sensor_val;
    }
  }
}


void main()
{   
    lcd_init();
    init();
    // lcd_display_str("waiting");
    ADMUX = 0x45;
    ADCSRA = 0xC7;
    ADCSRB = 0x00;
    int raw;
    int counter = 0;
    uint16_t sum = 0;
    uint8_t sound_level = 0;
    float avg;

    // uint8_t colorList[3][3] = {{122,17,16},{155,148,147},{205,25,21}};////
    while (1)
    {   
        while(counter < 100)
        {
            raw = sensor_read();
            // sound_level = sensor_read();
            counter++;
            sum = sum + raw;
            avg = sum / counter;
            sound_level = raw - avg;
            
            // test(sound_level);///////
            // set_color((uint8_t *) colorList,sound_level,1);
            if(counter == 99)
            {
                set_color((uint8_t *) colorList,sound_level,1);
                // lcd_display_str("sound val : ");
            }
            else{
                set_color((uint8_t *) colorList,sound_level,0);
            }
            // lcd_display_str("sound val : ");
            // lcd_display_int(raw);
            // lcd_display_int(sum);
            // lcd_display_int(avg);
            // lcd_display_int(counter);
            // lcd_display_int(sound_level);    
            _delay_ms(50);
        }

        counter = 0;
        sum = 0;
        avg = 0;

        // if(disp)
        // {   
        //     lcd_display_str("Recieved ele");
        //     for(int j = 0; j <= 2 ; j++)
        //     {
        //         for(int k = 0; k <= 2; k++)
        //         {
        //             lcd_display_int(colorList[j][k]);
        //         }
        //     }
        //     disp = false;
        // }
    }
}