#include QMK_KEYBOARD_H

/**************** SOME GLOBALS *********************/
 
bool onMac = false;
bool isLeader = false;
bool isBlinking = false;
const float led_dim_ratio = 0.50; 

static uint16_t blink_cycle_timer, 
                blink_fade_in_timer, 
                blink_fade_out_timer;
static uint8_t  fade_in_step_counter, 
                fade_out_step_counter,
                blink_hsv_value;

/**************** LED BLINK HELPER FUNCTIONS *********************/

/*
Function to set color with hsv arguments
- "hue", "sat" and "val" arguments above 255 will get value from rgb matrix config
- "val_ratio" is used to adjust brightness ratio
*/
void rgb_matrix_set_color_hsv(uint8_t led, uint16_t hue, uint16_t sat, uint16_t val, float val_ratio) {
    const uint8_t h = hue <= 255 ? hue : rgb_matrix_config.hsv.h;    
    const uint8_t s = sat <= 255 ? sat : rgb_matrix_config.hsv.s;
    const uint8_t v = val <= 255 ? val * val_ratio : rgb_matrix_config.hsv.v * val_ratio;
    HSV hsv_in = {h, s, v};
    RGB rgb_out = hsv_to_rgb(hsv_in);
    rgb_matrix_set_color(led, rgb_out.r, rgb_out.g, rgb_out.b);
} 

void reset_blink_cycle(void) {
    blink_cycle_timer = timer_read();
    blink_fade_in_timer = timer_read();
    blink_fade_out_timer = timer_read();
    blink_hsv_value = 0;
    fade_in_step_counter = 0;
    fade_out_step_counter = 0;
}

void reset_blink_status(void) {
    isBlinking = false;
}

void get_this_led_blinking(uint8_t led_index, bool speed, uint8_t hue, uint8_t sat) {
    const uint16_t static_on_time = speed ? 200 : 500;
    const uint16_t static_off_time = speed ? 200 : 500;
    const uint8_t fade_timing = speed ? 100 : 150;
    const uint8_t fade_step = speed ? 10 : 15;
    const uint8_t fade_value_step_size = rgb_matrix_config.hsv.v / fade_step;
    const uint8_t fade_cycle_time_elapsed = fade_timing / fade_step;
    if (timer_elapsed(blink_cycle_timer) < static_on_time) {
        if (timer_elapsed(blink_fade_in_timer) > fade_cycle_time_elapsed && fade_in_step_counter < fade_step) {
            blink_hsv_value = blink_hsv_value + fade_value_step_size;
            fade_in_step_counter = fade_in_step_counter + 1;
            blink_fade_in_timer = timer_read();
        }
    } else {
        if (timer_elapsed(blink_fade_out_timer) > fade_cycle_time_elapsed && fade_out_step_counter < fade_step) {
            blink_hsv_value = blink_hsv_value - fade_value_step_size;
            fade_out_step_counter = fade_out_step_counter + 1;
            blink_fade_out_timer = timer_read();
        }
    }

    rgb_matrix_set_color_hsv(led_index, hue, sat, blink_hsv_value, 0.75);

    if (timer_elapsed(blink_cycle_timer) > static_on_time + static_off_time) {
        reset_blink_cycle();
    }
}


/**************** LAYOUT *********************/

/*
[LEDS]
 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14
15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29
30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,       42,  43
44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,       56,  57
58,  59,  60,            61,                 62,  63,  64,  65,  66,  67

[KEYS]
ESC, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   -,   =,  BCK, INS
TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   [,   ],    \,PGUP
CPS, A,   S,   D,   F,   G,   H,   J,   K,   L,   COL, QOT,  RETURN,PGDN
SFT, Z,   X,   C,   V,   B,   N,   M,   COM, DOT, SLS, SHIFT,    UP, DEL
CTL, GUI, ALT,           SPACEBAR,           ALT, FN, CTL, LFT, DWN, RIT
*/

enum layers {
    _MAIN,
    _FN
}

