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
    ram,
    star_stop,
    titilar
  };
  enum sexo_fernandez tiempo = seteo;
  uint16_t time_set = 0;
  uint8_t st = 0;
  uint8_t timi = 0;
  uint8_t e_a_bE;
  int indice_ram = 0;
  int16_t ram1[5];
  while (1)
  {
    switch (tiempo)
    {
    case seteo:
      angle = reloja(angle) % 100;
      if (botonE == 1 && e_a_bE == 0) // Detección de flanco de botón1
      {
        if (indice_ram < 5) // Verifica que no exceda los 5 valores
        {
          ram1[indice_ram] = reloja(angle);
          indice_ram++;
        }
        else
        {
          indice_ram = 0; // Reinicia el índice al llegar a 5
          tiempo = ram;   // Cambia al modo ram para recorrer los valores
        }
      }
      e_a_bE = botonE;
      break;

    case ram:
      descomponer(ram1[indice_ram]);  // Descompone el valor almacenado en ram1[indice_ram]
      if (boton1 == 1 && e_a_b1 == 0) // Detección del flanco de botón2
      {
        if (indice_ram < 4) // No exceder el tamaño del arreglo (0 a 4)
          indice_ram++;
        else
          indice_ram = 0; // Si llega al final, reinicia el índice
      }
      e_a_b1 = boton1;

      if (botonE == 1 && e_a_bE == 0) // Detección del flanco de botón1
      {
        time_set = ram1[indice_ram]; // Establece time_set con el valor almacenado
        tiempo = star_stop;          // Cambia al modo star_stop
      }
      e_a_bE = botonE;
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
        tiempo = ram;
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
  anti_reb1();
}
ISR(INT0_vect) // me interrumpio el PD2
{
  (PIND & 0b00001000) ? angle-- : angle++; // le pregunto el estado al PD3
}
ISR(INT1_vect) // me interrumpio el PD3
{
  (PIND & 0b00000100) ? angle++ : angle--; // le pregunto el estado al PD2
}
