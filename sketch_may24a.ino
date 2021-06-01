// **** TSOP is connected to port PB2 **** //////
#define MOUSE_SENSITIVITY 1
#define REPEAT_DELAY 220

//Set to 0 after finding your codes
#define DEBUG 1

//Specify your remote codes here:
#define REMOTE_1      0xFD00FF
#define REMOTE_2      0xFD807F
#define REMOTE_3      0xFD40BF
#define REMOTE_4      0xFD20DF
#define REMOTE_5      0xFDA05F
#define REMOTE_6      0xFD609F
#define REMOTE_7      0xFD10EF
#define REMOTE_8      0xFD906F
#define REMOTE_9      0xFD50AF
#define REMOTE_STAR   0xFD30CF
#define REMOTE_0      0xFDB04F
#define REMOTE_DIES   0xFD708F
#define REMOTE_UP     0xFD8877
#define REMOTE_LEFT   0xFD28D7
#define REMOTE_OK     0xFDA857
#define REMOTE_RIGHT  0xFD6897
#define REMOTE_DOWN   0x7ECC33

volatile uint8_t m = 0, tcnt = 0, startflag = 0;
uint32_t irdata = 0, keydata = 0 ;
bool pressed = false;
bool complete = false;

#include <avr/delay.h>
#include "TrinketHidCombo.h"


void setup() {
  //delay(30000);
  DDRB |= (1 << DDB1); //P1 (LED) OUT not used in sketch
  PORTB |= 1 << PB2; // a PB2 lift will not hurt.
  GIMSK |= 1 << INT0; //interrupt int0 enable
  MCUCR |= 1 << ISC00; //Any logical change on INT0 generates an interrupt request
  GTCCR |= 1 << PSR0; TCCR0A = 0;
  TCCR0B = (1 << CS02) | (1 << CS00); // divider /1024
  TIMSK = 1 << TOIE0; //interrupt Timer/Counter1 Overflow  enable
  TrinketHidCombo.begin(); // start the USB device engine and enumerate
}

void loop() {

  if (complete) { // if a code has been received
    if (keydata != 0) //if a code is new
    {
      Action(keydata);
      pressed = true;
    }
    complete = false;
    ms_delay(REPEAT_DELAY);// to balance repeating/input delay of the remote

  }
  else if (pressed)
  {
    digitalWrite(1, LOW);
    TrinketHidCombo.pressKey(0, 0);
    pressed = false;
  }
  else
  {
    _delay_ms(1);//restrain USB polling on empty cycles
    TrinketHidCombo.poll(); // check if USB needs anything done
  }

}

ISR (INT0_vect) {
  if (PINB & 1 << 2) { // If log1
    TCNT0 = 0;
  }
  else {
    tcnt = TCNT0; // If log0
    if (startflag) {
      if (30 > tcnt  && tcnt > 2) {
        if (tcnt > 15 && m < 32) {
          irdata |= (2147483648 >> m);
        }
        m++;
      }
    }
    else  startflag = 1;
  }
}
ISR (TIMER0_OVF_vect) {
  if (m)
    complete = true; m = 0; startflag = 0; keydata = irdata; irdata = 0; // if the index is not 0, then create an end flag
}

void ms_delay(uint16_t x) // USB polling delay function
{
  for (uint16_t m = 0; m < (x / 10); m++) {
    _delay_ms(10);
    TrinketHidCombo.poll();
  }
}

void Action(uint32_t keycode)
{
  switch (keycode)
  {
    case REMOTE_1:
      TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_UP);
      break;
    case REMOTE_2:
      TrinketHidCombo.pressMultimediaKey(MMKEY_MUTE);
      break;
    case REMOTE_3:
      TrinketHidCombo.pressMultimediaKey(MMKEY_LOUDNESS);
      break;
    case REMOTE_4:
      TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_DOWN);
      break;
    case REMOTE_5:
      TrinketHidCombo.pressMultimediaKey(MMKEY_PLAYPAUSE);
      break;
    case REMOTE_6:
      TrinketHidCombo.pressMultimediaKey(MMKEY_SCAN_NEXT_TRACK);
      break;
    case REMOTE_7:
      TrinketHidCombo.pressKey(0,KEYCODE_ESC);
      break;
    case REMOTE_8:
      TrinketHidCombo.pressKey(0,KEYCODE_SPACE);
      break;
    case REMOTE_9:
      TrinketHidCombo.pressMultimediaKey(MMKEY_SCAN_PREV_TRACK);
      break;
    case REMOTE_STAR:
      TrinketHidCombo.pressSystemCtrlKey(SYSCTRLKEY_POWER);
      break;
    case REMOTE_0:
      TrinketHidCombo.pressKey(0,KEYCODE_BACKSPACE);
      break;
    case REMOTE_DIES:
      TrinketHidCombo.pressSystemCtrlKey(SYSCTRLKEY_SLEEP);
      break;
    case REMOTE_UP:
      TrinketHidCombo.pressKey(0,KEYCODE_ARROW_UP);
      break;
    case REMOTE_LEFT:
      TrinketHidCombo.pressKey(0,KEYCODE_ARROW_LEFT);
      break;
    case REMOTE_OK:
      TrinketHidCombo.pressKey(0,KEYCODE_ENTER);
      break;
    case REMOTE_RIGHT:
      TrinketHidCombo.pressKey(0,KEYCODE_ARROW_RIGHT);
      break;
    case REMOTE_DOWN:
        TrinketHidCombo.pressKey(0,KEYCODE_ARROW_DOWN);
      break; 
      
  }
//      if(DEBUG)
//        TrinketHidCombo.println(keydata, HEX);
//      else
//        return;
  digitalWrite(1, HIGH);
}
