#include "log.h"
#include <cstdarg>
#include <cstdio>

void Log(const char* fmt, ...) {
	va_list args;
	char buffer[1024];

	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);

	FILE* f;
	fopen_s(&f, LOGFILE, "a");
	fprintf(f,"%s\n",buffer);
	fflush(f);
	fclose(f);
}