#include "print.h"
#include "shell.h"
#include "keyboardio.h"
#include "char.h"
#include <stdio.h>
#include <stdbool.h>
#include "string.h"

#define MAX_COMMAND_LENGTH 1000


bool contains_word(const char *str, const char *word) {
    size_t str_len = strlen(str);
    size_t word_len = strlen(word);
    
    for (size_t i = 0; i <= str_len - word_len; i++) {
        if (strncmp(str + i, word, word_len) == 0) {
            // Check if the word is surrounded by whitespace or is at the beginning/end of the string
            if ((i == 0 || is_whitespace(str[i - 1])) && 
                (i + word_len == str_len || is_whitespace(str[i + word_len]))) {
                return true;
            }
        }
    }
    
    return false;
}

void process_command(const char *command) {
  // Determine the length of the command string
  size_t command_length = strlen(command);

  // Temporary storage for a single argument
  char argument[MAX_COMMAND_LENGTH]; // Adjust the size as needed

  size_t arg_index = 0; // Index within the argument buffer

  bool is_command = true;
  char ARGUMENTS[MAX_COMMAND_LENGTH * 2];
  size_t argument_length = 0; // Length of the ARGUMENTS

  for (size_t i = 0; i <= command_length; i++) {
    char current_char = command[i];

    if (current_char == ' ' || current_char == '\0') {
      // Null-terminate the argument string
      argument[arg_index] = '\0';

      if (arg_index > 1) {
        // Commands that require ARGUMENTS.

        if (!is_command) {
          if (argument_length < sizeof(ARGUMENTS) - 1) {
            // Add the argument to ARGUMENTS
            strcat(ARGUMENTS, argument);
            strcat(ARGUMENTS, " "); // Add a space between ARGUMENTS
            argument_length += strlen(argument) + 1;
          } else {
            cprint("ERR: Exceeded MAX_COMMAND_LENGTH", RED, BLACK);
            break;
          }
        }
        is_command = false;
      }

      // Reset the argument buffer
      arg_index = 0;
    } else {
      // Store the character in the argument buffer
      argument[arg_index++] = current_char;
    }
  }

  if (command_strcmp(command, "test")) {
    print("\nThe test command successfully ran!\n");
  } else if (command_strcmp(command, "ls")) {
    print("\nThis isn't Linux, silly!\n");
  } else if (command_strcmp(command, "exit")) {
    print("\nThere is no exiting, you are here forever...\n");
  } else if (command_strcmp(command, "")) {
    print("\n");
  } else if (contains_word(command, "echo")) {
    print("\n"); print(ARGUMENTS); print("\n");
  } else if (command_strcmp(command, "clear")) {
    clear();
  } else {
    cprint("\nERR: Command not found.\n", RED, BLACK);
  }

  // Clear ARGUMENTS for next round
      for (size_t i = 0; i < argument_length; i++)
      {
        ARGUMENTS[i] = 0;
      }
      argument_length = 0;
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

void prompt()
{
  char USER_COMMAND[MAX_COMMAND_LENGTH]; // for ARGUMENTS, just detect spaces. ez.
  int new = 1;

  size_t command_length = 0;

  while (1)
  {
    char ch = 0;
    char keycode = 0;

    if (new)
    {
      cprint("$ ", LGRAY, BLACK);
      new = 0;
    }
    keycode = get_input_keycode();

    if (keycode == KEY_ENTER)
    {
      new = 1;

      process_command(USER_COMMAND);

      // Clear command for next input
      for (size_t i = 0; i < command_length; i++)
      {
        USER_COMMAND[i] = 0;
      }
      command_length = 0;
    }
    else
    {
      ch = get_ascii_char(keycode);
      if (ch == '!')
      {
        if (command_length > 0)
        {
          go_backspace();
          command_length--;
          USER_COMMAND[command_length] = '\0';
        }
      }
      else if (ch != 0)
      {
        if (command_length < sizeof(USER_COMMAND) - 1)
        {
          print_char(ch);
          USER_COMMAND[command_length] = ch;
          command_length++;
        }
      }
    }

    sleep(25000000);
  }
}

void shell_main()
{
  // If needed, do config shit here
  prompt();
}
