#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 60
#define HEIGHT 20 
#define MAX_SHAPES 100

typedef enum
{
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct
{
    int x1, y1;
    int x2, y2;
} LineParams;

typedef struct
{
    int x1, y1;
    int x2, y2;
} RectParams;

typedef struct
{
    int cx, cy;
    int r;
} CircleParams;

typedef struct
{
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleParams;

typedef struct
{
    int id;
    int active;
    ShapeType type;
    union
    {
        LineParams line;
        RectParams rect;
        CircleParams circle;
        TriangleParams triangle;
    } data;
} Shape;

char canvas[HEIGHT][WIDTH];
Shape shapes[MAX_SHAPES];
int next_id = 1;

// Function prototypes
void init_canvas(void);
void display_canvas(void);
void draw_pixel(int x, int y);
void draw_line(int x1, int y1, int x2, int y2);
void draw_rectangle(int x1, int y1, int x2, int y2);
void draw_circle(int cx, int cy, int r);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void redraw_canvas(void);
int add_shape(Shape shape);
int delete_shape(int id);
void list_shapes(void);
int get_safe_int(const char *prompt, int min_val, int max_val);
void show_menu(void);
void handle_add_shape(void);
void handle_delete_shape(void);

void init_canvas(void)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            canvas[y][x] = '_';
        }
    }
}

void display_canvas(void)
{
    printf("\n");

    printf("    ");
    for (int x = 0; x < WIDTH; x++)
    {
        if (x % 10 == 0)
        {
            printf("%d", x / 10);
        }
        else
        {
            printf(" ");
        }
    }
    printf("\n");

    printf("    ");
    for (int x = 0; x < WIDTH; x++)
    {
        printf("%d", x % 10);
    }
    printf("\n");

    printf("   +");
    for (int x = 0; x < WIDTH; x++)
    {
        printf("-");
    }
    printf("+\n");

    for (int y = 0; y < HEIGHT; y++)
    {
        printf("%2d |", y);
        for (int x = 0; x < WIDTH; x++)
        {
            putchar(canvas[y][x]);
        }
        printf("|\n");
    }

    printf("   +");
    for (int x = 0; x < WIDTH; x++)
    {
        printf("-");
    }
    printf("+\n\n");
}

void draw_pixel(int x, int y)
{
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
    {
        canvas[y][x] = '*';
    }
}


void draw_line(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        draw_pixel(x1, y1);
        if (x1 == x2 && y1 == y2)
        {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

// Rectangle Drawing 
void draw_rectangle(int x1, int y1, int x2, int y2)
{
    int min_x = (x1 < x2) ? x1 : x2;
    int max_x = (x1 > x2) ? x1 : x2;
    int min_y = (y1 < y2) ? y1 : y2;
    int max_y = (y1 > y2) ? y1 : y2;

    // Draw horizontal borders
    for (int x = min_x; x <= max_x; x++)
    {
        draw_pixel(x, min_y);
        draw_pixel(x, max_y);
    }
    // Draw vertical borders
    for (int y = min_y; y <= max_y; y++)
    {
        draw_pixel(min_x, y);
        draw_pixel(max_x, y);
    }
}
