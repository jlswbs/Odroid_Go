// Biham-Middleton-Levine Traffic Model //

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

  int x, y;
  int density = 35;

  int rule[] = {0,0,0,0,2,2,1,1,1,2,2,2,0,2,2,1,1,1,0,0,0,0,2,2,1,1,1};
  uint8_t *pixles = NULL;
  uint8_t *slexip = NULL;
  

int color2state(int c){ return c == RED ? 1 : (c == GREEN ? 2 : 0); }

int state2color(int s){ return s == 1 ? RED : (s == 2 ? GREEN : WHITE); }

void rules() { for(int i=0; i<27; i++) rule[i] = esp_random()%3; }

void trafficSet(){

  memset((uint16_t *) col, 0, 4*SCR);

  for(x=0; x<WIDTH; x++){
    for(y=0; y<HEIGHT; y++){
      if((esp_random()%100)<density){
        if((esp_random()%100)<50){
          col[(2*x)+WFULL*(2*y)] = RED;
        }else{
          col[(2*x)+WFULL*(2*y)] = GREEN;
        }
      }
    }
  }

  for(x=0; x<WIDTH; x++){
    for(y=0; y<HEIGHT; y++){
     pixles[y*WIDTH+x] = color2state(col[(2*x)+WFULL*(2*y)]);
    }
  }

}
  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(BLACK);

  pixles = (uint8_t*)ps_malloc(size);
  slexip = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*SCR);

  trafficSet();
  
}


void loop() {

  if (M5.BtnA.wasPressed()) { rules(); trafficSet(); M5.Lcd.drawString("Rule", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { trafficSet(); M5.Lcd.drawString("Seed", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for(x = 0; x<WIDTH; x++){
    for(y = 0; y<HEIGHT; y++){
      slexip[y*WIDTH+x] = rule[pixles[y*WIDTH+(x+1)%WIDTH]+3*pixles[y*WIDTH+x]+9*pixles[y*WIDTH+(x+WIDTH-1)%WIDTH]];
    }
  }

  for(x = 0; x<WIDTH; x++){
    for(y = 0; y<HEIGHT; y++){
      pixles[y*WIDTH+x] = rule[slexip[x+((y+1)%HEIGHT)*WIDTH]+3*slexip[y*WIDTH+x]+9*slexip[x+((y+HEIGHT-1)%HEIGHT)*WIDTH]];
    }
  }
  
  for(x = 0; x<WIDTH; x++){
    for(y = 0; y<HEIGHT; y++){
      col[(2*x)+WFULL*(2*y)] = state2color(pixles[y*WIDTH+x]);
    }
  }
  
  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}
