// Langtons Ant Multi //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR    (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

#define NUMANTS 6
#define ITER 64
  
  uint16_t coll[NUMANTS];
  
  int x[NUMANTS];
  int y[NUMANTS];
  int antsdir[NUMANTS];


void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  for(int i = 0; i < NUMANTS; i++){
  
    x[i] = esp_random()%WIDTH;
    y[i] = esp_random()%HEIGHT;
    antsdir[i] = esp_random()%4;
    coll[i] = 1 + esp_random();
    
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
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for(int k = 0; k < ITER; k++){
  
    for(int i = 0; i < NUMANTS; i++){
    
      if (col[x[i]+WIDTH*y[i]] > TFT_BLACK){ antsdir[i] = antsdir[i] - 1; col[x[i]+WIDTH*y[i]] = TFT_BLACK; }
      else { antsdir[i] = antsdir[i] + 1; col[x[i]+WIDTH*y[i]] = coll[i]; }

      if (antsdir[i] > 3) antsdir[i] = 0;   
      if (antsdir[i] < 0) antsdir[i] = 3;   
    
      if (antsdir[i] == 0) x[i] = x[i] - 1;
      if (antsdir[i] == 1) y[i] = y[i] + 1;
      if (antsdir[i] == 2) x[i] = x[i] + 1;
      if (antsdir[i] == 3) y[i] = y[i] - 1;
    
      if (x[i] > WIDTH-1) x[i] = 0;
      if (x[i] < 0) x[i] = WIDTH-1;
      if (y[i] > HEIGHT-1) y[i] = 0;
      if (y[i] < 0) y[i] = HEIGHT-1;
    
    }
  }
  
  M5.lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);

  M5.update();
    
}
