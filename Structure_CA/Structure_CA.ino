// Structure 1D cellular automata //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)


  uint32_t size = ((2*WIDTH) * (2*HEIGHT));

  uint16_t *col = NULL;
  uint16_t coll;
  uint8_t *state = NULL;
  uint8_t *temp = NULL;
  uint8_t k;
  
  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  state = (uint8_t*)ps_malloc(size);
  temp = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*(SCR));

  rndrule();
  
}

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  for (int x=0; x<WIDTH; x++) state[x] = esp_random()%16;
  for (int x=0; x<WIDTH; x++) temp[x] = 0;
  k = esp_random()%16;

}

void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RULE", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();

  for (int y = 0; y < HEIGHT; y++) {
    
    for (int x = 0; x < WIDTH; x++) {
      
      k = k ^ temp[x] ^ state[k];
      state[x] = k;

      if(state[x] == 0) coll = TFT_BLACK;
      if(state[x] == 1) coll = TFT_BLUE;
      if(state[x] == 2) coll = TFT_RED;
      if(state[x] == 3) coll = TFT_GREEN;
      if(state[x] == 4) coll = TFT_CYAN;
      if(state[x] == 5) coll = TFT_MAGENTA;
      if(state[x] == 6) coll = TFT_YELLOW;
      if(state[x] == 7) coll = TFT_BLUE;
      if(state[x] == 8) coll = TFT_RED;
      if(state[x] == 9) coll = TFT_GREEN;
      if(state[x] == 10) coll = TFT_CYAN;
      if(state[x] == 11) coll = TFT_MAGENTA;
      if(state[x] == 12) coll = TFT_YELLOW;
      if(state[x] == 13) coll = TFT_BLUE;
      if(state[x] == 14) coll = TFT_RED;
      if(state[x] == 15) coll = TFT_WHITE;
      
      col[(2*x)+WFULL*(2*y)] = coll;
      
    }

    memcpy(temp, state, WIDTH);
    
  }

  M5.Lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}
