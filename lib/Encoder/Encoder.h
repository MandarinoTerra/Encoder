#ifndef __TEST_H_
#define __TEST_H_

#include <Arduino.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LARGO_VECTOR_SALIDA 180

    void send_reg(uint8_t data);

    void Transistores(uint8_t display, uint8_t num, uint8_t punto);

    void multiplexado(uint8_t uni, uint8_t dec, uint8_t cent, uint8_t mil, uint8_t punto);

    void descomponer(uint16_t variable, uint8_t punto);

    void timer1(void);

    void initInterrupts(void);

#ifdef __cplusplus
}
#endif

#endif //__TEST_H_