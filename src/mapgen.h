#include <stdlib.h>

static inline float minf(float a, float b) {
    return a > b ? b : a;
}

void fill_random_rect(char **map, int x, int y);
char **GenerateMap(int x, int y);
