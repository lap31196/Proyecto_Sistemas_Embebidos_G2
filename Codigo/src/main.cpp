#ifndef F_CPU
#define F_CPU 16000000UL // Frecuencia del controlador 16MHz
#endif
#define UBRRVAL 103

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ADC.h"
#include "SERVO.h"
#include "Bluetooth.h"

#include <ctype.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned int sensor_1, sensor_2, sensor_USB;
float voltaje_1, voltaje_2, voltaje_USB, corriente_USB;
float porcentaje_1, porcentaje_2;
float LDR_1, LDR_2;
float angulo_panel;
float angle = 90.0;
int state = 0; // Estado: "0"=automatico; "1"=manual

void setup()
{
  Serial.begin(4800);
  DDRC = 0x00;                      // Todos los pins del puerto C son INPUT
  DDRB = 0x00;                      // Todos los pins del puerto B son INPUT
  DDRB |= (1 << PINB5);             // Todos los pins del puerto B son INPUT
  PORTB |= (1 << PB0) | (1 << PB4); // Pin PB0 y PB2 es HIGH
  DDRC |= (1 << 4) | (1 << 5);      // El pin A4 y A5 son un OUTPUT
  PORTC |= (1 << 4) | (1 << 5);     // Los pines A4 y A5 son HIGH

  adc_referencia();
  adc_iniciar();

  pwm_iniciar();
  sei();

  usart_init();

  servo_set(90, 160);

  lcd.init();
  lcd.backlight();
}
void loop()
{
  if ((PINB & 0x01))
  {
    state = 1;
  }
  switch (state)
  {
  case 0:
    PORTB |= (0 << PB5);
    sensor_1 = adc_converter(0x40);
    voltaje_1 = (sensor_1 * 5.0) / 1023.0;
    LDR_1 = (sensor_1 * 40.0) / 1023.0;

    sensor_2 = adc_converter(0x41);
    voltaje_2 = (sensor_2 * 5.0) / 1023.0;
    LDR_2 = (sensor_2 * 35.0) / 1023.0;

    angulo_panel = 90.0 + LDR_1 - LDR_2;

    Serial.print("LDR_1: ");
    Serial.print(LDR_1);
    Serial.print(" LDR_2: ");
    Serial.print(LDR_2);
    Serial.print(" angulo_panel: ");
    Serial.println(angulo_panel);

    servo_set(angulo_panel, 180);

    sensor_USB = adc_converter(0x42);
    voltaje_USB = (sensor_USB * 5.0) / 1023.0;
    corriente_USB = (voltaje_USB / 20);

    lcd.setCursor(0, 0);
    lcd.print("Voltaje:");
    lcd.print(voltaje_USB);
    lcd.print("V");

    lcd.setCursor(0, 1);
    lcd.print("Corriente:");
    lcd.print(corriente_USB);
    lcd.print("mA");
    _delay_ms(15);

    break;

  case 1:
    PORTB |= (1 << PB5);
    rxdata = receivebyte();
    sensor_USB = adc_converter(0x42);
    voltaje_USB = (sensor_USB * 5.0) / 1023.0;
    corriente_USB = (voltaje_USB / 20);

    lcd.setCursor(0, 0);
    lcd.print("Voltaje:");
    lcd.print(voltaje_USB);
    lcd.print("V");

    lcd.setCursor(0, 1);
    lcd.print("Corriente:");
    lcd.print(corriente_USB);
    lcd.print("mA");

    if (rxdata == '1')
    {
      angle += 10.0;
      servo_set(angle, 160);
    }
    else if (rxdata == '2')
    {
      angle -= 10.0;
      servo_set(angle, 160);
    }
    else if (rxdata == '0')
    {
      rxdata = 0;
      state = 0;
      PORTB |= (0 << PB5);
    }
    break;

  default:
    break;
  }
}
