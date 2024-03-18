#pragma once
#include "dconfig.h"
#include "dutils.h"

HWND foundWindow = NULL;

const char *win_error_message() {
    DWORD error = GetLastError();
    switch (error) {
        case ERROR_SUCCESS:             return std::string("The operation completed successfully").c_str();
        case ERROR_FILE_NOT_FOUND:      return std::string("The system cannot find the file specified").c_str();
        case ERROR_PATH_NOT_FOUND:      return std::string("The system cannot find the path specified").c_str();
        case ERROR_ACCESS_DENIED:       return std::string("Access is denied").c_str();
        case ERROR_INVALID_HANDLE:      return std::string("The handle is invalid").c_str();
        case ERROR_NOT_ENOUGH_MEMORY:   return std::string("Not enough storage is available to process this command").c_str();
        case ERROR_OUTOFMEMORY:         return std::string("Not enough storage is available to complete this operation").c_str();
        case ERROR_INVALID_DRIVE:       return std::string("The system cannot find the drive specified").c_str();
        case ERROR_NO_MORE_FILES:       return std::string("There are no more files").c_str();
        case ERROR_BAD_NETPATH:         return std::string("The network path was not found").c_str();
        case ERROR_BAD_NET_NAME:        return std::string("The network name cannot be found").c_str();
        case ERROR_SHARING_VIOLATION:   return std::string("The process cannot access the file because it is being used by another process").c_str();
        case ERROR_LOCK_VIOLATION:      return std::string("The process cannot access the file because another process has locked a portion of the file").c_str();
        case ERROR_HANDLE_EOF:          return std::string("Reached the end of the file").c_str();
        case ERROR_HANDLE_DISK_FULL:    return std::string("The disk is full").c_str();
        case ERROR_NOT_SUPPORTED:       return std::string("The request is not supported").c_str();
        case ERROR_FILE_EXISTS:         return std::string("The file exists").c_str();
        case ERROR_CANNOT_MAKE:         return std::string("The directory or file cannot be created").c_str();
        case ERROR_INVALID_PARAMETER:   return std::string("The parameter is incorrect").c_str();
        case ERROR_OPEN_FAILED:         return std::string("The system cannot open the file").c_str();
        case ERROR_DISK_FULL:           return std::string("There is not enough space on the disk").c_str();
        case ERROR_CALL_NOT_IMPLEMENTED:return std::string("This function is not supported on this system").c_str();
        case ERROR_ALREADY_EXISTS:      return std::string("Cannot create a file when that file already exists").c_str();
        case ERROR_FILENAME_EXCED_RANGE:return std::string("The filename or extension is too long").c_str();
        case ERROR_DIRECTORY:           return std::string("The directory name is invalid").c_str();
        case ERROR_NOTIFY_ENUM_DIR:     return std::string("The buffer is not large enough to hold a single entry").c_str();
        /* Add additional case statements for other error codes as needed */
        default:                        return std::string("An unknown error occurred").c_str();
    }
}

/* Function to convert a char* to a wchar_t* */
wchar_t* ConvertToWideChar(const char* str) {
    if (str == NULL) return NULL;

    /* Get the length of the string required for the conversion */
    int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (length == 0) {
        /* Handle error if needed */
        return NULL;
    }

    /* Allocate memory for the wide-character string */
    wchar_t* wstr = (wchar_t*)malloc(length * sizeof(wchar_t));
    if (wstr == NULL) {
        /* Handle memory allocation error if needed */
        return NULL;
    }

    /* Perform the conversion */
    if (MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, length) == 0) {
        /* Handle conversion error if needed */
        free(wstr);
        return NULL;
    }

    return wstr;
}

