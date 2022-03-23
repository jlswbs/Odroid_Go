// Gumowski-Mira chaotic map //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR     (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

  #define ITER  2000

  uint16_t coll = TFT_WHITE;
  bool colen = true;
  float a = 0.000001f;
  float b = 0.05f;
  float m = -0.031f;

  float x = 0.0f;
  float y = 0.5f;

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31))*(maxf - minf) / (1UL << 31);}  

float mira(float x) {
  
  float xx = x * x;
  return (m * x) + ((2.0f * (1.0f - m) * xx) / (1.0f + xx));
  
}

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  x = 0.0f;
  y = 0.5f;
  m = randomf(-0.95f, -0.05f);

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

  if (colen == false) memset((uint16_t *) col, 0, 4*SCR);

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { colen = !colen; M5.Lcd.drawString("COLOR", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();

  coll = esp_random();

  for (int i = 0; i < ITER; i++) {

    float nx = x;
    float ny = y;
    
    x = ny + a * (1.0f - (b * powf(ny,2.0f))) * ny + mira(x);
    y = -nx + mira(x);

    uint16_t xx = -8 + (WIDTH/2) + 4.0f * x; 
    uint16_t yy = (HEIGHT/2) + 4.0f * y;
    
    if (coll == TFT_WHITE && xx == (WIDTH/2)+8){
      memset((uint16_t *) col, 0, 4*SCR);
      coll = TFT_WHITE;
      m = randomf(-0.95f, -0.05f);
    } else if (xx == yy) coll = esp_random()%65535;
    
    if (xx > 0 && xx < WIDTH && yy > 0 && yy < HEIGHT) {
      if (colen) col[xx+yy*WIDTH] = coll;
      else col[xx+yy*WIDTH] = TFT_WHITE;
    }

  }

  M5.Lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);
  M5.update();

}
