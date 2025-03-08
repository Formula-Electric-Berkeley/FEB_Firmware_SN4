/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --font /Users/mihir/SquareLine/assets/Roboto-Bold.ttf -o /Users/mihir/SquareLine/assets/ui_font_robotf.c --format lvgl -r 0x20-0x7f --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_ROBOTF
#define UI_FONT_ROBOTF 1
#endif

#if UI_FONT_ROBOTF

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xf,

    /* U+0022 "\"" */
    0xff, 0xff,

    /* U+0023 "#" */
    0x1a, 0x9, 0x4, 0x8f, 0xf7, 0xf8, 0xb0, 0xd1,
    0xfe, 0xff, 0x12, 0x9, 0x5, 0x80,

    /* U+0024 "$" */
    0x8, 0x8, 0x3e, 0x63, 0x63, 0x60, 0x78, 0x3e,
    0xf, 0x3, 0xc3, 0xe3, 0x7e, 0x8, 0x8,

    /* U+0025 "%" */
    0x70, 0x1f, 0x3, 0x66, 0x6d, 0x8f, 0xa0, 0xec,
    0x3, 0xf0, 0x7f, 0x1e, 0x66, 0xcc, 0x1f, 0x81,
    0xe0,

    /* U+0026 "&" */
    0x3c, 0x1f, 0x86, 0x61, 0x98, 0x7c, 0xe, 0x7,
    0x9b, 0x36, 0xcf, 0xb1, 0xc7, 0xf8, 0xf6,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x13, 0x66, 0xcc, 0xcc, 0xcc, 0xcc, 0x66, 0x33,
    0x0,

    /* U+0029 ")" */
    0x8c, 0x66, 0x23, 0x33, 0x33, 0x33, 0x66, 0xcc,
    0x0,

    /* U+002A "*" */
    0x18, 0x18, 0xda, 0x7e, 0x3c, 0x2c, 0x66,

    /* U+002B "+" */
    0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18,

    /* U+002C "," */
    0x6d, 0xbc,

    /* U+002D "-" */
    0xff,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0xc, 0x21, 0x86, 0x10, 0xc3, 0x8, 0x61, 0x84,
    0x30, 0xc0,

    /* U+0030 "0" */
    0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xe7, 0x7e, 0x3c,

    /* U+0031 "1" */
    0xb, 0xff, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x30,

    /* U+0032 "2" */
    0x3c, 0x7e, 0xc6, 0xc6, 0x6, 0xc, 0x1c, 0x38,
    0x30, 0x60, 0xff, 0xff,

    /* U+0033 "3" */
    0x3d, 0xff, 0x18, 0x30, 0x67, 0x8f, 0x3, 0x7,
    0x8f, 0xfb, 0xe0,

    /* U+0034 "4" */
    0x6, 0x7, 0x7, 0x83, 0xc3, 0x61, 0x31, 0x99,
    0xff, 0xff, 0x83, 0x1, 0x80, 0xc0,

    /* U+0035 "5" */
    0x7f, 0x7f, 0x60, 0x60, 0x7e, 0x7f, 0x27, 0x3,
    0x3, 0x63, 0x7e, 0x3c,

    /* U+0036 "6" */
    0x1c, 0x3c, 0x70, 0x60, 0xdc, 0xfe, 0xc7, 0xc3,
    0xc3, 0xe7, 0x7e, 0x3c,

    /* U+0037 "7" */
    0xff, 0xff, 0x7, 0x6, 0x6, 0xc, 0xc, 0x18,
    0x18, 0x38, 0x30, 0x70,

    /* U+0038 "8" */
    0x7d, 0xff, 0x1e, 0x3c, 0x6f, 0x9f, 0x63, 0xc7,
    0x8f, 0xfb, 0xe0,

    /* U+0039 "9" */
    0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xe3, 0x7f, 0x7f,
    0x7, 0xe, 0x3c, 0x38,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0xf0, 0xf, 0xf8,

    /* U+003C "<" */
    0x6, 0x3d, 0xf7, 0xf, 0xf, 0x87, 0x81,

    /* U+003D "=" */
    0xff, 0xfc, 0x7, 0xff, 0xe0,

    /* U+003E ">" */
    0x81, 0xe1, 0xf0, 0xf1, 0xff, 0xb8, 0x40,

    /* U+003F "?" */
    0x3c, 0x7f, 0xc3, 0x3, 0x3, 0xe, 0x1c, 0x18,
    0x18, 0x0, 0x18, 0x18,

    /* U+0040 "@" */
    0xf, 0x80, 0xff, 0xe, 0x1c, 0xce, 0x66, 0xf9,
    0xe6, 0x4f, 0x66, 0x7b, 0x33, 0xd9, 0x9e, 0xcd,
    0xf7, 0xbc, 0xdd, 0xc7, 0x0, 0x1f, 0xc0, 0x7c,
    0x0,

    /* U+0041 "A" */
    0xe, 0x1, 0xc0, 0x78, 0xf, 0x81, 0xb0, 0x76,
    0xc, 0xe1, 0x8c, 0x7f, 0x8f, 0xf9, 0x83, 0x70,
    0x70,

    /* U+0042 "B" */
    0xfc, 0xff, 0xc3, 0xc3, 0xc3, 0xfe, 0xfe, 0xc3,
    0xc3, 0xc3, 0xff, 0xfc,

    /* U+0043 "C" */
    0x3e, 0x3f, 0x98, 0xf8, 0x7c, 0x6, 0x3, 0x1,
    0x80, 0xc3, 0xb1, 0xdf, 0xc7, 0xc0,

    /* U+0044 "D" */
    0xfc, 0x7f, 0x31, 0xd8, 0x7c, 0x1e, 0xf, 0x7,
    0x83, 0xc3, 0xe1, 0xbf, 0x9f, 0x80,

    /* U+0045 "E" */
    0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xfe, 0xfe, 0xc0,
    0xc0, 0xc0, 0xff, 0xff,

    /* U+0046 "F" */
    0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xfe, 0xfe, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0x3e, 0x3f, 0x98, 0xf8, 0x3c, 0x6, 0x3, 0x1f,
    0x8f, 0xc1, 0xb0, 0xdf, 0xe3, 0xc0,

    /* U+0048 "H" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1f, 0xff, 0xff,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x30,

    /* U+0049 "I" */
    0xff, 0xff, 0xff,

    /* U+004A "J" */
    0x6, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x83, 0xc7,
    0x8f, 0xf3, 0xc0,

    /* U+004B "K" */
    0xc3, 0x63, 0xb3, 0x9b, 0x8d, 0x87, 0xc3, 0xf1,
    0xf8, 0xce, 0x63, 0x31, 0xd8, 0x70,

    /* U+004C "L" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xff, 0xff,

    /* U+004D "M" */
    0xe0, 0x7e, 0x7, 0xf0, 0xff, 0xf, 0xd9, 0xfd,
    0x9b, 0xd9, 0xbc, 0xf3, 0xcf, 0x3c, 0xf3, 0xc6,
    0x3c, 0x63,

    /* U+004E "N" */
    0xc1, 0xf0, 0xfc, 0x7e, 0x3f, 0x9e, 0xcf, 0x77,
    0x9f, 0xc7, 0xe3, 0xf0, 0xf8, 0x30,

    /* U+004F "O" */
    0x1e, 0x1f, 0xe6, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xd8, 0x67, 0xf8, 0x78,

    /* U+0050 "P" */
    0xfe, 0x7f, 0xb0, 0xf8, 0x3c, 0x1e, 0x1f, 0xfd,
    0xfc, 0xc0, 0x60, 0x30, 0x18, 0x0,

    /* U+0051 "Q" */
    0x1e, 0x1f, 0xe6, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xd8, 0x67, 0xf8, 0x7e, 0x1,
    0xc0, 0x20,

    /* U+0052 "R" */
    0xfc, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xfc,
    0xce, 0xc6, 0xc7, 0xc3,

    /* U+0053 "S" */
    0x1e, 0x1f, 0x98, 0x6c, 0x37, 0x1, 0xe0, 0x3c,
    0x7, 0xc1, 0xf0, 0xdf, 0xc7, 0xc0,

    /* U+0054 "T" */
    0xff, 0xff, 0xc3, 0x1, 0x80, 0xc0, 0x60, 0x30,
    0x18, 0xc, 0x6, 0x3, 0x1, 0x80,

    /* U+0055 "U" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xf1, 0xdf, 0xc7, 0xc0,

    /* U+0056 "V" */
    0xe0, 0xd8, 0x76, 0x1d, 0xc6, 0x33, 0x8c, 0xe3,
    0xb0, 0x6c, 0x1f, 0x7, 0x81, 0xe0, 0x38,

    /* U+0057 "W" */
    0xe3, 0x1d, 0x8c, 0x66, 0x79, 0x99, 0xe6, 0x67,
    0x99, 0xde, 0xe3, 0x4b, 0xf, 0x3c, 0x3c, 0xf0,
    0xf3, 0xc3, 0xc7, 0x6, 0x18,

    /* U+0058 "X" */
    0x61, 0xdc, 0xe3, 0x38, 0xfc, 0x1e, 0x7, 0x81,
    0xe0, 0x78, 0x3f, 0xc, 0xe7, 0x3b, 0x87,

    /* U+0059 "Y" */
    0xe1, 0x98, 0x67, 0x38, 0xcc, 0x3f, 0x7, 0x81,
    0xe0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30,

    /* U+005A "Z" */
    0xff, 0xff, 0xc1, 0xc0, 0xe0, 0xe0, 0xe0, 0x60,
    0x70, 0x70, 0x30, 0x3f, 0xff, 0xf0,

    /* U+005B "[" */
    0xff, 0x6d, 0xb6, 0xdb, 0x6d, 0xb7, 0xe0,

    /* U+005C "\\" */
    0xe0, 0xc1, 0x83, 0x83, 0x6, 0xe, 0xc, 0x18,
    0x38, 0x30, 0x70, 0x60,

    /* U+005D "]" */
    0xfd, 0xb6, 0xdb, 0x6d, 0xb6, 0xdf, 0xe0,

    /* U+005E "^" */
    0x38, 0x70, 0xa3, 0x66, 0xc8, 0x80,

    /* U+005F "_" */
    0xff, 0xfc,

    /* U+0060 "`" */
    0x63,

    /* U+0061 "a" */
    0x7d, 0xff, 0x1b, 0xff, 0xf8, 0xf1, 0xff, 0x7e,

    /* U+0062 "b" */
    0xc1, 0x83, 0x7, 0xef, 0xd8, 0xf1, 0xe3, 0xc7,
    0x8f, 0xf7, 0xe0,

    /* U+0063 "c" */
    0x3c, 0xff, 0x1e, 0xc, 0x18, 0x31, 0xbf, 0x3c,

    /* U+0064 "d" */
    0x6, 0xc, 0x1b, 0xf7, 0xfc, 0xf1, 0xe3, 0xc7,
    0xcd, 0xfb, 0xf0,

    /* U+0065 "e" */
    0x3c, 0x7e, 0x63, 0x7f, 0xff, 0x60, 0x72, 0x7f,
    0x3e,

    /* U+0066 "f" */
    0x3d, 0xf6, 0x3e, 0xf9, 0x86, 0x18, 0x61, 0x86,
    0x18,

    /* U+0067 "g" */
    0x3f, 0x7f, 0xe3, 0xc3, 0xc3, 0xc3, 0xe3, 0x7f,
    0x3f, 0x47, 0xfe, 0x3c,

    /* U+0068 "h" */
    0xc1, 0x83, 0x6, 0xef, 0xf8, 0xf1, 0xe3, 0xc7,
    0x8f, 0x1e, 0x30,

    /* U+0069 "i" */
    0xf3, 0xff, 0xff,

    /* U+006A "j" */
    0x33, 0x3, 0x33, 0x33, 0x33, 0x33, 0x3f, 0xe0,

    /* U+006B "k" */
    0xc0, 0xc0, 0xc0, 0xce, 0xdc, 0xd8, 0xf0, 0xf8,
    0xf8, 0xdc, 0xce, 0xc6,

    /* U+006C "l" */
    0xff, 0xff, 0xff,

    /* U+006D "m" */
    0xdc, 0xef, 0xff, 0xc6, 0x3c, 0x63, 0xc6, 0x3c,
    0x63, 0xc6, 0x3c, 0x63, 0xc6, 0x30,

    /* U+006E "n" */
    0xdd, 0xff, 0x1e, 0x3c, 0x78, 0xf1, 0xe3, 0xc6,

    /* U+006F "o" */
    0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e,
    0x3c,

    /* U+0070 "p" */
    0xfd, 0xfb, 0x3e, 0x3c, 0x78, 0xf3, 0xfe, 0xf9,
    0x83, 0x6, 0x0,

    /* U+0071 "q" */
    0x7e, 0xff, 0x9e, 0x3c, 0x78, 0xf9, 0xbf, 0x7e,
    0xc, 0x18, 0x30,

    /* U+0072 "r" */
    0xff, 0xf1, 0x8c, 0x63, 0x18, 0xc0,

    /* U+0073 "s" */
    0x3e, 0x7f, 0x63, 0x60, 0x3e, 0x3, 0x63, 0x7f,
    0x3e,

    /* U+0074 "t" */
    0x63, 0x3f, 0xf6, 0x31, 0x8c, 0x63, 0xce,

    /* U+0075 "u" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xff, 0x7e,

    /* U+0076 "v" */
    0xe7, 0x67, 0x66, 0x66, 0x3e, 0x3c, 0x3c, 0x3c,
    0x18,

    /* U+0077 "w" */
    0xe6, 0x66, 0x66, 0x6e, 0x66, 0xf6, 0x6f, 0xc2,
    0xbc, 0x39, 0xc3, 0x9c, 0x31, 0x80,

    /* U+0078 "x" */
    0x67, 0x66, 0x3e, 0x3c, 0x1c, 0x3c, 0x3e, 0x66,
    0xe7,

    /* U+0079 "y" */
    0xe3, 0x67, 0x66, 0x76, 0x3e, 0x3c, 0x3c, 0x1c,
    0x18, 0x18, 0x70, 0x70,

    /* U+007A "z" */
    0xff, 0xfc, 0x30, 0xe3, 0x8e, 0x18, 0x7f, 0xfe,

    /* U+007B "{" */
    0x19, 0xcc, 0x63, 0x18, 0xdc, 0xe1, 0x8c, 0x63,
    0x18, 0xe3,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xc7, 0x18, 0xc6, 0x31, 0x87, 0x3b, 0x18, 0xc6,
    0x33, 0x98,

    /* U+007E "~" */
    0x71, 0xfc, 0xf3, 0xf8, 0xe0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 70, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 82, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 6, .adv_w = 153, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 20, .adv_w = 147, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 35, .adv_w = 189, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 52, .adv_w = 168, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 67, .adv_w = 41, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 68, .adv_w = 90, .box_w = 4, .box_h = 17, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 77, .adv_w = 90, .box_w = 4, .box_h = 17, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 86, .adv_w = 116, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 93, .adv_w = 140, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 101, .adv_w = 63, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 103, .adv_w = 99, .box_w = 4, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 104, .adv_w = 74, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 96, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 115, .adv_w = 147, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 147, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 147, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 147, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 147, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 147, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 147, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 196, .adv_w = 147, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 208, .adv_w = 147, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 219, .adv_w = 147, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 231, .adv_w = 72, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 234, .adv_w = 67, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 237, .adv_w = 130, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 244, .adv_w = 147, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 249, .adv_w = 132, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 256, .adv_w = 127, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 268, .adv_w = 229, .box_w = 13, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 293, .adv_w = 172, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 163, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 322, .adv_w = 168, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 166, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 350, .adv_w = 144, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 362, .adv_w = 140, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 374, .adv_w = 174, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 181, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 402, .adv_w = 75, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 143, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 416, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 139, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 442, .adv_w = 224, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 181, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 474, .adv_w = 177, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 489, .adv_w = 165, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 503, .adv_w = 177, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 521, .adv_w = 163, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 533, .adv_w = 157, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 547, .adv_w = 158, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 561, .adv_w = 169, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 575, .adv_w = 167, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 590, .adv_w = 224, .box_w = 14, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 611, .adv_w = 163, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 626, .adv_w = 158, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 155, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 655, .adv_w = 71, .box_w = 3, .box_h = 17, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 662, .adv_w = 108, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 674, .adv_w = 71, .box_w = 3, .box_h = 17, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 681, .adv_w = 112, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 687, .adv_w = 114, .box_w = 7, .box_h = 2, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 689, .adv_w = 85, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 690, .adv_w = 137, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 698, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 709, .adv_w = 134, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 717, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 728, .adv_w = 138, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 737, .adv_w = 92, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 746, .adv_w = 146, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 758, .adv_w = 143, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 769, .adv_w = 68, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 772, .adv_w = 67, .box_w = 4, .box_h = 15, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 780, .adv_w = 137, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 792, .adv_w = 68, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 795, .adv_w = 222, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 809, .adv_w = 143, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 817, .adv_w = 145, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 826, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 837, .adv_w = 145, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 848, .adv_w = 93, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 854, .adv_w = 132, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 863, .adv_w = 87, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 870, .adv_w = 143, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 878, .adv_w = 129, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 887, .adv_w = 188, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 901, .adv_w = 130, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 910, .adv_w = 129, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 922, .adv_w = 130, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 930, .adv_w = 85, .box_w = 5, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 940, .adv_w = 65, .box_w = 2, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 944, .adv_w = 85, .box_w = 5, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 954, .adv_w = 166, .box_w = 9, .box_h = 4, .ofs_x = 1, .ofs_y = 3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    1, 53,
    3, 3,
    3, 8,
    3, 34,
    3, 66,
    3, 68,
    3, 69,
    3, 70,
    3, 72,
    3, 78,
    3, 79,
    3, 80,
    3, 81,
    3, 82,
    3, 84,
    3, 88,
    8, 3,
    8, 8,
    8, 34,
    8, 66,
    8, 68,
    8, 69,
    8, 70,
    8, 72,
    8, 78,
    8, 79,
    8, 80,
    8, 81,
    8, 82,
    8, 84,
    8, 88,
    9, 55,
    9, 56,
    9, 58,
    13, 3,
    13, 8,
    15, 3,
    15, 8,
    16, 16,
    34, 3,
    34, 8,
    34, 32,
    34, 36,
    34, 40,
    34, 48,
    34, 50,
    34, 53,
    34, 54,
    34, 55,
    34, 56,
    34, 58,
    34, 78,
    34, 79,
    34, 80,
    34, 81,
    34, 85,
    34, 86,
    34, 87,
    34, 88,
    34, 90,
    34, 91,
    35, 53,
    35, 55,
    35, 58,
    36, 10,
    36, 53,
    36, 62,
    36, 94,
    37, 13,
    37, 15,
    37, 34,
    37, 53,
    37, 55,
    37, 57,
    37, 58,
    37, 59,
    38, 53,
    38, 68,
    38, 69,
    38, 70,
    38, 71,
    38, 72,
    38, 80,
    38, 82,
    38, 86,
    38, 87,
    38, 88,
    38, 90,
    39, 13,
    39, 15,
    39, 34,
    39, 43,
    39, 53,
    39, 66,
    39, 68,
    39, 69,
    39, 70,
    39, 72,
    39, 80,
    39, 82,
    39, 83,
    39, 86,
    39, 87,
    39, 90,
    41, 34,
    41, 53,
    41, 57,
    41, 58,
    42, 34,
    42, 53,
    42, 57,
    42, 58,
    43, 34,
    44, 14,
    44, 36,
    44, 40,
    44, 48,
    44, 50,
    44, 68,
    44, 69,
    44, 70,
    44, 72,
    44, 80,
    44, 82,
    44, 86,
    44, 87,
    44, 88,
    44, 90,
    45, 3,
    45, 8,
    45, 34,
    45, 36,
    45, 40,
    45, 48,
    45, 50,
    45, 53,
    45, 54,
    45, 55,
    45, 56,
    45, 58,
    45, 86,
    45, 87,
    45, 88,
    45, 90,
    46, 34,
    46, 53,
    46, 57,
    46, 58,
    47, 34,
    47, 53,
    47, 57,
    47, 58,
    48, 13,
    48, 15,
    48, 34,
    48, 53,
    48, 55,
    48, 57,
    48, 58,
    48, 59,
    49, 13,
    49, 15,
    49, 34,
    49, 43,
    49, 57,
    49, 59,
    49, 66,
    49, 68,
    49, 69,
    49, 70,
    49, 72,
    49, 80,
    49, 82,
    49, 85,
    49, 87,
    49, 90,
    50, 53,
    50, 55,
    50, 56,
    50, 58,
    51, 53,
    51, 55,
    51, 58,
    53, 1,
    53, 13,
    53, 14,
    53, 15,
    53, 34,
    53, 36,
    53, 40,
    53, 43,
    53, 48,
    53, 50,
    53, 52,
    53, 53,
    53, 55,
    53, 56,
    53, 58,
    53, 66,
    53, 68,
    53, 69,
    53, 70,
    53, 72,
    53, 78,
    53, 79,
    53, 80,
    53, 81,
    53, 82,
    53, 83,
    53, 84,
    53, 86,
    53, 87,
    53, 88,
    53, 89,
    53, 90,
    53, 91,
    54, 34,
    55, 10,
    55, 13,
    55, 14,
    55, 15,
    55, 34,
    55, 36,
    55, 40,
    55, 48,
    55, 50,
    55, 62,
    55, 66,
    55, 68,
    55, 69,
    55, 70,
    55, 72,
    55, 80,
    55, 82,
    55, 83,
    55, 86,
    55, 87,
    55, 90,
    55, 94,
    56, 10,
    56, 13,
    56, 14,
    56, 15,
    56, 34,
    56, 53,
    56, 62,
    56, 66,
    56, 68,
    56, 69,
    56, 70,
    56, 72,
    56, 80,
    56, 82,
    56, 83,
    56, 86,
    56, 94,
    57, 14,
    57, 36,
    57, 40,
    57, 48,
    57, 50,
    57, 55,
    57, 68,
    57, 69,
    57, 70,
    57, 72,
    57, 80,
    57, 82,
    57, 86,
    57, 87,
    57, 90,
    58, 7,
    58, 10,
    58, 11,
    58, 13,
    58, 14,
    58, 15,
    58, 34,
    58, 36,
    58, 40,
    58, 43,
    58, 48,
    58, 50,
    58, 52,
    58, 53,
    58, 54,
    58, 55,
    58, 56,
    58, 57,
    58, 58,
    58, 62,
    58, 66,
    58, 68,
    58, 69,
    58, 70,
    58, 71,
    58, 72,
    58, 78,
    58, 79,
    58, 80,
    58, 81,
    58, 82,
    58, 83,
    58, 84,
    58, 85,
    58, 86,
    58, 87,
    58, 89,
    58, 90,
    58, 91,
    58, 94,
    59, 34,
    59, 36,
    59, 40,
    59, 48,
    59, 50,
    59, 68,
    59, 69,
    59, 70,
    59, 72,
    59, 80,
    59, 82,
    59, 86,
    59, 87,
    59, 88,
    59, 90,
    60, 43,
    60, 54,
    66, 3,
    66, 8,
    66, 87,
    66, 90,
    67, 3,
    67, 8,
    67, 87,
    67, 89,
    67, 90,
    67, 91,
    68, 3,
    68, 8,
    70, 3,
    70, 8,
    70, 87,
    70, 90,
    71, 3,
    71, 8,
    71, 10,
    71, 62,
    71, 68,
    71, 69,
    71, 70,
    71, 72,
    71, 82,
    71, 94,
    73, 3,
    73, 8,
    76, 68,
    76, 69,
    76, 70,
    76, 72,
    76, 82,
    78, 3,
    78, 8,
    79, 3,
    79, 8,
    80, 3,
    80, 8,
    80, 87,
    80, 89,
    80, 90,
    80, 91,
    81, 3,
    81, 8,
    81, 87,
    81, 89,
    81, 90,
    81, 91,
    83, 3,
    83, 8,
    83, 13,
    83, 15,
    83, 66,
    83, 68,
    83, 69,
    83, 70,
    83, 71,
    83, 72,
    83, 80,
    83, 82,
    83, 85,
    83, 87,
    83, 88,
    83, 90,
    85, 80,
    87, 3,
    87, 8,
    87, 13,
    87, 15,
    87, 66,
    87, 68,
    87, 69,
    87, 70,
    87, 71,
    87, 72,
    87, 80,
    87, 82,
    88, 13,
    88, 15,
    89, 68,
    89, 69,
    89, 70,
    89, 72,
    89, 80,
    89, 82,
    90, 3,
    90, 8,
    90, 13,
    90, 15,
    90, 66,
    90, 68,
    90, 69,
    90, 70,
    90, 71,
    90, 72,
    90, 80,
    90, 82,
    91, 68,
    91, 69,
    91, 70,
    91, 72,
    91, 80,
    91, 82,
    92, 43,
    92, 54
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -7, -5, -5, -15, -6, -7, -7, -7,
    -7, -2, -2, -11, -2, -7, -11, 1,
    -5, -5, -15, -6, -7, -7, -7, -7,
    -2, -2, -11, -2, -7, -11, 1, 3,
    5, 3, -36, -36, -36, -36, -31, -15,
    -15, -10, -3, -3, -3, -3, -15, -2,
    -10, -5, -19, -6, -6, -1, -6, -2,
    -1, -6, -4, -6, 2, -3, -3, -7,
    -3, -4, -1, -2, -15, -15, -3, -11,
    -3, -3, -5, -3, 3, -2, -2, -2,
    -2, -2, -2, -2, -2, -3, -3, -3,
    -34, -34, -24, -26, 3, -4, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3,
    2, -4, 2, -3, 2, -4, 2, -3,
    -3, -20, -4, -4, -4, -4, -3, -3,
    -3, -3, -3, -3, -3, -5, -8, -5,
    -36, -36, 2, -8, -8, -8, -8, -26,
    -3, -26, -12, -35, -2, -15, -6, -15,
    2, -4, 2, -3, 2, -4, 2, -3,
    -15, -15, -3, -11, -3, -3, -5, -3,
    -50, -50, -22, -23, -6, -4, -1, -2,
    -2, -2, -2, -2, -2, 2, 2, 2,
    -4, -3, -2, -4, -6, -2, -6, -7,
    -32, -34, -32, -15, -3, -3, -27, -3,
    -3, -2, 2, 2, 2, 2, -21, -11,
    -11, -11, -11, -11, -11, -26, -11, -11,
    -8, -9, -8, -10, -6, -10, -10, -7,
    -3, 3, -27, -20, -27, -9, -2, -2,
    -2, -2, 2, -6, -5, -5, -5, -5,
    -6, -5, -4, -3, -1, -1, 2, 2,
    -18, -7, -18, -5, 2, 2, -4, -4,
    -4, -4, -4, -4, -4, -3, -2, 2,
    -19, -3, -3, -3, -3, 2, -3, -3,
    -3, -3, -3, -3, -3, -4, -4, -4,
    3, -6, -29, -19, -29, -19, -4, -4,
    -12, -4, -4, -2, 2, -12, 2, 2,
    2, 2, 2, -8, -8, -8, -8, -3,
    -8, -5, -5, -8, -5, -8, -5, -7,
    -3, -5, -2, -3, -2, -4, 2, 2,
    -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -2, -3, -3, -3, -2, -2,
    -2, -2, -2, -2, -4, -4, -1, -2,
    -1, -2, -1, -1, -2, -2, -2, -2,
    2, 2, 3, 2, -3, -3, -3, -3,
    -3, 2, -10, -10, -2, -2, -2, -2,
    -2, -10, -10, -10, -10, -11, -11, -2,
    -3, -2, -2, -4, -4, -1, -2, -1,
    -2, 2, 2, -22, -22, -4, -2, -2,
    -2, 3, -2, -4, -2, 6, 2, 2,
    2, -4, 2, 2, -21, -21, -2, -2,
    -2, -2, 2, -2, -2, -2, -15, -15,
    -2, -2, -2, -2, -5, -2, 2, 2,
    -21, -21, -2, -2, -2, -2, 2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 434,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t ui_font_robotf = {
#else
lv_font_t ui_font_robotf = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if UI_FONT_ROBOTF*/

