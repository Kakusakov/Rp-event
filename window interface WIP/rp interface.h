#pragma once

#include "resource.h"
#include<windows.h>
#include<winuser.h>  // for  MessageBox()
#include<string>


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