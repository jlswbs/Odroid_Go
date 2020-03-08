// Gumowski-Mira strange attractor //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <M5Stack.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)
  #define SCR2    (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

  uint16_t coll;
  uint16_t xx, yy;
  int iterations = 10000;
  float a = 0.000001f;
  float b = 0.05f;
  float m = -0.031f;

  float nx = 0.0f;
  float ny = 0.5f;

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31))*(maxf - minf) / (1UL << 31);}  

float mira(float x) {
  
  float xx = x * x;
  return (m * x) + ((2.0f * (1.0f - m) * xx) / (1.0f + xx));
  
}

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  m = randomf(-0.95f, -0.05f);

}

void setup() {

  srand(time(NULL));

  col = (uint16_t*)ps_malloc(4*SCR);

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(BLACK);

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

    float x = nx;      
    float y = ny;
    
    nx = y + a * (1.0f - (b * (y * y))) * y + mira(x);
    ny = -x + mira(nx);

    xx = -8 + (WIDTH/2) + 3.0f * nx; 
    yy = (HEIGHT/2) + 3.0f * ny;
    
    if(xx > 0 && xx < WIDTH && yy > 0 && yy < HEIGHT) col[xx+yy*WIDTH] = coll;

  }

  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}