/* Function to convert a wchar_t* to a char* */
char* ConvertToMultiByte(const wchar_t* wstr) {
    if (wstr == NULL) return NULL;

    /* Get the length of the string required for the conversion */
    int length = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (length == 0) {
        /* Handle error if needed */
        return NULL;
    }

    /* Allocate memory for the multi-byte string */
    char* str = (char*)malloc(length * sizeof(char));
    if (str == NULL) {
        /* Handle memory allocation error if needed */
        return NULL;
    }

    /* Perform the conversion */
    if (WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, length, NULL, NULL) == 0) {
        /* Handle conversion error if needed */
        free(str);
        return NULL;
    }

    return str;
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    char currentWindowTitle[256];
    const char* targetWindowTitle = (const char*)lParam;

    /* Get the title of the current window */
    GetWindowText(hwnd, currentWindowTitle, sizeof(currentWindowTitle));

    /* Check if the current window's title contains the target substring */
    if (strstr(currentWindowTitle, targetWindowTitle) != NULL) {
        foundWindow = hwnd;
        return FALSE; /* Stop enumerating windows */
    }

    return TRUE; /* Continue enumerating */
}

/* Function to find a window by partial title */
HWND FindWindowByPartialTitle(const char* windowTitle) {
    EnumWindows(EnumWindowsProc, (LPARAM)windowTitle);
    return foundWindow;
}

/* Function to find a window by title */
HWND FindWindowByTitle(const char* windowTitle) {
    return FindWindow(NULL, windowTitle);
}

/* Save butmap to file */
BOOL SaveBitmapToFile(HBITMAP hBitmap, HDC hDC, LPCSTR filename) {
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD cClrBits;
    HANDLE hf;                  /* file handle */
    BITMAPFILEHEADER hdr;       /* bitmap file-header */
    LPBYTE lpBits;              /* memory pointer */
    DWORD dwTmp;

    /* Retrieve the bitmap color format, width, and height. */
    if (!GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp)) return FALSE;

    /* Convert the color format to a count of bits. */
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1) cClrBits = 1;
    else if (cClrBits <= 4) cClrBits = 4;
    else if (cClrBits <= 8) cClrBits = 8;
    else if (cClrBits <= 16) cClrBits = 16;
    else if (cClrBits <= 24) cClrBits = 24;
    else cClrBits = 32;

    /* Allocate memory for the BITMAPINFO structure. */
    pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits));

    /* Initialize the fields in the BITMAPINFO structure. */
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;

    /* If the bitmap is not compressed, set the BI_RGB flag. */
    pbmi->bmiHeader.biCompression = BI_RGB;

    /* Compute the number of bytes in the array of color indices and store the result in biSizeImage. */
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8 * pbmi->bmiHeader.biHeight;

    /* Set biClrImportant to 0, indicating that all colors are important. */
    pbmi->bmiHeader.biClrImportant = 0;

    /* Open the file for writing. */
    hf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);

    if (hf == INVALID_HANDLE_VALUE) return FALSE;

    hdr.bfType = 0x4d42;        /* 0x42 = "B" 0x4d = "M" */
    /* Compute the size of the entire file. */
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbmi->bmiHeader.biSize + pbmi->bmiHeader.biClrUsed * sizeof(RGBQUAD) + pbmi->bmiHeader.biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    /* Compute the offset to the array of color indices. */
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbmi->bmiHeader.biSize + pbmi->bmiHeader.biClrUsed * sizeof (RGBQUAD);

    /* Copy the BITMAPFILEHEADER into the .bmp file. */
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp, NULL)) {
        CloseHandle(hf);
        return FALSE;
    }

    /* Copy the BITMAPINFOHEADER and RGBQUAD array into the file. */
    if (!WriteFile(hf, (LPVOID) pbmi, sizeof(BITMAPINFOHEADER) + pbmi->bmiHeader.biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, ( NULL))) {
        CloseHandle(hf);
        return FALSE;
    }

    /* Allocate memory for the bitmap bits. */
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbmi->bmiHeader.biSizeImage);

    if (!lpBits) {
        CloseHandle(hf);
        return FALSE;
    }

    /* Retrieve the color table (RGBQUAD array) and the bits */
    if (!GetDIBits(hDC, hBitmap, 0, (WORD) pbmi->bmiHeader.biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
        GlobalFree((HGLOBAL)lpBits);
        CloseHandle(hf);
        return FALSE;
    }

    /* Write the array of color indices to the .bmp file. */
    if (!WriteFile(hf, (LPSTR) lpBits, (int) pbmi->bmiHeader.biSizeImage, (LPDWORD) &dwTmp, NULL)) {
        GlobalFree((HGLOBAL)lpBits);
        CloseHandle(hf);
        return FALSE;
    }

    /* Clean up. */
    GlobalFree((HGLOBAL)lpBits);
    CloseHandle(hf);

    return TRUE;
}

