// StarWars Cellular Automata //

#include <M5Stack.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define WFULL   320
  #define HFULL   240
  #define SCR     (WFULL*HFULL)
  #define SCR2    (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

  #define ALIVE   3
  #define DEATH   2
  #define DEAD    0

  uint8_t *current = NULL;
  uint8_t *next = NULL;
  uint8_t *alive_counts = NULL;
  uint8_t *temp = NULL;

  int x, y, nx, ny;


void rndrule(){
  
  int x, y;
  
  memset((uint16_t *) col, 0, 4*SCR);
  memset(alive_counts, 0, 4*SCR2);

  for (x = 0; x < WIDTH; x++) {  
    for (y = 0; y < HEIGHT; y++) current[x+y*WIDTH] = esp_random() % 4;
  }

}

void symrule(){
  
  int x, y;
  
  memset((uint16_t *) col, 0, 4*SCR);
  memset(alive_counts, 0, 4*SCR2);
  memset(current, 0, 4*SCR2);

  current[80+(60*WIDTH)] = 3;
  current[79+(60*WIDTH)] = 3;

}

void step() {
  
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
      
      if (self == ALIVE) next_val = ((neighbors == 3) || (neighbors == 4) || (neighbors == 5)) ? ALIVE : DEATH;
      else if (self == DEAD) next_val = (neighbors == 2) ? ALIVE : DEAD;
      else next_val = self-1;
   
      next[x+y*WIDTH] = next_val;
  
      if (next_val == ALIVE) {
        if ((alive_counts[x+y*WIDTH]+1) <= 100) alive_counts[x+y*WIDTH] = (int)(alive_counts[x+y*WIDTH]+1);
      } else if (next_val == DEAD) alive_counts[x+y*WIDTH] = 0;
    }
  }
    
  memcpy(temp, current, 4*SCR2);
  memcpy(current, next, 4*SCR2);
  memcpy(next, temp, 4*SCR2);

}
  
int wrap(int v, int m) {

    if (v < 0) return v + m;

    else if (v >= m) return v - m;

    else return v;
}
  
void draw_type(int min_alive, int max_alive, uint16_t s) {  
    
  for (x = 0; x < WIDTH; x++) {      
    for (y = 0; y < HEIGHT; y++) {
    
      int self = current[x+y*WIDTH];       
      if (self == DEAD) continue;       
      int alive = alive_counts[x+y*WIDTH]; 
      if (alive < min_alive || alive > max_alive) continue;
      
      col[(2*x)+(2*y)*WFULL] = s;
   
    }
  }
}

  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.lcd.fillScreen(BLACK);

  current = (uint8_t*)ps_malloc(4*SCR2);
  next = (uint8_t*)ps_malloc(4*SCR2);
  alive_counts = (uint8_t*)ps_malloc(4*SCR2);
  temp = (uint8_t*)ps_malloc(4*SCR2);
  col = (uint16_t*)ps_malloc(4*SCR);

  rndrule();
  
}


void loop() {

  if (M5.BtnA.wasPressed()) { rndrule(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnB.wasPressed()) { symrule(); M5.Lcd.drawString("Symmetry", 10, 10, 2); }

   
  step();
      
  memset((uint16_t *) col, 0, 4*SCR);
  
  draw_type(50, 100, YELLOW);
  draw_type(2, 49, PURPLE);
  draw_type(0, 1, WHITE);
  
  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();
    
}
