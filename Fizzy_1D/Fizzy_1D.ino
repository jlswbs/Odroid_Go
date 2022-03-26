// Fizzy 1D cellular automata //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR     (WIDTH*HEIGHT)

  uint16_t RGB565(byte R, byte G, byte B){ return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );}

  static uint16_t gray2rgb565[64] = {
    0x0000,
    0x2000,
    0x4108,
    0x6108,
    0x8210,
    0xa210,
    0xc318,
    0xe318,
    0x0421,
    0x2421,
    0x4529,
    0x6529,
    0x8631,
    0xa631,
    0xc739,
    0xe739,
    0x0842,
    0x2842,
    0x494a,
    0x694a,
    0x8a52,
    0xaa52,
    0xcb5a,
    0xeb5a,
    0x0c63,
    0x2c63,
    0x4d6b,
    0x6d6b,
    0x8e73,
    0xae73,
    0xcf7b,
    0xef7b,
    0x1084,
    0x3084,
    0x518c,
    0x718c,
    0x9294,
    0xb294,
    0xd39c,
    0xf39c,
    0x14a5,
    0x34a5,
    0x55ad,
    0x75ad,
    0x96b5,
    0xb6b5,
    0xd7bd,
    0xf7bd,
    0x18c6,
    0x38c6,
    0x59ce,
    0x79ce,
    0x9ad6,
    0xbad6,
    0xdbde,
    0xfbde,
    0x1ce7,
    0x3ce7,
    0x5def,
    0x7def,
    0x9ef7,
    0xbef7,
    0xdfff,
    0xffff,
  };


  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;
  uint16_t coll;
  int Calm = 233;
  int CellIndex = 0;
  float CellVal[WIDTH];
  bool color = false;

void rndrule(){
      
  memset((uint16_t *) col, 0, 4*SCR);

  Calm = 16 + esp_random()%233;
  for (int i = 0; i < WIDTH; i++) CellVal[i] = esp_random()%128;
         
}

void setup() {

  srand(time(NULL));

  col = (uint16_t*)ps_malloc(4*SCR);

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  rndrule();

}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { color = !color; M5.Lcd.drawString("Color", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();

  for(int y=0;y<HEIGHT; y++) {

    for(int x=0;x<WIDTH; x++) {
    
      CellIndex = (CellIndex+1)%WIDTH;

      if (color == true) {

        uint8_t colr = (uint8_t)round(CellVal[CellIndex]*4.7f)%100;
        uint8_t colg = (uint8_t)round(CellVal[CellIndex]*3.9f)%100;
        uint8_t colb = (uint8_t)round(CellVal[CellIndex]*5.5f)%100;
        coll = RGB565(colr, colg, colb);
            
      } else {
        
        uint8_t colw = 4.7f * CellVal[CellIndex];
        coll = gray2rgb565[(uint8_t)colw>>2];
        
      }

      col[x+y*WIDTH] = coll;

      int below      = (CellIndex+1)%WIDTH;
      int above      = (CellIndex+WIDTH-1)%WIDTH;
      int left       = (CellIndex+WIDTH-1)%WIDTH;
      int right      = (CellIndex+1)%WIDTH;
      int aboveright = ((CellIndex-1) + 1 + WIDTH)%WIDTH;
      int aboveleft  = ((CellIndex-1) - 1 + WIDTH)%WIDTH;
      int belowright = ((CellIndex+1) + 1 + WIDTH)%WIDTH;
      int belowleft  = ((CellIndex+1) - 1 + WIDTH)%WIDTH;

      float NeighbourMix = powf((CellVal[left]*CellVal[right]*CellVal[above]*CellVal[below]*CellVal[belowleft]*CellVal[belowright]*CellVal[aboveleft]*CellVal[aboveright]),0.125f);
      CellVal[CellIndex] = fmod(sqrtf(CellVal[CellIndex]*NeighbourMix)+0.5f, Calm);
      
    }

  }

  M5.Lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);
  M5.update();

}
