#ifndef ADC_ATMEGA_328_H_
#define ADC_ATMEGA_328_H_
#include <avr/io.h>

void adc_referencia();
void adc_iniciar();
uint16_t adc_converter(unsigned char valor);

void adc_referencia()
{
    ADMUX &= ~(1 << REFS1);
    ADMUX |= (1 << REFS0);
}

void adc_iniciar()
{
    ADCSRA |= (1 << ADEN);
    ADCSRA &= ~(1 << ADLAR);
    ADCSRA &= ~(1 << ADATE);
    ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
}

uint16_t adc_converter(unsigned char valor)
{
    ADMUX = valor;
    _delay_us(10);
    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADIF)) == 0)
        ;
    _delay_us(10);
    return ADCW;
}
#endif /* ADC_ATMEGA_328_H_ */