// 3-state cellular automata //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR    (WIDTH*HEIGHT)

  #define RND 7

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;
  uint16_t coll;
  uint8_t parent[WIDTH]; 
  uint8_t child[WIDTH];
  int count;
  int a, b, c, d, e, f, g, h, i;


void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  a = esp_random()%RND;
  b = esp_random()%RND;
  c = esp_random()%RND;
  d = esp_random()%RND;
  e = esp_random()%RND;
  f = esp_random()%RND;
  g = esp_random()%RND;
  h = esp_random()%RND;
  i = esp_random()%RND;

  for(int i = 0; i < WIDTH; i++){
  
    parent[i] = esp_random()%3;
    child[i] = 0;
    
  }

}

void rndrule2(){

  memset((uint16_t *) col, 0, 4*SCR);

  for(int i = 0; i < WIDTH; i++){
  
    parent[i] = esp_random()%3;
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

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { rndrule2(); M5.Lcd.drawString("SEED", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for (int j = 0; j < HEIGHT; j++) {
 
    for (int i = 0; i < WIDTH; i++) {
          
      if (i == 0) count = parent[WIDTH-1] + parent[0] + parent[1];
      else if (i == WIDTH-1) count = parent[WIDTH-2] + parent[WIDTH-1] + parent[0];
      else count = parent[i-1] + parent[i] + parent[i+1];        
            
        if (count == a || count == b || count == c) child[i] = 0;
        if (count == d || count == e || count == f) child[i] = 1;
        if (count == g || count == h || count == i) child[i] = 2;
               
        if (child[i] == 0) coll = TFT_RED;
        if (child[i] == 1) coll = TFT_WHITE;
        if (child[i] == 2) coll = TFT_BLUE;

        col[i+WIDTH*j] = coll;
                                                   
      }
                                                 
      memcpy(parent, child, WIDTH);
  }
  
  M5.lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);

  M5.update();
    
}
