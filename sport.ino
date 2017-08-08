#include <stdint.h>

#define MAX_OBSTACLES 3
enum SPORT_CHAR_STATE{
  RUNNING,
  JUMPING
};

enum obstacle_type
{
  BIRD = 0,
  ROCK = 1
};

struct obstacle_t
{
  char active;
  obstacle_type type;
  int position;
};

SPORT_CHAR_STATE old_state = RUNNING;
int jumpHeight = 0;
int jumpSpeed = 0;
int sport_hit_countdown=0;
int sport_win_countdown=0;

int sport_progress=0;

char obstacles_init=0;
int obstacle_cooldown=40;
obstacle_t sport_obstacles[MAX_OBSTACLES];

void sport_take_hit()
{
  for(int x=0; x<MAX_OBSTACLES; x++)
    sport_obstacles[x].active=0;
  obstacle_cooldown=40;
  sport_progress-=250;
  if(sport_progress<0)
    sport_progress=0;
  sport_hit_countdown=20;
}

void sport_win()
{
  for(int x=0; x<MAX_OBSTACLES; x++)
    sport_obstacles[x].active=0;
  obstacle_cooldown=40;
  
  sport_win_countdown=120;
  sport_progress=0;
}

void goSport()
{
  obstacles_init = false;
  state=SPORT;
}

void doSport()
{
  //Init obstacle struct
  if(!obstacles_init)
  {
    for(int x=0; x<MAX_OBSTACLES; x++)
    {
      sport_obstacles[x].active=0;
    }
    obstacles_init=1;
  }
  
  display.clearDisplay();
  //Player update
  SPORT_CHAR_STATE state=old_state;
  if(state==RUNNING && action_button==PRESSED)
  {
    state = JUMPING;
    jumpSpeed = -12;
    timer = 0;
  }

  if(state==JUMPING)
  {
    jumpHeight += jumpSpeed/3;
    jumpSpeed++;
    if(jumpHeight>0)
    {
      jumpHeight=0;
      jumpSpeed=0;
      state=RUNNING;
    }
  }

  //5% chance to spawn obstacle when not on cooldown
  if(obstacle_cooldown<0 && !random(20))
  {
    for(int x=0; x<MAX_OBSTACLES; x++)
    {
      if(!sport_obstacles[x].active)
      {
        sport_obstacles[x].active = 1;
        sport_obstacles[x].type = random(2);
        sport_obstacles[x].position=128;
        obstacle_cooldown=30+random(20);
        break;
      }
    }
  }

  if(sport_hit_countdown)
  {
     display.drawChar(13, 20, 'O', 1, 0, 4);
     display.drawChar(35, 20, 'U', 1, 0, 4);
     display.drawChar(57, 20, 'C', 1, 0, 4);
     display.drawChar(79, 20, 'H', 1, 0, 4);
     sport_hit_countdown--;
     return;
  }

  if(sport_win_countdown)
  {
     display.drawChar(33, 20, 'W', 1, 0, 4);
     display.drawChar(55, 20, 'I', 1, 0, 4);
     display.drawChar(77, 20, 'N', 1, 0, 4);
     sport_win_countdown--;
     return;
  }

  //Draw background
  display.drawLine(0,62,128,62,1);
  //Draw progress
  display.drawRect(0,0,128,8,1);
  display.fillRect(0,0,1+sport_progress/10,8,1);
  
  //Obstacle update and draw
  for(int x=0; x<MAX_OBSTACLES; x++)
  {
    if(!sport_obstacles[x].active)
      continue;
    //Collision check
    if(sport_obstacles[x].position<24 && sport_obstacles[x].position>8)
    {
      if(sport_obstacles[x].type==ROCK && jumpHeight>-8)
          sport_take_hit();
      if(sport_obstacles[x].type==BIRD && jumpHeight<-16)
          sport_take_hit();
    }
    sport_obstacles[x].position-=2;
    if(sport_obstacles[x].position<-16)
      sport_obstacles[x].active=0;
    display.drawBitmap(sport_obstacles[x].position, sport_obstacles[x].type?46:25, sport_obstacles[x].type?rock:bird, 16,16,1);
  }
  
  //Draw new frame
  timer++;
  sport_progress++;
  if(sport_progress>1260)
  {
    sport_win();
  }

  if(timer>500)
  {
    goIdle();
    return;
  }
  int frame = (timer/2)%4;
  const uint8_t* animation = state==RUNNING?run_frames:jump_frames;
  display.drawBitmap(10, jumpHeight+47, animation+frame*16*16/8, 16,16,1);
  old_state=state;
  obstacle_cooldown--;
}
