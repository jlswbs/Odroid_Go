// Brian's Brain Cellular Automata //

#include "esp_partition.h"
#include "esp_ota_ops.h"
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

  #define DENSITY     7
  #define READY       0
  #define REFRACTORY  1
  #define FIRING      2

  uint8_t *world = NULL;
  uint8_t *temp = NULL;


int weighted_randint(int true_weight)
{
    int choice = esp_random() % 10;
    
    if (choice > true_weight) return 1;
    else return 0;
}


int count_neighbours(uint8_t world[], int x_pos, int y_pos)
{
    int x, y, cx, cy, cell;
    int count = 0;

    for (y = -1; y < 2; y++) {
        for (x = -1; x < 2; x++) {
            cx = x_pos + x;
            cy = y_pos + y;
            if ( (0 <= cx && cx < WIDTH) && (0 <= cy && cy < HEIGHT)) {
                cell = world[(x_pos + x)+(y_pos + y)*WIDTH];
                if (cell == FIRING) count ++;
            }
        }
    }
  return count;
}


void apply_rules(uint8_t world[])
{
  int x, y, cell, neighbours;

  memcpy(temp, world, 4*SCR2);

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++){
      cell = temp[x+y*WIDTH];          
      if (cell == READY) {
        neighbours = count_neighbours(temp, x, y);
        if (neighbours == 2) world[x+y*WIDTH] = FIRING; }
      else if (cell == FIRING) world[x+y*WIDTH] = REFRACTORY;
      else world[x+y*WIDTH] = READY;
    }
  }
}

void populate()
{
  int x, y, r;
  
  memset((uint16_t *) col, 0, 4*SCR);

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++){
      r = weighted_randint(DENSITY);
      if (r == 1) world[x+y*WIDTH] = FIRING;
      else world[x+y*WIDTH] = READY;
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

  world = (uint8_t*)ps_malloc(4*SCR2);
  temp = (uint8_t*)ps_malloc(4*SCR2);
  col = (uint16_t*)ps_malloc(4*SCR);

  populate();
  
}


void loop() {

  if (M5.BtnA.wasPressed()) { populate(); M5.Lcd.drawString("RND", 10, 10, 2); }
  if (M5.BtnC.wasPressed()) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    esp_ota_set_boot_partition(partition);
    esp_restart();
  }
   
  apply_rules(world);
  
  int x, y;
    
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++){
      if (world[x+y*WIDTH] == FIRING) col[(2*x)+(2*y)*WFULL] = WHITE;   
      else if (world[x+y*WIDTH] == REFRACTORY) col[(2*x)+(2*y)*WFULL] = YELLOW;
      else col[(2*x)+(2*y)*WFULL] = BLACK;
    }
  }

  M5.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  M5.update();
    
}
