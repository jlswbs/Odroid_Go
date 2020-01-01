// Four state 1D cellular automata //

#include <odroid_go.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;
  
  uint8_t *parent = NULL;
  uint8_t *child = NULL;

  int count, i, j;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  for(i = 0; i < WIDTH; i++) parent[i] = esp_random()%16;

}
  
void setup() {

  srand(time(NULL));

  GO.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  GO.lcd.fillScreen(BLACK);

  parent = (uint8_t*)ps_malloc(size);
  child = (uint8_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*SCR);

  memset((uint16_t *) col, 0, 4*SCR);

  for(i = 0; i < WIDTH; i++) parent[i] = 0;
  
  parent[(80)-1] = 1;
  parent[(80)] = 3;
  parent[(80)+1] = 2;
  
}


void loop() {

  if (GO.BtnA.wasPressed()) rndrule();

  for(j = 0; j < HEIGHT; j++) {
 
    for(i = 0; i < WIDTH; i++) {

      if(i == 0) count = parent[WIDTH-1] + parent[0] + parent[1];
      else if(i == WIDTH-1) count = parent[WIDTH-2] + parent[WIDTH-1] + parent[0];
      else count = parent[i-1] + parent[i] + parent[i+1];        
            
      if(count == 0 || count == 5 || count == 6) child[i] = 0;
      if(count == 3 || count == 4) child[i] = 1;
      if(count == 1 || count == 2) child[i] = 2;
      if(count == 7 || count == 5 || count == 1) child[i] = 3;
                
      if(child[i] == 0) col[(2*i)+(2*j)*WFULL] = BLACK;
      if(child[i] == 1) col[(2*i)+(2*j)*WFULL] = GREEN;
      if(child[i] == 2) col[(2*i)+(2*j)*WFULL] = RED;
      if(child[i] == 3) col[(2*i)+(2*j)*WFULL] = BLUE;
     
    }

    memcpy(parent, child, WIDTH);

  }

  GO.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  GO.update();

}
