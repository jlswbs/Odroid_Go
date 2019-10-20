// Structure recursion cellular automata //

#include <odroid_go.h>

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

  GO.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  GO.lcd.fillScreen(BLACK);

  state = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*(SCR));

  rndrule();
  
}


void loop() {

  if (GO.BtnA.wasPressed()) rndrule();

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      k = k^state[x]^state[k];
      state[x] = k;
      col[(2*x)+WFULL*(2*y)] = (65535>>k)^k;
    }
  }

  GO.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  GO.update();

}

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  for (x=0;x<WIDTH;x++) state[x] = esp_random()%16;
  k = esp_random()%16;

}
