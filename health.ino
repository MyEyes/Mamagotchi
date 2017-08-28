#define HEALTH_MAX_VIRUS 4
#define HEALTH_DESCENT_SPEED 1
#define HEALTH_DESCENT_DIVIDER 2
#define HEALTH_VIRUS_COOLDOWN 60

struct health_virus_t
{
  char active;
  int positionX;
  int positionY;
  int deathCountdown;
};

int lastSpawn = 64;
int paddlePos = 64;
int health_timer = 0;
int health_virus_spawn_timer = HEALTH_VIRUS_COOLDOWN;
health_virus_t viruses[HEALTH_MAX_VIRUS];

#define HEALTH_TARGET 1280
int healthProgress = 0;

int healthWinCountdown = 0;
int healthDamageCountdown = 0;
int healthDamagePos = 0;

void goHealth()
{
  state = HEALTH;
  timer = 0;
  lastSpawn=64;
  paddlePos = 64;
  health_timer=0;
  health_virus_spawn_timer = HEALTH_VIRUS_COOLDOWN;
  delete_viruses();
  randomSeed(analogRead(14));
}

void health_damage(int x)
{
  healthDamageCountdown = 40;
  healthDamagePos = x;
  status_health -= 5;
}

void health_win()
{
  healthProgress = 0;
  healthWinCountdown = 120;
  healthDamageCountdown = 0;
  status_health+=80;
  delete_viruses();
  health_virus_spawn_timer = HEALTH_VIRUS_COOLDOWN;
}

void drawHealthWin()
{
   display.clearDisplay();
   display.drawChar(33, 10, 'W', 1, 0, 4);
   display.drawChar(55, 10, 'I', 1, 0, 4);
   display.drawChar(77, 10, 'N', 1, 0, 4);
   if(healthWinCountdown<60)
   {
     if(healthWinCountdown<58)
      display.drawChar(20, 44, 'H', 1, 0, 1);
     if(healthWinCountdown<56)
      display.drawChar(29, 44, 'e', 1, 0, 1);
     if(healthWinCountdown<54)
      display.drawChar(38, 44, 'a', 1, 0, 1);
     if(healthWinCountdown<52)
      display.drawChar(47, 44, 'l', 1, 0, 1);
     if(healthWinCountdown<50)
      display.drawChar(56, 44, 't', 1, 0, 1);
     if(healthWinCountdown<48)
      display.drawChar(65, 44, 'h', 1, 0, 1);
     if(healthWinCountdown<44)
      display.drawChar(74, 44, '+', 1, 0, 1);
     if(healthWinCountdown<42)
      display.drawChar(83, 44, '8', 1, 0, 1);
     if(healthWinCountdown<40)
      display.drawChar(92, 44, '0', 1, 0, 1);
   }
}

void drawHealthDamage()
{
  display.drawChar(healthDamagePos, 56, '-', 1, 0, 1);
  display.drawChar(healthDamagePos+8, 56, '5', 1, 0, 1);
}

void delete_viruses()
{
  for(int x=0; x<HEALTH_MAX_VIRUS; x++)
    viruses[x].active=0;
}

void spawn_virus()
{
  for(int x=0; x<HEALTH_MAX_VIRUS; x++)
  {
    if(viruses[x].active)
      continue;
    viruses[x].active=1;
    viruses[x].positionX = 16*random(8)+1;
    lastSpawn=viruses[x].positionX;
    viruses[x].positionY=-16;
    viruses[x].deathCountdown = 0;
    break;
  }
  health_virus_spawn_timer = HEALTH_VIRUS_COOLDOWN;
}
extern const uint8_t health_virus[2*16*16/8];
extern const uint8_t explosion[3*16*16/8];
void doHealth()
{
  display.clearDisplay();

  if(health_virus_spawn_timer==0)
  {
    spawn_virus();
  }
  health_virus_spawn_timer--;

  if(action_button==PRESSED)
  {
    timer=0;
    paddlePos+=16;
  }
  if(mode_button==PRESSED)
  {
    timer=0;
    paddlePos-=16;
  }

  if(paddlePos>112)
    paddlePos=112;
  if(paddlePos<0)
    paddlePos=0;

  for(int x=1; x<8; x++)
  {
    display.drawLine(x*16,8,x*16,64,1);
  }
  
  for(int x=0; x<HEALTH_MAX_VIRUS; x++)
  {
    if(!viruses[x].active)
      continue;
    //Don't do normal update if virus is dead
    if(!viruses[x].deathCountdown)
    {
      if(!(health_timer%HEALTH_DESCENT_DIVIDER))
        viruses[x].positionY++;
      //Check for paddle position
      if(viruses[x].positionY>44)
      {
        if(viruses[x].positionX<paddlePos+14 && paddlePos<viruses[x].positionX+14)
          viruses[x].deathCountdown=10;
      }
      if(viruses[x].positionY>64)
      {
        viruses[x].active=0;
        health_damage(viruses[x].positionX);
      }
    }
    else
    {
      //Do nothing
    }
    if(!viruses[x].deathCountdown)
    {
      unsigned int frame = ((viruses[x].positionX+health_timer)/4)%2;
      display.drawBitmap(viruses[x].positionX,viruses[x].positionY,health_virus+(16*16/8)*frame,16,16,1);
    }
    else
    {
      //Virus is dead, show explosion effect
      display.drawBitmap(viruses[x].positionX,viruses[x].positionY,explosion+((20-viruses[x].deathCountdown)/7)*(16*16/8),16,16,1);
      if(--viruses[x].deathCountdown ==0)
        viruses[x].active=0;
    }
  }

  display.drawRect(0,0,128,8,1);
  display.fillRect(0,0,1+healthProgress/10,8,1);

  display.fillRect(paddlePos, 60, 16, 4,1);

  if(healthDamageCountdown>0)
  {
    healthDamageCountdown--;
    drawHealthDamage();
  }

  if(healthWinCountdown>0)
  {
    healthWinCountdown--;
    healthProgress = 0;
    drawHealthWin();
  }
  
  health_timer++;

  healthProgress++;
  if(healthProgress>HEALTH_TARGET)
  {
    health_win();
  }
  
  if(timer++>500 && !healthWinCountdown)
  {
    goIdle();
    return;
  }
}

