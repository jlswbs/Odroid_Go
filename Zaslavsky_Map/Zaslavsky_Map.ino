// Zaslavsky web map //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR    (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

  uint16_t coll;
  uint16_t xx, yy;
  int iterations = 4096;
  float x = 0.1f;
  float y = 0.1f;
  float a = 0.19f;      
  float dt = 0.0f;
  

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31))*(maxf - minf) / (1UL << 31);}  

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  x = 0.1f;
  y = 0.0f;
  dt = 0.0f;
  a = randomf(0.0099f, 0.9999f);

}

void setup() {

  srand(time(NULL));

  col = (uint16_t*)ps_malloc(4*SCR);

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(TFT_BLACK);

  rndrule();

}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  coll = esp_random();

  for (int i = 0; i < iterations; i++) {

    float nx = x;
    float ny = y;
        
    x = ny;
    y = -nx - a * sinf(2.0f * PI * ny - dt);

    dt = dt + 0.005f;

    xx = (WIDTH/2) + (x/3);
    yy = (HEIGHT/2) + (y/3);
    
    if (xx > 0 && xx < WIDTH && yy > 0 && yy < HEIGHT) col[xx+yy*WIDTH] = coll;

  }

  M5.lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);
  M5.update();

}
