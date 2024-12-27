#if 0

#include "mapgen.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>

struct BSPNode;

typedef struct BSPNode {
    int x0, x1, y0, y1;

    int split_direction;
    int symbol;
    struct BSPNode *r, *l;
} BSPNode;

enum Direction {
    DIR_VERTICAL = 0,
    DIR_HORIZONTAL = 0
};

int RandomSymbol() {
    // char chars[] = "#+<O=";
    char chars[] = "qwertyuiopasdfghjklzxcvbnm1234567890!@#$%^&*()-=_+<>?/,.;'\":";
    char symbol_char = chars[(int)(randf() * strlen(chars))];
    // fprintf(stderr, "\nCog %c %d\n", symbol_char, symbol_char);
    // int color_pair = randf() * 4 + 1;
    // return symbol_char | COLOR_PAIR(color_pair);
    return symbol_char;
}

void SplitNode(BSPNode *node, int direction) {
    node->split_direction = direction;
    node->r = malloc(sizeof(BSPNode));
    node->l = malloc(sizeof(BSPNode));

    node->r->r = NULL;
    node->r->l = NULL;
    node->r->symbol = RandomSymbol();

    node->l->r = NULL;
    node->l->l = NULL;
    node->l->symbol = RandomSymbol();

    if (direction == DIR_HORIZONTAL) {
        node->r->y0 = node->y0;
        node->l->y0 = node->y0;
        node->r->y1 = node->y1;
        node->l->y1 = node->y1;

        // int mid = (node->x0 + node->x1) / 2;
        int mid = (node->x0 + node->x1) / 2 + (randf() - 0.5) * 0.0f * (node->x1 - node->x0);
        node->r->x0 = node->x0;
        node->r->x1 = mid;
        node->l->x0 = mid;
        node->l->x1 = node->x1;
    } else {
        node->r->x0 = node->x0;
        node->l->x0 = node->x0;
        node->r->x1 = node->x1;
        node->l->x1 = node->x1;

        // int mid = (node->y0 + node->y1) / 2;
        int mid = (node->y0 + node->y1) / 2 + (randf() - 0.5) * 0.0f * (node->y1 - node->y0);
        node->r->y0 = node->y0;
        node->r->y1 = mid;
        node->l->y0 = mid;
        node->l->y1 = node->y1;
    }
}

void fprintNode(FILE *f, BSPNode *node) {
    fprintf(f, "Node (%c) { %d..%d, %d..%d }", (char)node->symbol, node->x0, node->x1, node->y0, node->y1);
}

// TODO: Split on the longest length
void SplitSomewhere(BSPNode *node, int prev_dir) {
    if (node->r == NULL) { // Is a leaf
        // SplitNode(node, (node->x1 - node->x0) < (node->y1 - node->y0));
        SplitNode(node, 1 - prev_dir);
    } else {
        assert(node->l);
        BSPNode *next;
        if (randb())
            next = node->r;
        else
            next = node->l;

        SplitSomewhere(next, node->split_direction);
    }
}

void GenerateBSP(BSPNode *node, int n_leafs) {
    while (n_leafs--) {
        SplitSomewhere(node, randb());
    }
}

void RenderBSP(BSPNode *node, char **map) {
    for (int i = node->x0; i < node->x1; i++)
        for (int j = node->y0; j < node->y1; j++)
            map[i][j] = node->symbol;

    if (node->r) {
        assert(node->l);
        RenderBSP(node->r, map);
        RenderBSP(node->l, map);
    }
}

// void GenerateBSP(BSPNode *node, int *n_leaf_remaining) {
//     if (n_leaf_remaining == 1) {
//         node->r = NULL;
//         node->l = NULL;
//         n_leaf_remaining--;
//     } else {
//         BSPNode *new = malloc(sizeof(BSPNode));

//         if (randf() > 0.5) {
//             node->r = new;
//             node->l = NULL;
//         } else {
//             node->r = NULL;
//             node->l = new;
//         }

//         GenerateBSP(new, n - 1);
//     } else {
//         node->r = malloc(sizeof(BSPNode));
//         node->l = malloc(sizeof(BSPNode));

//         GenerateBSP(node->r, n - 2);
//     }
// }

void FillRandomRect(char **map, int x, int y) {
    float a = randf();
    float b = randf();

    float x0 = minf(a, b);
    float x1 = a + b - x0;

    a = randf();
    b = randf();

    float y0 = minf(a, b);
    float y1 = a + b - y0;

    x0 *= x;
    x1 *= x;

    y0 *= y;
    y1 *= y;

    // fprintf(stderr, "%f %f %f %f\n", x0, x1, y0, y1);

    for (int i = 0; i < x; i++) {
        // map[i] = malloc(y);
        for (int j = 0; j < y; j++) {
            if (x0 < i && i < x1 && y0 < j && j < y1)
                map[i][j] = '.';
                // map[i][j] = ' ';
            else {

                // map[i][j] = ' ';
                // fprintf(stderr, "%f %d %f, %f %d %f\n", x0, i, x1, y0, j, y1);
            }
                // map[i][j] = '#';

        }
    }
}

char **GenerateMap(int x, int y) {
    // srand(time(NULL));
    // srand(49);

    // fprintf(stderr, "Generating: %d, %d\n", x, y);

    BSPNode node = { .x0 = 0, .y0 = 0, .x1 = x, .y1 = y, .symbol = RandomSymbol(), .r = NULL, .l = NULL };
    GenerateBSP(&node, 6);
    // SplitNode(&node, 1);
    // SplitNode(node.r, 0);
    // fprintNode(stderr, &node); fprintf(stderr, "\n");
    // fprintf(stderr, "\t"); fprintNode(stderr, node.r); fprintf(stderr, "\n");
    // fprintf(stderr, "\t\t"); fprintNode(stderr, node.r->r); fprintf(stderr, "\n");
    // fprintf(stderr, "\t\t"); fprintNode(stderr, node.r->l); fprintf(stderr, "\n");
    // fprintf(stderr, "\t"); fprintNode(stderr, node.l); fprintf(stderr, "\n");

    char **map = malloc(x * sizeof(char *));
    for (int i = 0; i < x; i++) {
        map[i] = malloc(y);
        memset(map[i], ' ', y);
    }

    RenderBSP(&node, map);

    // for (int i = node.l->x0; i < node.l->x1; i++)
    //     for (int j = node.l->y0; j < node.l->y1; j++)
    //         map[i][j] = node.l->symbol;

    // for (int i = node.r->r->x0; i < node.r->r->x1; i++)
    //     for (int j = node.r->r->y0; j < node.r->r->y1; j++)
    //         map[i][j] = node.r->r->symbol;

    // for (int i = node.r->l->x0; i < node.r->l->x1; i++)
    //     for (int j = node.r->l->y0; j < node.r->l->y1; j++)
    //         map[i][j] = node.r->l->symbol;

    // FillRandomRect(map, x, y);
    // FillRandomRect(map, x, y);
    // FillRandomRect(map, x, y);
    // FillRandomRect(map, x, y);
    // FillRandomRect(map, x, y);
    // FillRandomRect(map, x, y);
    // FillRandomRect(map, x, y);

    return map;
}
#endif

