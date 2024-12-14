#include <Encoder.h>
#include <Arduino.h>

#define set_bit(reg, bit) reg |= (1 << bit)
#define clear_bit(reg, bit) reg &= ~(1 << bit)

#define DATA_PIN_ PD5 // DATA
#define LATCH_PIN PD6 // STC
#define CLOCK_PIN PD7 // SHC
/* Esta funcion me configura el adc*/
void adc_init()
{
  // Configurar el prescaler a 128 (frecuencia ADC = 16 MHz / 128 = 125 kHz)
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

  // Seleccionar la referencia de voltaje como AVcc (5V con capacitor en AREF)
  ADMUX |= (1 << REFS0);

  // Habilitar el ADC
  ADCSRA |= (1 << ADEN);
}
/*
 * Esta funcion recibe un el canal de adc que quiero leer
 * y me devuelve el valor en 10bits
 */
uint16_t adc_read(uint8_t channel)
{
  // Asegurarse de que el canal esté en el rango válido (0-7)
  channel &= 0b00000111;

  // Seleccionar el canal configurando los bits MUX
  ADMUX = (ADMUX & 0xF8) | channel;

  // Iniciar la conversión
  ADCSRA |= (1 << ADSC);

  // Esperar a que la conversión termine (ADSC se pone en 0)
  while (ADCSRA & (1 << ADSC))
    ;

  // Retornar el valor convertido (10 bits)
  return ADC;
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
void Transistores(uint8_t display, uint8_t num, uint8_t punto)
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
    if (punto == 1)
      send_reg(1 + farfadox[num]);
    else
      send_reg(farfadox[num]);
    clear_bit(PORTB, PB0);
  }
  if (display == 2)
  {
    if (punto == 2)
      send_reg(1 + farfadox[num]);
    else
      send_reg(farfadox[num]);
    clear_bit(PORTB, PB1);
  }
  if (display == 3)
  {
    if (punto == 3)
      send_reg(1 + farfadox[num]);
    else
      send_reg(farfadox[num]);
    clear_bit(PORTB, PB2);
  }
  if (display == 4)
  {
    if (punto == 4)
      send_reg(1 + farfadox[num]);
    else
      send_reg(farfadox[num]);
    clear_bit(PORTB, PB3);
  }
}
/* Esta funcion recibe un numero descompuesto y los manda a el display*/
void multiplexado(uint8_t uni, uint8_t dec, uint8_t cent, uint8_t mil, uint8_t punto)
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
    Transistores(1, uni, punto);
    estado = deces;
    break;
  case deces:
    Transistores(2, dec, punto);
    estado = cente;
    break;
  case cente:
    Transistores(3, cent, punto);
    set_bit(PORTC, PC4);
    estado = mile;
    break;
  case mile:
    clear_bit(PORTC, PC4);
    Transistores(4, mil, punto);
    estado = unie;
    break;
  }
}
/* Esta funcion manda el numnero que le mande a los displays*/
void descomponer(uint16_t variable, uint8_t punto)
{
  static uint8_t unidad;
  static uint8_t decena;
  static uint8_t centena;
  static uint8_t unimil;
  unidad = variable % 10;
  decena = (variable % 100) / 10;
  centena = (variable / 100) % 10;
  unimil = variable / 1000;
  multiplexado(unidad, decena, centena, unimil, punto);
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
  clear_bit(DDRC, PC2); // boton 1
  set_bit(PORTC, PC2);

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

uint8_t numeros[4] = {0x00, 0x30, 0x79, 0xFF};

uint32_t total_ms = 0;
uint32_t tiempoV1 = 0, tiempoV2 = 0; // Timers Multiplexado

typedef enum parte
{
  miles,
  cienes,
  dieces,
  unos
} ParteNum_t;
ParteNum_t parte_num = miles;
void multiplexado_vumetro(uint8_t NumDisplay, uint8_t Dato)
{
  send_reg(numeros[Dato]);
  switch (NumDisplay)
  {
  case 0:
    PORTB &=~(1 << PB0);
    PORTB |= (1 << PB1);
    PORTB |= (1 << PB2);
    PORTB |= (1 << PB3);
    break;

  case 1:
    PORTB |= (1 << PB0);
    PORTB &=~(1 << PB1);
    PORTB |= (1 << PB2);
    PORTB |= (1 << PB3);
    break;
  case 2:
    PORTB |= (1 << PB0);
    PORTB |= (1 << PB1);
    PORTB &=~(1 << PB2);
    PORTB |= (1 << PB3);
    break;
  case 3:
    PORTB |= (1 << PB0);
    PORTB |= (1 << PB1);
    PORTB |= (1 << PB2);
    PORTB &=~(1 << PB3);
    break;
  }
}

void val(uint8_t mil, uint8_t centena, uint8_t decena, uint8_t unidad)
{
  switch (parte_num)
  {
  case miles:
    multiplexado_vumetro(0, mil);
    parte_num = cienes;
    break;
  case cienes:
    multiplexado_vumetro(1, centena);
    parte_num = dieces;
    break;
  case dieces:
    multiplexado_vumetro(2, decena);
    parte_num = unos;
    break;

  case unos:
    multiplexado_vumetro(3, unidad);
    parte_num = miles;
    break;
  }
}

void valoresCadc(uint16_t valorAD)
{
  switch (valorAD / 79)
  {
  case 0:
    val(0, 0, 0, 0);
    break;
  case 1:
    val(1, 0, 0, 0);
    break;
  case 2:
    val(2, 0, 0, 0);
    break;
  case 3:
    val(3, 0, 0, 0);
    break;
  case 4:
    val(3, 1, 0, 0);
    break;
  case 5:
    val(3, 2, 0, 0);
    break;
  case 6:
    val(3, 3, 0, 0);
    break;
  case 7:
    val(3, 3, 1, 0);
    break;
  case 8:
    val(3, 3, 2, 0);
    break;
  case 9:
    val(3, 3, 3, 0);
    break;
  case 10:
    val(3, 3, 3, 1);
    break;
  case 11:
    val(3, 3, 3, 2);
    break;
  case 12:
    val(3, 3, 3, 3);
    break;
  }
}