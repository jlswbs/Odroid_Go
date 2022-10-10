// StarWars cellular automata //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WIDTH*HEIGHT)
  #define SCR2    (WFULL*HFULL)

  #define ALIVE     3
  #define DEATH_1   2
  #define DEATH_2   1
  #define DEAD      0

  uint16_t *col = NULL;
  uint8_t current[SCR];
  uint8_t next[SCR];
  uint8_t alive_counts[SCR];
  uint8_t swap[SCR];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR2);

  for(int i = 0; i < SCR; i++){
    current[i] = (esp_random()%100) < 20 ? ALIVE : DEAD;
    alive_counts[i] = 0;
  }

}

void symrule(){

  uint8_t k = 1 + esp_random() % (HEIGHT/2);
  
  memset((uint16_t *) col, 0, 4*SCR2);
  memset(alive_counts, 0, sizeof(alive_counts));
  memset(current, 0, sizeof(current));

  for(int y = (HEIGHT/2)-k; y < (HEIGHT/2)+k; y+=2){
    for (int x = (WIDTH/2)-k; x < (WIDTH/2)+k; x+=2) current[x+y*WIDTH] = ALIVE;
  }

}

void step(){

  for(int y = 0; y < HEIGHT; y++){
    for(int x = 0; x < WIDTH; x++){
    
      int count = 0;
      int next_val;   
      int mx = WIDTH-1;
      int my = HEIGHT-1;    
      int self = current[x+y*WIDTH];
    
      for (int nx = x-1; nx <= x+1; nx++) { 
        for (int ny = y-1; ny <= y+1; ny++) { 
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
    
  memcpy(swap, current, sizeof(swap));
  memcpy(current, next, sizeof(current));
  memcpy(next, swap, sizeof(next));

}
  
int wrap(int v, int m){

    if (v < 0) return v + m;
    else if (v >= m) return v - m;
    else return v;

}
  
void draw_type(int min_alive, int max_alive, uint16_t image){  

  for(int y = 0; y < HEIGHT; y++){  
    for(int x = 0; x < WIDTH; x++){
      
      uint16_t coll;
      int self = current[x+y*WIDTH];
      if (self == DEAD) continue;
      int alive = alive_counts[x+y*WIDTH];
      if (alive < min_alive || alive > max_alive) continue;   
      if (self == ALIVE) coll = image;
      else if (self == DEATH_1) coll = TFT_RED;
      else if (self == DEATH_2) coll = TFT_BLACK;
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

  col = (uint16_t*)ps_malloc(4*SCR);

  rndrule();
  
}


void loop(){

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { symrule(); M5.Lcd.drawString("Symmetry", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();

  step();
      
  draw_type(50, 100, TFT_YELLOW);
  draw_type(2, 49, TFT_BLUE);
  draw_type(0, 1, TFT_WHITE);
  
  M5.Lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();
    
}
