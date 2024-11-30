#include <Arduino.h>
#include "Encoder.c"
#define set_bit(reg, bit) reg |= (1 << bit)
#define clear_bit(reg, bit) reg &= ~(1 << bit)
// put function declarations here:

uint16_t time;
int16_t angle = 10;
int main()
{
  timer1();
  initInterrupts();
  sei();
  set_bit(DDRB, PB5);
  set_bit(DDRB, PB0);
  set_bit(DDRB, PB1);
  set_bit(DDRB, PB2);
  set_bit(DDRB, PB3);
  uint8_t cont=0;
  while (1)
  {
    if (time >= 250)
    {
      time = 0;
      if (angle > 1000)
      {
        angle = 1000;
        if(cont < 0b00001111)
          cont++;
        
      }
      if (angle < 1000)
      {
        angle = 1000;
        if (cont > 0)
          cont--;
      }
      PORTB=(PORTB & 0b11110000) + cont;
    }
    _delay_ms(1);
  }

}
ISR(TIMER1_COMPA_vect)
{
  time++;
}
ISR(INT0_vect) // me interrumpio el PD2
{
  (PIND & 0b00001000) ? angle-- : angle++; // le pregunto el estado al PD3
}

ISR(INT1_vect) // me interrumpio el PD3
{
  (PIND & 0b00000100) ? angle++ : angle--; // le pregunto el estado al PD2
}
