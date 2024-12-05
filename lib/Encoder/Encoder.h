#ifndef __TEST_H_
#define __TEST_H_

#include <Arduino.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LARGO_VECTOR_SALIDA 180

    void Transistores(uint16_t display, uint16_t num);

    void multiplexado(uint8_t uni, uint8_t dec, uint8_t cent, uint8_t mil);

    void descomponer(uint16_t variable);

    void timer1(void);

    void initInterrupts(void);

#ifdef __cplusplus
}
#endif

#endif //__TEST_H_