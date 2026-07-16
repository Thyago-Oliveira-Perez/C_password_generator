#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct
{
  int length;
  int use_uppercase;
  int use_lowercase;
  int use_digits;
  int use_symbols;
} PasswordOptions;

/* Caller owns the returned buffer and must free() it. Returns NULL if
 * no character type was selected or length is invalid. */
char *generatePassword(const PasswordOptions *options);

void runMenu(void);

#endif
