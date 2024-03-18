#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>
#include "dconfig.h"

tesseract::TessBaseAPI *api;

void initialize_tesseract() {
    // read config
    char *lang_path = config_languages_path();
    char *lang = config_language();
    
    // Create a Tesseract API instance
    api = new tesseract::TessBaseAPI();
    
    // Initialize Tesseract (English language in this example)
    BOOL api_init_result = api->Init(lang_path, lang);
    free(lang_path);
    free(lang);
    
    // validate
    if (api_init_result) { 
        log_error("Error initializing Tesseract!\n");
        exit(1);
    }
}
void finalize_tesseract() {
    // Clean up
    api->End();
}
void read_image(std::string image_path, std::string& output) {
    // Load the image
    PIX *image = pixRead(image_path.c_str()); 

    // Set the image for OCR
    api->SetImage(image);

    // Extract the text
    char* text = api->GetUTF8Text(); 
    output = std::string(text);

    pixDestroy(&image);
    delete [] text;
}