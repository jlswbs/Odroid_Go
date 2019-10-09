// Multi-Scale Turing Patterns //

#include <odroid_go.h>

  const unsigned char startup_music[] = {0};

  #define WIDTH  320
  #define HEIGHT 240
  #define n (WIDTH * HEIGHT)

  #define min(a,b) ((a)<(b)?(a):(b))
  #define max(a,b) ((a)>(b)?(a):(b))

  static uint16_t gray2rgb565[64] = {
    0x0000,
    0x2000,
    0x4108,
    0x6108,
    0x8210,
    0xa210,
    0xc318,
    0xe318,
    0x0421,
    0x2421,
    0x4529,
    0x6529,
    0x8631,
    0xa631,
    0xc739,
    0xe739,
    0x0842,
    0x2842,
    0x494a,
    0x694a,
    0x8a52,
    0xaa52,
    0xcb5a,
    0xeb5a,
    0x0c63,
    0x2c63,
    0x4d6b,
    0x6d6b,
    0x8e73,
    0xae73,
    0xcf7b,
    0xef7b,
    0x1084,
    0x3084,
    0x518c,
    0x718c,
    0x9294,
    0xb294,
    0xd39c,
    0xf39c,
    0x14a5,
    0x34a5,
    0x55ad,
    0x75ad,
    0x96b5,
    0xb6b5,
    0xd7bd,
    0xf7bd,
    0x18c6,
    0x38c6,
    0x59ce,
    0x79ce,
    0x9ad6,
    0xbad6,
    0xdbde,
    0xfbde,
    0x1ce7,
    0x3ce7,
    0x5def,
    0x7def,
    0x9ef7,
    0xbef7,
    0xdfff,
    0xffff,
  };


  uint32_t size = ((2*WIDTH) * (2*HEIGHT));

  uint16_t *col = NULL;
  
  int8_t *bestLevel = NULL;
  float *grid = NULL;
  float *blurBuffer = NULL;
  float *variation = NULL;
  float *bestVariation = NULL;
  float *activator = NULL;
  float *inhibitor = NULL;
  float *swap = NULL;
  bool *direction = NULL;
  float *stepSizes = NULL;
  int8_t *radii = NULL;

  float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  int level, i, x, y;
 
  float base;
  int levels;
  float stepScale;
  float stepOffset;

  
void setup() {

  srand(time(NULL));

  GO.begin();
  GO.Speaker.setVolume(1);
  GO.Speaker.playMusic(startup_music, 22050);
  GO.lcd.fillScreen(BLACK);

  bestLevel = (int8_t*)ps_malloc(size);
  grid = (float*)ps_malloc(size);
  blurBuffer = (float*)ps_malloc(size);
  variation = (float*)ps_malloc(size);
  bestVariation = (float*)ps_malloc(size);
  activator = (float*)ps_malloc(size);
  inhibitor = (float*)ps_malloc(size);
  swap = (float*)ps_malloc(size);
  direction = (bool*)ps_malloc(size);
  stepSizes = (float*)ps_malloc(WIDTH);
  radii = (int8_t*)ps_malloc(WIDTH);
  col = (uint16_t*)ps_malloc(size);

  rndrule();
  
}


void loop() {

  if (GO.BtnA.wasPressed()) rndrule();

  memcpy(activator, grid, 4*n);
 
    for (level = 0; level < levels - 1; level++) {
      
      int radius = radii[level];
    
      for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
          int t = y * WIDTH + x;
          if (y == 0 && x == 0) blurBuffer[t] = activator[t];
          else if (y == 0) blurBuffer[t] = blurBuffer[t - 1] + activator[t];
          else if (x == 0) blurBuffer[t] = blurBuffer[t - WIDTH] + activator[t];
          else blurBuffer[t] = blurBuffer[t - 1] + blurBuffer[t - WIDTH] - blurBuffer[t - WIDTH - 1] + activator[t];
        }
      }

    for (y = 0; y < HEIGHT; y++) {
      for (x = 0; x < WIDTH; x++) {
        int minx = max(0, x - radius);
        int maxx = min(x + radius, WIDTH - 1);
        int miny = max(0, y - radius);
        int maxy = min(y + radius, HEIGHT - 1);
        int area = (maxx - minx) * (maxy - miny);
       
        int nw = miny * WIDTH + minx;
        int ne = miny * WIDTH + maxx;
        int sw = maxy * WIDTH + minx;
        int se = maxy * WIDTH + maxx;
       
        int t = y * WIDTH + x;
        inhibitor[t] = (blurBuffer[se] - blurBuffer[sw] - blurBuffer[ne] + blurBuffer[nw]) / area;
      }
    }
    
    for (i = 0; i < n; i++) {
      variation[i] = activator[i] - inhibitor[i];
      if (variation[i] < 0) variation[i] = -variation[i];
    }
 
    if (level == 0) {
      for (i = 0; i < n; i++) {
        bestVariation[i] = variation[i];
        bestLevel[i] = level;
        direction[i] = activator[i] > inhibitor[i];
      } 
    }
    else {
      for (i = 0; i < n; i++) {
        if (variation[i] < bestVariation[i]) {
          bestVariation[i] = variation[i];
          bestLevel[i] = level;
          direction[i] = activator[i] > inhibitor[i];
        }
      }
      
      memcpy(swap, activator, 4*n);
      memcpy(activator, inhibitor, 4*n);
      memcpy(inhibitor, swap, 4*n);
      }
    }

  float smallest = MAXFLOAT;
  float largest = -MAXFLOAT;

  for (i = 0; i < n; i++) {
    float curStep = stepSizes[bestLevel[i]];
    if (direction[i])grid[i] += curStep;
    else grid[i] -= curStep;
    smallest = min(smallest, grid[i]);
    largest = max(largest, grid[i]);
  }

  float range = (largest - smallest) / 2.0f;

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      grid[x+WIDTH*y] = ((grid[x+WIDTH*y] - smallest) / range) - 1.0f; 
      uint8_t coll = 128 + (128 * grid[x+WIDTH*y]);
      col[x+WIDTH*y] = gray2rgb565[(uint8_t)coll>>2];
    }
  }

  GO.lcd.pushRect(0, 0, WIDTH, HEIGHT,(uint16_t *) col);
  GO.update();

}

void rndrule(){
  
  base = randomf(1.5f, 2.4f);
  stepScale = randomf(0.01f, 0.2f);
  stepOffset = randomf(0.01f, 0.4f);
  
  levels = (int) (log(max(WIDTH,HEIGHT)) / logf(base)) - 1.0f;

  for (i = 0; i < levels; i++) {
    int radius = (int)powf(base, i);
    radii[i] = radius;
    stepSizes[i] = logf(radius) * stepScale + stepOffset;
  }

  for (i = 0; i < n; i++) grid[i] = randomf(-1.0f, 1.0f);

}
