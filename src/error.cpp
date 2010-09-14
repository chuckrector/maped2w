#include "error.h"
#include "vdriver.h"
#include "timer.h"
#include "keyboard.h"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

#include "log.h"

void err(const char* fmt, ...) {
	va_list args;
	char buffer[1024];

	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);

	ShutdownVideo();
	ShutdownTimer();
	ShutdownKeyboard();

	printf("%s\n",buffer);
	Log("%s\n",buffer);
	exit(-1);
}