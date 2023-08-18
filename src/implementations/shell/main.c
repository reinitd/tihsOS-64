#include "print.h"
#include "shell.h"
#include "keyboardio.h"
#include "char.h"
#include "string.h"
#include <stdio.h>
#include <stdbool.h>

#define MAX_COMMAND_LENGTH 1000


void process_command(const char* command) {
    if (command_strcmp(command, "test")) {
        print("\nThe test command succesfully ran!\n");
    } else if (command_strcmp(command, "ls")) {
        print("\nThis isn't Linux, silly!\n");
    } else if (command_strcmp(command, "exit")) {
        print("\nThere is no exiting, you are here forever...\n");
    }
    else if (command_strcmp(command, "")) {
      print("\n");
    } else if (command_strcmp(command, "clear")) {
      clear();
    } else {
        cprint("\nERR: Command \"", RED, BLACK);
        print(command);
        cprint("\" not found.\n", RED, BLACK);
    }
}

uint8_t inb(uint16_t port)
{
  uint8_t ret;
  asm volatile("inb %1, %0"
               : "=a"(ret)
               : "d"(port));
  return ret;
}

void outb(uint16_t port, uint8_t data)
{
  asm volatile("outb %0, %1"
               : "=a"(data)
               : "d"(port));
}

char get_input_keycode()
{
  char ch = 0;
  while ((ch = inb(KEYBOARD_PORT)) != 0)
  {
    if (ch > 0)
      return ch;
  }
  return ch;
}

/*
keep the cpu busy for doing nothing(nop)
so that io port will not be processed by cpu
here timer can also be used, but lets do this in looping counter
*/
void wait_for_io(uint32_t timer_count)
{
  while (1)
  {
    asm volatile("nop");
    timer_count--;
    if (timer_count <= 0)
      break;
  }
}

void sleep(uint32_t timer_count)
{
  wait_for_io(timer_count);
}

void prompt() {
    char USER_COMMAND[MAX_COMMAND_LENGTH]; // for arguments, just text spaces. ez.
    int new = 1;

    size_t command_length = 0;

    while (1) {
        char ch = 0;
        char keycode = 0;

        if (new) {
          cprint("$ ", LGRAY, BLACK);
          new = 0;
        }
        keycode = get_input_keycode();

        if (keycode == KEY_ENTER) {
            new = 1;
            
            process_command(USER_COMMAND);
            // switch(USER_COMMAND)
            // {
            //   case "test":
            //     print("\nTest command ran\n");
            //   default:
            //     print("ERR: No such command exists");
            // }

            // Clear command for next input
            for (size_t i = 0; i < command_length; i++) {
                USER_COMMAND[i] = 0;
            }
            command_length = 0;
        }
        else {
            ch = get_ascii_char(keycode);
            if (ch == '!') {
                if (command_length > 0) {
                    go_backspace();
                    command_length--;
                    USER_COMMAND[command_length] = '\0';
                }
            }
            else if (ch != 0) {
                if (command_length < sizeof(USER_COMMAND) - 1) {
                    print_char(ch);
                    USER_COMMAND[command_length] = ch;
                    command_length++;
                }
            }
        }

        sleep(35000000);
    }
}

void shell_main()
{
  // If needed, do config shit here
  prompt();
}
