#pragma once

// Pins
#define TFT_CS      15
#define TFT_RST     4
#define TFT_DC      2
#define TFT_SCK     18
#define TFT_MOSI    23
#define TFT_BLK     13

// Colors
#define TFT_YELLOW_COLOR    0xFFE0
#define TFT_GREEN_COLOR     0x07E0
#define TFT_BLUE_COLOR      0x001F
#define TFT_RED_COLOR       0xF800
#define TFT_WHITE_COLOR     0xFFFF
#define TFT_BLACK_COLOR     0x0000
#define TFT_ORANGE_COLOR    0xFD20
#define TFT_CYAN_COLOR      0x07FF
#define TFT_MAGENTA_COLOR   0xF81F
#define TFT_GRAY_COLOR      0x7BEF

// Top Bar Height
#define TOP_BAR_MAX_HEIGHT   35
#define TOP_BAR_CURSOR_X      1
#define TOP_BAR_CURSOR_Y      1

// Pages
enum ScreenPage {
    SCREEN_HOME,
    SCREEN_MENU,
    SCREEN_SETTINGS,
    SCREEN_ABOUT,
    SCREEN_WEATHER,
    SCREEN_HEARTRATE,
    SCREEN_HIBERNATE,
    SCREEN_NONE
};

/*
# SINGLE PRESSES
Single press on the left button will open up the weather menu.
Single press on the home button will open up the home screen.
Single press on the right button will open up the heart rate menu.


# DOUBLE PRESSES
? no idea yet

# LONG PRESSES
Long press on the home button will open up the settings.
? no idea yet

*/