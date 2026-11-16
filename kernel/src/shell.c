#include <stdint.h>
#include "shell.h"
#include "keyboard.h"
#include "timer.h"

#define VGA_MEMORY  0xB8000
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define COMMAND_SIZE 128

#define COLOR_NORMAL 0x07
#define COLOR_BRIGHT 0x0F
#define COLOR_CYAN   0x0B

static volatile char *vga = (volatile char *)VGA_MEMORY;

static int cursor_row;
static int cursor_col;

static char command[COMMAND_SIZE];
static unsigned int command_length;

static int string_compare(const char *a, const char *b)
{
    while (*a && *b && *a == *b)
    {
        a++;
        b++;
    }

    return (unsigned char)*a - (unsigned char)*b;
}

static void vga_put_at(int row, int col, char c, uint8_t attribute)
{
    if (row < 0 || row >= VGA_HEIGHT ||
        col < 0 || col >= VGA_WIDTH)
        return;

    int position = row * VGA_WIDTH + col;

    vga[position * 2] = c;
    vga[position * 2 + 1] = attribute;
}

static void vga_write_at(int row, int col,
                         const char *text, uint8_t attribute)
{
    while (*text && col < VGA_WIDTH)
        vga_put_at(row, col++, *text++, attribute);
}

static void vga_clear_row(int row)
{
    for (int col = 0; col < VGA_WIDTH; col++)
        vga_put_at(row, col, ' ', COLOR_NORMAL);
}

static void clear_screen(void)
{
    for (int row = 0; row < VGA_HEIGHT; row++)
        vga_clear_row(row);

    cursor_row = 4;
    cursor_col = 0;

    vga_write_at(0, 0, "Arceon 0.2", COLOR_CYAN);
    vga_write_at(2, 0, "PIT ticks: ", COLOR_BRIGHT);
}

static void scroll_screen(void)
{
    for (int row = 4; row < VGA_HEIGHT - 1; row++)
    {
        for (int col = 0; col < VGA_WIDTH; col++)
        {
            int source = ((row + 1) * VGA_WIDTH + col) * 2;
            int destination = (row * VGA_WIDTH + col) * 2;

            vga[destination] = vga[source];
            vga[destination + 1] = vga[source + 1];
        }
    }

    vga_clear_row(VGA_HEIGHT - 1);
    cursor_row = VGA_HEIGHT - 1;
    cursor_col = 0;
}

static void print_text(const char *text)
{
    while (*text)
    {
        if (*text == '\n')
        {
            cursor_row++;
            cursor_col = 0;
        }
        else
        {
            vga_put_at(cursor_row, cursor_col,
                       *text, COLOR_BRIGHT);

            cursor_col++;

            if (cursor_col >= VGA_WIDTH)
            {
                cursor_col = 0;
                cursor_row++;
            }
        }

        if (cursor_row >= VGA_HEIGHT)
            scroll_screen();

        text++;
    }
}

static void print_prompt(void)
{
    print_text("arceon> ");

    command_length = 0;
    command[0] = '\0';
}

static void print_number(uint32_t number)
{
    char digits[11];
    int count = 0;

    do
    {
        digits[count++] = '0' + (number % 10);
        number /= 10;
    }
    while (number != 0 && count < 10);

    while (count > 0)
    {
        char output[2] = { digits[--count], '\0' };
        print_text(output);
    }
}

static void execute_command(void)
{
    command[command_length] = '\0';

    cursor_row++;
    cursor_col = 0;

    if (string_compare(command, "help") == 0)
    {
        print_text("Available commands:\n");
        print_text("  help  - Show available commands\n");
        print_text("  about - About Arceon\n");
        print_text("  ticks - Show PIT timer count\n");
        print_text("  clear - Clear the screen\n");
    }
    else if (string_compare(command, "about") == 0)
    {
        print_text("Arceon 0.2\n");
        print_text("An experimental x86 operating system.\n");
    }
    else if (string_compare(command, "ticks") == 0)
    {
        print_text("PIT ticks: ");
        print_number(timer_get_ticks());
        print_text("\n");
    }
    else if (string_compare(command, "clear") == 0)
    {
        clear_screen();
    }
    else if (command_length != 0)
    {
        print_text("Unknown command. Type 'help'.\n");
    }

    print_prompt();
}

static void handle_character(int c)
{
    if (c == '\n')
    {
        execute_command();
        return;
    }

    if (c == '\b')
    {
        if (command_length > 0 && cursor_col > 8)
        {
            command_length--;
            command[command_length] = '\0';
            cursor_col--;

            vga_put_at(cursor_row, cursor_col,
                       ' ', COLOR_NORMAL);
        }

        return;
    }

    if (c < 32 || c > 126)
        return;

    if (command_length >= COMMAND_SIZE - 1)
        return;

    command[command_length++] = (char)c;
    command[command_length] = '\0';

    vga_put_at(cursor_row, cursor_col,
               (char)c, COLOR_BRIGHT);

    cursor_col++;

    if (cursor_col >= VGA_WIDTH)
    {
        cursor_col = 0;
        cursor_row++;

        if (cursor_row >= VGA_HEIGHT)
            scroll_screen();
    }
}

void shell_init(void)
{
    command_length = 0;
    command[0] = '\0';

    clear_screen();

    print_text("Kernel shell initialized.\n");
    print_text("Type 'help' to get started.\n\n");

    print_prompt();
}

void shell_update(void)
{
    int c;

    while ((c = keyboard_getchar()) != -1)
        handle_character(c);
}

void shell_update_ticks(void)
{
    uint32_t ticks = timer_get_ticks();
    char digits[11];
    int count = 0;

    do
    {
        digits[count++] = '0' + (ticks % 10);
        ticks /= 10;
    }
    while (ticks != 0 && count < 10);

    for (int i = 0; i < 10; i++)
    {
        char c = ' ';

        if (i < count)
            c = digits[count - 1 - i];

        vga_put_at(2, 11 + i, c, COLOR_BRIGHT);
    }
}