#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "raylib.h"

#define GRID_WIDTH 800
#define GRID_HEIGHT 600
#define CELL_SIZE 20
#define STEP_SPEED 100

#define MODE_EDIT 0
#define MODE_PLAY 1
#define MODE_COLOR (mode == MODE_PLAY ? GREEN : RED)

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

static int paused = 0;
static long long step = 0;
static long long old_time;
static long long new_time;
static int mode = MODE_EDIT;
static int cell[GRID_WIDTH / CELL_SIZE][GRID_HEIGHT / CELL_SIZE];
static int cell_next[GRID_WIDTH / CELL_SIZE][GRID_HEIGHT / CELL_SIZE];

long long time_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

int mod(int x, int n)
{
    int r = x % n;
    return r < 0 ? r + n : r;
}

int neighbours_count(int i, int j)
{
    int n = 0;
    int cw = GRID_WIDTH / CELL_SIZE;
    int ch = GRID_HEIGHT / CELL_SIZE;

    n += cell[mod((i - 1), cw)][mod((j - 1), ch)];
    n += cell[mod((i), cw)][mod((j - 1), ch)];
    n += cell[mod((i + 1), cw)][mod((j - 1), ch)];
    n += cell[mod((i - 1), cw)][mod((j), ch)];
    n += cell[mod((i + 1), cw)][mod((j), ch)];
    n += cell[mod((i - 1), cw)][mod((j + 1), ch)];
    n += cell[mod((i), cw)][mod((j + 1), ch)];
    n += cell[mod((i + 1), cw)][mod((j + 1), ch)];
    return n;
}

void cgol_step()
{
    for (int i = 0; i < GRID_WIDTH / CELL_SIZE; i++)
    {
        for (int j = 0; j < GRID_HEIGHT / CELL_SIZE; j++)
        {
            int n = neighbours_count(i, j);
            if (cell[i][j] && (n == 2 || n == 3))
            {
                cell_next[i][j] = 1;
            }
            if (!cell[i][j] && n == 3)
            {
                cell_next[i][j] = 1;
            }
        }
    }
    // update cell, clear cell_next
    memcpy(cell, cell_next, ARRAY_LENGTH(cell) * ARRAY_LENGTH(cell[0]) * sizeof(int));
    memset(cell_next, 0, ARRAY_LENGTH(cell_next) * ARRAY_LENGTH(cell_next[0]) * sizeof(int));
    step++;
}

void set_mode(int m)
{
    mode = m;
    if (mode == MODE_PLAY)
    {
        old_time = time_ms();
        step = 0;
    }
}

void handle_keys()
{
    if (IsKeyPressed(KEY_E))
    {
        set_mode(!mode);
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        if (mode != MODE_PLAY)
        {
            set_mode(MODE_PLAY);
            paused = 0;
        }
        else
        {
            paused = !paused;
        }
    }
    if (IsKeyPressed(KEY_S))
    {
        if (mode != MODE_PLAY)
        {
            set_mode(MODE_PLAY);
        }
        paused = 1;
        cgol_step();
    }
    if (mode == MODE_EDIT)
    {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        int ci = (mouseX - mouseX % CELL_SIZE) / CELL_SIZE;
        int cj = (mouseY - mouseY % CELL_SIZE) / CELL_SIZE;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            cell[ci][cj] = 1;
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            cell[ci][cj] = 0;
        }
        if (IsKeyPressed(KEY_C))
        {
            memset(cell, 0, ARRAY_LENGTH(cell) * ARRAY_LENGTH(cell[0]) * sizeof(int));
        }
    }
}

void update_grid()
{
    new_time = time_ms();
    if ((new_time - old_time) < STEP_SPEED)
    {
        return;
    }
    old_time = new_time;
    if (!paused && mode == MODE_PLAY)
    {
        cgol_step();
    }
}

void draw_grid()
{
    for (int i = 0; i < GRID_WIDTH / CELL_SIZE; i++)
    {
        for (int j = 0; j < GRID_HEIGHT / CELL_SIZE; j++)
        {
            if (cell[i][j])
            {
                DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, MODE_COLOR);
            }
        }
    }
    for (int i = 0; i <= GRID_HEIGHT; i += CELL_SIZE)
    {
        DrawLine(0, i, GRID_WIDTH, i, GRAY);
    }
    for (int i = 0; i <= GRID_WIDTH; i += CELL_SIZE)
    {
        DrawLine(i, 0, i, GRID_HEIGHT, GRAY);
    }
}

