#include <complex.h>
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define TILESIZE 50

#define SCREEN_W 800
#define SCREEN_H 600
#define RECT_COUNT (SCREEN_H / TILESIZE) * (SCREEN_W / TILESIZE)
#define GRID_H (int) (SCREEN_W / TILESIZE);
#define GRID_W (int) (SCREEN_H / TILESIZE);

Rectangle rectangles[RECT_COUNT];

Rectangle RECTANGLE_TYPES[] = {
    (Rectangle){0, 0, 0, 0},
    (Rectangle){0, 0, TILESIZE, TILESIZE},
    (Rectangle){0, 0, TILESIZE/2.0, TILESIZE},
    (Rectangle){0, 0, TILESIZE, TILESIZE/2.0},
    (Rectangle){TILESIZE/2.0, 0, TILESIZE/2.0, TILESIZE},
    (Rectangle){0, TILESIZE/2.0, TILESIZE, TILESIZE/2.0},
};

void draw_grid() {
    // vertical lines
    for (int x = 0; x < SCREEN_W; x += TILESIZE) {
        DrawLine(x, 0, x, SCREEN_H, DARKGRAY);
    }
    // vertical lines
    for (int y = 0; y < SCREEN_H; y += TILESIZE) {
        DrawLine(0, y, SCREEN_W, y, DARKGRAY);
    }
}

int rect_valid(Rectangle rect) {
    return rect.width || rect.height;
}

int rect_type(Rectangle rect) {
    if (!rect_valid(rect))
        return 0;

    if (rect.width == TILESIZE / 2.0) {
        return (int) rect.x % TILESIZE == 0 ? 2 : 4;
    }

    if (rect.height == TILESIZE / 2.0) {
        return (int) rect.y % TILESIZE == 0 ? 3 : 5;
    }

    return 1;
}

int rects_touch_y(Rectangle rect_a, Rectangle rect_b) {
    return rect_a.x == rect_b.x && rect_a.width == rect_b.width &&
           (rect_a.y == rect_b.y + rect_b.height ||
            rect_b.y == rect_a.y + rect_a.height);
}

int rects_touch_x(Rectangle rect_a, Rectangle rect_b) {
    return rect_a.y == rect_b.y && rect_a.height == rect_b.height &&
           (rect_a.x == rect_b.x + rect_b.width ||
            rect_b.x == rect_a.x + rect_a.width);
}

void optimise_rectangles(Rectangle* rectangles, Rectangle *optimised) {
    Rectangle rect_a, rect_b;

    // copy rectangles to optimised

    for (int i = 0; i < RECT_COUNT; i++) {
        Rectangle rectangle = rectangles[i];
        if (rect_valid(rectangle)) {
            optimised[i] = rectangle;
        }
    }

    // merge rectangles in x direction

    for (int i = 0; i < RECT_COUNT; i++) {
        if (rect_valid(rect_a = optimised[i]))
            for (int j = 0; j < RECT_COUNT; j++) {
              if (i != j && rect_valid(rect_b = optimised[j]) 
                  && rects_touch_x(rect_a, rect_b)) {
                optimised[i].x = rect_a.x < rect_b.x ? rect_a.x : rect_b.x;
                optimised[i].width = rect_a.width + rect_b.width;
                rect_a = optimised[i];
                // delete j
                optimised[j].width = 0;
              }
            }
    }

    // merge rectangles in y direction
     
    for (int i = 0; i < RECT_COUNT; i++) {
        if (rect_valid(rect_a = optimised[i]))
            for (int j = 0; j < RECT_COUNT; j++) {
              if (i != j && rect_valid(rect_b = optimised[j]) 
                  && rects_touch_y(rect_a, rect_b)) {

                optimised[i].y = rect_a.y < rect_b.y ? rect_a.y : rect_b.y;
                optimised[i].height = rect_a.height + rect_b.height;
                rect_a = optimised[i];
                // delete j
                optimised[j].width = 0;
              }
            }
    }
}

void draw_rectangles() {
    for (int i = 0; i < RECT_COUNT; i++) {
        Rectangle rectangle = rectangles[i];
        if (rect_valid(rectangle)) {
            DrawRectangleRec(rectangle, DARKGRAY);
        }
    }
}

void draw_optimised_rectangles() {
    Rectangle *optimised = malloc(sizeof(Rectangle) * RECT_COUNT);

    optimise_rectangles(rectangles, optimised);

    Rectangle rectangle;
    for (int i = 0; i < RECT_COUNT; i++) {
        if (rect_valid(rectangle = optimised[i]))
            DrawRectangleLinesEx(rectangle, 2, GREEN);
    }

    free(optimised);
}

int draw(int *mode) {

    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    draw_grid();
    draw_rectangles();
    draw_optimised_rectangles();

    int gridx = (int) floor(GetMouseX() / TILESIZE);
    int gridy = (int) floor(GetMouseY() / TILESIZE);

    int rect_index = gridx + gridy*GRID_H;

    Rectangle rectangle = (Rectangle){gridx * TILESIZE, gridy * TILESIZE, TILESIZE, TILESIZE};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
       *mode = (rect_type(rectangles[rect_index]) + 1) 
           % (sizeof(RECTANGLE_TYPES)/sizeof(Rectangle));
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Rectangle rectangle = RECTANGLE_TYPES[*mode];
        rectangle.x += gridx * TILESIZE;
        rectangle.y += gridy * TILESIZE;
        rectangles[rect_index] = rectangle;
    }

    EndDrawing();
}

int main() {
    SetTargetFPS(60);
    InitWindow(SCREEN_W, SCREEN_H, "Rectangles");

    for (int i = 0; i < RECT_COUNT; i++) {
        rectangles[i] = (Rectangle){0, 0, 0, 0};
    }

    int mode = 0;
    while(!WindowShouldClose()) {
        draw(&mode);
    }

    CloseWindow();
    
    return 0;
}
