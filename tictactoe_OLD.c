#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "tictactoe_icons.h"

//the determines what should be drawn to the screen
int current_screen = 0;
int blink_counter = 0;
bool blink = true;
bool multiplayer = true;

//the thing to draw to the screen
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    //draws the title screen
    if(current_screen == 0) {
        canvas_draw_line(canvas, 10, 30, 27, 7);
        canvas_draw_line(canvas, 19, 35, 39, 11);
        canvas_draw_line(canvas, 5, 18, 37, 38);
        canvas_draw_line(canvas, 24, 26, 22, 19);
        canvas_draw_line(canvas, 17, 23, 28, 21);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 22, 42, "O");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 19, 10, "o");
        canvas_draw_line(canvas, 15, 9, 44, 26);
        canvas_draw_line(canvas, 42, 12, 44, 23);
        canvas_draw_line(canvas, 36, 18, 49, 18);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 6, 28, "o");
        canvas_draw_line(canvas, 29, 8, 32, 17);
        canvas_draw_line(canvas, 25, 12, 35, 12);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 53, 11, "Tic-Tac-Toe");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 84, 19, "By Ravioli");
        canvas_draw_icon(canvas, 46, 21, &I_DolphinNice_96x59);
        canvas_draw_icon(canvas, 0, 46, &I_Space_65x18);
        //flashing play button go brrrr
        if(blink) {
            if(blink_counter == 10) {
                blink = false;
                blink_counter = 0;
            }
            blink_counter += 1;
            canvas_set_font(canvas, FontPrimary);
            canvas_draw_str(canvas, 4, 59, "Play");
        } else {
            blink_counter += 1;
            if(blink_counter == 10) {
                blink = true;
                blink_counter = 0;
            }
        }
        //settings screen 1, multiplayer/singleplayer
    }
    if(current_screen == 1) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 2, 10, "Game Settings");

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 48, 32, "Multiplayer Mode");

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 48, 47, "Singleplayer Mode");
        if(multiplayer) {
            canvas_draw_frame(canvas, 46, 22, 82, 14);
            canvas_draw_icon(canvas, 0, 22, &I_WarningDolphinee_45x42);
        } else {
            canvas_draw_frame(canvas, 46, 36, 82, 15);
            canvas_draw_icon(canvas, 0, 22, &I_WarningDolphin_45x42);
        }
    }
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t stuff(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure viewport
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register viewport in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    bool running = true;
    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                //title screen input
                if(current_screen == 0) {
                    switch(event.key) {
                    //goes to the options menu when ok is pressed
                    case InputKeyOk:
                        current_screen = 1;
                        view_port_update(view_port);
                        break;
                    //exits the program if back is pressed
                    case InputKeyBack:
                        running = false;
                        break;
                    default:
                        break;
                    }
                }
                //choosing if multiplayer or singleplayer
                if(current_screen == 1) {
                    switch(event.key) {
                    case InputKeyOk:
                        //goes to next screen
                        current_screen = 0;
                        break;
                    //goes back to title if back is pressed
                    case InputKeyBack:
                        current_screen = 0;
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyUp:
                        multiplayer = !multiplayer;
                        break;
                    case InputKeyDown:
                        multiplayer = !multiplayer;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        view_port_update(view_port);
    }
    //cleanup go brrrrr
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
