#include "../../include/main.h"
#include "../../include/menu.h"

static const Uint32 piece_colours[9] = {
    RGB(  0,   0,   0),
    RGB(255, 255,   0),
    RGB(  0, 255,   0),
    RGB(255,   0,   0),
    RGB(255, 128,   0),
    RGB(  0,   0, 255),
    RGB(255,   0, 255),
    RGB(  0, 255, 255),
    RGB(200, 200, 200)
};

static const Point srs_minos[7][4][PIECE_MINO_COUNT] = {
    { // O
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}
    },
    { // S
        {{ 0, -1}, { 1, -1}, {-1,  0}, { 0,  0}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    },
    { // Z
        {{-1, -1}, { 0, -1}, { 0,  0}, { 1,  0}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, {-1,  1}}
    }, { // L
        {{ 1, -1}, {-1,  0}, { 0,  0}, { 1,  0}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}},
        {{-1, -1}, { 0, -1}, { 0,  0}, { 0,  1}}
    },
    { // J
        {{-1, -1}, {-1,  0}, { 0,  0}, { 1,  0}},
        {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}}
    },
    { // T
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 1,  0}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    },
    { // I
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
        {{-1,  1}, { 0,  1}, { 1,  1}, { 2,  1}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 0,  2}}
    }
};

#define KICK_TRIES 4
// NESW/0R2L
static const Point srs_szljt_kicks[4][4][KICK_TRIES] = {
    {
        {},
        {{-1,  0}, {-1, -1}, { 0,  2}, {-1,  2}},
        {},
        {{ 1,  0}, { 1, -1}, { 0,  2}, { 1,  2}}
    },
    {
        {{ 1,  0}, { 1,  1}, { 0, -2}, { 1, -2}},
        {},
        {{ 1,  0}, { 1,  1}, { 0, -2}, { 1, -2}},
        {}
    },
    {
        {},
        {{-1,  0}, {-1, -1}, { 0,  2}, {-1,  2}},
        {},
        {{ 1,  0}, { 1, -1}, { 0,  2}, { 1,  2}}
    },
    {
        {{-1,  0}, {-1,  1}, { 0, -2}, {-1, -2}},
        {},
        {{-1,  0}, {-1,  1}, { 0, -2}, {-1, -2}},
        {}
    }
};

static const Point srs_i_kicks[4][4][KICK_TRIES] = {
    {
        {},
        {{-2,  0}, { 1,  0}, {-2,  1}, { 1, -2}},
        {},
        {{-1,  0}, { 2,  0}, {-1, -2}, { 2,  1}}
    },
    {
        {{ 2,  0}, {-1,  0}, { 2, -1}, {-1,  2}},
        {},
        {{-1,  0}, { 2,  0}, {-1, -2}, { 2,  1}},
        {}
    },
    {
        {},
        {{ 1,  0}, {-2,  0}, { 1,  2}, {-2, -1}},
        {},
        {{ 2,  0}, {-1,  0}, { 2, -1}, {-1,  2}}
    },
    {
        {{-2,  0}, { 1,  0}, {-2,  1}, { 1, -2}},
        {},
        {{ 1,  0}, {-2,  0}, { 1,  2}, {-2, -1}},
        {}
    }
};

static MinoType bag[14];

static void new_bag(void) {

    bag[7] = Piece_O;
    bag[8] = Piece_S;
    bag[9] = Piece_Z;
    bag[10] = Piece_L;
    bag[11] = Piece_J;
    bag[12] = Piece_T;
    bag[13] = Piece_I;

    // https://stackoverflow.com/a/6127606
    for (int i = 7; i < 14; i++) {

        int j = i + rand() / (RAND_MAX / (14 - i) + 1);

        int temp = bag[j];
        bag[j] = bag[i];
        bag[i] = temp;

    }
}

static void init(void) {
    new_bag();
    memcpy(bag, bag + 7, 7 * sizeof(MinoType));
    new_bag();
}

