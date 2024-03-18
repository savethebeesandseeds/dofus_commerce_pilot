#pragma once
#include "dconfig.h"
#include <cctype> // for std::isdigit

/* Enum for log levels */
typedef enum {
    LOG_DEBUG,
    LOG_EVENT,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_EUREKA
} LogLevel;

/* Helper function to get a string representation of the log level */
const char* logLevelToString(LogLevel level) {
    switch(level) {
        case LOG_DEBUG: return "DEBUG";
        case LOG_EVENT: return "EVENT";
        case LOG_INFO: return "INFO";
        case LOG_WARNING: return "WARNING";
        case LOG_ERROR: return "ERROR";
        case LOG_EUREKA: return "EUREKA";
        default: return "UNKNOWN";
    }
}
/* This function now accepts va_list directly */
void log_flush(LogLevel level, const char* format, va_list args) {
    char buffer[1024];
    time_t now;
    struct tm *tm_info;

    /* Get current time */
    time(&now);
    tm_info = localtime(&now);

    /* Format the timestamp */
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    /* Start of the critical section to ensure thread safety */
    #pragma omp critical
    {
        /* Print the timestamp and log level before the user message */
        fprintf(stdout, "[%s][%3d] %s: ", buffer, omp_get_thread_num(), logLevelToString(level));
        
        /* Use vfprintf to print the variadic arguments to the specified stream */
        vfprintf(stdout, format, args);
        
        /* Flush the stream to ensure the output is written immediately */
        fflush(stdout);
    }
}

/* Wrapper function for log_flush that takes variadic arguments */
void lodg_message(LogLevel level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_flush(level, format, args);
    va_end(args);
}
/* Simplified log_info for a specific log level, e.g., LOG_INFO */
void log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_flush(LOG_INFO, format, args);
    va_end(args);
}
/* Simplified log_debug for a specific log level, e.g., LOG_DEBUG */
void log_debug(const char* format, ...) {
    char *log_lvl = config_log_lvl();
    if(strcmp(log_lvl, "debug") == 0) {
        va_list args;
        va_start(args, format);
        log_flush(LOG_DEBUG, format, args);
        va_end(args);
    }
    free(log_lvl);
}
/* Simplified log_event for a specific log level, e.g., LOG_DEBUG */
void log_event(const char* format, ...) {
    char *log_lvl = config_log_lvl();
    if(strcmp(log_lvl, "event") == 0 || strcmp(log_lvl, "debug") == 0) {
        va_list args;
        va_start(args, format);
        log_flush(LOG_EVENT, format, args);
        va_end(args);
    }
    free(log_lvl);
}
/* Simplified log_warning for a specific log level, e.g., LOG_WARNING */
void log_warning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_flush(LOG_WARNING, format, args);
    va_end(args);
}
/* Simplified log_error for a specific log level, e.g., LOG_ERROR */
void log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_flush(LOG_ERROR, format, args);
    va_end(args);
}
/* Simplified log_eureka for a specific log level, e.g., LOG_EUREKA */
void log_eureka(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_flush(LOG_EUREKA, format, args);
    va_end(args);
}
// Store start times for each label
std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timers;
void tick(const std::string& label) {
    timers[label] = std::chrono::high_resolution_clock::now();
}
void tock(const std::string& label) {
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - timers[label];
    log_debug("Enlapsed time [%s] : %f [seconds]\n", label.c_str(), elapsed.count());

    timers.erase(label); // Remove the timer after printing
}
double extractAndConvertToDecimal(const std::string& input) {
    std::string numericPart;
    bool decimalPointFound = false;

    // Extract digits and a single decimal point from the input string
    for (char ch : input) {
        if (std::isdigit(ch)) {
            numericPart += ch;
        } else if (ch == '.' && !decimalPointFound) {
            // Allow only one decimal point
            numericPart += ch;
            decimalPointFound = true;
        }
    }

    // Convert the extracted part into a floating-point number
    if (!numericPart.empty()) {
        return std::stod(numericPart);
    } else {
        return 0.0; // Return 0.0 or an appropriate value if no numeric part was found
    }
}