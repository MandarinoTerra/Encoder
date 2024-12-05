#include <Arduino.h>
#include "Encoder.c"
// put function declarations here:

uint16_t time;
int16_t angle = 10;
int main()
{
  timer1();
  initInterrupts();
  config_IO();
  sei();
  uint8_t cont=0;
  while (1)
  {
    
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
