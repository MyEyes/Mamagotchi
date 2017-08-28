/*********************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 128x64 size display using I2C to communicate
  3 pins are required to interface (2 I2C and one reset)

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#define DISPLAY_PIN 10

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

enum state_t {
  LOGO,
  IDLE,
  SELECT,
  FOOD,
  DRINK,
  HEALTH,
  SPORT,
  STATUS
};

enum button_state_t
{
  DOWN = 0,
  UP = 1,
  PRESSED = 2,
  RELEASED = 3
};

int x = 0;
volatile int timer = 0;
state_t state = HEALTH;
state_t old_state = HEALTH;
volatile char awake = 1;
volatile char displayOff = false;

volatile button_state_t action_button;
int action_held=0;
volatile button_state_t mode_button;
int mode_held=0;

volatile long wdt_runs = 0;
ISR(WDT_vect)
{
  wdt_runs++;
}

void clear()
{
  display.fillRect(0, 0, SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT, 0);
}

void setup()   {
  Serial.begin(9600);

  Serial.write("Starting\n");
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, 1);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  Serial.write("Init done\n");

  //Set up WDT
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);

  loadStatus();
  Serial.println(EEPROM.length());

  pinMode(2, INPUT);

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.clearDisplay();
  display.display();
}

void toggle()
{
  awake = !awake;
}

void powerDown()
{
  saveStatus();
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  displayOff=true;
  noInterrupts();
  //Not awake
  awake = 0;
  //Enable interrupt on pin 2
  attachInterrupt(digitalPinToInterrupt(2), wakeup, CHANGE);
  //Turn off power to the display
  delayMicroseconds(20000);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  interrupts();
  sleep_cpu();
}

void wakeup()
{
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));
  //display.ssd1306_command(SSD1306_DISPLAYON);
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C, 1);  // initialize with the I2C addr 0x3D (for the 128x64)
  awake = 1;
  timer = 0;
}

extern volatile int status_fitness;
extern volatile int status_health;
extern volatile int status_fed;
extern volatile int status_drink;
#define DECAY_DIVIDER 120
void doDecay()
{
  if(wdt_runs>DECAY_DIVIDER)
  {
    status_fitness -= wdt_runs/DECAY_DIVIDER;
    status_health -= wdt_runs/DECAY_DIVIDER;
    status_fed -= wdt_runs/DECAY_DIVIDER;
    status_drink -= wdt_runs/DECAY_DIVIDER;
    wdt_runs=wdt_runs%DECAY_DIVIDER;
  }
}

void doLogo()
{
  int offset = 3 * timer;
  display.drawChar(3, 3, 'M', 1, 0, 4);
  display.drawChar(25, 3, 'A', 1, 0, 4);
  display.drawChar(47, 3, 'M', 1, 0, 4);
  display.drawChar(69, 3, 'A', 1, 0, 4);
  display.drawChar(3, 34, 'G', 1, 0, 3);
  display.drawChar(20, 34, 'O', 1, 0, 3);
  display.drawChar(37, 34, 'T', 1, 0, 3);
  display.drawChar(54, 34, 'C', 1, 0, 3);
  display.drawChar(71, 34, 'H', 1, 0, 3);
  display.drawChar(88, 34, 'I', 1, 0, 3);
  if (offset < 91)
  {
    display.fillRect(offset, 0, 91 - offset, 33, 0);
  }
  if (offset < 91 + 105)
  {
    int start = 0;
    if (offset > 91)
      start = offset - 91;
    display.fillRect(start, 34, 105 - start, SSD1306_LCDHEIGHT, 0);
  }
  if (offset > 250)
  {
    state = IDLE;
    timer = 0;
    display.clearDisplay();
    return;
  }
  timer++;
}

void goIdle()
{
    old_state=state;
    state = IDLE;
    timer = 0;
    display.clearDisplay();
}

extern const uint8_t heart[32];
void doIdle()
{
  limitStatus();
  display.drawBitmap(x % SSD1306_LCDWIDTH, 26+28*sin((float)x/35.0f), heart, 16, 16, 0);
  x++;
  if(digitalRead(2))
  {
    state = old_state;
    timer = 0;
    display.clearDisplay();
    return;
  }
  display.drawBitmap(x % SSD1306_LCDWIDTH, 26+28*sin((float)x/35.0f), heart, 16, 16, 1);
  timer++;
  if(timer>150)
  {
    display.dim(0);
  }
  if(timer > 450)
  {
    powerDown();
  }
}

void doHunger()
{
  timer++;
  display.drawChar(56, 24, 'H', 1, 0, 4);
  if(timer>200)
  {
    timer=0;
    state=IDLE;
    display.clearDisplay();
  }
  display.display();
}

void handle_input()
{
  
  //Action button
  if(digitalRead(2))
  {
    if(action_button&1)
    {
      action_button=PRESSED;
      action_held=0;
    }
    else
      action_button=DOWN;
    action_held++;
  }
  else
  {
    if(action_button&1)
      action_button=UP;
    else
      action_button=RELEASED;
  }

  if(digitalRead(9))
  {
    if(mode_button&1)
    {
      mode_button=PRESSED;
      mode_held=0;
    }
    else
      mode_button=DOWN;
    mode_held++;
  }
  else
  {
    if(mode_button&1)
      mode_button=UP;
    else
      mode_button=RELEASED;
  }
}

void loop() {
  //Button inputs
  handle_input();
  
  if(mode_held>50)
  {
    mode_held=0;
    goSelect();
  }

  doDecay();
  
  if(awake && displayOff)
  {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C, 1);
    displayOff=false;
    display.dim(255);
  }
  
  switch (state)
  {
    case LOGO: doLogo(); break;
    case IDLE: doIdle(); break;
    case FOOD: doHunger(); break;
    case SPORT: doSport(); break;
    case SELECT: doSelect(); break;
    case STATUS: doStatus(); break;
    case HEALTH: doHealth(); break;
  }
  display.display();
}


