#include <Encoder.h>
#include <Arduino.h>

#define set_bit(reg, bit) reg |= (1 << bit)
#define clear_bit(reg, bit) reg &= ~(1 << bit)

#define data_pin set_bit(DDRD, PD5); // DATA
#define latch_pin set_bit(DDRD, PD6) ;         // STC
#define clk_pin set_bit(DDRD, PD7);  // SHC
void send_reg(uint8_t data)
{
  // Comienza desde el bit más significativo
  for (uint8_t i = 0; i < 8; i++)
  {
    // Extrae el bit en la posición correspondiente
    if ((data & (1ULL << (7 - i))) != 0) // Cambiado para obtener el bit más significativo primero
      set_bit(PORTB, data_pin);
    else
      clear_bit(PORTB, data_pin);

    // Genera un pulso de reloj
    set_bit(PORTB, clk_pin);
    //_delay_ms(250);
    clear_bit(PORTB, clk_pin);
    //_delay_ms(250);
  }
  set_bit(PORTB, clk_pin);
  //_delay_ms(250);
  clear_bit(PORTB, clk_pin);
}
/* Esta funcion recibe un que display quiero prender y que numero mandarle*/
void Transistores(uint16_t display, uint16_t num)
{
  /*
  set_bit(DDRB, PB0); // uni
  set_bit(DDRB, PB1); // dece
  set_bit(DDRB, PB2); // cente
  set_bit(DDRB, PB3); // Mile
  */
  clear_bit(PORTB, PB0);
  clear_bit(PORTB, PB1);
  clear_bit(PORTB, PB2);
  clear_bit(PORTB, PB3);

  // wels

  if (display == 1)
    set_bit(PORTB, PB0);
  if (display == 2)
    set_bit(PORTB, PB1);
  if (display == 3)
    set_bit(PORTB, PB2);
  if (display == 4)
    set_bit(PORTB, PB3);
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
