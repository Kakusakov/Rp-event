#pragma once

#include "resource.h"
#include "rp_core.h"


struct grid_identifier {
    const unsigned int x_amount;
    const unsigned int y_amount;
    const unsigned int interval;
    int x_offset = interval; // = interval by default
    int y_offset = interval; // = interval by default
    // optionaly can reformat to class and add a sanity check for values using constructor
};

const void draw_line_grid(const HDC hdc, grid_identifier g) {
    //if (g.x_offset == -1) g.x_offset = g.interval;  // if default sets to = interval
    //if (g.y_offset == -1) g.y_offset = g.interval;  // if default sets to = interval
    for (unsigned int y = 0; y <= g.y_amount; y++) {
        MoveToEx(hdc, g.x_offset, g.y_offset + g.interval * y, NULL);
        LineTo(hdc, g.x_offset + g.interval * g.x_amount, g.y_offset + g.interval * y);
    }
    for (unsigned int x = 0; x <= g.x_amount; x++) {
        MoveToEx(hdc, g.x_offset + g.interval * x, g.y_offset, NULL);
        LineTo(hdc, g.x_offset + g.interval * x, g.y_offset + g.interval * g.y_amount);
    }
    //MoveToEx(hdc, 0, 0, NULL);
    //LineTo(hdc, 50, 50);
}

const RECT get_square_from_grid(
    const grid_identifier g,
    const unsigned int x_position,  // starts at 1
    const unsigned int y_position  // starts at 1
) {
    //if (g.x_offset == -1) g.x_offset = g.interval;  // if default sets to = interval
    //if (g.y_offset == -1) g.y_offset = g.interval;  // if default sets to = interval
    if (
        g.x_amount < x_position
        || g.y_amount < y_position
        ) {
        MessageBox(
            NULL,
            L"get_square_from_grid() recieved invalid position argument",
            NULL,
            MB_OK | MB_ICONERROR
        );
    }
    const long left = g.x_offset + (x_position - 1) * g.interval;
    const long top = g.y_offset + (y_position - 1) * g.interval;
    const long right = left + g.interval;
    const long bottom = top + g.interval;
    const RECT out = { left, top, right, bottom };
    return out;
}

void create_input_box(HWND parent_window, HWND *hwndButton, HWND *hwndField, HWND *hwndprompt, const unsigned int x, const unsigned int y) {
    (*hwndButton) = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"OK ->",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | BS_DEFPUSHBUTTON,  // Styles 
        x,         // x position 
        y + 30,         // y position 
        70,        // Button width
        70,        // Button height
        parent_window,     // Parent window
        (HMENU)2,       // No menu.
        NULL,
        NULL);      // Pointer not needed.

    (*hwndField) = CreateWindow(
        L"EDIT",  // Predefined class; Unicode assumed 
        L"",      // text 
        WS_BORDER | WS_VISIBLE | WS_CHILD,  // Styles 
        x + 90,         // x position 
        y,         // y position 
        90,        // Button width
        40,        // Button height
        parent_window,     // Parent window
        NULL,       // No menu.
        NULL,
        NULL);      // Pointer not needed.

    (*hwndprompt) = CreateWindow(
        L"STATIC",  // Predefined class; Unicode assumed 
        L"ббнд: ",      // text 
        WS_VISIBLE | WS_CHILD | WS_BORDER,  // Styles 
        x,         // x position 
        y,         // y position 
        70,        // Button width
        20,        // Button height
        parent_window,     // Parent window
        NULL,       // No menu.
        NULL,
        NULL);      // Pointer not needed.
}

std::wstring get_input_field_value(HWND hwndField) {
    const size_t input_size = 63;
    wchar_t temp_input[input_size] = { };
    int x = GetWindowText(hwndField, &temp_input[0], input_size);
    std::wstring out = temp_input;
    SendMessageA(hwndField, WM_SETTEXT, WPARAM(0), LPARAM(""));
    return out;
}

void update_info(HWND parent_window) {
    RedrawWindow(parent_window, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
}
