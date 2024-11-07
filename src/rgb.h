#define F_CPU 16000000UL
#define LED_STRIP_PORT PORTD
#define LED_STRIP_DDR  DDRD
#define LED_STRIP_PIN  3
#define LED_COUNT 28

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>


volatile int choose = 0;
volatile float bright_in = 0; 
typedef struct rgb_color
{
  uint8_t red, green, blue;
} rgb_color;

void __attribute__((noinline)) led_strip_write(rgb_color * colors, uint16_t count)
{
  // Set the pin to be an output driving low.
  LED_STRIP_PORT &= ~(1<<LED_STRIP_PIN);
  LED_STRIP_DDR |= (1<<LED_STRIP_PIN);

  cli();   // Disable interrupts temporarily because we don't want our pulse timing to be messed up.
  while (count--)
  {
    asm volatile (
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0\n"
        "rcall send_led_strip_byte%=\n"  // Send red component.
        "ld __tmp_reg__, -%a0\n"
        "rcall send_led_strip_byte%=\n"  // Send green component.
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0+\n"
        "rcall send_led_strip_byte%=\n"  // Send blue component.
        "rjmp led_strip_asm_end%=\n"     // Jump past the assembly subroutines.

        "send_led_strip_byte%=:\n"
        "rcall send_led_strip_bit%=\n"  // Send most-significant bit (bit 7).
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"  // Send least-significant bit (bit 0).
        "ret\n"


        "send_led_strip_bit%=:\n"
#if F_CPU == 8000000
        "rol __tmp_reg__\n"                      // Rotate left through carry.
#endif
        "sbi %2, %3\n"                           // Drive the line high.

#if F_CPU != 8000000
        "rol __tmp_reg__\n"                      // Rotate left through carry.
#endif

#if F_CPU == 16000000
        "nop\n" "nop\n"
#elif F_CPU == 20000000
        "nop\n" "nop\n" "nop\n" "nop\n"
#elif F_CPU != 8000000
#error "Unsupported F_CPU"
#endif

        "brcs .+2\n" "cbi %2, %3\n"              // If the bit to send is 0, drive the line low now.

#if F_CPU == 8000000
        "nop\n" "nop\n"
#elif F_CPU == 16000000
        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
#elif F_CPU == 20000000
        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
        "nop\n" "nop\n"
#endif

        "brcc .+2\n" "cbi %2, %3\n"              // If the bit to send is 1, drive the line low now.

        "ret\n"
        "led_strip_asm_end%=: "
        : "=b" (colors)
        : "0" (colors),         // %a0 points to the next color to display
          "I" (_SFR_IO_ADDR(LED_STRIP_PORT)),   // %2 is the port register (e.g. PORTC)
          "I" (LED_STRIP_PIN)     // %3 is the pin number (0-8)
    );


  }
  sei();          // Re-enable interrupts now that we are done.
  _delay_us(80);  // Send the reset signal.
}


void bright(uint8_t *arr,int index,uint8_t level)
{ 

  level = level/32; //mapping senseor val between 0-7

  if(level > 3)
  {
        bright_in = 1 + (level*0.1);
  }
   else
   {
        bright_in = 1 - (level*0.1);
   }
        //   lcd_display_str("bright");
//   lcd_display_int(level);
  for (int j = 0; j < 3; j++)
  {   
    uint8_t *x = &(*((arr+index*3) + j));
//     lcd_display_int((*x)); 
    *x = (((*x)*bright_in) < 255) ? ((*x)*bright_in) : 255; //buzzinga
//     lcd_display_int((*x));        
  }
}

uint8_t get_random_0_to_2() {
    static uint16_t seed = 12345;  // Static seed to retain value between calls
    seed = (1103515245 * seed + 12345) % 32768;  // Update seed using LCG
    return seed % 3;  // Return value between 0 and 2
}


void set_color(uint8_t *color_list, uint8_t sound_level, int change)
{    
    if(true)
    { 
      if (change == 1)
      {
        choose = get_random_0_to_2();  
      }

      // Create a temporary copy of color_list
      uint8_t temp_color_list[3 * LED_COUNT];
      memcpy(temp_color_list, color_list, 3 * LED_COUNT * sizeof(uint8_t));

      rgb_color colors[LED_COUNT];

      // Pass the copied list to bright
      bright((uint8_t *)temp_color_list, choose, sound_level);

      for (uint16_t i = 0; i < LED_COUNT; i++)
      {
          colors[i] = (rgb_color){ (*((temp_color_list + choose * 3) + 0)), 
                                   (*((temp_color_list + choose * 3) + 1)), 
                                   (*((temp_color_list + choose * 3) + 2)) };
      }

      led_strip_write(colors, LED_COUNT);

      _delay_ms(20);
    }
}


void test(uint8_t sound_level)
{   
        uint8_t colorList[3][3] = {{122,17,16},{155,148,147},{205,25,21}};
        
        set_color((uint8_t *) colorList,sound_level,1);
}