/* Function to capture the window client area and save it to a bitmap file */
BOOL CaptureWindow(HWND hWnd, LPCSTR filename) {
    BOOL result = FALSE;
    UINT dpi = GetDpiForWindow(hWnd); /* Requires Windows 10, version 1607 or later. */
    float scalingFactor = dpi / 96.0f; /* Adjust based on actual DPI. */

    HDC hDCWindow = GetDC(hWnd);
    HDC hDCMem = CreateCompatibleDC(hDCWindow);

    RECT rect;
    GetWindowRect(hWnd, &rect);
    int width = (int)((rect.right - rect.left) * scalingFactor);
    int height = (int)((rect.bottom - rect.top) * scalingFactor);


    HBITMAP hBitmap = CreateCompatibleBitmap(hDCWindow, width, height);
    SelectObject(hDCMem, hBitmap);

    BitBlt(hDCMem, 0, 0, width, height, hDCWindow, 0, 0, SRCCOPY);

    #pragma omp critical
    {
        result = SaveBitmapToFile(hBitmap, hDCMem, filename); /* Implement this function to save the bitmap. */
    }

    DeleteObject(hBitmap);
    DeleteDC(hDCMem);
    ReleaseDC(hWnd, hDCWindow);

    return result;
}

/* delete single file */
void DeleteSingleFile(const char *directoryPath, const char* fileName) {
    char filePath[MAX_PATH];
    snprintf(filePath, sizeof(filePath), "%s\\%s", directoryPath, fileName);
    /* Delete the file */
    if (!DeleteFile(filePath)) {
        log_error("Failed to delete file %s (%lu)\n", filePath, GetLastError());
    } else {
        log_debug("Deleted File: %s\n", filePath);
    }
}

