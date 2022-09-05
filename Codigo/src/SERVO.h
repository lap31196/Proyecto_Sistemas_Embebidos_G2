#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define PWM_TOP (39999u)
#define SERVO_MIN (1999u)
#define SERVO_MAX (4999u)

volatile static uint8_t update_pwm_ready = 0; // Se lo usara cuando se quiera actualizar el PWM

void pwm_iniciar(void)
{
    DDRB |= (1 << DDB1);                   // Port B0 como OUTPUT
    TIMSK1 = (1 << TOIE1) | (1 << OCIE1A); // Se obtiene un interrupt en el final del ancho de pulso y en el punto donde cambia el pulso

    OCR1AH = (SERVO_MIN & 0xFF00) >> 8;
    OCR1AL = (SERVO_MIN & 0x00FF);

    ICR1H = (PWM_TOP & 0xFF00) >> 8;
    ICR1L = (PWM_TOP & 0x00FF);

    TCCR1A = (0b10 << COM1A0) | (0b00 << COM1B0) | (0b10 << WGM10); // Clear on Compare match
    TCCR1B = (0b11 << WGM12) | (0b010 << CS10);
}

static void update_pwm(uint16_t i)
{
    update_pwm_ready = 1;
    while (update_pwm_ready != 0)
    {
        OCR1AH = (i & 0xFF00) >> 8;
        OCR1AL = (i & 0x00FF);
    }
}

void pwm_sweep(void)
{
    uint16_t i = 0;
    for (i = SERVO_MIN; i <= SERVO_MAX; i += 50)
    {
        update_pwm(i);
        _delay_ms(40);
    }
    for (i = SERVO_MAX; i >= SERVO_MIN; i -= 50)
    {
        update_pwm(i);
        _delay_ms(40);
    }
}
void servo_set(uint16_t deg, uint16_t max_deg)
{
    float set = (float)deg / (float)max_deg;                                // Rango de los angulos
    set = (((float)SERVO_MAX - (float)SERVO_MIN) * set) + (float)SERVO_MIN; // Rango de los valores de PWM
    uint16_t point = (uint16_t)set;
    update_pwm(point);
}

ISR(TIMER1_OVF_vect) // Timer Overflow Vector
{
    update_pwm_ready = 0;
}
ISR(TIMER1_COMPA_vect) // Timer Compare Vector
{
}