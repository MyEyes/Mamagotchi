state_t selected_state;

void goSelect()
{
   selected_state=STATUS;
   state=SELECT;
   timer=0;
}

void doSelect()
{
  display.clearDisplay();
  //input handling
  if(mode_button==PRESSED)
    switch(selected_state)
    {
      case STATUS: selected_state = FOOD; break;
      case FOOD: selected_state = DRINK; break;
      case DRINK: selected_state = HEALTH; break;
      case HEALTH: selected_state = SPORT; break;
      case SPORT: selected_state = STATUS; break;
    }
    
  switch(selected_state)
  {
    #define STATUS_OFF 10
    case STATUS: 
       display.drawChar(STATUS_OFF+3, 20, 'S', 1, 0, 3);
       display.drawChar(STATUS_OFF+20, 20, 'T', 1, 0, 3);
       display.drawChar(STATUS_OFF+37, 20, 'A', 1, 0, 3);
       display.drawChar(STATUS_OFF+54, 20, 'T', 1, 0, 3);
       display.drawChar(STATUS_OFF+71, 20, 'U', 1, 0, 3);
       display.drawChar(STATUS_OFF+88, 20, 'S', 1, 0, 3);
       if(action_button==PRESSED)
        goStatus();
      break;
    case FOOD:
    #define FOOD_OFF 26
       display.drawChar(FOOD_OFF+3, 20, 'F', 1, 0, 3);
       display.drawChar(FOOD_OFF+20, 20, 'O', 1, 0, 3);
       display.drawChar(FOOD_OFF+37, 20, 'O', 1, 0, 3);
       display.drawChar(FOOD_OFF+54, 20, 'D', 1, 0, 3);
      break;
    case DRINK:
    #define DRINK_OFF 18
       display.drawChar(DRINK_OFF+3, 20, 'D', 1, 0, 3);
       display.drawChar(DRINK_OFF+20, 20, 'R', 1, 0, 3);
       display.drawChar(DRINK_OFF+37, 20, 'I', 1, 0, 3);
       display.drawChar(DRINK_OFF+54, 20, 'N', 1, 0, 3);
       display.drawChar(DRINK_OFF+71, 20, 'K', 1, 0, 3);
      break;
    case HEALTH:
    #define HEALTH_OFF 10
       display.drawChar(HEALTH_OFF+3, 20, 'H', 1, 0, 3);
       display.drawChar(HEALTH_OFF+20, 20, 'E', 1, 0, 3);
       display.drawChar(HEALTH_OFF+37, 20, 'A', 1, 0, 3);
       display.drawChar(HEALTH_OFF+54, 20, 'L', 1, 0, 3);
       display.drawChar(HEALTH_OFF+71, 20, 'T', 1, 0, 3);
       display.drawChar(HEALTH_OFF+88, 20, 'H', 1, 0, 3);
       if(action_button==PRESSED)
        goHealth();
      break;
    case SPORT:
    #define SPORT_OFF 18
       display.drawChar(SPORT_OFF+3, 20, 'S', 1, 0, 3);
       display.drawChar(SPORT_OFF+20, 20, 'P', 1, 0, 3);
       display.drawChar(SPORT_OFF+37, 20, 'O', 1, 0, 3);
       display.drawChar(SPORT_OFF+54, 20, 'R', 1, 0, 3);
       display.drawChar(SPORT_OFF+71, 20, 'T', 1, 0, 3);
       if(action_button==PRESSED)
        goSport();
      break;
  }
}

