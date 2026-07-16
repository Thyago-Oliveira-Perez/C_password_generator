#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_LENGTH 4
#define MAX_LENGTH 128

#if defined(__unix__) || defined(__APPLE__)
#define HAVE_DEV_URANDOM 1
#endif

typedef struct
{
  const char *chars;
  size_t len;
} Category;

/* Returns a byte from /dev/urandom when available (Linux/macOS/BSD),
 * falling back to the seeded rand() everywhere else (e.g. Windows). */
static unsigned char randomByte(void)
{
#ifdef HAVE_DEV_URANDOM
  static FILE *urandom = NULL;
  static int tried_open = 0;

  if (!tried_open)
  {
    urandom = fopen("/dev/urandom", "rb");
    tried_open = 1;
  }

  if (urandom)
  {
    unsigned char byte;
    if (fread(&byte, 1, 1, urandom) == 1)
    {
      return byte;
    }
  }
#endif
  return (unsigned char)(rand() % 256);
}

static void seedRandom(void)
{
  srand((unsigned int)time(NULL));
}

char *generatePassword(const PasswordOptions *options)
{
  static const char UPPER[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static const char LOWER[] = "abcdefghijklmnopqrstuvwxyz";
  static const char DIGITS[] = "0123456789";
  static const char SYMBOLS[] = "!@#$%^&*()-_=+[]{}<>?~";

  if (!options || options->length < MIN_LENGTH || options->length > MAX_LENGTH)
  {
    return NULL;
  }

  Category categories[4];
  int category_count = 0;

  if (options->use_uppercase)
  {
    categories[category_count].chars = UPPER;
    categories[category_count].len = sizeof(UPPER) - 1;
    category_count++;
  }
  if (options->use_lowercase)
  {
    categories[category_count].chars = LOWER;
    categories[category_count].len = sizeof(LOWER) - 1;
    category_count++;
  }
  if (options->use_digits)
  {
    categories[category_count].chars = DIGITS;
    categories[category_count].len = sizeof(DIGITS) - 1;
    category_count++;
  }
  if (options->use_symbols)
  {
    categories[category_count].chars = SYMBOLS;
    categories[category_count].len = sizeof(SYMBOLS) - 1;
    category_count++;
  }

  if (category_count == 0)
  {
    return NULL;
  }

  char charset[sizeof(UPPER) + sizeof(LOWER) + sizeof(DIGITS) + sizeof(SYMBOLS)];
  size_t charset_len = 0;

  for (int i = 0; i < category_count; i++)
  {
    memcpy(charset + charset_len, categories[i].chars, categories[i].len);
    charset_len += categories[i].len;
  }

  char *password = malloc((size_t)options->length + 1);
  if (!password)
  {
    return NULL;
  }

  for (int i = 0; i < options->length; i++)
  {
    password[i] = charset[randomByte() % charset_len];
  }

  /* Guarantee at least one character from each selected category, then
   * shuffle so those guaranteed characters don't always land up front. */
  if (options->length >= category_count)
  {
    for (int i = 0; i < category_count; i++)
    {
      password[i] = categories[i].chars[randomByte() % categories[i].len];
    }
    for (int i = options->length - 1; i > 0; i--)
    {
      int j = randomByte() % (i + 1);
      char tmp = password[i];
      password[i] = password[j];
      password[j] = tmp;
    }
  }

  password[options->length] = '\0';
  return password;
}

static void flushInputLine(void)
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
  {
  }
}

static int readLine(char *buffer, size_t size)
{
  if (!fgets(buffer, (int)size, stdin))
  {
    return 0;
  }

  size_t len = strlen(buffer);
  if (len > 0 && buffer[len - 1] == '\n')
  {
    buffer[len - 1] = '\0';
  }
  else
  {
    flushInputLine();
  }
  return 1;
}

static int readIntInRange(const char *prompt, int min, int max)
{
  char line[64];

  while (1)
  {
    printf("%s", prompt);
    if (!readLine(line, sizeof(line)))
    {
      printf("\nGoodbye!\n");
      exit(0);
    }

    char *endptr;
    long value = strtol(line, &endptr, 10);

    if (endptr != line && *endptr == '\0' && value >= min && value <= max)
    {
      return (int)value;
    }

    printf("Please enter a whole number between %d and %d.\n", min, max);
  }
}

static int readYesNo(const char *prompt, int default_value)
{
  char line[16];

  while (1)
  {
    printf("%s", prompt);
    if (!readLine(line, sizeof(line)))
    {
      printf("\nGoodbye!\n");
      exit(0);
    }

    if (line[0] == '\0')
    {
      return default_value;
    }
    if (line[0] == 'y' || line[0] == 'Y')
    {
      return 1;
    }
    if (line[0] == 'n' || line[0] == 'N')
    {
      return 0;
    }

    printf("Please answer 'y' or 'n'.\n");
  }
}

static void generateAndShow(void)
{
  PasswordOptions options;

  char length_prompt[64];
  snprintf(length_prompt, sizeof(length_prompt), "Password length (%d-%d): ", MIN_LENGTH, MAX_LENGTH);
  options.length = readIntInRange(length_prompt, MIN_LENGTH, MAX_LENGTH);

  printf("Choose which character types to include (Enter = yes):\n");
  options.use_uppercase = readYesNo("  Uppercase letters (A-Z)? [Y/n]: ", 1);
  options.use_lowercase = readYesNo("  Lowercase letters (a-z)? [Y/n]: ", 1);
  options.use_digits = readYesNo("  Digits (0-9)?          [Y/n]: ", 1);
  options.use_symbols = readYesNo("  Symbols (!@#...)?      [Y/n]: ", 1);

  if (!options.use_uppercase && !options.use_lowercase &&
      !options.use_digits && !options.use_symbols)
  {
    printf("No character type selected; defaulting to lowercase + digits.\n");
    options.use_lowercase = 1;
    options.use_digits = 1;
  }

  char *password = generatePassword(&options);
  if (password)
  {
    printf("\nGenerated password: %s\n\n", password);
    free(password);
  }
  else
  {
    printf("\nFailed to generate password.\n\n");
  }
}

void runMenu(void)
{
  seedRandom();

  while (1)
  {
    printf("==== Password Generator ====\n");
    printf("1. Generate password\n");
    printf("0. Exit\n");

    int option = readIntInRange("Choose an option: ", 0, 1);

    if (option == 0)
    {
      printf("Goodbye!\n");
      break;
    }

    generateAndShow();
  }
}
