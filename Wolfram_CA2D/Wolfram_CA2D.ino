// Wolfram 2D cellular automata //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <M5Stack.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)
  #define SCR2    (WIDTH*HEIGHT)


  uint32_t size = ((2*WIDTH) * (2*HEIGHT));

  uint16_t *col = NULL;
  
  bool *state = NULL;
  bool *newstate = NULL;
  bool rules[10] = {0,0,1,1,1,1,0,0,0,0};
  int x,y;


void rndrule() {

  memset((uint16_t *) col, 0, 4*SCR);

  rules[0] = esp_random()%2;
  rules[1] = esp_random()%2;
  rules[2] = esp_random()%2;
  rules[3] = esp_random()%2;
  rules[4] = esp_random()%2;
  rules[5] = esp_random()%2;
  rules[6] = esp_random()%2;
  rules[7] = esp_random()%2;
  rules[8] = esp_random()%2;
  rules[9] = esp_random()%2;

  memset(newstate, 0, SCR2);
  memset(state, 0, SCR2);
  
  state[(WIDTH/2)+(HEIGHT/2)*WIDTH] = 1;
  state[(WIDTH/2)+((HEIGHT/2)-1)*WIDTH] = 1;
  state[((WIDTH/2)-1)+((HEIGHT/2)-1)*WIDTH] = 1;
  state[((WIDTH/2)-1)+(HEIGHT/2)*WIDTH] = 1;

}

uint8_t neighbors(uint16_t x, uint16_t y) {
  
  uint8_t result = 0;

  if(y > 0 && state[x+(y-1)*WIDTH] == 1) result = result + 1;
  if(x > 0 && state[(x-1)+y*WIDTH] == 1) result = result + 1;
  if(x < WIDTH-1 && state[(x+1)+y*WIDTH] == 1) result = result + 1;
  if(y < HEIGHT-1 && state[x+(y+1)*WIDTH] == 1) result = result + 1;
  
  return result;
 
}
  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(BLACK);

  state = (bool*)ps_malloc(size);
  newstate = (bool*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*(SCR));

  memset((uint16_t *) col, 0, 4*SCR);
  
  state[(WIDTH/2)+(HEIGHT/2)*WIDTH] = 1;
  state[(WIDTH/2)+((HEIGHT/2)-1)*WIDTH] = 1;
  state[((WIDTH/2)-1)+((HEIGHT/2)-1)*WIDTH] = 1;
  state[((WIDTH/2)-1)+(HEIGHT/2)*WIDTH] = 1;
  
}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for(x = 0; x < WIDTH; x++){
    
    for(y = 0; y < HEIGHT; y++){
           
      uint8_t totalNeighbors = neighbors(x,y);
            
      if(state[x+y*WIDTH] == 0 && totalNeighbors == 0)      {newstate[x+y*WIDTH] = rules[0]; col[(2*x)+(2*y)*WFULL] = WHITE;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 0) {newstate[x+y*WIDTH] = rules[1]; col[(2*x)+(2*y)*WFULL] = RED;}
      else if(state[x+y*WIDTH] == 0 && totalNeighbors == 1) {newstate[x+y*WIDTH] = rules[2]; col[(2*x)+(2*y)*WFULL] = GREEN;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 1) {newstate[x+y*WIDTH] = rules[3]; col[(2*x)+(2*y)*WFULL] = BLUE;}
      else if(state[x+y*WIDTH] == 0 && totalNeighbors == 2) {newstate[x+y*WIDTH] = rules[4]; col[(2*x)+(2*y)*WFULL] = YELLOW;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 2) {newstate[x+y*WIDTH] = rules[5]; col[(2*x)+(2*y)*WFULL] = DARKGREY;}
      else if(state[x+y*WIDTH] == 0 && totalNeighbors == 3) {newstate[x+y*WIDTH] = rules[6]; col[(2*x)+(2*y)*WFULL] = MAGENTA;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 3) {newstate[x+y*WIDTH] = rules[7]; col[(2*x)+(2*y)*WFULL] = CYAN;}
      else if(state[x+y*WIDTH] == 0 && totalNeighbors == 4) {newstate[x+y*WIDTH] = rules[8]; col[(2*x)+(2*y)*WFULL] = NAVY;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 4) {newstate[x+y*WIDTH] = rules[9]; col[(2*x)+(2*y)*WFULL] = BLACK;}
      
    }
  }
 
  memcpy(state, newstate, SCR2);

  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}
