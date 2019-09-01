// Brian's Brain Cellular Automata //

#include <odroid_go.h>

const unsigned char startup_music[] = {0};

#define WIDTH 162
#define HEIGHT 162

#define DENSITY     7
#define READY       0
#define REFRACTORY  1
#define FIRING      2

  uint16_t world[WIDTH][HEIGHT];
  uint16_t temp[WIDTH][HEIGHT];


int weighted_randint(int true_weight)
{
    int choice = esp_random() % 10;
    
    if (choice > true_weight) return 1;
    else return 0;
}


int count_neighbours(uint16_t world[WIDTH][HEIGHT], int x_pos, int y_pos)
{
    int x, y, cx, cy, cell;
    int count = 0;

    for (y = -1; y < 2; y++) {
        for (x = -1; x < 2; x++) {
            cx = x_pos + x;
            cy = y_pos + y;
            if ( (0 <= cx && cx < WIDTH) && (0 <= cy && cy < HEIGHT)) {
                cell = world[x_pos + x][y_pos + y];
                if (cell == FIRING) count ++;
            }
        }
    }
  return count;
}


void apply_rules(uint16_t world[WIDTH][HEIGHT])
{
  int x, y, cell, neighbours;

  memcpy(temp, world, sizeof(temp));

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++){
      cell = temp[x][y];          
      if (cell == READY) {
        neighbours = count_neighbours(temp, x, y);
        if (neighbours == 2) world[x][y] = FIRING; }
      else if (cell == FIRING) world[x][y] = REFRACTORY;
      else world[x][y] = READY;
    }
  }
}

void populate()
{
  int x, y, r;
  
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++){
      r = weighted_randint(DENSITY);
      if (r == 1) world[x][y] = FIRING;
      else world[x][y] = READY;
    }
  }
}
  
void setup() {

  GO.begin();
  GO.Speaker.setVolume(1);
  GO.Speaker.playMusic(startup_music, 22050);
  GO.lcd.fillScreen(BLACK);

  GO.lcd.setCursor(68, 8);
  GO.lcd.print("Brian's Brain Cellular Automata");
  GO.lcd.setCursor(86, 224);
  GO.lcd.print("Press button A = Restart");

  srand(time(NULL));

  populate();
  
}


void loop() {

  if (GO.BtnA.wasPressed()) populate();
   
  apply_rules(world);
  
  int x, y;
    
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++){
      if (world[x][y] == FIRING) temp[x][y] = WHITE;   
      else if (world[x][y] == REFRACTORY) temp[x][y] = WHITE;
      else temp[x][y] = BLACK;
    }
  }

  GO.lcd.drawBitmap(79, 39, WIDTH, HEIGHT,(uint16_t *) temp);

  GO.update();
    
}
