// Mira chaotic map //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR    (WIDTH*HEIGHT)

  #define ITER  2000

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

  uint16_t coll = TFT_WHITE;
  float x = 1.0f;
  float y = 0.0f;
  float f = 0.0f;
  float a = 0.2f;
  

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31))*(maxf - minf) / (1UL << 31);}  

void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);
  x = 1.0f;
  y = 0.0f;
  f = 0.0f;
  a = randomf(0.099f, 0.499f);

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
  if (M5.BtnC.wasPressed()) esp_restart();
 
  for (int i = 0; i < ITER; i++){
    
    float nx = x;
    float ny = y;
        
    x = ny + f;
    f = a * x + ((2.0f*(1.0f-a)*powf(x,2.0f))/(1.0f+powf(x,2.0f)));
    y = -nx + f;

    uint16_t xx = (WIDTH/2) + 8.5f * x; 
    uint16_t yy = (HEIGHT/2) + 8.5f * y; 
      
    if (coll == TFT_WHITE && xx == (WIDTH/2)+8){
      memset((uint16_t *) col, 0, 4*SCR);
      coll = TFT_WHITE;
      a = randomf(0.099f, 0.499f);
    } else if (xx == yy) coll = esp_random()%65535;
      
    if(xx > 0 && xx < WIDTH && yy > 0 && yy < HEIGHT) col[xx+yy*WIDTH] = coll;
    
  }
  
  M5.Lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);
  M5.update();

}
