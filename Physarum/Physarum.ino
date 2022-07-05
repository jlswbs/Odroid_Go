// Physarum growth //

#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   160
  #define HEIGHT  120
  #define FULLW   320
  #define FULLH   240
  #define SCR     (FULLW*FULLH)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  
  uint16_t *col = NULL;

  uint16_t RGB565( byte R, byte G, byte B){ return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );}

  #define ITER  10000

  uint16_t image;
  uint16_t *grid = NULL;
  int state[8];
  int t, q;

void rndseed(){

  memset((uint16_t *) col, 0, 4*SCR);

  for (int y = 0; y < HEIGHT; y++){
    
    for (int x = 0; x < WIDTH; x++){
      
      if(x == 0 || x == 1 || x == WIDTH-2 || x == WIDTH-1 || y == 0 || y == 1 || y == HEIGHT-2 || y == HEIGHT-1) grid[x+y*WIDTH] = 1;
      else grid[x+y*WIDTH] = 0;

    }

  }
  
  for (int i = 1; i < 8; i++){
    
    int x = 2 * (5 + esp_random()%(WIDTH/2)-5);
    int y = 2 * (5 + esp_random()%(HEIGHT/2)-5);
    if(grid[x+y*WIDTH] == 0) grid[x+y*WIDTH] = 1000+(i*100);

  }

}
  
void setup() {

  srand(time(NULL));

  M5.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillScreen(TFT_BLACK);

  grid = (uint16_t*)ps_malloc(size);
  col = (uint16_t*)ps_malloc(4*SCR);

  rndseed();
  
}

void nextstep(){

  for (int i = 0; i < ITER; i++){
  
    int x = 2 * (1 + esp_random()%(WIDTH/2)-1);
    int y = 2 * (1 + esp_random()%(HEIGHT/2)-1);
    
    if(grid[x+y*WIDTH] >= 100 && grid[x+y*WIDTH] < 1000){
      
      q = grid[x+y*WIDTH]/100;
      int p = grid[x+y*WIDTH] - (q*100);
      
      if(p < 30){
        
        t = 1 + esp_random()%5;
        if(t == 1 && grid[(x+2)+y*WIDTH] == 0){ grid[(x+2)+y*WIDTH] = q*100; grid[(x+1)+y*WIDTH] = q*100; } 
        if(t == 2 && grid[x+(y+2)*WIDTH] == 0){ grid[x+(y+2)*WIDTH] = q*100; grid[x+(y+1)*WIDTH] = q*100; } 
        if(t == 3 && grid[(x-2)+y*WIDTH] == 0){ grid[(x-2)+y*WIDTH] = q*100; grid[(x-1)+y*WIDTH] = q*100; } 
        if(t == 4 && grid[x+(y-2)*WIDTH] == 0){ grid[x+(y-2)*WIDTH] = q*100; grid[x+(y-1)*WIDTH] = q*100; } 
        grid[x+y*WIDTH] = grid[x+y*WIDTH] + 1;
        
      } else {
        
        t = 0;
        if(grid[(x+1)+y*WIDTH] > 1) t = t + 1;
        if(grid[x+(y+1)*WIDTH] > 1) t = t + 1;
        if(grid[(x-1)+y*WIDTH] > 1) t = t + 1;
        if(grid[x+(y-1)*WIDTH] > 1) t = t + 1;
        if(t <= 1){
          grid[x+y*WIDTH] = 9100;
          grid[(x+1)+y*WIDTH] = 0;
          grid[x+(y+1)*WIDTH] = 0;
          grid[(x-1)+y*WIDTH] = 0;
          grid[x+(y-1)*WIDTH] = 0; 
        }
      }      
    }
    
    if(grid[x+y*WIDTH] >= 1000 && grid[x+y*WIDTH] < 2000){
      
      q = (grid[x+y*WIDTH]/100)-10;
      if(state[q] == 0 && grid[(x+2)+y*WIDTH] == 0){ grid[(x+2)+y*WIDTH] = q*100; grid[(x+1)+y*WIDTH] = q*100; }
      if(state[q] == 0 && grid[x+(y+2)*WIDTH] == 0){ grid[x+(y+2)*WIDTH] = q*100; grid[x+(y+1)*WIDTH] = q*100; }
      if(state[q] == 0 && grid[(x-2)+y*WIDTH] == 0){ grid[(x-2)+y*WIDTH] = q*100; grid[(x-1)+y*WIDTH] = q*100; }
      if(state[q] == 0 && grid[x+(y-2)*WIDTH] == 0){ grid[x+(y-2)*WIDTH] = q*100; grid[x+(y-1)*WIDTH] = q*100; }
    
    }
    
    if(grid[x+y*WIDTH] >= 9000){
      
      grid[x+y*WIDTH] = grid[x+y*WIDTH] - 1;
      if(grid[x+y*WIDTH] < 9000) grid[x+y*WIDTH] = 0;
    
    }
  }
  
}

void loop() {

  if (M5.BtnA.wasPressed()) { rndseed(); M5.Lcd.drawString("Seed", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) esp_restart();

  nextstep();

  memset((uint16_t *) col, 0, 4*SCR);

  for (int y = 0; y < HEIGHT; y++){
    
    for (int x = 0; x < WIDTH; x++){
    
      if(grid[x+y*WIDTH] == 1) image = RGB565(0,0,0);
      if(grid[x+y*WIDTH] >= 100 && grid[x+y*WIDTH] < 1000){
        q = grid[x+y*WIDTH]/100;
        if(q == 1) image = RGB565(0,0,255);
        if(q == 2) image = RGB565(0,255,0);
        if(q == 3) image = RGB565(0,255,255);
        if(q == 4) image = RGB565(255,0,0);
        if(q == 5) image = RGB565(255,0,255);
        if(q == 6) image = RGB565(255,255,0);
        if(q == 7) image = RGB565(255,255,255);
      
        col[(2*x)+(2*y)*FULLW] = image;
      
      }
    }
  }
  
  M5.Lcd.pushRect(0, 0, FULLW, FULLH,(uint16_t *) col);
  M5.update();

}
