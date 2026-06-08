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
// Midpoint/Bresenham Circle Drawing Algorithm
void draw_circle(int cx, int cy, int r)
{
    if (r < 0)
        return;
    if (r == 0)
    {
        draw_pixel(cx, cy);
        return;
    }

    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (y >= x)
    {
        // Draw the octants
        draw_pixel(cx + x, cy + y);
        draw_pixel(cx - x, cy + y);
        draw_pixel(cx + x, cy - y);
        draw_pixel(cx - x, cy - y);
        draw_pixel(cx + y, cy + x);
        draw_pixel(cx - y, cy + x);
        draw_pixel(cx + y, cy - x);
        draw_pixel(cx - y, cy - x);

        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// Drawing TRIANGLE
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// Clearing the canvas and redrawing all the shapes
void redraw_canvas(void)
{
    init_canvas();
    for (int i = 0; i < MAX_SHAPES; i++)
    {
        if (shapes[i].active)
        {
            switch (shapes[i].type)
            {
            case SHAPE_LINE:
                draw_line(shapes[i].data.line.x1, shapes[i].data.line.y1, shapes[i].data.line.x2, shapes[i].data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(shapes[i].data.rect.x1, shapes[i].data.rect.y1, shapes[i].data.rect.x2, shapes[i].data.rect.y2);
                break;
            case SHAPE_CIRCLE:
                draw_circle(shapes[i].data.circle.cx, shapes[i].data.circle.cy, shapes[i].data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(shapes[i].data.triangle.x1, shapes[i].data.triangle.y1, shapes[i].data.triangle.x2, shapes[i].data.triangle.y2, shapes[i].data.triangle.x3, shapes[i].data.triangle.y3);
                break;
            }
        }
    }
}

// Add a shape
int add_shape(Shape shape)
{
    for (int i = 0; i < MAX_SHAPES; i++)
    {
        if (!shapes[i].active)
        {
            shapes[i] = shape;
            shapes[i].active = 1;
            shapes[i].id = next_id++;
            redraw_canvas();
            return shapes[i].id;
        }
    }
    return -1; // Registry full}

    // Delete a shape from the registry by ID
    int delete_shape(int id)
    {
        for (int i = 0; i < MAX_SHAPES; i++)
        {
            if (shapes[i].active && shapes[i].id == id)
            {
                shapes[i].active = 0;
                redraw_canvas();
                return 1; // Success}}return 0; // Not found}

                // List all the current active shapes
                void list_shapes(void)
                {
                    int count = 0;
                    printf("\n=== Active Shapes ===\n");
                    for (int i = 0; i < MAX_SHAPES; i++)
                    {
                        if (shapes[i].active)
                        {
                            count++;
                            printf("ID %d: ", shapes[i].id);
                            switch (shapes[i].type)
                            {
                            case SHAPE_LINE:
                                printf("Line from (%d, %d) to (%d, %d)\n", shapes[i].data.line.x1, shapes[i].data.line.y1, shapes[i].data.line.x2, shapes[i].data.line.y2);
                                break;
                            case SHAPE_RECTANGLE:
                                printf("Rectangle corners (%d, %d) and (%d, %d)\n", shapes[i].data.rect.x1, shapes[i].data.rect.y1, shapes[i].data.rect.x2, shapes[i].data.rect.y2);
                                break;
                            case SHAPE_CIRCLE:
                                printf("Circle center (%d, %d) with radius %d\n", shapes[i].data.circle.cx, shapes[i].data.circle.cy, shapes[i].data.circle.r);
                                break;
                            case SHAPE_TRIANGLE:
                                printf("Triangle vertices (%d, %d), (%d, %d), (%d, %d)\n", shapes[i].data.triangle.x1, shapes[i].data.triangle.y1, shapes[i].data.triangle.x2, shapes[i].data.triangle.y2, shapes[i].data.triangle.x3, shapes[i].data.triangle.y3);
                                break;
                            }
                        }
                    }
                    if (count == 0)
                    {
                        printf("(No active shapes on canvas)\n");
                    }
                    printf("=====================\n\n");
                }

                int get_safe_int(const char *prompt, int min_val, int max_val)
                {
                    int val;
                    char buffer[100];
                    while (1)
                    {
                        printf("%s", prompt);
                        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
                        {
                            printf("Error reading input. Please try again.\n");
                            continue;
                        }

                        buffer[strcspn(buffer, "\n")] = '\0';

                        // Parse integer
                        char *endptr;
                        long parsed = strtol(buffer, &endptr, 10);

                        if (endptr == buffer || *endptr != '\0')
                        {
                            printf("Invalid integer format. Please enter a valid number.\n");
                            continue;
                        }

                        if (parsed < min_val || parsed > max_val)
                        {
                            printf("Value out of range [%d, %d]. Please try again.\n", min_val, max_val);
                            continue;
                        }

                        val = (int)parsed;
                        break;
                    }
                    return val;
                }