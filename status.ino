void goStatus()
{
  state=STATUS;
  timer=0;
}

#define EEPR_HEALTH 0
#define EEPR_FITNESS 4
#define EEPR_HUNGER 8
#define EEPR_THIRST 12
#define EEPR_MAGIC 16
#define MAGIC_CONST 0xAAFF

volatile int status_health = 100;
volatile int status_fitness = 100;
volatile int status_fed = 100;
volatile int status_drink = 100;

void saveStatus()
{
  EEPROM.put(EEPR_HEALTH, status_health);
  EEPROM.put(EEPR_FITNESS, status_fitness);
  EEPROM.put(EEPR_HUNGER, status_fed);
  EEPROM.put(EEPR_THIRST, status_drink);
  EEPROM.put(EEPR_MAGIC, MAGIC_CONST);
}

void loadStatus()
{
  unsigned int test;
  EEPROM.get(EEPR_MAGIC, test);
  Serial.println(test);
  if(test!=MAGIC_CONST)
    return;
  EEPROM.get(EEPR_HEALTH, status_health);
  Serial.println(status_health);
  EEPROM.get(EEPR_FITNESS, status_fitness);
  Serial.println(status_fitness);
  EEPROM.get(EEPR_HUNGER, status_fed);
  Serial.println(status_fed);
  EEPROM.get(EEPR_THIRST, status_drink);
  Serial.println(status_drink);
  limitStatus();
}

void limitStatus()
{
  if(status_health<0)
    status_health=0;
  if(status_health>999)
    status_health=999;
  if(status_fitness<0)
    status_fitness=0;
  if(status_fitness>999)
    status_fitness=999;
  if(status_fed<0)
    status_fed = 0;
  if(status_fed>999)
    status_fed=999;
  if(status_drink<0)
    status_drink=0;
  if(status_drink>999)
    status_drink=999;
}

void printNumber3Digit(int x, int y, int num)
{
  int curr_x=x;
  int curr_num=num;
  int curr_digit = curr_num%10;
  display.drawChar(x+24, y, curr_digit+'0', 1,0,2);
  curr_num/=10;
  curr_digit = curr_num%10;
  display.drawChar(x+12, y, curr_digit+'0', 1,0,2);
  curr_num/=10;
  curr_digit = curr_num%10;
  display.drawChar(x, y, curr_digit+'0', 1,0,2);
};

void doStatus()
{
  limitStatus();
  display.clearDisplay();
  display.drawBitmap(5,11,food_icon,16,16,1);
  printNumber3Digit(23,12,status_fed);
  display.drawBitmap(5,43,health_icon,16,16,1);
  printNumber3Digit(23,44,status_health);
  display.drawBitmap(69,11,drink_icon,16,16,1);
  printNumber3Digit(86,12,status_drink);
  display.drawBitmap(69,43,sport_icon,16,16,1);
  printNumber3Digit(86,44,status_fitness);
  timer++;
  if(timer>500)
  {
    goIdle();
    return;
  }
}

