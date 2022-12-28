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
//used to disable input until scene changes, took 7 hours to realise i needed this
bool changing_screen = false;

//bools for which mode is being played
bool subghzmulti = false;
bool gpiomulti = false;
bool localmulti = false;
bool commulti = false;

//game variables
int player_turn = 1;
int current_highlighted = 0;
char board[9] = {'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};
//starting locations
int x_base = 64;
int y_base = 17;
//easier placing of x's and o's i hope
int x_mult = 20;
int y_mult = 20;
//iterative out here just for the sake of not being in there.
int i = 0;

//determines is board is full
bool isboardfull = false;

//determines if theres a winner
int winner = 0;

int initial_launch = true;
void defaults() {
    //sets every single global variable back to default
    current_screen = 0;
    (void)current_screen;
    blink_counter = 0;
    (void)blink_counter;
    blink = true;
    (void)blink;
    multiplayer = true;
    (void)multiplayer;
    changing_screen = false;
    (void)changing_screen;
    subghzmulti = false;
    (void)subghzmulti;
    gpiomulti = false;
    (void)gpiomulti;
    localmulti = false;
    (void)localmulti;
    commulti = false;
    (void)commulti;
    player_turn = 1;
    (void)player_turn;
    current_highlighted = 0;
    (void)current_highlighted;
    int board[9] = {'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};
    (void)board[0];
    isboardfull = false;
    (void)isboardfull;
    winner = 0;
    (void)winner;
}

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
    //draw game screen
    if(current_screen == 2) {
        if(winner == 0) {
            canvas_set_font(canvas, FontSecondary);
            canvas_draw_str(canvas, 3, 10, "Player 1");
            canvas_draw_str(canvas, 3, 19, "Drihanit");
            canvas_draw_str(canvas, 3, 61, "Guest");
            canvas_draw_str(canvas, 3, 52, "Player 2");

            if(player_turn == 1) {
                canvas_draw_frame(canvas, 0, 0, 43, 23);
            } else {
                canvas_draw_frame(canvas, 0, 41, 43, 23);
            }
        } else if(winner == 1) {
            canvas_set_font(canvas, FontPrimary);
            canvas_draw_str(canvas, 3, 30, "Drihanit");
            canvas_draw_str(canvas, 3, 40, "Wins!");
        } else if(winner == 2) {
            canvas_set_font(canvas, FontPrimary);
            canvas_draw_str(canvas, 3, 30, "Draw!");
        }

        canvas_draw_icon(canvas, 55, 0, &I_GameBoard_64x64);
        //draws the selector if its the players turn and the game is ongoing
        if(player_turn == 1 && winner == 0) {
            canvas_draw_icon(
                canvas,
                (x_base + ((current_highlighted % 3) * x_mult) - 4),
                (y_base + ((current_highlighted / 3) * y_mult) - 12),
                &I_Select_15x15);
        }
        //draws the current x's and o's
        while(i != 9) {
            if(board[i] == 'x') {
                canvas_set_font(canvas, FontPrimary);
                canvas_draw_str(
                    canvas, (x_base + ((i % 3) * x_mult)), (y_base + ((i / 3) * y_mult)), "X");
            }
            i += 1;
        }
        i = 0;
        while(i != 9) {
            if(board[i] == 'o') {
                canvas_set_font(canvas, FontPrimary);
                canvas_draw_str(
                    canvas, (x_base + ((i % 3) * x_mult)), (y_base + ((i / 3) * y_mult)), "O");
            }
            i += 1;
        }
        i = 0;
    }
}
//grabs opponents moves from wherever they are coming from
void opponent_move() {
    if(commulti) {
        int oppMove = (rand() % 9);

        while((board[oppMove] == 'x') || (board[oppMove] == 'o')) {
            oppMove = (rand() % 9);
        }
        board[oppMove] = 'o';
        player_turn = 1;
    }
}

//checks if someoen has won
void check_if_winner() {
    if(board[0] == board[1] && board[1] == board[2] && board[0] != 'y') {
        winner = 1;
    } else if(board[3] == board[4] && board[4] == board[5] && board[3] != 'y')
        winner = 1;
    else if(board[6] == board[7] && board[7] == board[8] && board[6] != 'y')
        winner = 1;
    else if(board[0] == board[4] && board[4] == board[8] && board[0] != 'y')
        winner = 1;
    else if(board[2] == board[4] && board[4] == board[6] && board[2] != 'y')
        winner = 1;
    else if(board[0] == board[3] && board[3] == board[6] && board[0] != 'y')
        winner = 1;
    else if(board[1] == board[4] && board[4] == board[7] && board[1] != 'y')
        winner = 1;
    else if(board[2] == board[5] && board[5] == board[8] && board[2] != 'y')
        winner = 1;
    else if(isboardfull) {
        winner = 2;
    }

    if(winner != 0) {
        furi_delay_ms(2000);
        defaults();
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
        if(initial_launch) {
            defaults();
            initial_launch = false;
        }
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                //title screen input
                if((current_screen == 0) && (!changing_screen)) {
                    switch(event.key) {
                        //goes to the options menu when ok is pressed
                    case InputKeyOk:
                        //goes to next screen
                        current_screen = 1;
                        changing_screen = true;
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
                if((current_screen == 1) && (!changing_screen)) {
                    switch(event.key) {
                    case InputKeyOk:
                        //goes to next screen

                        if(!multiplayer) {
                            current_screen = 2;
                            commulti = true;
                            changing_screen = true;
                        }
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

                if((current_screen == 2) && (!changing_screen)) {
                    //checks if the board is full
                    isboardfull = true;
                    for(int i = 0; i < 9; i++) {
                        if(board[i] == 'y') {
                            isboardfull = false;
                            break;
                        }
                    }

                    switch(event.key) {
                        //goes to the options menu when ok is pressed
                    case InputKeyOk:
                        if(player_turn == 1 && winner == 0) {
                            if((board[current_highlighted] == 'x') ||
                               (board[current_highlighted] == 'o')) {
                                break;
                            } else {
                                board[current_highlighted] = 'x';
                                player_turn = 2;

                                isboardfull = true;
                                for(int i = 0; i < 9; i++) {
                                    if(board[i] == 'y') {
                                        isboardfull = false;
                                        break;
                                    }
                                }
                                if(!isboardfull) {
                                    opponent_move();
                                }
                            }
                        }
                        break;
                    //exits the program if back is pressed
                    case InputKeyBack:

                        current_screen = 0;
                        break;
                    case InputKeyLeft:
                        if(current_highlighted == 0) {
                            current_highlighted = 8;
                        } else {
                            current_highlighted -= 1;
                        }
                        break;
                    case InputKeyRight:
                        if(current_highlighted == 8) {
                            current_highlighted = 0;
                        } else {
                            current_highlighted += 1;
                        }
                        break;
                    case InputKeyUp:
                        if(current_highlighted <= 2) {
                            current_highlighted += 6;
                        } else {
                            current_highlighted -= 3;
                        }
                        break;
                    case InputKeyDown:
                        if(current_highlighted >= 6) {
                            current_highlighted -= 6;
                        } else {
                            current_highlighted += 3;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        changing_screen = false;
        check_if_winner();
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
