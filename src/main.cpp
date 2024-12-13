#include <Arduino.h>
#include "Encoder.c"
// put function declarations here:
#define set_bit(reg, bit) reg |= (1 << bit)
#define clear_bit(reg, bit) reg &= ~(1 << bit)

#define botonE (PIND >> PD4 & 1)
uint16_t time;
int16_t angle = 10;
int main()
{
  
  timer1();
  initInterrupts();
  config_IO();
  sei();
  enum sexo_fernandez
  {
    seteo,
    star_stop,
    titilar
  };
  enum sexo_fernandez tiempo = seteo;
  uint16_t time_set = 0;
  uint8_t st = 0;
  uint8_t timi = 0;
  uint8_t e_a_bE;
  while (1)
  {
    switch (tiempo)
    {
    case seteo:
      angle = reloja(angle) % 100;
      if (botonE == 0)
      {
        tiempo = star_stop;
        time_set = reloja(angle);
      }
      break;
      
    case star_stop:
      descomponer(time_set);
      if (st)
      {
        if (time >= 10)
        {
          time_set = decrementar(time_set);
          time = 0;
        }
        if (time_set == 0)
        {
          tiempo = titilar;
          PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
        }
      }
      if (botonE == 1 && e_a_bE == 0) // deteccion de flanco bot1
      {
        st = ~st;
      }
      e_a_bE = botonE;
      break;
    case titilar:
      send_reg(0xFF);
      if (time >= 250)
      {
        PORTB ^= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
        timi++;
        time = 0;
      }
      if (timi >= 10)
      {
        tiempo = seteo;
        timi = 0;
      }

      break;
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
