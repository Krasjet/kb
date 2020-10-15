/* util.c: common utilities */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void
die(const char *msg)
{
  fprintf(stderr, "[error] %s\n", msg);
  exit(1);
}

void
info(const char *msg, ...)
{
  va_list ap;

  fputs("[info] ", stdout);

  va_start(ap, msg);
  vfprintf(stdout, msg, ap);
  puts("");
  va_end(ap);
}
