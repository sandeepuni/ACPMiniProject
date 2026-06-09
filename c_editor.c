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
    int x1, y1, x2, y2;
} LineParams;
typedef struct
{
    int x1, y1, x2, y2;
} RectParams;
typedef struct
{
    int cx, cy, r;
} CircleParams;
typedef struct
{
    int x1, y1, x2, y2, x3, y3;
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
    printf("\n    ");
    for (int x = 0; x < WIDTH; x++)
    {
        if (x % 10 == 0)
            printf("%d", x / 10);
        else
            printf(" ");
    }
    printf("\n    ");
    for (int x = 0; x < WIDTH; x++)
        printf("%d", x % 10);
    printf("\n   +");
    for (int x = 0; x < WIDTH; x++)
        printf("-");
    printf("+\n");

    for (int y = 0; y < HEIGHT; y++)
    {
        printf("%2d |", y);
        for (int x = 0; x < WIDTH; x++)
            putchar(canvas[y][x]);
        printf("|\n");
    }
    printf("   +");
    for (int x = 0; x < WIDTH; x++)
        printf("-");
    printf("+\n\n");
}

void draw_pixel(int x, int y)
{
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        canvas[y][x] = '*';
}

void draw_line(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (1)
    {
        draw_pixel(x1, y1);
        if (x1 == x2 && y1 == y2)
            break;
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

void draw_rectangle(int x1, int y1, int x2, int y2)
{
    int min_x = (x1 < x2) ? x1 : x2, max_x = (x1 > x2) ? x1 : x2;
    int min_y = (y1 < y2) ? y1 : y2, max_y = (y1 > y2) ? y1 : y2;
    for (int x = min_x; x <= max_x; x++)
    {
        draw_pixel(x, min_y);
        draw_pixel(x, max_y);
    }
    for (int y = min_y; y <= max_y; y++)
    {
        draw_pixel(min_x, y);
        draw_pixel(max_x, y);
    }
}

void draw_circle(int cx, int cy, int r)
{
    if (r < 0)
        return;
    if (r == 0)
    {
        draw_pixel(cx, cy);
        return;
    }
    int x = 0, y = r, d = 3 - 2 * r;
    while (y >= x)
    {
        draw_pixel(cx + x, cy + y);
        draw_pixel(cx - x, cy + y);
        draw_pixel(cx + x, cy - y);
        draw_pixel(cx - x, cy - y);
        draw_pixel(cx + y, cy + x);
        draw_pixel(cx - y, cy + x);
        draw_pixel(cx + y, cy - x);
        draw_pixel(cx - y, cy - x);
        if (d < 0)
            d = d + 4 * x + 6;
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

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
    return -1;
}

int delete_shape(int id)
{
    for (int i = 0; i < MAX_SHAPES; i++)
    {
        if (shapes[i].active && shapes[i].id == id)
        {
            shapes[i].active = 0;
            redraw_canvas();
            return 1;
        }
    }
    return 0;
}

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
        printf("(No active shapes on canvas)\n");
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
            continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        char *endptr;
        long parsed = strtol(buffer, &endptr, 10);
        if (endptr != buffer && *endptr == '\0' && parsed >= min_val && parsed <= max_val)
        {
            val = (int)parsed;
            break;
        }
        printf("Invalid input. Please try again.\n");
    }
    return val;
}

void handle_add_shape(void)
{
    printf("\nChoose a shape type to add:\n1. Line\n2. Rectangle\n3. Circle\n4. Triangle\n5. Back to Main Menu\n");
    int choice = get_safe_int("Enter shape type (1-5): ", 1, 5);
    if (choice == 5)
        return;

    Shape new_shape = {0};
    new_shape.type = (ShapeType)(choice - 1);

    if (choice == 1)
    {
        new_shape.data.line.x1 = get_safe_int("Enter X1 (0-59): ", 0, WIDTH - 1);
        new_shape.data.line.y1 = get_safe_int("Enter Y1 (0-19): ", 0, HEIGHT - 1);
        new_shape.data.line.x2 = get_safe_int("Enter X2 (0-59): ", 0, WIDTH - 1);
        new_shape.data.line.y2 = get_safe_int("Enter Y2 (0-19): ", 0, HEIGHT - 1);
    }
    else if (choice == 2)
    {
        new_shape.data.rect.x1 = get_safe_int("Enter corner X1: ", 0, WIDTH - 1);
        new_shape.data.rect.y1 = get_safe_int("Enter corner Y1: ", 0, HEIGHT - 1);
        new_shape.data.rect.x2 = get_safe_int("Enter opposite X2: ", 0, WIDTH - 1);
        new_shape.data.rect.y2 = get_safe_int("Enter opposite Y2: ", 0, HEIGHT - 1);
    }
    else if (choice == 3)
    {
        new_shape.data.circle.cx = get_safe_int("Enter center X: ", 0, WIDTH - 1);
        new_shape.data.circle.cy = get_safe_int("Enter center Y: ", 0, HEIGHT - 1);
        new_shape.data.circle.r = get_safe_int("Enter radius: ", 0, 60);
    }
    else if (choice == 4)
    {
        new_shape.data.triangle.x1 = get_safe_int("Vertex X1: ", 0, WIDTH - 1);
        new_shape.data.triangle.y1 = get_safe_int("Vertex Y1: ", 0, HEIGHT - 1);
        new_shape.data.triangle.x2 = get_safe_int("Vertex X2: ", 0, WIDTH - 1);
        new_shape.data.triangle.y2 = get_safe_int("Vertex Y2: ", 0, HEIGHT - 1);
        new_shape.data.triangle.x3 = get_safe_int("Vertex X3: ", 0, WIDTH - 1);
        new_shape.data.triangle.y3 = get_safe_int("Vertex Y3: ", 0, HEIGHT - 1);
    }

    int id = add_shape(new_shape);
    if (id != -1)
        printf("Shape added successfully with ID %d!\n", id);
    else
        printf("Error: Registry is full!\n");
}

void handle_delete_shape(void)
{
    list_shapes();
    int active_exists = 0;
    for (int i = 0; i < MAX_SHAPES; i++)
        if (shapes[i].active)
            active_exists = 1;
    if (!active_exists)
        return;

    int id = get_safe_int("Enter shape ID to delete: ", 1, 100000);
    if (delete_shape(id))
        printf("Shape with ID %d deleted successfully.\n", id);
    else
        printf("Shape with ID %d not found.\n", id);
}

void show_menu(void)
{
    printf("\n=== 2D Character Graphics Editor ===\n1. Display\n2. Add\n3. Delete\n4. List\n5. Clear\n6. Exit\n");
}

int main(void)
{
    for (int i = 0; i < MAX_SHAPES; i++)
        shapes[i].active = 0;
    init_canvas();
    printf("Welcome to the 2D character graphics editor!\n");
    display_canvas();

    while (1)
    {
        show_menu();
        int option = get_safe_int("Select option (1-6): ", 1, 6);
        if (option == 6)
            break;
        switch (option)
        {
        case 1:
            display_canvas();
            break;
        case 2:
            handle_add_shape();
            display_canvas();
            break;
        case 3:
            handle_delete_shape();
            display_canvas();
            break;
        case 4:
            list_shapes();
            break;
        case 5:
            for (int i = 0; i < MAX_SHAPES; i++)
                shapes[i].active = 0;
            redraw_canvas();
            printf("All shapes deleted.\n");
            display_canvas();
            break;
        }
    }
    return 0;
}