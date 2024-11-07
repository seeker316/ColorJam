#define F_CPU 16000000UL //defining clock speed
#define BAUD 9600 //defining baud rate (bits per second)

#include <avr/io.h>
#include <util/delay.h>
#include "lcd_debug.h"
#define CLK_BIT PORTD4
#define BAUD_PRESCALER ((F_CPU / (2UL *BAUD)) - 1) //calculating baud prescaler value to define the baudrate value in the UBRR register

void init() //function to initialize USART communication
{   
    //The Baudrate prescaler is a 12-bit value with 4 upper bits in the UBBR0H register and the remaining 8 lower bits 
    // stored in the UBBR0L register.
    UBRR0H = BAUD_PRESCALER >> 8;
    UBRR0L =  BAUD_PRESCALER;
    DDRD |= (1 << CLK_BIT);

    UCSR0C = (1<<UMSEL00) | (0<<UPM00) | (0<<USBS0) | (3<<UCSZ00);

    //enabling reciever and transmiter for in UCSR0B register.
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << UCSZ02 ) ;
}
void transmit(uint8_t data)
{
    while(!(UCSR0A & (1 << UDRE0))); //wait loop until data register empty flag is set
    
    UCSR0B &= ~(1<<TXB80);
    
    if(data & 0x0100)
      UCSR0B |= (1<<TXB80);

    UDR0 = data; // placing data in data register.
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

  init();
  lcd_init();

  ADMUX = 0x45;
  ADCSRA = 0xC7;
  ADCSRB = 0x00; 

  int counter = 0;
  int sum = 0;
  int sound_level = 0;
  float avg;

  while (1)
  {
    while(counter < 250)
    {
      int raw = sensor_read();
      
      counter++;
      sum = sum + raw;
      avg = sum / counter;
      sound_level = raw - avg;
      
      transmit(raw);
      _delay_ms(100);
    
    }
    counter = 0;
    sum = 0;
    avg = 0;

  }
        




}