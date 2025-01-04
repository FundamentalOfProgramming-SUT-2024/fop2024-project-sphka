#include <stdlib.h>
#include <stdbool.h>

static inline float randf() {
    return (float)rand() / (float)RAND_MAX;
}

static inline int randn(int range) {
    if (!range)
        return 0;

    return rand() % range;
}

static inline bool randb() {
    return randf() > 0.5f;
}
