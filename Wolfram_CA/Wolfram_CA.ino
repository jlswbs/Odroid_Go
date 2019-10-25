// Wolfram 1D Cellular Automata // 

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
  uint8_t *newstate = NULL;
  uint16_t coll;
  int x,y, k;
  bool rules[8] = {0, 1, 1, 1, 1, 0, 0, 0};
  bool center = false;
  
  
void setup() {

  srand(time(NULL));

  GO.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  GO.lcd.fillScreen(BLACK);

  state = (uint8_t*)ps_malloc(size);
  newstate = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*(SCR));

  rndrule();
  
}


void loop() {

  if (GO.BtnA.wasPressed()) {rndrule(); for (x=0;x<8;x++) rules[x] = esp_random()%2;}
  if (GO.BtnB.wasPressed()) {center = !center; rndrule();}

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
      col[(2*x)+WFULL*(2*y)] = coll;
    }

  }

  GO.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  GO.update();

}

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  if (center) {memset (state, 0, 4*WIDTH); state[WIDTH/2] = 1;}
  else for (x=0;x<WIDTH;x++) state[x] = esp_random()%2;

}
