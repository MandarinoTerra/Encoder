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
  adc_init();
  config_IO();
  sei();
  enum sexo_fernandez
  {
    u8bit,
    u10bit,
    miliv,
    vumetro
  };
  enum sexo_fernandez estado = vumetro;
  uint8_t e_a_bE;
  while (1)
  {
    switch (estado)
    {
    case u8bit:
      descomponer(adc_read(0) >> 2, 0);
      if (botonE == 1 && e_a_bE == 0)
      {
        estado = u10bit;
      }
      e_a_bE = botonE;
      break;
    case u10bit:
      descomponer(adc_read(0), 0);
      if (botonE == 1 && e_a_bE == 0)
      {
        estado = miliv;
      }
      e_a_bE = botonE;
      break;
    case miliv:
      descomponer((uint32_t)adc_read(0) * 5000 / 1024, 4);
      if (botonE == 1 && e_a_bE == 0)
      {
        estado = vumetro;
      }
      e_a_bE = botonE;
      break;
    case vumetro:
      valoresCadc(adc_read(0));
      if (botonE == 1 && e_a_bE == 0)
      {
        estado = u8bit;
      }
      e_a_bE = botonE;
      break;
    }
    _delay_ms(1);
  }
}
ISR(TIMER1_COMPA_vect)
{
  time++;
}
