#include <avr/io.h>

#define SET(bit)        (PORTB |=  (1 << bit))
#define CLEAR(bit)      (PORTB &= ~(1 << bit))
#define TOGGLE(bit)     (PORTB ^=  (1 << bit))
#define READ(bit)       ((PINB  &  (1 << bit)) == 0)

#define LED_RED         PB1
#define LED_YELLOW      PB2
#define LED_GREEN       PB3
#define BUTTON          PB4
#define JUMPER          PB5

uint16_t readAnalog(void);
inline void delay(uint16_t ms);
void doAmpelThings(void);
void blink(uint16_t ms);

volatile uint8_t enableDelay = 0;

int main(void)
{
	/* Timer initialisieren */
	TCCR0A = 0x0;                  // Timer in normal mode initialisieren
	TCCR0B |= (1 << CS02);         // Pre-scaler: 256

	/* ADC initialisieren */
	ADMUX  |= (1 << REFS0) | (1 << MUX2);     // Referenzspannung AVcc und Kanal 4
	ADCSRA |= (1 << ADEN);                    // ADC Aktivieren

	/* GPIO initialisieren */
	DDRB  = 0b00001110;            // Ein und Ausgänge definieren
	PORTB = 0b00110000;            // Interne Pull-Up Widerstände aktivieren

	while (1)
	{
		if (READ(JUMPER))
		{
			if (READ(BUTTON))
			{
				enableDelay = 0;
				doAmpelThings();
				delay(150);
			}
			continue;
		}

		if (READ(BUTTON))
		{
			blink(readAnalog() << 2UL);     // Shift nach links um die Zeit zu vervielfachen => (t * 4)
		}
		else
		{
			enableDelay = 1;
			doAmpelThings();
		}
	}

	return 0;
}

uint16_t readAnalog(void)
{
	uint16_t val;
	ADCSRA |= (1 << ADSC);            // Konvertierung starten
	while (ADCSRA & (1 << ADSC));     // Warten bis Werte gemessen wurden
	val = ADCL;                       // Low lesen (Bits 1 bis 8)
	val |= (ADCH << 8);               // High lesen (Bit 9 und 10)
	return val;
}

inline void delay(uint16_t ms)
{
	while (ms--)
	{
		TCNT0 = 209;                  // Vorladen
		while (TCNT0);                // Auf Überlauf warten
	}
}

void doAmpelThings(void)
{
	static uint8_t step = 0;

	switch (step++)
	{
	case 0:
		SET(LED_RED);
		CLEAR(LED_YELLOW);
		CLEAR(LED_GREEN);
		if (enableDelay) delay(2400);
		break;
	case 1:
		SET(LED_YELLOW);
		if (enableDelay) delay(600);
		break;
	case 2:
		CLEAR(LED_RED);
		CLEAR(LED_YELLOW);
		SET(LED_GREEN);
		if (enableDelay) delay(2400);
		break;
	case 3:
		CLEAR(LED_GREEN);
		SET(LED_YELLOW);
		step = 0;
		if (enableDelay) delay(1000);
		break;
	default:
		break;
	}
}

void blink(uint16_t ms)
{
	CLEAR(LED_RED);
	CLEAR(LED_GREEN);
	TOGGLE(LED_YELLOW);
	delay(ms);
}
