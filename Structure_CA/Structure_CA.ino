// Structure recursion cellular automata //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <M5Stack.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)


  uint32_t size = ((2*WIDTH) * (2*HEIGHT));

  uint16_t *col = NULL;
  
  uint8_t *state = NULL;
  int x, y, k;
  
  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(BLACK);

  state = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*(SCR));

  rndrule();
  
}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      k = k^state[x]^state[k];
      state[x] = k;
      col[(2*x)+WFULL*(2*y)] = (65535>>k)^k;
    }
  }

  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  for (x=0;x<WIDTH;x++) state[x] = esp_random()%16;
  k = esp_random()%16;

}
