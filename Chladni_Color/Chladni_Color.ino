// Chladni patterns - color //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WIDTH*HEIGHT)
  #define SCR2    (WFULL*HFULL)
  #define NUMS    16

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31))*(maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  float *p = NULL;
  float *v = NULL;
  float a, b, c, d;
  uint16_t color[NUMS];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR2);

  a = randomf(0.005f, 0.05f);
  b = randomf(0.005f, 0.05f);
  c = randomf(0.005f, 0.05f);
  d = randomf(1.0f, 12.0f);
  
  for (int i = 0; i < NUMS; i++) color[i] = esp_random();

  memset(p, 0, 4*SCR);
  memset(v, 0, 4*SCR);

}

void setup(){

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  col = (uint16_t*)ps_malloc(4*SCR2);
  p = (float*)ps_malloc(4*SCR);
  v = (float*)ps_malloc(4*SCR);

  rndrule();
  
}


void loop(){

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();

  int frame = millis() / 32;

  v[(WIDTH/2)+(HEIGHT/2)*WIDTH] = 0;
  p[(WIDTH/2)+(HEIGHT/2)*WIDTH] = (sinf(frame * a) + sinf(frame * b) + sinf(frame * c)) * d;
  
  for (int y = 1; y < HEIGHT-1; y++) {
    for (int x = 1; x < WIDTH-1; x++) {
      v[x+y*WIDTH] += (p[(x-1)+y*WIDTH] + p[(x+1)+y*WIDTH] + p[x+(y-1)*WIDTH] + p[x+(y+1)*WIDTH]) * 0.25f - p[x+y*WIDTH];
    }
  }
  
  for(int y = 0; y < HEIGHT; y++){
    for(int x = 0; x < WIDTH; x++){
      p[x+y*WIDTH] += v[x+y*WIDTH];
      uint8_t coll = NUMS * fabs(constrain(p[x+y*WIDTH], -1.0f, 1.0f));
      col[(2*x)+(2*y)*WFULL] = color[coll%NUMS];
    }
  }
  
  M5.Lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();
    
}
