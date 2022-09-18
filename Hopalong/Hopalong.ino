// Hopalong orbit fractal //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR     WIDTH*HEIGHT

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

  #define ITER    10000

  long ranfseed;
  float randf() {return (ranfseed=(ranfseed*1629+1)%1048576)/1048576.0f;}

  uint16_t coll;
  float a, b, c, x, y, t;


void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  x = 0.1f;
  y = 0.1f;
  t = 0.0f;
  a = 0.0f;
  b = 0.0f;
  c = 0.0f;

  ranfseed = 1 + esp_random()%SCR;
  
  float pmax = WIDTH, logpmax = logf(pmax);
  
  a = expf(randf()*logpmax);
  b = expf(randf()*logpmax);
  c = randf()*pmax; 

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

  coll = esp_random();

  for (int i=0; i<ITER; i++) {

    float nx = x;
    float ny = y;

    t = sqrtf(fabs(b*nx-c));
    x = ny - ((nx<0) ? t : -t);
    y = a - nx;

    int ax = (WIDTH/2) + (x/2);
    int ay = (HEIGHT/2) + (y/2);
      
    if (ax>0 && ax<WIDTH && ay>0 && ay <HEIGHT) col[ax+ay*WIDTH] = coll;

  }

  M5.Lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);
  M5.update();

}
