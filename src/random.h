#include <stdlib.h>

static inline float randf() {
    return (float)rand() / RAND_MAX;
}

static inline int randn(int range) {
    if (!range)
        return 0;

    return rand() % range;
}

static inline int randb() {
    return randf() > 0.5f;
}
