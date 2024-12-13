#include <Encoder.h>
#include <Arduino.h>

#define set_bit(reg, bit) reg |= (1 << bit)
#define clear_bit(reg, bit) reg &= ~(1 << bit)

#define DATA_PIN_ PD5 // DATA
#define LATCH_PIN PD6 // STC
#define CLOCK_PIN PD7 // SHC

/* Esta funcion decrementa un numero entre 0 y 20.00 en formato
de reloj, envia este numero descompuesto a los 4 displays */
uint16_t decrementar(uint16_t tiempo)
{
  uint8_t segundos;
  uint8_t minutos;
  segundos = tiempo % 100;
  minutos = tiempo / 100;
  if (segundos >= 1)
  {
    segundos--;
  }
  else
  {
    if (minutos >= 1)
    {
      minutos--;
      segundos = 59;
    }
  }
  return (minutos * 100) + segundos;
}
/* Esta funcion recibe el angulo de un encoder para setear un tiempo en un display cuadruple,
 retorna el mismo angle para igualarlo y evitar usar punteros de mierda */
int16_t reloja(int16_t angle)
{
  static uint8_t mins = 0;
  if (mins < 20)
  {
    if (angle > 59)
    {
      mins++;
      angle = 0;
    }
    if (angle < 0)
    {
      if (mins > 0)
      {
        angle = 59;
        mins--;
      }
      else
      {
        angle = 59;
        mins = 19;
      }
    }
  }
  else
  {
    mins = 0;
    angle = 0;
  }
  descomponer((mins * 100) + angle);
  return (mins * 100) + angle;
}
/* Esta funcion recibe un  uint8_t y lo manda al registro 74hc595*/
void send_reg(uint8_t data)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // Enviar el bit más bajo de 'data'
    if (data & (1 << i))
      PORTD |= (1 << DATA_PIN_); // Si el bit está a 1, ponemos el pin DATA en alto
    else
      PORTD &= ~(1 << DATA_PIN_); // Si el bit está a 0, ponemos el pin DATA en bajo
    // Pulsar el reloj para mover el dato al registro de desplazamiento
    PORTD |= (1 << CLOCK_PIN);  // Reloj en alto
    PORTD &= ~(1 << CLOCK_PIN); // Reloj en bajo
  }
  // Después de enviar todos los bits, ponemos el latch en alto para actualizar las salidas
  PORTD |= (1 << LATCH_PIN);  // Latch en alto
  PORTD &= ~(1 << LATCH_PIN); // Latch en bajo
}
/* Esta funcion recibe un que display quiero prender y que numero mandarle*/
void Transistores(uint8_t display, uint8_t num)
{
  static uint8_t farfadox[10] = {
      0b11111100, // 0'
      0b01100000, // 1
      0b11011010, // 2
      0b11110010, // 3
      0b01100110, // 4
      0b10110110, // 5
      0b10111110, // 6
      0b11100000, // 7
      0b11111110, // 8
      0b11110110  // 9
  };
  set_bit(PORTB, PB0);
  set_bit(PORTB, PB1);
  set_bit(PORTB, PB2);
  set_bit(PORTB, PB3);

  if (display == 1)
  {
    send_reg(farfadox[num]);
    clear_bit(PORTB, PB0);
  }
  if (display == 2)
  {
    send_reg(farfadox[num]);
    clear_bit(PORTB, PB1);
  }
  if (display == 3)
  {
    send_reg(1 + farfadox[num]);
    clear_bit(PORTB, PB2);
  }
  if (display == 4)
  {
    send_reg(farfadox[num]);
    clear_bit(PORTB, PB3);
  }
}
/* Esta funcion recibe un numero descompuesto y los manda a el display*/
void multiplexado(uint8_t uni, uint8_t dec, uint8_t cent, uint8_t mil)
{
  enum estados_posibles
  {
    unie,
    deces,
    cente,
    mile
  };
  static enum estados_posibles estado = unie;
  switch (estado)
  {
  case unie:
    Transistores(1, uni);
    estado = deces;
    break;
  case deces:
    Transistores(2, dec);
    estado = cente;
    break;
  case cente:
    Transistores(3, cent);
    set_bit(PORTC, PC4);
    estado = mile;
    break;
  case mile:
    clear_bit(PORTC, PC4);
    Transistores(4, mil);
    estado = unie;
    break;
  }
}
/* Esta funcion manda el numnero que le mande a los displays*/
void descomponer(uint16_t variable)
{
  static uint8_t unidad;
  static uint8_t decena;
  static uint8_t centena;
  static uint8_t unimil;
  unidad = variable % 10;
  decena = (variable % 100) / 10;
  centena = (variable / 100) % 10;
  unimil = variable / 1000;
  multiplexado(unidad, decena, centena, unimil);
}
/* Esta funcion configura las interrupciones para el encoder*/
void initInterrupts(void)
{
  EIMSK |= (1 << INT0);  // habilita interrupcion externa INT0 en PD2
  EIMSK |= (1 << INT1);  // habilita interrupcion externa INT1 en PD3
  EICRA |= (1 << ISC01); // configura flanco ascendente en interrupcion externa INT0
  EICRA |= (1 << ISC11); // configura flanco ascendente en interrupcion externa INT1
}
/* Esta funcion configura In&Out*/
void config_IO(void)
{
  clear_bit(DDRD, PB4);
  set_bit(PORTD, PD4);
  set_bit(DDRD, PD5); // DATA
  set_bit(DDRD, PD6); // STC
  set_bit(DDRD, PD7); // SHC
  set_bit(DDRB, PB5); // LED PRUEBA
  set_bit(DDRB, PB0); // uni
  set_bit(DDRB, PB1); // dece
  set_bit(DDRB, PB2); // cente
  set_bit(DDRB, PB3); // Mile
}
/* Esta funcion configura el timer1*/
void timer1(void)
{
  TCCR1A = 0b00000000; // CTC
  TCCR1B = 0b00001011; // clk/64
  TCCR1C = 0;
  OCR1A = 249;
  TIMSK1 = (1 << OCIE1A);
}