/* Delete all files in a directory */
void DeleteFilesInDirectory(const char* directoryPath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    /* Create a search pattern */
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*.*", directoryPath);

    /* Find the first file in the directory. */
    hFind = FindFirstFile(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        log_error("Unable to delete files in folder %s : error_nr (%lu)\n", directoryPath, GetLastError());
        return;
    }

    do {
        /* Skip directories, only delete files */
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            DeleteSingleFile(directoryPath, findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    if (GetLastError() != ERROR_NO_MORE_FILES) {
        log_error("FindNextFile failed (%lu)\n", GetLastError());
    }

    FindClose(hFind);
}
/* copy one file to another path */
void copy_file(const char* sourceFilePath, const char* destinationFilePath) {
    /* Attempt to copy the file */
    if (CopyFile(sourceFilePath, destinationFilePath, FALSE)) {
        log_debug("File copied successfully: %s\n", destinationFilePath);
    } else {
        log_error("Failed to copy file: %s. Error: %d\n", sourceFilePath, GetLastError());
    }
}
/* notify sound */
void notify_sound() {
    Beep(750, 300);     /* 750 Hz for 300 milliseconds */
    Beep(1250, 300);    /* 1250 Hz for 300 milliseconds */
    Beep(350, 300);     /* 350 Hz for 300 milliseconds */
}

void sendCtrlEnter(HWND hwnd) {
    // // Hold down the CTRL key
    // INPUT ctrlInput = { 0 };
    // ctrlInput.type = INPUT_KEYBOARD;
    // ctrlInput.ki.wVk = VK_CONTROL; 
    // SendInput(1, &ctrlInput, sizeof(INPUT));

    // // Press the Enter key
    // INPUT enterInput = { 0 };
    // enterInput.type = INPUT_KEYBOARD;
    // enterInput.ki.wVk = VK_RETURN;  
    // SendInput(1, &enterInput, sizeof(INPUT));

    // // Release the Enter key
    // enterInput.ki.dwFlags = KEYEVENTF_KEYUP;  
    // SendInput(1, &enterInput, sizeof(INPUT));

    // // Release the CTRL key
    // ctrlInput.ki.dwFlags = KEYEVENTF_KEYUP; 
    // SendInput(1, &ctrlInput, sizeof(INPUT));

    // Important: Target the input to the specific window
    PostMessage(hwnd, WM_KEYDOWN, VK_CONTROL, 0);
    PostMessage(hwnd, WM_KEYDOWN, VK_RETURN, 0);
    PostMessage(hwnd, WM_KEYUP, VK_RETURN, 0);
    PostMessage(hwnd, WM_KEYUP, VK_CONTROL, 0);
}

void sendEnter(HWND hwnd) {
    // Important: Target the input to the specific window
    PostMessage(hwnd, WM_KEYDOWN, VK_RETURN, 0);
    PostMessage(hwnd, WM_KEYUP, VK_RETURN, 0);
}


void sendText(HWND hwnd, const char* text) {
    int textLength = strlen(text); 

    // Prepare array of INPUT structures for each character
    INPUT* inputs = new INPUT[textLength];
    memset(inputs, 0, sizeof(INPUT) * textLength); 

    for (int i = 0; i < textLength; ++i) {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wScan = text[i]; 
        inputs[i].ki.dwFlags = KEYEVENTF_UNICODE; 
    }

    // Send the text input
    // SendInput(textLength, inputs, sizeof(INPUT));

    // Target the input to the specific window
    for (int i = 0; i < textLength; ++i) {
       PostMessage(hwnd, WM_CHAR, inputs[i].ki.wScan, 0);
    }

    delete[] inputs; 
}

void SetClipboardTextW(const wchar_t* text) {
    const size_t len = (wcslen(text) + 1) * sizeof(wchar_t); // Calculate the length in bytes, including null terminator
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (hMem) {
        void* pMem = GlobalLock(hMem);
        if (pMem) {
            memcpy(pMem, text, len); // Copy the wide-character string into the global memory
            GlobalUnlock(hMem);
            if (OpenClipboard(nullptr)) {
                EmptyClipboard();
                SetClipboardData(CF_UNICODETEXT, hMem); // Use CF_UNICODETEXT for wide-character text
                CloseClipboard();
            } else {
                GlobalFree(hMem); // Free the memory if we cannot open the clipboard
            }
        }
    }
}

void SendCtrlV(HWND hwnd) {
    // Simulate pressing the Ctrl key down
    PostMessage(hwnd, WM_KEYDOWN, VK_CONTROL, 0);
    // Simulate pressing the V key down
    PostMessage(hwnd, WM_KEYDOWN, 'V', 0);
    // Simulate releasing the Ctrl key
    PostMessage(hwnd, WM_KEYUP, VK_CONTROL, 0);
}

void SendClipboardContentW(HWND hwnd, const wchar_t* text) {
    // Set the clipboard content with the specified wide-character text
    SetClipboardTextW(text);
    // Simulate pressing Ctrl+V to paste from the clipboard
    SendCtrlV(hwnd);
}



// Helper function to adjust coordinates based on the window's DPI
void AdjustForDpi(int& x, int& y) {
    double inv_scale = config_screen_dpi_scale();
    x = static_cast<int>(x * inv_scale);
    y = static_cast<int>(y * inv_scale);
}

void ClickAtPoint(HWND hwnd, int x, int y) {
    // Adjust x and y based on the window's DPI scaling
    AdjustForDpi(x, y);

    // Calculate the lParam value for the adjusted coordinates
    LPARAM lParam = MAKELPARAM(x, y);

    // Post the mouse down and up messages to the specific window at the specified coordinates
    PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
    PostMessage(hwnd, WM_LBUTTONUP, 0, lParam);
}

void MoveMouseToPoint(HWND hwnd, int x, int y) {
    // Adjust x and y based on any necessary DPI scaling or other adjustments
    AdjustForDpi(x, y);

    // Convert window-relative coordinates to screen coordinates
    POINT point = {x, y};
    ClientToScreen(hwnd, &point);

    // Move the mouse cursor to the adjusted, screen-mapped coordinates
    SetCursorPos(point.x, point.y);
}