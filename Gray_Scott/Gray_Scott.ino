// Gray-Scott reaction-diffusion //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   80
  #define HEIGHT  60
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL * HFULL)
  #define SCR2    (WIDTH * HEIGHT)
  #define ITER    64

  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;

  float diffU = 0.16f;
  float diffV = 0.08f;
  float paramF = 0.035f;
  float paramK = 0.06f;

  float U[WIDTH][HEIGHT];
  float V[WIDTH][HEIGHT];

  float dU[WIDTH][HEIGHT];
  float dV[WIDTH][HEIGHT];
 
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
  
void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  diffU = randomf(0.0999f, 0.1999f);
  diffV = randomf(0.0749f, 0.0849f);
  paramF = randomf(0.0299f, 0.0399f);
  paramK = randomf(0.0549f, 0.0649f);
  
  for (int j = 0; j < HEIGHT; j++) { 
    for (int i = 0; i < WIDTH; i++) {    
      U[i][j] = 1.0f;
      V[i][j] = 0.0f;        
    }
  }

  for(int j=(HEIGHT-4)/2;j<(HEIGHT+4)/2;++j){      
    for(int i=(WIDTH-4)/2;i<(WIDTH+4)/2;++i){          
      U[i][j] = randomf(0.5f, 1.0f);
      V[i][j] = randomf(0.25f, 0.5f); 
    }
  }

}

void timestep(float F, float K, float diffU, float diffV) {

   for (int j = 1; j < HEIGHT-1; j++) {
    for (int i = 1; i < WIDTH-1; i++) {
            
      float u = U[i][j];
      float v = V[i][j];
          
      float uvv = u * v * v;     
       
      float lapU = (U[i-1][j] + U[i+1][j] + U[i][j-1] + U[i][j+1] - 4.0f * u);
      float lapV = (V[i-1][j] + V[i+1][j] + V[i][j-1] + V[i][j+1] - 4.0f * v);
          
      dU[i][j] = diffU*lapU - uvv + F*(1.0f-u);
      dV[i][j] = diffV*lapV + uvv - (K+F)*v;
          
    }
  }
      
  for (int j = 0; j < HEIGHT; j++){   
    for (int i= 0; i < WIDTH; i++) {
         
      U[i][j] += dU[i][j];
      V[i][j] += dV[i][j];

    }
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
  
  rndrule();
  
}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();
 
  for (int k = 0; k < ITER; k++) timestep(paramF, paramK, diffU, diffV);

  for (int j = 0; j < HEIGHT; j++){   
    for (int i= 0; i < WIDTH; i++) {

      uint8_t coll = 255 * U[i][j];
      col[(4*i)+(4*j)*WFULL] = gray2rgb565[(uint8_t)coll>>2]; 
      
    }
  }

  M5.Lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();

}
