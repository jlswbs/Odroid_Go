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

  for(int x = 0; x < WIDTH; x++){
  
    parent[x] = esp_random()%3;
    child[x] = 0;
    
  }

}

void rndrule2(){

  memset((uint16_t *) col, 0, 4*SCR);

  for(int x = 0; x < WIDTH; x++){
  
    parent[x] = esp_random()%3;
    child[x] = 0;
    
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

  for (int y = 0; y < HEIGHT; y++) {
 
    for (int x = 0; x < WIDTH; x++) {
          
      if (x == 0) count = parent[WIDTH-1] + parent[0] + parent[1];
      else if (x == WIDTH-1) count = parent[WIDTH-2] + parent[WIDTH-1] + parent[0];
      else count = parent[x-1] + parent[x] + parent[x+1];        
            
        if (count == a || count == b || count == c) child[x] = 0;
        if (count == d || count == e || count == f) child[x] = 1;
        if (count == g || count == h || count == i) child[x] = 2;
               
        if (child[x] == 0) coll = TFT_RED;
        if (child[x] == 1) coll = TFT_WHITE;
        if (child[x] == 2) coll = TFT_BLUE;

        col[x+WIDTH*y] = coll;
                                                   
      }
                                                 
      memcpy(parent, child, WIDTH);
  }
  
  M5.lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);

  M5.update();
    
}
