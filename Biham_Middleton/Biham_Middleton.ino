// Biham-Middleton-Levine traffic model //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define FULLW   320
  #define FULLH   240
  #define SCR     (FULLW*FULLH)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  
  uint16_t *col = NULL;

  int x, y;
  int density = 35;

  int rule[] = {0,0,0,0,2,2,1,1,1,2,2,2,0,2,2,1,1,1,0,0,0,0,2,2,1,1,1};
  uint8_t *world = NULL;
  uint8_t *newworld = NULL;
  

int color2state(int c){ return c == TFT_RED ? 1 : (c == TFT_GREEN ? 2 : 0); }

int state2color(int s){ return s == 1 ? TFT_RED : (s == 2 ? TFT_GREEN : TFT_WHITE); }

void rndrule() { for(int i=0; i<27; i++) rule[i] = esp_random()%3; }

void rndseed(){

  memset((uint16_t *) col, 0, 4*SCR);

  density = 25 + esp_random()%50;

  for(y=0; y<HEIGHT; y++){
    
    for(x=0; x<WIDTH; x++){
    
      if((esp_random()%100)<density){
        
        if((esp_random()%100)<50) col[(2*x)+(2*y)*FULLW] = TFT_RED;
        else col[(2*x)+(2*y)*FULLW] = TFT_GREEN;

      }
    }
  }

  for(y=0; y<HEIGHT; y++){
    
    for(x=0; x<WIDTH; x++) world[x+y*WIDTH] = color2state(col[(2*x)+(2*y)*FULLW]);

  }

}
  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  world = (uint8_t*)ps_malloc(size);
  newworld = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*SCR);

  rndseed();
  
}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); rndseed(); M5.Lcd.drawString("Rule", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { rndseed(); M5.Lcd.drawString("Seed", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();

  for(y = 0; y<HEIGHT; y++){
    
    for(x = 0; x<WIDTH; x++) newworld[x+y*WIDTH] = rule[world[y*WIDTH+(x+1)%WIDTH]+3*world[y*WIDTH+x]+9*world[y*WIDTH+(x+WIDTH-1)%WIDTH]];

  }

  for(y = 0; y<HEIGHT; y++){
    
    for(x = 0; x<WIDTH; x++){
      
      world[x+y*WIDTH] = rule[newworld[x+((y+1)%HEIGHT)*WIDTH]+3*newworld[y*WIDTH+x]+9*newworld[x+((y+HEIGHT-1)%HEIGHT)*WIDTH]];
      col[(2*x)+(2*y)*FULLW] = state2color(world[x+y*WIDTH]);
    
    }
  
  }
  
  M5.Lcd.pushRect(0, 0, FULLW, FULLH,(uint16_t *) col);
  M5.update();

}
