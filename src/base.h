#pragma once

typedef struct {
    int x, y;
} Coord;

static inline int Square(int a) {
    return a * a;
}

static inline int SqDistance(Coord a, Coord b) {
    return Square(a.x - b.x) + Square(a.y - b.y);
}
