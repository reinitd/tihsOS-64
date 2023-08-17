#include "print.h"
#include "shell.h"

void kernel_main() {
    clear();
    print("Setting up the Kernel...\n");

    // Put any code needed to setup here.

    clear();
    cprint("Welcome to tihsOS-64!\n\n", LGREEN, BLACK);
    shell_main();
}
