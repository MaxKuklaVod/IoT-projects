#include <avr/io.h>
#include <avr/interrupt.h>

#define RX_PIN      PD2 
#define TX_PIN      PD3 

#define BUFFER_SIZE 64  

volatile char tx_buffer[BUFFER_SIZE];
volatile uint8_t tx_head = 0, tx_tail = 0;
volatile char rx_buffer[BUFFER_SIZE];
volatile uint8_t rx_head = 0, rx_tail = 0;
volatile uint16_t timer_ticks_per_bit, timer_ticks_per_half_bit;

enum TX_STATE { TX_IDLE, TX_START, TX_DATA, TX_STOP };
enum RX_STATE { RX_IDLE, RX_DATA, RX_STOP };
volatile TX_STATE tx_state = TX_IDLE;
volatile RX_STATE rx_state = RX_IDLE;
volatile uint8_t tx_byte_to_send, tx_bit_index;
volatile uint8_t rx_byte_received, rx_bit_index;

void uart_set_baudrate(int rate) {
    timer_ticks_per_bit = (16000000 / 8) / rate;
    timer_ticks_per_half_bit = timer_ticks_per_bit / 2;
}

void uart_send(char b) {
    uint8_t next_head = (tx_head + 1) % BUFFER_SIZE;
    while (next_head == tx_tail); 
    tx_buffer[tx_head] = b;
    tx_head = next_head;
    cli();
    if (tx_state == TX_IDLE) {
        tx_state = TX_START;
        TIMSK1 |= (1 << OCIE1A);
        OCR1A = TCNT1 + 10;
    }
    sei();
}

void uart_send_string(const char *msg) { while (*msg) uart_send(*msg++); }
uint8_t uart_available() { cli(); uint8_t head = rx_head, tail = rx_tail; sei(); return (head - tail + BUFFER_SIZE) % BUFFER_SIZE; }
char uart_read() { if (rx_head == rx_tail) return -1; char data = rx_buffer[rx_tail]; rx_tail = (rx_tail + 1) % BUFFER_SIZE; return data; }
bool uart_read_string(char *rx_data) { if (uart_available() == 0) return false; int i = 0; while(uart_available()) rx_data[i++] = uart_read(); rx_data[i] = '\0'; return true; }

ISR(TIMER1_COMPA_vect) {  
    switch (tx_state) {
        case TX_START: PORTD &= ~(1 << TX_PIN); tx_bit_index = 0; tx_state = TX_DATA; OCR1A += timer_ticks_per_bit; break;
        case TX_DATA: if (tx_bit_index < 8) { if ((tx_byte_to_send >> tx_bit_index) & 1) PORTD |= (1 << TX_PIN); else PORTD &= ~(1 << TX_PIN); tx_bit_index++; OCR1A += timer_ticks_per_bit; } else { tx_state = TX_STOP; } break;
        case TX_STOP: PORTD |= (1 << TX_PIN); tx_state = TX_IDLE; OCR1A += timer_ticks_per_bit; break;
        case TX_IDLE: default: if (tx_head != tx_tail) { tx_byte_to_send = tx_buffer[tx_tail]; tx_tail = (tx_tail + 1) % BUFFER_SIZE; tx_state = TX_START; } else { TIMSK1 &= ~(1 << OCIE1A); } break;
    }
}
ISR(TIMER1_COMPB_vect) { 
    switch(rx_state) {
        case RX_DATA: if (rx_bit_index < 8) { if (PIND & (1 << RX_PIN)) rx_byte_received |= (1 << rx_bit_index); rx_bit_index++; OCR1B += timer_ticks_per_bit; } else { rx_state = RX_STOP; } break;
        case RX_STOP: if (PIND & (1 << RX_PIN)) { uint8_t next_head = (rx_head + 1) % BUFFER_SIZE; if (next_head != rx_tail) { rx_buffer[rx_head] = rx_byte_received; rx_head = next_head; } } rx_state = RX_IDLE; break;
        case RX_IDLE: default: TIMSK1 &= ~(1 << OCIE1B); EIFR |= (1 << INTF0); EIMSK |= (1 << INT0); break;
    }
}
ISR(INT0_vect) { 
    EIMSK &= ~(1 << INT0); rx_state = RX_DATA; rx_bit_index = 0; rx_byte_received = 0; OCR1B = TCNT1 + timer_ticks_per_half_bit + timer_ticks_per_bit; TIMSK1 |= (1 << OCIE1B);
}

void setup() {
    uart_set_baudrate(9600);
  
    DDRD |= (1 << TX_PIN);  
    PORTD |= (1 << TX_PIN); 
    DDRD &= ~(1 << RX_PIN); 
    PORTD |= (1 << RX_PIN); 

    TCCR1A = 0; TCCR1B = (1 << CS11); 
    EICRA |= (1 << ISC01); EIMSK |= (1 << INT0);  
    
	Serial.begin(9600);
    Serial.println("Software UART Test. Type something to send to the echo board.");
    
    sei();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    uart_send(c);
  }

  if (uart_available()) {
    char c = uart_read();
    Serial.write(c);
  }
}