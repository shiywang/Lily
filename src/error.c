#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


/*
static void syntax_error(const char *fmt, ...)
{
    char command[64] = {0};
    va_list ap;

    if (first_time_error)
    {
    
    fclose(output_fp);
    output_fp = fopen("/dev/null", "r+");
    first_time_error = 0;
    
    sprintf(command, "rm -f %s", output_file_name);
    system(command);
  }

  
  fprintf(stderr, "%s:%d: syntax error: ", input_file_name, line_no);

  
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  return;
} */

