// Turmites cellular automata //

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include <ESP32-Chimera-Core.h>

  #define SPEAKER 25
  #define WIDTH   320
  #define HEIGHT  240
  #define SCR    (WIDTH*HEIGHT)

  uint32_t size = ((2*WIDTH) * (2*HEIGHT));
  uint16_t *col = NULL;

  int posx,posy;
  int oldposx, oldposy;
  int state;
  int dir;
  uint8_t world[WIDTH][HEIGHT];
  int moves;
  int filled;
  int current_col;
  uint8_t next_col[4][4];
  uint8_t next_state[4][4];
  uint8_t directions[4][4];
   
void rndrule(){

  memset((uint16_t *) col, 0, 4*SCR);

  state = esp_random()%4;
  filled = 0;
  moves = 0;
  dir = 0;
  posx = WIDTH/2;
  posy = HEIGHT/2;
  
  for(int j=0; j<4; j++){   
    for(int i=0; i<4; i++){         
      next_col[i][j] = esp_random()%4;
      next_state[i][j] = esp_random()%4;
      directions[i][j] = esp_random()%8;
    }   
  }

  for (int j=0; j < HEIGHT; j++){ for (int i = 0; i < WIDTH; i++) world[i][j]=0; } 

  world[posx][posy] = esp_random()%4;

}

void move_turmite(){
  
  moves ++;
  int cols = world[posx][posy];
  
  if((cols > 0) && (next_col[cols][state] == 0)) filled--;
  if((cols == 0) && (next_col[cols][state] > 0)) filled++;
  
  oldposx = posx;
  oldposy = posy;
  current_col = next_col[cols][state];
  world[posx][posy] = next_col[cols][state];
  state = next_state[cols][state];    

  dir = (dir + directions[cols][state]) % 8;

  switch(dir){
    case 0: posy--; break;
    case 1: posy--; posx++; break;
    case 2: posx++; break;
    case 3: posx++; posy++; break;
    case 4: posy++; break;
    case 5: posy++; posx--; break;
    case 6: posx--; break;
    case 7: posx--; posy--; break;
  }

  if(posy < 0) posy = HEIGHT-1;
  if(posy >= HEIGHT) posy = 0;
  if(posx < 0) posx = WIDTH-1;
  if(posx >= WIDTH) posx=0;
  
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
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }

  for(int i=0; i<120; i++){
    
    move_turmite();
    
    switch(current_col){
      case 0: col[oldposx + oldposy * WIDTH] = TFT_RED; break;
      case 1: col[oldposx + oldposy * WIDTH] = TFT_GREEN; break;
      case 2: col[oldposx + oldposy * WIDTH] = TFT_BLUE; break;
      case 3: col[oldposx + oldposy * WIDTH] = TFT_WHITE; break;
    }
    
  }

  if((moves>1000) && (filled < 50)) rndrule();
  
  M5.lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);
  M5.update();
    
}
