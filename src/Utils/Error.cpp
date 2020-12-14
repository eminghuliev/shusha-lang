#include "Error.hpp"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
void Error(const std::string& err){
    throw std::runtime_error(err.c_str());
}

void
shusha_panic (const char *format, ...)
{
    va_list arg;
    va_start (arg, format);
    vfprintf (stderr, format, arg);
    fprintf(stderr, "\n");
    va_end (arg);
    abort();  
}

