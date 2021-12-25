// 5-state 1D cellular automata //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR    (WIDTH*HEIGHT)

  #define RND   11
  #define VAL   15
  #define COL   5

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;
  uint16_t coll;
  uint8_t parent[WIDTH]; 
  uint8_t child[WIDTH];
  int count;
  int a[VAL];


void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  for(int i = 0; i < VAL; i++) a[i] = esp_random()%RND;

  for(int i = 0; i < WIDTH; i++){
  
    parent[i] = esp_random()%COL;
    child[i] = 0;

  }

}

void rndseed(){

  memset((uint16_t *) col, 0, 4*SCR);

  for(int i = 0; i < WIDTH; i++){
  
    parent[i] = esp_random()%COL;
    child[i] = 0;
    
  }

}


void setup(){

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  col = (uint16_t*)ps_malloc(4*SCR);

  rndrule();
  
}


void loop(){

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RULE", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { rndseed(); M5.Lcd.drawString("SEED", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for (int y = 0; y < HEIGHT; y++) {
 
    for (int x = 0; x < WIDTH; x++) {
          
      if (x == 0) count = parent[WIDTH-1] + parent[0] + parent[1];
      else if (x == WIDTH-1) count = parent[WIDTH-2] + parent[WIDTH-1] + parent[0];
      else count = parent[x-1] + parent[x] + parent[x+1];        
            
        if(count == a[0] || count == a[1] || count == a[2]) child[x] = 0;
        if(count == a[3] || count == a[4] || count == a[5]) child[x] = 1;
        if(count == a[6] || count == a[7] || count == a[8]) child[x] = 2;
        if(count == a[9] || count == a[10] || count == a[11]) child[x] = 3;
        if(count == a[12] || count == a[13] || count == a[14]) child[x] = 4;
               
        if (child[x] == 0) coll = TFT_RED;
        if (child[x] == 1) coll = TFT_GREEN;
        if (child[x] == 2) coll = TFT_BLUE;
        if (child[x] == 3) coll = TFT_YELLOW;
        if (child[x] == 4) coll = TFT_WHITE;

        col[x+WIDTH*y] = coll;
                                                   
      }
                                                 
      memcpy(parent, child, WIDTH);
  }
  
  M5.Lcd.pushImage(0, 0, WIDTH, HEIGHT,(uint16_t *) col);

  M5.update();
    
}
