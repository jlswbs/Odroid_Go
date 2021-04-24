// Metaballs demo //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define HALFW   160
  #define HALFH   120
  #define SCR    (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

#define BALLS_MAX     6
#define MIN_VELOCITY  3
#define MAX_VELOCITY  5
#define MIN_RADIUS    8
#define MAX_RADIUS    10

uint16_t coll;

uint16_t col1 = TFT_BLUE;
uint16_t col2 = TFT_WHITE;
uint16_t col3 = TFT_YELLOW;

int NUM_BALLS = BALLS_MAX;


struct vector2 {
    int16_t x;
    int16_t y;
};

struct ball {
    struct vector2 position;
    struct vector2 velocity;
    uint16_t radius;
};

struct ball balls[BALLS_MAX];

void metaballs_init(){

  memset((uint16_t *) col, 0, 4*SCR);

  col1 = esp_random();
  col2 = esp_random();
  col3 = esp_random();

  NUM_BALLS = 2 + esp_random()%5;
  
  for (int i = 0; i < NUM_BALLS; i++) {
    balls[i].radius = (esp_random() % MAX_RADIUS) + MIN_RADIUS;
    balls[i].position.x = esp_random() % HALFW;
    balls[i].position.y = esp_random() % HALFH;
    balls[i].velocity.x = (esp_random() % MAX_VELOCITY) + MIN_VELOCITY;
    balls[i].velocity.y = (esp_random() % MAX_VELOCITY) + MIN_VELOCITY;
  }
  
}

void metaballs_animate(){
  
  for (int i = 0; i < NUM_BALLS; i++) {
    balls[i].position.x += balls[i].velocity.x;
    balls[i].position.y += balls[i].velocity.y;

    if ((balls[i].position.x < 0) | (balls[i].position.x > HALFW)) balls[i].velocity.x = balls[i].velocity.x * -1;
    if ((balls[i].position.y < 0) | (balls[i].position.y > HALFH)) balls[i].velocity.y = balls[i].velocity.y * -1;
  }
  
}

void metaballs_render(){  

  for (int y = 0; y < HALFH; y++) {
    for (int x = 0; x < HALFW; x++) {
      float sum = 0;
      for (int i = 0; i < NUM_BALLS; i++) {
        float dx = x - balls[i].position.x;
        float dy = y - balls[i].position.y;
        float d2 = dx * dx + dy * dy;
        sum += balls[i].radius * balls[i].radius / d2;
      }
      
      if (sum > 0.75f) coll = col1;
      else if (sum > 0.5f) coll = col2;
      else if (sum > 0.35f) coll = col3;
      else coll = TFT_BLACK;
      
      col[(2*x)+(2*y)*WIDTH] = coll;
            
    }
  }
}


void setup(){

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  col = (uint16_t*)ps_malloc(4*SCR);

  metaballs_init();
  
}


void loop(){

  if (M5.BtnA.wasPressed()) { metaballs_init(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  metaballs_animate();
  metaballs_render();
  
  M5.lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);

  M5.update();
    
}