static Point get_kick(Rotation new_rotation, int attempt) {

    if (state.piece.type == Piece_O) {
        return (Point){0, 0};
    }

    return (state.piece.type == Piece_I ? srs_i_kicks : srs_szljt_kicks)[state.piece.rotation][new_rotation][attempt - 1];

}

static MinoType generate_new_piece(void) {

    MinoType new_piece = bag[0];

    if (bag[7] == Piece_Empty) {
        new_bag();
    }
        
    for (int i = 0; i < 13; i++) {
        bag[i] = bag[i + 1];
    }

    bag[13] = Piece_Empty;

    return new_piece;

}

static void draw(void) {
    draw_board();
    draw_next(bag, 5);
}

static void sprint_draw(void) {
    draw();
    draw_info_timer(0);
    draw_info_value(1, "lines %d", get_lines_cleared());
}

static void speed_on_exit(int target_lines) {
    sprintf(state.result.line1, lines_cleared_at(target_lines) ? "Success" : "Fail");
    get_timer_formatted(state.result.line2);
}

static void sprint_on_exit(void) {
    speed_on_exit(40);
}

static void marathon_on_exit(void) {
    speed_on_exit(150);
}

#define MODERN_SETTINGS \
    \
    .show_ghost = true, \
    .can_hold = true, \
    .lock_on_down_held = false, \
    .irs = false, \
    .move_rotate_lock_reset_max = 10, \
    .instant_drop_type = Drop_Hard, \
    .socd_allow_das_overwrite = SOCD_Both, \
    .num_kicks = 4, \
    .piece_rot_minos = &srs_minos, \
    .piece_colours = (Uint32 (*const)[]) &piece_colours, \
    \
    .init = init, \
    .get_kick = get_kick, \
    .generate_new_piece = generate_new_piece

#define PPT1_SETTINGS \
    \
    MODERN_SETTINGS, \
    \
    .line_clear_delay = 35, \
    .are_delay = 7, \
    .lock_delay = 30, \
    .das_delay = 10, \
    .arr_delay = 2, \
    .soft_drop_factor = 256, \
    .gravity = 4, \
    .gravity_factor = 256

static const Gamemode ppt1_infinite_mode = {

    PPT1_SETTINGS,

    .draw = draw

};

static const Gamemode ppt1_sprint_mode = {

    PPT1_SETTINGS,

    .draw = sprint_draw,
    .on_exit = sprint_on_exit

};

static const Gamemode ppt1_marathon_mode = {

    PPT1_SETTINGS,

    .draw = sprint_draw,
    .on_exit = sprint_on_exit

};

#define FAST_SETTINGS \
    \
    MODERN_SETTINGS, \
    \
    .line_clear_delay = 0, \
    .are_delay = 0, \
    .lock_delay = 30, \
    .das_delay = 7, \
    .arr_delay = 0, \
    .soft_drop_factor = 5120, \
    .gravity = 4, \
    .gravity_factor = 256

static const Gamemode fast_infinite_mode = {

    FAST_SETTINGS,

    .draw = draw

};

static const Gamemode fast_sprint_mode = {

    FAST_SETTINGS,

    .draw = sprint_draw,
    .on_exit = sprint_on_exit

};

static const Gamemode fast_marathon_mode = {

    FAST_SETTINGS,

    .draw = sprint_draw,
    .on_exit = marathon_on_exit

};

const Menu modern_menu = MENU(
    BUTTON_NEW_MENU("Back", main_menu),
    SEPARATOR,
    BUTTON_LOAD_GAMEMODE("PPT 1 Infinite", ppt1_infinite_mode),
    BUTTON_LOAD_GAMEMODE("PPT 1 Sprint", ppt1_sprint_mode),
    BUTTON_LOAD_GAMEMODE("PPT 1 Marathon", ppt1_marathon_mode),
    SEPARATOR,
    BUTTON_LOAD_GAMEMODE("Fast Infinite", fast_infinite_mode),
    BUTTON_LOAD_GAMEMODE("Fast Sprint", fast_sprint_mode),
    BUTTON_LOAD_GAMEMODE("Fast Marathon", fast_marathon_mode)
);