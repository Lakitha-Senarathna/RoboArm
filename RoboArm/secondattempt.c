#define F_CPU 8000000UL 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#define adc_channel_1 0
#define adc_channel_2 1
#define adc_channel_3 2
#define adc_channel_4 3

#define SERVO1_DDR DDRD
#define SERVO2_DDR DDRD
#define SERVO3_DDR DDRB
#define SERVO4_DDR DDRB

#define SERVO1_PORT PORTD
#define SERVO2_PORT PORTD
#define SERVO3_PORT PORTB
#define SERVO4_PORT PORTB

#define SERVO1_PIN PD0
#define SERVO2_PIN PD1
#define SERVO3_PIN PB1
#define SERVO4_PIN PB2

volatile uint16_t servo1_pulse = 1500;
volatile uint16_t servo2_pulse = 1500;
volatile uint16_t servo3_pulse = 1500;
volatile uint16_t servo4_pulse = 1500;

volatile uint16_t servo1_low_chunks = 0;
volatile uint16_t servo2_low_chunks = 0;

volatile uint16_t servo1_low_rem = 0;
volatile uint16_t servo2_low_rem = 0;

volatile uint8_t servo1_state = 1;
volatile uint8_t servo2_state = 1;

void setADC(void){
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADEN);
}

void setTimer0(void){
	TCCR0A = (1 << WGM01);                  
    TCCR0B = (1 << CS01) | (1 << CS00);     
    OCR0A = 255;                          
    TIMSK0 = (1 << OCIE0A);
}

void setTimer1(void){
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    ICR1 = 19999;
}

void setTimer2(void){
	TCCR2A = (1 << WGM21);                  
    TCCR2B = (1 << CS21) | (1 << CS20);     
    OCR2A = 255;                            
    TIMSK2 = (1 << OCIE2A);
}

uint16_t readADC(uint8_t channel){
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

uint16_t adcMapping_servo1(uint16_t adcValue1){
	return 1000 + ((uint32_t)adcValue1 * 1000) / 1023;
}

uint16_t adcMapping_servo2(uint16_t adcValue2){
	return 1000 + ((uint32_t)adcValue2 * 1000) / 1023;
}

uint16_t adcMapping_servo3(uint16_t adcValue3){
	return 1000 + ((uint32_t)adcValue3 * 1000) / 1023;
}

uint16_t adcMapping_servo4(uint16_t adcValue4){
	return 1000 + ((uint32_t)adcValue4 * 1000) / 1023;
}

int main(void){

    SERVO1_DDR |= (1 << SERVO1_PIN);
    SERVO2_DDR |= (1 << SERVO2_PIN);
    SERVO3_DDR |= (1 << SERVO3_PIN);
    SERVO4_DDR |= (1 << SERVO4_PIN);

	setADC();
    setTimer0();
	setTimer1();
    setTimer2();

    //initial pulse width for servo1
    OCR0A = servo1_pulse/8;

    //initial pulse width for servo2
    OCR2A = servo2_pulse/8;

    //initial pulse width for servo3 and servo4
    OCR1A = servo3_pulse;
    OCR1B = servo4_pulse;

	sei();

	while(1){
		uint16_t adcJoint1 = readADC(adc_channel_1);
		uint16_t adcJoint2 = readADC(adc_channel_2);
		uint16_t adcJoint3 = readADC(adc_channel_3);
		uint16_t adcJoint4 = readADC(adc_channel_4);
		
        servo1_pulse = adcMapping_servo1(adcJoint1);
        servo2_pulse = adcMapping_servo2(adcJoint2);
        servo3_pulse = adcMapping_servo3(adcJoint3);
        servo4_pulse = adcMapping_servo4(adcJoint4);
	}
	
	return 0;
}

ISR(TIMER0_COMPA_vect){
    if (servo1_state == 1) {
        SERVO1_PORT &= ~(1 << SERVO1_PIN);
        servo1_state = 0;

        uint32_t rem_us = 20000 - (uint32_t)servo1_pulse;
        uint32_t rem_ticks = rem_us / 8; 

        servo1_low_chunks = (uint16_t)(rem_ticks / 256);
        servo1_low_rem = (uint8_t)(rem_ticks % 256);

        if (servo1_low_rem > 0){
            OCR0A = (uint8_t)(servo1_low_rem - 1);
        } 
        else{
            if(servo1_low_chunks > 0){
                OCR0A = 255;
                servo1_low_chunks--;
            } 
            else{
                OCR0A = (uint8_t)((servo1_pulse/8) - 1);
            }
        }
    } 
    else{
        if (servo1_low_chunks > 0) {
            OCR0A = 255;
            servo1_low_chunks--;
        } 
        else{
            SERVO1_PORT |= (1 << SERVO1_PIN);
            servo1_state = 1;
            OCR0A = (uint8_t)((servo1_pulse/8) - 1);
        }
    }
}

ISR(TIMER2_COMPA_vect){
    if (servo2_state == 1) {
        SERVO2_PORT &= ~(1 << SERVO2_PIN);
        servo2_state = 0;

        uint32_t rem_us = 20000 - (uint32_t)servo2_pulse;
        uint32_t rem_ticks = rem_us / 8; 

        servo2_low_chunks = (uint16_t)(rem_ticks / 256);
        servo2_low_rem = (uint8_t)(rem_ticks % 256);

        if (servo2_low_rem > 0){
            OCR2A = (uint8_t)(servo2_low_rem - 1);
        } 
        else{
            if(servo2_low_chunks > 0){
                OCR2A = 255;
                servo2_low_chunks--;
            } 
            else{
                OCR2A = (uint8_t)((servo2_pulse/8) - 1);
            }
        }
    } 
    else{
        if (servo2_low_chunks > 0) {
            OCR2A = 255;
            servo2_low_chunks--;
        } 
        else{
            SERVO2_PORT |= (1 << SERVO2_PIN);
            servo2_state = 1;
            OCR2A = (uint8_t)((servo2_pulse/8) - 1);
        }
    }
}
