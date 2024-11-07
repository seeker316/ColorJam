#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#include "lcd_debug.h"

#define AUDIO_SENSOR PORTC5

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
        lcd_display_int(sensor_val );
        
        return sensor_val;
    }
  }

}

int main(void)
{ 
  lcd_init();
  ADMUX = 0x45;
  ADCSRA = 0xC7;
  ADCSRB = 0x00;  

  int raw;
  while (1)
  {
    raw = sensor_read();
      
  }

}
