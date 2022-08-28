// Worms cellular automata //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR     WIDTH*HEIGHT

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  
  #define NUMANTS 7
  #define ITER    64

  uint16_t *col = NULL;
  uint16_t coll[NUMANTS];
  int x[NUMANTS];
  int y[NUMANTS];
  int antsdir[NUMANTS];

void rndseed(){

  memset((uint16_t *) col, 0, 4*SCR);

  for(int i = 0; i < NUMANTS; i++){
  
    x[i] = esp_random()%WIDTH;
    y[i] = esp_random()%HEIGHT;
    antsdir[i] = esp_random()%8;
    coll[i] = esp_random();
    
  }

}

  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  col = (uint16_t*)ps_malloc(4*SCR);

  rndseed();
  
}

void loop() {

  if (M5.BtnA.wasPressed()) { rndseed(); M5.Lcd.drawString("Seed", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();

  for(int k = 0; k < ITER; k++){
  
    for(int i = 0; i < NUMANTS; i++){
    
      if (col[x[i]+WIDTH*y[i]] > TFT_BLACK){ antsdir[i] = antsdir[i] - 1; col[x[i]+WIDTH*y[i]] = TFT_BLACK; }
      else { antsdir[i] = antsdir[i] + 1; col[x[i]+WIDTH*y[i]] = coll[i]; }

      if (antsdir[i] > 7) antsdir[i] = 0;   
      if (antsdir[i] < 0) antsdir[i] = 7;
    
      switch(antsdir[i]){
        case 0: y[i]--; break;
        case 1: y[i]--; x[i]++; break;
        case 2: x[i]++; break;
        case 3: x[i]++; y[i]++; break;
        case 4: y[i]++; break;
        case 5: y[i]++; x[i]--; break;
        case 6: x[i]--; break;
        case 7: x[i]--; y[i]--; break;
      }
    
      if (x[i] > WIDTH-1) x[i] = 0;
      if (x[i] < 0) x[i] = WIDTH-1;
      if (y[i] > HEIGHT-1) y[i] = 0;
      if (y[i] < 0) y[i] = HEIGHT-1;
    
    }
    
  }
  
  M5.Lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);
  M5.update();

}
