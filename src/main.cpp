#include <Arduino.h>
#include "Encoder.c"
// put function declarations here:
#define set_bit(reg, bit) reg |= (1 << bit)
#define clear_bit(reg, bit) reg &= ~(1 << bit)
void pront(char *buff);
uint8_t tx_down(void);
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
  while (1)
  {
    switch (caracterRX)
    {
    case 'O':
      descomponer(adc_read(0) >> 2, 0);
      u8bits = adc_read(0) >> 2;
      sprintf(&str[0], "8bit adc: %d \n", u8bits);
      pront(&str[0]);
      break;
    case 'D':
      descomponer(adc_read(0), 0);
      u10bits = adc_read(0);
      sprintf(&str[0], "10bit adc: %d \n", u10bits);
      pront(&str[0]);
      break;
    case 'T':
      descomponer((uint32_t)adc_read(0) * 5000 / 1024, 4);
      tension = (uint32_t)adc_read(0) * 5000 / 1024;
      sprintf(&str[0], "%dmV          \n", tension);
      pront(&str[0]);
      break;
    case 'V':
      valoresCadc(adc_read(0));
      if (tx_down() == 0)
      {
        memset(&str, '_', 22);
        size_t i = (uint32_t)adc_read(0) * 22 / 1024;
        memset(&str, '|', i);

        str[17] = '\r';
        str[18] = '\n';
        str[19] = 0;
        pront(&str[0]);
      }
      break;
    case 23:
    break;
    default:
        pront(" LE PIFIASTE AL COMANDO CAPO!\n comandos validos: V, O , D ,T\n");
        caracterRX=23;

      break;
    }
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

size_t tx_index = 0;
char *data_ptr = 0;

void pront(char *buff)
{
  if (tx_index == 0)
  {
    if (buff[0])
    {
      data_ptr = buff;
      UDR0 = buff[0];
      tx_index = 1;
    }
  }
}
uint8_t tx_down(void)
{
  return tx_index;
}
ISR(USART_UDRE_vect)
{
  if (data_ptr)
  {
    if (data_ptr[tx_index])
    {
      UDR0 = data_ptr[tx_index];
      tx_index++;
    }
    else
      tx_index = 0;
  }
}
