// Super Langtons Ant //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR    (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

#define LENGHT 32

  uint16_t *state = NULL;;
  uint16_t antX = WIDTH/2;
  uint16_t antY = HEIGHT/2;
  uint16_t direction;
  uint16_t stateCount;
  bool type[LENGHT];
  uint16_t stateCols[LENGHT];


void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  antX = WIDTH/2;
  antY = HEIGHT/2;

  stateCount = 2 + esp_random()%(LENGHT-2);

  direction = esp_random()%4;

  memset((uint16_t *) state, 0, 4*SCR);

  for(int i = 0; i < stateCount; i++) type[i] = esp_random()%2;
  for(int i = 0; i < stateCount; i++) stateCols[i] = esp_random();

}


void setup(){

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  col = (uint16_t*)ps_malloc(4*SCR);
  state = (uint16_t*)ps_malloc(size);

  rndrule();
  
}


void loop(){

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for(int i = 0; i < 1000; i++) {

    move();
    turn(type[(state[antX+antY*WIDTH]%stateCount)]);
    state[antX+antY*WIDTH]++;
    col[antX+antY*WIDTH] = stateCols[(state[antX+antY*WIDTH]%stateCount)];

  }
  
  M5.lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);

  M5.update();
    
}

void turn(int angle){
  
  if(angle == 0){
    if(direction == 0){
      direction = 3;
    } else {
      direction--;
    }
  } else {
    if(direction == 3){
      direction = 0;
    } else {
      direction++;
    }
  }
}

void move(){
  
  if(antY == 0 && direction == 0){
    antY = HEIGHT-1;
  } else {
    if(direction == 0 ){
      antY--;
    }
  }
  if(antX == WIDTH-1 && direction == 1){
    antX = 0;
  } else {
    if(direction == 1){
      antX++;
    }
  }
  if(antY == HEIGHT-1 && direction == 2){
   antY = 0; 
  } else {
    if(direction == 2){
      antY++;
    }
  }
  if(antX == 0 && direction == 3){
    antX = WIDTH-1;
  } else {
    if(direction == 3){
      antX--;
    }
  }
}
