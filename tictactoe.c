#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "tictactoe_icons.h"

enum current_screen {
    TitleScreen,
    SettingsScreen,
    GameScreen,
};

//the determines what should be drawn to the screen
static int current_screen = TitleScreen;
static int blink_counter = 0;
static bool blink = true;
static bool multiplayer = true;

//bools for which mode is being played
static bool subghzmulti = false;
static bool gpiomulti = false;
static bool localmulti = false;
static bool commulti = false;

//game variables
static int player_turn = 1;
static int current_player = 1;

static int current_highlighted = 0;
static char board[9] = {'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};

#define X_BASE 64
#define Y_BASE 17
#define X_OFF 20
#define Y_OFF 20
//iterative out here just for the sake of not being in there.
static int i = 0;

//determines is board is full
static bool isboardfull = false;

//determines if theres a winner
static int winner = 0;

static int initial_launch = true;
void defaults() {
    //sets every single global variable back to default
    current_screen = TitleScreen;

    current_player = 1;

    blink_counter = 0;

    blink = true;

    multiplayer = true;

    subghzmulti = false;

    gpiomulti = false;

    localmulti = false;

    commulti = false;

    player_turn = 1;

    current_highlighted = 0;

    for(int i = 0; i < 9; i++) {
        board[i] = 'y';
    }

    isboardfull = false;

    winner = 0;
}

//determines what the opponents name should be
const char * get_opponents_name(){
    if(commulti){
        return "Computer";
    }
    else if (localmulti){
        return "Guest";
    }
    else{
        return "Missingno";
    }
}

static void draw_title(Canvas* canvas) {
    canvas_draw_icon(canvas, 0, 0, &I_TitleScreen_128x64);
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
}

static void draw_settings(Canvas* canvas) {
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

static void draw_game(Canvas* canvas) {
    if(winner == 0) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 3, 10, "Player 1");
        canvas_draw_str(canvas, 3, 19, furi_hal_version_get_name_ptr());
        canvas_draw_str(canvas, 3, 61, get_opponents_name());
        canvas_draw_str(canvas, 3, 52, "Player 2");

        if(player_turn == 1) {
            canvas_draw_frame(canvas, 0, 0, 48, 23);
        } else {
            canvas_draw_frame(canvas, 0, 41, 48, 23);
            canvas_set_font(canvas, FontSecondary);
            
        }
        if(player_turn != current_player){
            canvas_draw_str(canvas, 3, 36, "Waiting...");
        }
    } else if(winner == 1) {
        canvas_set_font(canvas, FontPrimary);
        if(player_turn == 1) {
            canvas_draw_str(canvas, 3, 30, furi_hal_version_get_name_ptr());
        } else {
            canvas_draw_str(canvas, 3, 30, get_opponents_name());
        }
        canvas_draw_str(canvas, 3, 40, "Wins!");
    } else if(winner == 2) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 3, 30, "Draw!");
    }

    canvas_draw_icon(canvas, 55, 0, &I_GameBoard_64x64);
    //draws the selector if its the players turn and the game is ongoing
    if(player_turn == current_player && winner == 0) {
        canvas_draw_icon(
            canvas,
            (X_BASE + ((current_highlighted % 3) * X_OFF) - 4),
            (Y_BASE + ((current_highlighted / 3) * Y_OFF) - 12),
            &I_Select_15x15);
    }
    //draws the current x's and o's
    while(i != 9) {
        if(board[i] == 'x') {
            canvas_set_font(canvas, FontPrimary);
            canvas_draw_str(
                canvas, (X_BASE + ((i % 3) * X_OFF)), (Y_BASE + ((i / 3) * Y_OFF)), "X");
        }
        i += 1;
    }
    i = 0;
    while(i != 9) {
        if(board[i] == 'o') {
            canvas_set_font(canvas, FontPrimary);
            canvas_draw_str(
                canvas, (X_BASE + ((i % 3) * X_OFF)), (Y_BASE + ((i / 3) * Y_OFF)), "O");
        }
        i += 1;
    }
    i = 0;
}

//the thing to draw to the screen
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);

    if(current_screen == TitleScreen) {
        draw_title(canvas);
    } else if(current_screen == SettingsScreen) {
        draw_settings(canvas);
    } else if(current_screen == GameScreen) {
        draw_game(canvas);
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



//grabs opponents moves from wherever they are coming from
void opponent_move() {
    if(commulti) {
        int oppMove = (rand() % 9);

        while((board[oppMove] == 'x') || (board[oppMove] == 'o')) {
            oppMove = (rand() % 9);
        }
        furi_delay_ms(700);
        board[oppMove] = 'o';
        check_if_winner();
        player_turn = 1;
    }
    else if(localmulti){
        if(current_player == 1){
            current_player = 2;
        }
        else if(current_player == 2){
            current_player = 1;
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
        if(initial_launch) {
            defaults();
            initial_launch = false;
        }
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                //title screen input
                if(current_screen == TitleScreen) {
                    switch(event.key) {
                        //goes to the options menu when ok is pressed
                    case InputKeyOk:
                        //goes to next screen
                        defaults();
                        current_screen = SettingsScreen;

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
                else if(current_screen == SettingsScreen) {
                    switch(event.key) {
                    case InputKeyOk:
                        //goes to next screen

                        if(!multiplayer) {
                            current_screen = GameScreen;
                            commulti = true;
                        }
                        else if(multiplayer){
                            current_screen = GameScreen;
                            localmulti = true;
                        }
                        break;
                    //goes back to title if back is pressed
                    case InputKeyBack:
                        current_screen = TitleScreen;
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

                else if(current_screen == GameScreen) {
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
                        if(player_turn == current_player && winner == 0) {
                            if((board[current_highlighted] == 'x') ||
                               (board[current_highlighted] == 'o')) {
                                break;
                            } else {
                                if(current_player == 1){
                                    board[current_highlighted] = 'x';
                                    check_if_winner();
                                    player_turn = 2;
                                }
                                else if(current_player == 2){
                                    board[current_highlighted] = 'o';
                                    check_if_winner();
                                    player_turn = 1;
                                }
                                
                                

                                

                                isboardfull = true;
                                for(int i = 0; i < 9; i++) {
                                    if(board[i] == 'y') {
                                        isboardfull = false;
                                        break;
                                    }
                                }
                                if(!isboardfull && winner == 0) {
                                    opponent_move();
                                }
                            }
                        }
                        break;
                    //exits the program if back is pressed
                    case InputKeyBack:

                        current_screen = TitleScreen;
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
