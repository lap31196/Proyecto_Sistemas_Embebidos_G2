#include <avr/io.h>

#define UBRRVAL 103

void usart_init(void);
void sendbyte(unsigned char);
void sendstr(unsigned char *);
unsigned char receivebyte(void);
void receivestr(unsigned char *);

unsigned char onmsg[] = "ON/n";
unsigned char offmsg[] = "OFF/n";
unsigned char defaultmsg[] = "LED Status:/n";
unsigned char rxdata;

void usart_init(void)
{
    DDRB |= (1 << DDB5);                     // Port B5 es un OUTPUT
    UBRR0H = (unsigned char)(UBRRVAL >> 8);  // high byte
    UBRR0L = (unsigned char)UBRRVAL;         // low byte
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);   // Enable Transmitter and Receiver
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
}

void sendbyte(unsigned char MSG)
{
    while ((UCSR0A & (1 << UDRE0)) == 0)
        ; // Wait if a byte is being transmitted
    UDR0 = MSG;
}

int availiable()
{
    while ((UCSR0A & (1 << UDRE0)) == 0)
        ; // Wait if a byte is being transmitted
    return 1;
}

void sendstr(unsigned char *s)
{
    unsigned char i = 0;
    while (s[i] != '\0')
    {
        sendbyte(s[i]);
        i++;
    }
}

unsigned char receivebyte(void)
{
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}