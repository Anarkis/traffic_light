#include <avr/interrupt.h>
#include <avr/sleep.h>

const int button = PB2;
const int green = PB3;
const int yellow = PB1;
const int red = PB0;

int buttonState;
int lastButtonState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long LONG_PRESS_TIME  = 1000;
unsigned long holdTime = 0;
unsigned long startPressed = 0;
unsigned long endPressed = 0;

enum light {
  GREEN,
  YELLOW,
  RED
};

enum light current_light;

void setup() {
  DDRB &= ~(1 << button);     //set PB2 as input
  PORTB |= (1 << button);     //activate pull-up resistor for PB2
  
  DDRB |= (1 << red);         //set PB0 as output RED
  DDRB |= (1 << green);       //set PB3 as output GREEN
  DDRB |= (1 << yellow);      //set PB4 as output YELLOW

  PORTB |= (1 << green);      // activate green light 

  GIMSK |= (1 << PCIE);       //Pin change interrupt enable
  PCMSK |= (1 << PCINT2);     //Pin change interrupt to 2nd pin PB2

  sei();                      // enable interrupts
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
  current_light = GREEN;
  
}

void loop() {
  int reading = PINB & (1<< button );

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  long  gap = (millis() - lastDebounceTime);

  if ( gap > debounceDelay) {
    
    if (reading != buttonState) {
      buttonState = reading;
      
      if (buttonState == LOW) {
        startPressed = millis();
          
        switch (current_light){
          case GREEN:
            PORTB &= ~(1 << green);
            PORTB |= (1 << yellow);
            PORTB &= ~(1 << red);
            current_light = YELLOW;
            break;
          case YELLOW:
            PORTB &= ~(1 << green);
            PORTB &= ~(1 << yellow);
            PORTB |= (1 << red);
            current_light = RED;
            break;
          case RED:
            PORTB |= (1 << green);
            PORTB &= ~(1 << yellow);
            PORTB &= ~(1 << red);
            current_light = GREEN;
            break;
        }
      }
      else {
        endPressed = millis();
        holdTime = endPressed - startPressed;

        if (holdTime > LONG_PRESS_TIME ) {
          PORTB &= ~(1 << green);
          PORTB &= ~(1 << yellow);
          PORTB &= ~(1 << red);
          sleep_mode();
        }
        
      }
    }
  }

  lastButtonState = reading;
}

ISR (PCINT0_vect)
{
  MCUCR &= ~(1 << SE);      //Disabling sleep mode inside interrupt routine
}
