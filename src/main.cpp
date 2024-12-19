#include <Arduino.h>
#include "Encoder.c"
// put function declarations here:
#define set_bit(reg, bit) reg |= (1 << bit)
#define clear_bit(reg, bit) reg &= ~(1 << bit)

uint16_t time;
int main()
{
  timer1();
  adc_init();
  initUSART();
  config_IO();
  sei();

  static bool FlagMuestra = false;
  while (1)
  {
    switch (caracterRX)
    {
    case 'V':
      FlagMuestra = false;
      descomponer(adc_read(0) >> 2, 0);
      break;
    case 'O':
      FlagMuestra = false;
      descomponer(adc_read(0), 0);
      break;
    case 'D':
      FlagMuestra = false;
      descomponer((uint32_t)adc_read(0) * 5000 / 1024, 4);
      break;
    case 'T':
      FlagMuestra = false;
      valoresCadc(adc_read(0));
      break;

    default:
      if (FlagMuestra == false)
      {
        print("LE PIFIASTE AL COMANDO CAPO!\n");
        print("comandos validos: V, O , D ,T\n");
        FlagMuestra = true;
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
ISR(USART_RX_vect)
{
  caracterRX = UDR0; // UDR0 es el registro que transmite y recibe datos desde y hacia la USART
}
ISR(USART_UDRE_vect)
{

  UDR0 = caracterTX;
  UCSR0B &= ~(1 << UDRIE0); // deshabilito la interrupcion si no catarata de caracteres!!!
}