const layers_leds_map[] = {
    [_MAIN] = 45,
    [_FN] = 63
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_MAIN] = LAYOUT_65_ansi(
        KC_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_INS, 
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP, 
        KC_LEAD, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_PGDN, 
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, KC_DEL, 
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, MO(_FN), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_FN] = LAYOUT_65_ansi(
        KC_GESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_NO, KC_NO, 
        KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, RGB_SAD, RGB_SAI, DYN_MACRO_PLAY2, DYN_REC_START2, 
        KC_TRNS, KC_MPRV, KC_MUTE, KC_MNXT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, RGB_HUD, RGB_HUI, DYN_MACRO_PLAY1, DYN_REC_START1, 
        KC_TRNS, KC_VOLD, KC_MPLY, KC_VOLU, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, RGB_SPD, RGB_SPI, KC_TRNS, RGB_VAI, KC_NO, 
        KC_TRNS, KC_TRNS, KC_TRNS, TO(_MAC), KC_TRNS, KC_TRNS, KC_TRNS, RGB_RMOD, RGB_VAD, RGB_MOD
    ),
}; 

//**************** MATRIX SCANS *********************//

void rgb_matrix_indicators_user(void) { 

    #ifdef RGB_MATRIX_ENABLE

    /*Ensure some leds don't completeley turn off if hsv value setting is below 100 in the rgb matrix config */
    const uint8_t led_constant_val = rgb_matrix_config.hsv.v < 100 ? 100 : rgb_matrix_config.hsv.v;

    /* CapsLock LED indicator */
    if (IS_HOST_LED_ON(USB_LED_CAPS_LOCK)) {
        rgb_matrix_set_color_hsv(30, 999, 0, led_constant_val, 0.75); // WHITE
    } 

    /* Current layer LED indicator */
    // rgb_matrix_set_color_hsv(layers_leds_map[get_highest_layer(layer_state)], 999, 0, led_constant_val, led_dim_ratio); // WHITE

    /* Leader Key LED under-glow */
    if (isLeader) {
        rgb_matrix_set_color_hsv(14, 999, 999, 999, 1); // CONFIG
        rgb_matrix_set_color_hsv(30, 999, 999, 999, 1); // CONFIG
    } else {
        rgb_matrix_set_color_hsv(14, 999, 999, 999, led_dim_ratio); // CONFIG
    }   

    switch (biton32(layer_state)) {
        case _FN:
            // Show the available key to use in the _FN layer
            rgb_matrix_set_color_hsv(0, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(1, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(2, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(3, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(4, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(5, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(6, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(7, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(8, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(9, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(10, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(11, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(12, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(22, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(23, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(24, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(26, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(27, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(31, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(32, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(33, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(40, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(41, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(45, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(46, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(47, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(53, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(54, 999, 0, led_constant_val, led_dim_ratio);
            rgb_matrix_set_color_hsv(61, 999, 0, led_constant_val, led_dim_ratio);
            break; 
    }

    #endif /* RGB_MATRIX */
}
 
//**************** LEADER *********************//

#ifdef LEADER_ENABLE

/******* SPANISH ACCENT HELPER FUNCTIONS & DECLARATIONS *************/
// enum spanish_letter {
//     _A,
//     _E,
//     _I,
//     _O,
//     _U,
//     _N
// };

// const uint8_t french_letter_index[6] = {
//     [_A] = 0,
//     [_E] = 1,
//     [_I] = 2,
//     [_O] = 3,
//     [_U] = 4,
//     [_N] = 5
// };

// enum french_accent {
//     _CIRCUMFLEX,
//     _GRAVE,
//     _ACUTE
// };

// const uint8_t french_accent_index[3] = {
//     [_CIRCUMFLEX] = 0,
//     [_GRAVE] = 1,
//     [_ACUTE] = 2
// };

void break_int_in_array(uint8_t int_code, uint8_t size, uint8_t *array) {
    uint8_t i;
    i = size;

    while (i--) {
        array[i] = int_code%10;
        int_code /= 10;
    }
}

/*Couple functions used to output the same macro on two different sequences*/
/* (|) */ 
void ldrkey_send_paranthesis_wrap_ini(void) {
    SEND_STRING("()" SS_TAP(X_LEFT));
}

/* (X) */
void ldrkey_send_paranthesis_wrap_word(void) {
    onMac ? SEND_STRING(SS_LALT(SS_TAP(X_LEFT)) "(" SS_LALT(SS_TAP(X_RIGHT)) ")") : SEND_STRING(SS_LCTL(SS_TAP(X_LEFT)) "(" SS_LCTL(SS_TAP(X_RIGHT)) ")");
}

/* (selection) */ 
void ldrkey_send_paranthesis_wrap_selection(void) {
    onMac ? SEND_STRING(SS_LGUI(SS_TAP(X_X)) "()" SS_TAP(X_LEFT) SS_LGUI(SS_TAP(X_V)) SS_TAP(X_RIGHT)) : SEND_STRING(SS_LCTL(SS_TAP(X_X)) "()" SS_TAP(X_LEFT) SS_LCTL(SS_TAP(X_V)) SS_TAP(X_RIGHT));
}

/* [|] */
void ldrkey_send_bracket_wrap_ini(void) {
    SEND_STRING("[]" SS_TAP(X_LEFT));
}

/* [X] */
void ldrkey_send_bracket_wrap_word(void) {
    onMac ? SEND_STRING(SS_LALT(SS_TAP(X_LEFT)) "[" SS_LALT(SS_TAP(X_RIGHT)) "]") : SEND_STRING(SS_LCTL(SS_TAP(X_LEFT)) "[" SS_LCTL(SS_TAP(X_RIGHT)) "]");
}

/* [selection] */
void ldrkey_send_bracket_wrap_selection(void) {
    onMac ? SEND_STRING(SS_LGUI(SS_TAP(X_X)) "[]" SS_TAP(X_LEFT) SS_LGUI(SS_TAP(X_V)) SS_TAP(X_RIGHT)) : SEND_STRING(SS_LCTL(SS_TAP(X_X)) "[]" SS_TAP(X_LEFT) SS_LCTL(SS_TAP(X_V)) SS_TAP(X_RIGHT));
}

/* {|} */
void ldrkey_send_curlybrace_wrap_ini(void) {
    SEND_STRING("{}" SS_TAP(X_LEFT));
}

/* {X} */
void ldrkey_send_curlybrace_wrap_word(void) {
    onMac ? SEND_STRING(SS_LALT(SS_TAP(X_LEFT)) "{" SS_LALT(SS_TAP(X_RIGHT)) "}") : SEND_STRING(SS_LCTL(SS_TAP(X_LEFT)) "{" SS_LCTL(SS_TAP(X_RIGHT)) "}");
}

/* {selection} */
void ldrkey_send_curlybrace_wrap_selection(void) {
    onMac ? SEND_STRING(SS_LGUI(SS_TAP(X_X)) "{}" SS_TAP(X_LEFT) SS_LGUI(SS_TAP(X_V)) SS_TAP(X_RIGHT)) : SEND_STRING(SS_LCTL(SS_TAP(X_X)) "{}" SS_TAP(X_LEFT) SS_LCTL(SS_TAP(X_V)) SS_TAP(X_RIGHT));
}

LEADER_EXTERNS();

void matrix_scan_user(void) 
{
    LEADER_DICTIONARY() 
    {
        leading = false;
        leader_end();
        /*  CapsLock */
        SEQ_ONE_KEY(KC_LEAD) {
            tap_code(KC_CAPS);
        }

        /*  Backward delete current word (on cursor) */
        SEQ_TWO_KEYS(KC_BSPC, KC_BSPC) { 
            onMac ? SEND_STRING(SS_LALT(SS_TAP(X_RIGHT)) SS_LALT(SS_LSFT(SS_TAP(X_LEFT))) SS_TAP(X_BSPC)) : SEND_STRING(SS_LCTL(SS_TAP(X_RIGHT)) SS_LCTL(SS_LSFT(SS_TAP(X_LEFT))) SS_TAP(X_BSPC)); 
        }
        /*  Previous word delete */
        SEQ_ONE_KEY(KC_BSPC) { 
            onMac ? SEND_STRING(SS_LALT(SS_LSFT(SS_TAP(X_LEFT))) SS_TAP(X_BSPC)) : SEND_STRING(SS_LCTL(SS_LSFT(SS_TAP(X_LEFT))) SS_TAP(X_BSPC)); 
        }
        /*  Forward delete current word (on cursor) */
        SEQ_TWO_KEYS(KC_DEL, KC_DEL) { 

            onMac ? SEND_STRING(SS_LALT(SS_TAP(X_LEFT)) SS_LALT(SS_LSFT(SS_TAP(X_RIGHT))) SS_TAP(X_DEL)) : SEND_STRING(SS_LCTL(SS_TAP(X_LEFT)) SS_LCTL(SS_LSFT(SS_TAP(X_RIGHT))) SS_TAP(X_DEL)); 
        }
        /*  Next word delete */
        SEQ_ONE_KEY(KC_DEL) { 
            onMac ? SEND_STRING(SS_LALT(SS_LSFT(SS_TAP(X_RIGHT))) SS_TAP(X_DEL)): SEND_STRING(SS_LCTL(SS_LSFT(SS_TAP(X_RIGHT))) SS_TAP(X_DEL)); 
        }
        /*  `   => LdrKey > Escape */
        SEQ_ONE_KEY(KC_GESC) {
            SEND_STRING("`");
        }
        /*  ``` => LdrKey > Escape > Escape > Escape */
        SEQ_THREE_KEYS(KC_GESC, KC_GESC, KC_GESC) {
            SEND_STRING("```");
        }
        /*  Printscreen             => LdrKey > Insert */
        SEQ_ONE_KEY(KC_INS) {
            onMac ? SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_4)))) : SEND_STRING(SS_TAP(X_PSCR));
        }
        /*  Home                    => LdrKey > Page Up */
        SEQ_ONE_KEY(KC_PGUP) {
            onMac ? SEND_STRING(SS_TAP(X_HOME)) : SEND_STRING(SS_LCTL(SS_TAP(X_HOME)));
        }
        /*  End                     => LdrKey > Page Down */
        SEQ_ONE_KEY(KC_PGDN) {
            onMac ? SEND_STRING(SS_TAP(X_END)) : SEND_STRING(SS_LCTL(SS_TAP(X_END)));
        }
        /*  "           => LdrKey > ' */ 
        SEQ_ONE_KEY(KC_QUOT) {
            SEND_STRING("\"");
        }
        /*  "|"         => LdrKey > ' > ' */
        SEQ_TWO_KEYS(KC_QUOT, KC_QUOT) {
            SEND_STRING("\"\"" SS_TAP(X_LEFT));
        }
        /*  "X" wrap    => LdrKey > ' > ' > ' */
        SEQ_THREE_KEYS(KC_QUOT, KC_QUOT, KC_QUOT) {
            onMac ? SEND_STRING(SS_LALT(SS_TAP(X_LEFT)) "\"" SS_LALT(SS_TAP(X_RIGHT)) "\"") : SEND_STRING(SS_LCTL(SS_TAP(X_LEFT)) "\"" SS_LCTL(SS_TAP(X_RIGHT)) "\"");
        }
        /*  (|)         => LdrKey > Left Shift > Left Shift */
        SEQ_TWO_KEYS(KC_LSFT, KC_LSFT) {
            ldrkey_send_paranthesis_wrap_ini();
        }
        /*  (|)         => LdrKey > Right Shift > Right Shift */
        SEQ_TWO_KEYS(KC_RSFT, KC_RSFT) {
            ldrkey_send_paranthesis_wrap_ini();
        }
        /*  (X) wrap    => LdrKey > Left Shift > W */
        SEQ_TWO_KEYS(KC_LSFT, KC_W) {
            ldrkey_send_paranthesis_wrap_word();
        }
        /*  (X) wrap    => LdrKey > Right Shift > W */
        SEQ_TWO_KEYS(KC_RSFT, KC_W) {
            ldrkey_send_paranthesis_wrap_word();
        }
        /*  (X) wrap selection    => LdrKey > Left Shift > W > W */
        SEQ_THREE_KEYS(KC_LSFT, KC_W, KC_W) {
            ldrkey_send_paranthesis_wrap_selection();
        }
        /*  (X) wrap selection    => LdrKey > Right Shift > W > W */
        SEQ_THREE_KEYS(KC_RSFT, KC_W, KC_W) {
            ldrkey_send_paranthesis_wrap_selection();
        }
        /*  [|]         => LdrKey > Left CTL > Left CTL */
        SEQ_TWO_KEYS(KC_LCTL, KC_LCTL) {
            ldrkey_send_bracket_wrap_ini();
        }
        /*  [|]         => LdrKey > Right CTL > Right CTL */
        SEQ_TWO_KEYS(KC_RCTL, KC_RCTL) {
            ldrkey_send_bracket_wrap_ini();
        }
        /*  [X] wrap    => LdrKey > Left CTL > W */
        SEQ_TWO_KEYS(KC_LCTL, KC_W) {
            ldrkey_send_bracket_wrap_word();
        }
        /*  [X] wrap    => LdrKey > Right CTL > W */
        SEQ_TWO_KEYS(KC_RCTL, KC_W) {
            ldrkey_send_bracket_wrap_word();
        }
        /*  [X] wrap selection    => LdrKey > Left CTL > W > W */
        SEQ_THREE_KEYS(KC_LCTL, KC_W, KC_W) {
            ldrkey_send_bracket_wrap_selection();
        }
        /*  [X] wrap selection    => LdrKey > Right CTL > W > W */
        SEQ_THREE_KEYS(KC_RCTL, KC_W, KC_W) {
            ldrkey_send_bracket_wrap_selection();
        }
        /*  {|}         => LdrKey > Left ALT > Left ALT */
        SEQ_TWO_KEYS(KC_LALT, KC_LALT) {
            ldrkey_send_curlybrace_wrap_ini();
        }
        /*  {|}         => LdrKey > Right ALT > Right ALT */
        SEQ_TWO_KEYS(KC_RALT, KC_RALT) {
            ldrkey_send_curlybrace_wrap_ini();
        }
        /*  {X} wrap    => LdrKey > Left ALT > W */
        SEQ_TWO_KEYS(KC_LALT, KC_W) {
            ldrkey_send_curlybrace_wrap_word();
        }
        /*  {X} wrap    => LdrKey > Right ALT > W */
        SEQ_TWO_KEYS(KC_RALT, KC_W) {
            ldrkey_send_curlybrace_wrap_word();
        }
        /*  {X} wrap selection    => LdrKey > Left ALT > W > W */
        SEQ_THREE_KEYS(KC_LALT, KC_W, KC_W) {
            ldrkey_send_curlybrace_wrap_selection();
        }
        /*  {X} wrap selection    => LdrKey > Right ALT > W > W */
        SEQ_THREE_KEYS(KC_RALT, KC_W, KC_W) {
            ldrkey_send_curlybrace_wrap_selection();
        }
        /* HELPER => spit out the url of the layout description page on github */
        SEQ_FIVE_KEYS(KC_GESC, KC_GESC, KC_GESC, KC_GESC, KC_GESC) { 
            SEND_STRING("https://github.com/isaacsa51/kb_dz65v3");
        }
    }
}

void leader_start(void) {
    isLeader = true;
}

void leader_end(void) {
    isLeader = false;
}

#endif /* LEADER */