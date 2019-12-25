// Conway's Game of Life //

#include <odroid_go.h>

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

  uint8_t *grid = NULL;
  uint8_t *newgrid = NULL;


void initGrid(void) {

  int x,y;
  
  memset((uint16_t *) col, 0, 4*SCR);
  memset(newgrid, 0, 4*SCR2);
  
  for (x = 0; x < WIDTH; x++) {
    for (y = 0; y < HEIGHT; y++) {     
      if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) grid[x+y*WIDTH] = 0;
      else {
        if (esp_random()%3 == 1) grid[x+y*WIDTH] = 1;
        else grid[x+y*WIDTH] = 0;
      }
    }
  }
}

void drawGrid(void) {

  int x,y;
  
  for (x = 1; x < WIDTH - 1; x++) {
    for (y = 1; y < HEIGHT - 1; y++) {
      if ((grid[x+y*WIDTH]) != (newgrid[x+y*WIDTH])) {
        if (newgrid[x+y*WIDTH] == 1) col[(2*x)+(2*y)*WFULL] = WHITE;
        else col[(2*x)+(2*y)*WFULL] = BLACK;
      }
    }
  }
  
  memcpy(grid, newgrid, 4*SCR2);
  
}

void computeCA() {

  int x,y;
  
  for (x = 1; x < WIDTH; x++) {
    for (y = 1; y < HEIGHT; y++) {
      int neighbors = getNumberOfNeighbors(x, y);
      if (grid[x+y*WIDTH] == 1 && (neighbors == 2 || neighbors == 3 ))
      {
        newgrid[x+y*WIDTH] = 1;
      }
      else if (grid[x+y*WIDTH] == 1)  newgrid[x+y*WIDTH] = 0;
      if (grid[x+y*WIDTH] == 0 && (neighbors == 3))
      {
        newgrid[x+y*WIDTH] = 1;
      }
      else if (grid[x+y*WIDTH] == 0) newgrid[x+y*WIDTH] = 0;
    }

  }
}

uint8_t getNumberOfNeighbors(int x, int y) {
  return grid[(x-1)+y*WIDTH] + grid[(x-1)+(y-1)*WIDTH] + grid[x+(y-1)*WIDTH] + grid[(x+1)+(y-1)*WIDTH] + grid[(x+1)+y*WIDTH] + grid[(x+1)+(y+1)*WIDTH] + grid[x+(y+1)*WIDTH] + grid[(x-1)+(y+1)*WIDTH];
}

void setup() {

  srand(time(NULL));

  GO.begin();
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
  GO.lcd.fillScreen(BLACK);

  grid = (uint8_t*)ps_malloc(4*SCR2);
  newgrid = (uint8_t*)ps_malloc(4*SCR2);
  col = (uint16_t*)ps_malloc(4*SCR);

  initGrid();
  
}


void loop() {

  if (GO.BtnA.wasPressed()) initGrid();

  int x, y;

  computeCA();
  drawGrid();
    
  GO.lcd.pushRect(0, 0, WFULL, HFULL,(uint16_t *) col);
  GO.update();
    
}
