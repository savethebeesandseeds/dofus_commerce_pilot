
#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include "dconfig.h"
#include "dutils.h"
#include "dwin_api.h"
#include "dimages.h"
#include "dread.h"

long int price_loop_count = 0;

double get_price_step(std::wstring& search_name) {
    double price = 0;
    std::string search_price = "";
    char *temp_dir = config_temp_dir_prices();
    char temp_image_path[MAX_PATH];
    price_loop_count ++;
    /* capture the character window */
    char *window = config_window();
    HWND hWnd = FindWindowByPartialTitle(window);
    free(window);
    /* click on (x) to clear the bar */
    {
        ClickAtPoint(hWnd, config_prices_del_x_coord(), config_prices_del_y_coord());
        Sleep(config_step_period());
    }
    /* click on the bar */
    {
        ClickAtPoint(hWnd, config_prices_typebox_x_coord(), config_prices_typebox_y_coord());
        Sleep(config_step_period());
    }
    /* type the name */
    {
        wchar_t buff[512];
        buff[0] = L'\0'; // Initializing the buffer to an empty wide string
        swprintf(buff, 512, L"%ls", search_name.c_str());
        SendClipboardContentW(hWnd, buff); // sendText(hWnd, buff);
        Sleep(config_wait_to_update_price_list());
    }
    /* capture the price */
    {
        /* capture and save the total window image intp a temporal folder */
        sprintf(temp_image_path, "%s/window_capture_%ld.bmp", temp_dir, price_loop_count);
        if (!CaptureWindow(hWnd, temp_image_path)) {
            log_error("Unable to capture [price] image: %s\n", win_error_message());
            free(temp_dir);
            return price;
        }
        /* read the image */
        cv::Mat total_image = cv::imread(temp_image_path);
        /* debug clicks and rectangle */
        {
            cv::Mat debug_image = total_image.clone();
            cv::circle(debug_image, cv::Point(config_prices_del_x_coord(), config_prices_del_y_coord()), 5, cv::Scalar(0, 0, 255), cv::FILLED);
            cv::circle(debug_image, cv::Point(config_prices_typebox_x_coord(), config_prices_typebox_y_coord()), 5, cv::Scalar(0, 0, 255), cv::FILLED);
            cv::rectangle(debug_image, 
                cv::Point(config_box_price_x(), config_box_price_y()), 
                cv::Point(config_box_price_x()+config_box_price_width(), config_box_price_y()+config_box_price_height()), 
                cv::Scalar(0, 0, 255), 5);
            sprintf(temp_image_path, "%s/debug_capture_%ld.jpg", temp_dir, price_loop_count);
            cv::imwrite(temp_image_path, debug_image);
        }
        /* get the subimage that contains the image */
        cv::Mat sub_image = createSubImage(total_image, 
            config_box_price_x(), config_box_price_y(), config_box_price_width(), config_box_price_height());
        /* save the temporal subimage */
        sprintf(temp_image_path, "%s/price_capture_%ld.jpg", temp_dir, price_loop_count);
        if(!cv::imwrite(temp_image_path, sub_image)) {
            log_error("unable to save image: %s\n");
            free(temp_dir);
            return price;
        }
    }
    /* read the price tag */
    {
        sprintf(temp_image_path, "%s/price_capture_%ld.jpg", temp_dir, price_loop_count);
        log_debug("reading [%s]\n", temp_image_path);
        read_image(temp_image_path, search_price);
        price = extractAndConvertToDecimal(search_price);
    }
    /* terminate */
    free(temp_dir);
    return price;
}



void commerce_pilot_process_loop() {
    // Ensure std::wcout uses the global locale
    std::wcout.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t>));
    
    log_info("commerce_pilot_process_loop() started\n");

    /* start an observation loop for new files in temp folder */
    char buff[512];
    std::wstring search_name;
    
    /* read the configuration */
    char *temp_dir = config_temp_dir_prices();
    char *instruction_file_name = config_instruction_file();
    char *instruction_folder = config_instruction_folder();
    char *output_folder = config_output_folder();

    if (temp_dir == NULL || instruction_file_name == NULL || output_folder == NULL) { 
        log_error("Bad config\n"); 
        /* Clean up */
        free(temp_dir);
        free(instruction_folder);
        free(instruction_file_name);
        free(output_folder);
        return;
    }

    /* read the instruction file */
    sprintf(buff, "%s/%s.txt", instruction_folder, instruction_file_name);
    std::wifstream instruction_file(buff, std::wifstream::binary);

    /* Check if the file is open (i.e., exists and can be read) */
    if (!instruction_file.is_open()) {
        log_error("Failed to open file: %s\n", buff);
        /* Clean up */
        free(temp_dir);
        free(instruction_folder);
        free(instruction_file_name);
        free(output_folder);
        return;
    }
    instruction_file.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t>));

    /* create or open the output file */
    sprintf(buff, "%s/%s.csv", output_folder, instruction_file_name);
    std::wofstream output_file(buff, std::wifstream::binary);

    // Check if the file is open (i.e., can be written to)
    if (!output_file.is_open()) {
        log_error("Failed to open file: %s\n", buff);
        /* Clean up */
        instruction_file.close();
        free(temp_dir);
        free(instruction_folder);
        free(instruction_file_name);
        free(output_folder);
        return;
    }
    output_file.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t>));

    /* set output header */
    std::wcout << "price,name" << "\n";
    output_file << "price,name" << "\n";
    /* loop over the instruction file */
    while (std::getline(instruction_file, search_name)) {
        /* step, extract the price */
        double search_price = get_price_step(search_name);
        /* write the result in the output file */
        std::wcout << search_price << "," << search_name << "\n";
        output_file << search_price << "," << search_name << "\n";
    }

    /* Clean up */
    instruction_file.close();
    output_file.close();
    free(temp_dir);
    free(instruction_folder);
    free(instruction_file_name);
    free(output_folder);

    log_debug("commerce_pilot_process_loop() ended\n");
}
