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
  char str[50];
  uint16_t u8bits, u10bits, tension;
  static bool FlagMuestra = false;
  while (1)
  {
    switch (caracterRX)
    {
    case 'O':
      FlagMuestra = false;
      descomponer(adc_read(0) >> 2, 0);
      u8bits = adc_read(0) >> 2;
      sprintf(&str[0], "8bit adc: %d \n", u8bits);
      print(&str[0]);
      break;
    case 'D':
      FlagMuestra = false;
      descomponer(adc_read(0), 0);
      u10bits = adc_read(0);
      sprintf(&str[0], "10bit adc: %d \n", u10bits);
      print(&str[0]);
      break;
    case 'T':
      FlagMuestra = false;
      descomponer((uint32_t)adc_read(0) * 5000 / 1024, 4);
      tension = (uint32_t)adc_read(0) * 5000 / 1024;
      sprintf(&str[0], "%dmV          \n", tension);
      print(&str[0]);
      break;
    case 'V':
      FlagMuestra = false;
      valoresCadc(adc_read(0));
      memset(&str, '_', 14);
      str[15] = 0;
      for (size_t i = 0; i < ((uint32_t)adc_read(0) * 16 / 1024 ); i++)
      {
        memset(&str,'|',i);
        str[17] = 0;
      }
      print(&str[0]);
      print("\n");
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
