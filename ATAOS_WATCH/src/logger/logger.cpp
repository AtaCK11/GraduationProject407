#include "logger/logger.h"
#include <stdarg.h>

static const char* level_prefixes[] = {
    "E", // Error
    "W", // Warning
    "I", // Info
    "D", // Debug
    "V"  // Verbose
};

void logger::log(int level, const char* tag, const char* file, int line, const char* func, const char* format, ...) {
    if (level <= LOG_LEVEL) {
        TickType_t tick_count = xTaskGetTickCount();
        unsigned long timestamp = tick_count * portTICK_PERIOD_MS;

        const char* level_prefix = (level >= 0 && level < sizeof(level_prefixes) / sizeof(level_prefixes[0])) ? level_prefixes[level] : level_prefixes[0];

        va_list args;
        va_start(args, format);

        char buffer[256];
        int len = snprintf(buffer, sizeof(buffer), "[%5lu][%1s][%s:%d] %s(): [%s] ", timestamp, level_prefix, file, line, func, tag);

        if (len < sizeof(buffer) && len > 0) {
            vsnprintf(buffer + len, sizeof(buffer) - len, format, args);
            Serial.print(buffer);
            Serial.println();
        } else {
            // output the buffer
            buffer[sizeof(buffer)-1] = '\0';
            Serial.print(buffer);
            Serial.println("...");
        }

        va_end(args);
    }
}
