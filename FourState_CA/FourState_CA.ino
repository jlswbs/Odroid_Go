// Four state 1D cellular automata //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <M5Stack.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;
  
  uint8_t *parent = NULL;
  uint8_t *child = NULL;

  int count, i, j;
  uint16_t col1 = BLACK;
  uint16_t col2 = GREEN;
  uint16_t col3 = RED;
  uint16_t col4 = BLUE;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  for(i = 0; i < WIDTH; i++) parent[i] = esp_random()%16;

}
  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(BLACK);

  parent = (uint8_t*)ps_malloc(size);
  child = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*SCR);

  memset((uint16_t *) col, 0, 4*SCR);

  for(i = 0; i < WIDTH; i++) parent[i] = 0;
  
  parent[(WIDTH/2)-1] = 1;
  parent[(WIDTH/2)] = 3;
  parent[(WIDTH/2)+1] = 2;
  
}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) {
    col1 = esp_random();
    col2 = esp_random();
    col3 = esp_random();
    col4 = esp_random();
    M5.Lcd.drawString("Color", 10, 10, 2);
  }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for(j = 0; j < HEIGHT; j++) {
 
    for(i = 0; i < WIDTH; i++) {

      if(i == 0) count = parent[WIDTH-1] + parent[0] + parent[1];
      else if(i == WIDTH-1) count = parent[WIDTH-2] + parent[WIDTH-1] + parent[0];
      else count = parent[i-1] + parent[i] + parent[i+1];        
            
      if(count == 0 || count == 5 || count == 6) child[i] = 0;
      if(count == 3 || count == 4) child[i] = 1;
      if(count == 1 || count == 2) child[i] = 2;
      if(count == 7 || count == 5 || count == 1) child[i] = 3;

      if(child[i] == 0) col[i+j*WFULL] = col1;
      if(child[i] == 1) col[i+j*WFULL] = col2;
      if(child[i] == 2) col[i+j*WFULL] = col3;
      if(child[i] == 3) col[i+j*WFULL] = col4;  
     
    }

    memcpy(parent, child, WIDTH);

  }

  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}
