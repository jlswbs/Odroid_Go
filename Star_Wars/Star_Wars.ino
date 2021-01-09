// StarWars Cellular Automata //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)
  #define SCR2    (WIDTH*HEIGHT)

  uint16_t RGB565(uint8_t R, uint8_t G, uint8_t B){ return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );}

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;
  uint16_t coll;

  int ALIVE = 3;
  int DEATH_1 = 2;
  int DEATH_2 = 1;
  int DEAD = 0;

  uint8_t *current = NULL;
  uint8_t *next = NULL;
  uint8_t *alive_counts = NULL;
  uint8_t *temp = NULL;

  int x, y, nx, ny;


void rndrule(){
  
  int x, y;
  
  memset((uint16_t *) col, 0, 4*SCR);
  memset(alive_counts, 0, 4*SCR2);

  for (y = 0; y < HEIGHT; y++) { for (x = 0; x < WIDTH; x++) current[x+y*WIDTH] = (esp_random() % 100) < 20 ? ALIVE : DEAD; }

}

void symrule(){
  
  int x, y;

  uint8_t k = 1 + esp_random() % (HEIGHT/2);
  
  memset((uint16_t *) col, 0, 4*SCR);
  memset(alive_counts, 0, 4*SCR2);
  memset(current, 0, 4*SCR2);

  for (y = (HEIGHT/2)-k; y < (HEIGHT/2)+k; y+=2) { for (x = (WIDTH/2)-k; x < (WIDTH/2)+k; x+=2) current[x+y*WIDTH] = ALIVE; }

}

void step(){
  
  for (x = 0; x < WIDTH; x++) {
    for (y = 0; y < HEIGHT; y++) {
    
      int count = 0;
      int next_val;   
      int mx = WIDTH-1;
      int my = HEIGHT-1;    
      int self = current[x+y*WIDTH];
    
      for (nx = x-1; nx <= x+1; nx++) { 
        for (ny = y-1; ny <= y+1; ny++) { 
          if (nx == x && ny == y) continue;    
          if (current[wrap(nx, mx) + wrap(ny, my) * WIDTH] == ALIVE) count++;    
        }   
      }  

      int neighbors = count;
      
      if (self == ALIVE) next_val = ((neighbors == 3) || (neighbors == 4) || (neighbors == 5)) ? ALIVE : DEATH_1;
      else if (self == DEAD) next_val = (neighbors == 2) ? ALIVE : DEAD;
      else next_val = self-1;
   
      next[x+y*WIDTH] = next_val;
  
      if (next_val == ALIVE) alive_counts[x+y*WIDTH] = min(alive_counts[x+y*WIDTH]+1, 100);
      else if (next_val == DEAD) alive_counts[x+y*WIDTH] = 0;
    }
  }
    
  memcpy(temp, current, 4*SCR2);
  memcpy(current, next, 4*SCR2);
  memcpy(next, temp, 4*SCR2);

}
  
int wrap(int v, int m){

    if (v < 0) return v + m;
    else if (v >= m) return v - m;
    else return v;

}
  
void draw_type(int min_alive, int max_alive, uint8_t r, uint8_t g, uint8_t b){  
    
  for (x = 0; x < WIDTH; x++) {      
    for (y = 0; y < HEIGHT; y++) {
    
      int self = current[x+y*WIDTH];
      if (self == DEAD) continue;
   
      int alive = alive_counts[x+y*WIDTH];
      if (alive < min_alive || alive > max_alive) continue;
        
      if (self == ALIVE) coll = RGB565(r, g, b);
      else if (self == DEATH_1) coll = RGB565(r>>1, g>>1, b>>1);
      else if (self == DEATH_2) coll = RGB565(r>>2, g>>2, b>>2);
      
      col[(2*x)+(2*y)*WFULL] = coll;
   
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

  current = (uint8_t*)ps_malloc(4*SCR2);
  next = (uint8_t*)ps_malloc(4*SCR2);
  alive_counts = (uint8_t*)ps_malloc(4*SCR2);
  temp = (uint8_t*)ps_malloc(4*SCR2);
  col = (uint16_t*)ps_malloc(4*SCR);

  rndrule();
  
}


void loop(){

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { symrule(); M5.Lcd.drawString("Symmetry", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  step();
      
  memset((uint16_t *) col, 0, 4*SCR);
  
  draw_type(50, 100, 100, 100, 255);
  draw_type(2, 49, 255, 100, 100);
  draw_type(0, 1, 255, 255, 255);
  
  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();
    
}
