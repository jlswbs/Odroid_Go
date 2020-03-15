// Diffusion-Reaction 1D //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <M5Stack.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)
  #define SCR2    (WIDTH*HEIGHT)

  uint16_t RGB565( byte R, byte G, byte B){ return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );}

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

  int i,j;

  float A[WIDTH]; 
  float I[WIDTH];
  float D2A[WIDTH]; 
  float D2I[WIDTH];

  float p[6] = {0.5f, 2.0f, 2.0f, 2.0f, 1.0f, 0.0f};
  float dt = 0.05f;

  bool color = false;

  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31))*(maxf - minf) / (1UL << 31);}  

void rndrule(){
      
  memset((uint16_t *) col, 0, 4*SCR);

  p[0] = randomf(0.0f,1.0f);
  p[1] = randomf(0.0f,15.0f);
  p[2] = randomf(0.0f,4.0f);
  p[3] = randomf(0.0f,15.0f);
  p[4] = randomf(0.0f,4.0f);
  p[5] = randomf(0.0f,2.0f);
   
  for(i=0;i<WIDTH; i++) {
    
   A[i] = randomf(0.0f, 1.0f);
   I[i] = randomf(0.0f, 1.0f);
    
  }
  
  i = 0;
  j = 0;
         
}

void setup() {

  srand(time(NULL));

  col = (uint16_t*)ps_malloc(4*SCR);

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(BLACK);

  rndrule();

}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { color = !color; M5.Lcd.drawString("Color", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for(j=0;j<HEIGHT; j++) {
    
    for(i=1;i<WIDTH-1; i++) {
      
      D2A[i] = A[i-1] + A[i+1] - 2.0f * A[i];
      D2I[i] = I[i-1] + I[i+1] - 2.0f * I[i];
 
    }
  
    D2A[0] = A[1] - A[0]; 
    D2I[0] = I[1] - I[0]; 

    for(i=0;i<WIDTH; i++) {
    
      A[i] = A[i] + dt * (5.0f * A[i] *A[i] * A[i] / (I[i] * I[i]) + p[0] - p[1] * A[i] + p[2] * D2A[i]);
      I[i] = I[i] + dt * (A[i] * A[i] *A[i] - p[3] * I[i] + p[4] * D2I[i] + p[5]);  

      if (color == true) {
        
        uint16_t col1 = 250 - (50 * A[i]);
        uint16_t col2 = 250 - (50 * I[i]);    
        uint16_t col3 = 250 - (50 * D2I[i]);
        col[i+j*WIDTH] = RGB565(col1, col2, col3);
            
      } else {
        
        coll = 250 - (50 * A[i]);
        col[i+j*WIDTH] = gray2rgb565[(uint8_t)coll>>2];
        
      }
      
    }

  }

  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}
