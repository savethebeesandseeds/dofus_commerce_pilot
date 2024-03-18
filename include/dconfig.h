#pragma once
#define _WIN32_WINNT 0x0A00 /* Target Windows 10 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <omp.h>
#include <time.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <cstdlib>

#define CONFIG_FILE "./.config"
#define MAX_CONFIG_LINE 256

int readConfigValue(const char* configFile, const char* searchKey, char* valueBuffer, int bufferLength);
char *config_wrapper(const char* key);
char *config_window();
char *config_temp_dir_prices();
char *config_log_lvl();

#include "dutils.h"

/* Function now takes an additional parameter for the key to search for */
/* and a buffer to store the found value. */
/* Returns 1 if the key is found and the value is copied into the buffer, 0 otherwise. */
int readConfigValue(const char* configFile, const char* searchKey, char* valueBuffer, int bufferLength) {
    char buf[MAX_CONFIG_LINE];
    FILE* file = fopen(configFile, "r");
    if (!file) {
        log_error("Failed to open config file, problem opening the file.");
        return 0; /* Indicate failure to open file */
    }

    while (fgets(buf, sizeof(buf), file)) {
        char* line = strtok(buf, "\n"); /* Remove newline character */
        if (line[0] == '#' || line[0] == ';') continue; /* Skip comments */

        char* key = strtok(line, "=");
        char* value = strtok(NULL, "=");

        if (key && value && strcmp(key, searchKey) == 0) {
            /* If the searchKey matches the current key, copy the value to the buffer */
            strncpy(valueBuffer, value, bufferLength - 1);
            valueBuffer[bufferLength - 1] = '\0'; /* Ensure null-termination */
            fclose(file); /* Close the file before returning */
            return 1; /* Indicate success */
        }
    }

    fclose(file); /* Ensure the file is closed if the key is not found */
    return 0; /* Indicate that the key was not found */
}

/* Wrapper to capture a config value */
char *config_wrapper(const char* key) {
    /* Allocate memory */
    char* value = (char*)calloc(MAX_CONFIG_LINE, sizeof(char));
    if (value == NULL) {
        /* Handle memory allocation failure */
        return NULL; /* Return NULL if allocation fails */
    }
    /* Read the configuration file */
    if (readConfigValue(CONFIG_FILE, key, value, MAX_CONFIG_LINE)) {
        return value;
    } else {
        log_error("Key '%s' not found.\n", key);
        return NULL;
    }
}
int config_integer_wrapper(const char* key) {
    int ret = -1;
    char* endPtr;
    char *value_str = config_wrapper(key);
    double num = strtod(value_str, &endPtr);
    
    if (endPtr == value_str) {
        log_info("No digits were found in %s\n", key);
        ret = -1;
    } else {
        ret = num;
    }
    free(value_str);
    return ret;
}
double config_double_wrapper(const char* key) {
    double ret = -1;
    char* endPtr;
    char *value_str = config_wrapper(key);
    double num = strtod(value_str, &endPtr);

    if (endPtr == value_str) {
        log_info("No digits were found in %s\n", key);
    } else if (num <= 0) {
        log_error("Bad number for key : %s: %s\n", key, value_str);
    } else {
        ret = num;
    }

    free(value_str);

    return ret;
}
/* get config value for delete_temp key */
BOOL config_bolean_wrapper(const char* key) {
    char * temp = config_wrapper(key);
    BOOL ret = strcmp(temp, "true") == 0;
    free(temp);
    return ret;
}
/* get config value for window key */
char *config_window() {
    return config_wrapper("window");
}
/* get config value for temp_dir_prices key */
char *config_temp_dir_prices() {
    return config_wrapper("temp_dir_prices");
}
/* get config value for log_lvl key */
char *config_log_lvl() {
    return config_wrapper("log_lvl");
}
/* get config value for languages_path key */
char *config_languages_path() {
    return config_wrapper("languages_path");
}
/* get config value for language key */
char *config_language() {
    return config_wrapper("language");
}
/* get config value for instruction_file key */
char *config_instruction_file() {
    return config_wrapper("instruction_file");
}
/* get config value for instruction_folder key */
char *config_instruction_folder() {
    return config_wrapper("instruction_folder");
}
/* get config value for output_folder key */
char *config_output_folder() {
    return config_wrapper("output_folder");
}
/* get config value for delete_temp key */
BOOL config_delete_temp() {
    return config_bolean_wrapper("delete_temp");
}
/* get config value for down_size_coef key */
double config_down_size_coef() {
    return config_double_wrapper("down_size_coef");
}
/* get config value for screen_dpi_scale key */
double config_screen_dpi_scale() {
    return config_double_wrapper("screen_dpi_scale");
}
/* get config value for prices_del_x_coord key */
int config_prices_del_x_coord() {
    return config_integer_wrapper("prices_del_x_coord");
}
/* get config value for prices_del_y_coord key */
int config_prices_del_y_coord() {
    return config_integer_wrapper("prices_del_y_coord");
}
/* get config value for prices_typebox_x_coord key */
int config_prices_typebox_x_coord() {
    return config_integer_wrapper("prices_typebox_x_coord");
}
/* get config value for prices_typebox_y_coord key */
int config_prices_typebox_y_coord() {
    return config_integer_wrapper("prices_typebox_y_coord");
}
/* get config value for box_price_x key */
int config_box_price_x() {
    return config_integer_wrapper("box_price_x");
}
/* get config value for box_price_y key */
int config_box_price_y() {
    return config_integer_wrapper("box_price_y");
}
/* get config value for box_price_width key */
int config_box_price_width() {
    return config_integer_wrapper("box_price_width");
}
/* get config value for box_price_height key */
int config_box_price_height() {
    return config_integer_wrapper("box_price_height");
}
/* get config value for wait_to_update_price_list key */
int config_wait_to_update_price_list() {
    return config_integer_wrapper("wait_to_update_price_list");
}
/* get config value for step_period key */
int config_step_period() {
    return config_integer_wrapper("step_period");
}