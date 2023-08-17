#include "print.h"

void kernel_main() {
    print_clear();
    print_set_color(LGREEN, BLACK);
    print("Welcome to the kernel!");
}