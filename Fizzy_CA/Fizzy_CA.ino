// Fizzy Cellular Automata //

#include <odroid_go.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)
  #define SCR2    (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;
  
  uint8_t *parent = NULL;
  uint8_t *child = NULL;

  int Calm = 40; //233
  int CellIndex = 0;
  int i,j;

  float *CellVal = NULL;

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  for (i = 0; i < SCR2; i++) CellVal[i] = esp_random()%100;

}
  
void setup() {

  srand(time(NULL));

  GO.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  GO.lcd.fillScreen(BLACK);

  CellVal = (float*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*SCR);

  memset((uint16_t *) col, 0, 4*SCR);

  rndrule();
  
}


void loop() {

  if (GO.BtnA.wasPressed()) rndrule();

  for (i = 0; i < WIDTH; i++) {

    for (j = 0; j < HEIGHT; j++) {

      CellIndex = (CellIndex+1)%SCR2;

      uint8_t klimp = (uint8_t)(CellVal[CellIndex]*4.7f)%100;       
      uint8_t nifna = (uint8_t)(CellVal[CellIndex]*3.9f)%100;
      uint8_t blugg = (uint8_t)(CellVal[CellIndex]*5.5f)%100;
      
      col[(2*i)+(2*j)*WFULL] = GO.lcd.color565(klimp,nifna,blugg);

      int below      = (CellIndex+1)%SCR2;
      int above      = (CellIndex+SCR2-1)%SCR2;
      int left       = (CellIndex+SCR2-HEIGHT)%SCR2;
      int right      = (CellIndex+HEIGHT)%SCR2;
      int aboveright = ((CellIndex-1) + HEIGHT + SCR2)%SCR2;
      int aboveleft  = ((CellIndex-1) - HEIGHT + SCR2)%SCR2;
      int belowright = ((CellIndex+1) + HEIGHT + SCR2)%SCR2;
      int belowleft  = ((CellIndex+1) - HEIGHT + SCR2)%SCR2;

      float NeighbourMix = powf((CellVal[left]*CellVal[right]*CellVal[above]*CellVal[below]*CellVal[belowleft]*CellVal[belowright]*CellVal[aboveleft]*CellVal[aboveright]),0.125f);

      CellVal[CellIndex] = fmod(sqrtf(CellVal[CellIndex]*NeighbourMix)+0.5f, Calm);

    }

  }

  GO.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  GO.update();

}
