// Wolfram 1D Cellular Automata // 

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
  
  uint8_t *state = NULL;
  uint8_t *newstate = NULL;
  uint16_t coll;
  int x,y, k;
  bool rules[8] = {0, 1, 1, 1, 1, 0, 0, 0};
  bool center = false;
  
  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(BLACK);

  state = (uint8_t*)ps_malloc(size);
  newstate = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*(SCR));

  rndrule();
  
}


void loop() {

  if (M5.BtnA.wasPressed()) {rndrule(); for (x=0;x<8;x++) rules[x] = esp_random()%2; M5.Lcd.drawString("RND", 10, 10, 2);}
  if (M5.BtnB.wasPressed()) {center = !center; rndrule(); M5.Lcd.drawString("Center", 10, 10, 2);}
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for (y = 0; y < HEIGHT; y++) {

    memset (newstate, 0, 4*WIDTH);

    for (x=0;x<WIDTH;x++) {     
      k = 4*state[(x-1+WIDTH)%WIDTH] + 2*state[x] + state[(x+1)%WIDTH];
      newstate[x] = rules[k];
    }

    memcpy (state, newstate, 4*WIDTH);
  
    for (x = 0; x < WIDTH; x++) {
      if (state[x] == 1) coll = WHITE;       
      else coll = BLACK;
      col[x+WFULL*y] = coll;
    }

  }

  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  if (center) {memset (state, 0, 4*WIDTH); state[WIDTH/2] = 1;}
  else for (x=0;x<WIDTH;x++) state[x] = esp_random()%2;

}