// TODO ugly
void draw_info()
{
    int b_x = GRID_WIDTH + 10;
    int b_y = 5;
    char s[32];

    // Press LMB to place cells
    DrawText("Press ", b_x, b_y, 20, BLACK);
    b_x += MeasureText("Press ", 20);
    DrawText("LMB ", b_x, b_y, 20, BLUE);
    b_x += MeasureText("LMB ", 20);
    DrawText("to place cells", b_x, b_y, 20, BLACK);
    b_x = GRID_WIDTH + 10;
    b_y += 22;

    // Press RMB to delete cells
    DrawText("Press ", b_x, b_y, 20, BLACK);
    b_x += MeasureText("Press ", 20);
    DrawText("RMB ", b_x, b_y, 20, BLUE);
    b_x += MeasureText("RMB ", 20);
    DrawText("to delete cells", b_x, b_y, 20, BLACK);
    b_x = GRID_WIDTH + 10;
    b_y += 22;

    // Press E to change mode
    DrawText("Press ", b_x, b_y, 20, BLACK);
    b_x += MeasureText("Press ", 20);
    DrawText("E ", b_x, b_y, 20, BLUE);
    b_x += MeasureText("E ", 20);
    DrawText("to change mode", b_x, b_y, 20, BLACK);
    b_x = GRID_WIDTH + 10;
    b_y += 22;

    // Press C to clear the grid
    DrawText("Press ", b_x, b_y, 20, BLACK);
    b_x += MeasureText("Press ", 20);
    DrawText("C ", b_x, b_y, 20, BLUE);
    b_x += MeasureText("C ", 20);
    DrawText("to clear the grid", b_x, b_y, 20, BLACK);
    b_x = GRID_WIDTH + 10;
    b_y += 22;

    // Press S to step
    DrawText("Press ", b_x, b_y, 20, BLACK);
    b_x += MeasureText("Press ", 20);
    DrawText("S ", b_x, b_y, 20, BLUE);
    b_x += MeasureText("S ", 20);
    DrawText("to step", b_x, b_y, 20, BLACK);
    b_x = GRID_WIDTH + 10;
    b_y += 22;

    // Press Space to pause/play
    DrawText("Press ", b_x, b_y, 20, BLACK);
    b_x += MeasureText("Press ", 20);
    DrawText("Space ", b_x, b_y, 20, BLUE);
    b_x += MeasureText("Space ", 20);
    DrawText("to pause/play", b_x, b_y, 20, BLACK);
    b_x = GRID_WIDTH + 10;
    b_y += 22;

    b_y += 22;

    // Mode: %s
    DrawText("Mode: ", b_x, b_y, 20, BLACK);
    b_x += MeasureText("Mode: ", 20);
    DrawText(mode == MODE_PLAY ? "PLAY" : "EDIT", b_x, b_y, 20, MODE_COLOR);
    b_x = GRID_WIDTH + 10;
    b_y += 22;

    if (mode == MODE_PLAY)
    {
        // Paused: %s
        DrawText("Paused: ", b_x, b_y, 20, BLACK);
        b_x += MeasureText("Paused: ", 20);
        DrawText(paused ? "TRUE" : "FALSE", b_x, b_y, 20, paused ? RED : GREEN);
        b_x = GRID_WIDTH + 10;
        b_y += 22;

        // Step: %lld
        DrawText("Step: ", b_x, b_y, 20, BLACK);
        b_x += MeasureText("Step: ", 20);
        sprintf(s, "%lld", step);
        DrawText(s, b_x, b_y, 20, BLUE);
        b_x = GRID_WIDTH + 10;
        b_y += 22;
    }
}

int main()
{
    InitWindow(GRID_WIDTH + 320, GRID_HEIGHT, "Game of Life");
    SetWindowMinSize(GRID_WIDTH, GRID_HEIGHT);
    SetWindowMaxSize(GRID_WIDTH, GRID_HEIGHT);
    SetTargetFPS(60);
    set_mode(MODE_EDIT);

    // glider
    cell[4][3] = 1;
    cell[5][4] = 1;
    cell[5][5] = 1;
    cell[4][5] = 1;
    cell[3][5] = 1;

    while (!WindowShouldClose())
    {
        handle_keys();
        update_grid();
        BeginDrawing();
        {
            ClearBackground(WHITE);
            draw_grid();
            draw_info();
        }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
