#include "wxTerminalAnsiEscapeHandler.hpp"

#include "clModuleLogger.hpp"
#include "clResult.hpp"
#include "file_logger.h"

#include <wx/colour.h>

INITIALISE_MODULE_LOG(LOG, "AnsiEscapeHandler", "ansi_escape_parser.log");

typedef std::unordered_map<int, wxColour> ColoursMap_t;
namespace
{

thread_local ColoursMap_t LightThemeColours;
thread_local ColoursMap_t LightThemeColours8Bit;
thread_local ColoursMap_t DarkThemeColours;
thread_local ColoursMap_t DarkThemeColours8Bit;
thread_local ColoursMap_t* pColours = nullptr;

void initialise_colours()
{
    if(!LightThemeColours.empty()) {
        return;
    }

    LightThemeColours.insert({ 30, wxColour(1, 1, 1) });
    LightThemeColours.insert({ 31, wxColour(222, 56, 43) });
    LightThemeColours.insert({ 32, wxColour(57, 181, 74) });
    LightThemeColours.insert({ 33, wxColour(255, 199, 6) });
    LightThemeColours.insert({ 34, wxColour(0, 111, 184) });
    LightThemeColours.insert({ 35, wxColour(118, 38, 113) });
    LightThemeColours.insert({ 36, wxColour(44, 181, 233) });
    LightThemeColours.insert({ 37, wxColour(204, 204, 204) });
    LightThemeColours.insert({ 90, wxColour(128, 128, 128) });
    LightThemeColours.insert({ 91, wxColour(255, 0, 0) });
    LightThemeColours.insert({ 92, wxColour(0, 255, 0) });
    LightThemeColours.insert({ 93, wxColour(255, 255, 0) });
    LightThemeColours.insert({ 94, wxColour(0, 0, 255) });
    LightThemeColours.insert({ 95, wxColour(255, 0, 255) });
    LightThemeColours.insert({ 96, wxColour(0, 255, 255) });
    LightThemeColours.insert({ 97, wxColour(255, 255, 255) });

    // Background colours
    LightThemeColours.insert({ 40, wxColour(1, 1, 1) });
    LightThemeColours.insert({ 41, wxColour(222, 56, 43) });
    LightThemeColours.insert({ 42, wxColour(57, 181, 74) });
    LightThemeColours.insert({ 43, wxColour(255, 199, 6) });
    LightThemeColours.insert({ 44, wxColour(0, 111, 184) });
    LightThemeColours.insert({ 45, wxColour(118, 38, 113) });
    LightThemeColours.insert({ 46, wxColour(44, 181, 233) });
    LightThemeColours.insert({ 47, wxColour(204, 204, 204) });
    LightThemeColours.insert({ 100, wxColour(128, 128, 128) });
    LightThemeColours.insert({ 101, wxColour(255, 0, 0) });
    LightThemeColours.insert({ 102, wxColour(0, 255, 0) });
    LightThemeColours.insert({ 103, wxColour(255, 255, 0) });
    LightThemeColours.insert({ 104, wxColour(0, 0, 255) });
    LightThemeColours.insert({ 105, wxColour(255, 0, 255) });
    LightThemeColours.insert({ 106, wxColour(0, 255, 255) });
    LightThemeColours.insert({ 107, wxColour(255, 255, 255) });

    // 8 bit colours
    // ESC[35;5;<fg colour>
    // ESC[48;5;<bg colour>
    LightThemeColours8Bit.insert({ 0, wxColour("#000000") });
    LightThemeColours8Bit.insert({ 1, wxColour("#800000") });
    LightThemeColours8Bit.insert({ 2, wxColour("#008000") });
    LightThemeColours8Bit.insert({ 3, wxColour("#808000") });
    LightThemeColours8Bit.insert({ 4, wxColour("#000080") });
    LightThemeColours8Bit.insert({ 5, wxColour("#800080") });
    LightThemeColours8Bit.insert({ 6, wxColour("#008080") });
    LightThemeColours8Bit.insert({ 7, wxColour("#c0c0c0") });
    LightThemeColours8Bit.insert({ 8, wxColour("#808080") });
    LightThemeColours8Bit.insert({ 9, wxColour("#ff0000") });
    LightThemeColours8Bit.insert({ 10, wxColour("#00ff00") });
    LightThemeColours8Bit.insert({ 11, wxColour("#ffff00") });
    LightThemeColours8Bit.insert({ 12, wxColour("#0000ff") });
    LightThemeColours8Bit.insert({ 13, wxColour("#ff00ff") });
    LightThemeColours8Bit.insert({ 14, wxColour("#00ffff") });
    LightThemeColours8Bit.insert({ 15, wxColour("#ffffff") });
    LightThemeColours8Bit.insert({ 16, wxColour("#000000") });
    LightThemeColours8Bit.insert({ 17, wxColour("#00005f") });
    LightThemeColours8Bit.insert({ 18, wxColour("#000087") });
    LightThemeColours8Bit.insert({ 19, wxColour("#0000af") });
    LightThemeColours8Bit.insert({ 20, wxColour("#0000d7") });
    LightThemeColours8Bit.insert({ 21, wxColour("#0000ff") });
    LightThemeColours8Bit.insert({ 22, wxColour("#005f00") });
    LightThemeColours8Bit.insert({ 23, wxColour("#005f5f") });
    LightThemeColours8Bit.insert({ 24, wxColour("#005f87") });
    LightThemeColours8Bit.insert({ 25, wxColour("#005faf") });
    LightThemeColours8Bit.insert({ 26, wxColour("#005fd7") });
    LightThemeColours8Bit.insert({ 27, wxColour("#005fff") });
    LightThemeColours8Bit.insert({ 28, wxColour("#008700") });
    LightThemeColours8Bit.insert({ 29, wxColour("#00875f") });
    LightThemeColours8Bit.insert({ 30, wxColour("#008787") });
    LightThemeColours8Bit.insert({ 31, wxColour("#0087af") });
    LightThemeColours8Bit.insert({ 32, wxColour("#0087d7") });
    LightThemeColours8Bit.insert({ 33, wxColour("#0087ff") });
    LightThemeColours8Bit.insert({ 34, wxColour("#00af00") });
    LightThemeColours8Bit.insert({ 35, wxColour("#00af5f") });
    LightThemeColours8Bit.insert({ 36, wxColour("#00af87") });
    LightThemeColours8Bit.insert({ 37, wxColour("#00afaf") });
    LightThemeColours8Bit.insert({ 38, wxColour("#00afd7") });
    LightThemeColours8Bit.insert({ 39, wxColour("#00afff") });
    LightThemeColours8Bit.insert({ 40, wxColour("#00d700") });
    LightThemeColours8Bit.insert({ 41, wxColour("#00d75f") });
    LightThemeColours8Bit.insert({ 42, wxColour("#00d787") });
    LightThemeColours8Bit.insert({ 43, wxColour("#00d7af") });
    LightThemeColours8Bit.insert({ 44, wxColour("#00d7d7") });
    LightThemeColours8Bit.insert({ 45, wxColour("#00d7ff") });
    LightThemeColours8Bit.insert({ 46, wxColour("#00ff00") });
    LightThemeColours8Bit.insert({ 47, wxColour("#00ff5f") });
    LightThemeColours8Bit.insert({ 48, wxColour("#00ff87") });
    LightThemeColours8Bit.insert({ 49, wxColour("#00ffaf") });
    LightThemeColours8Bit.insert({ 50, wxColour("#00ffd7") });
    LightThemeColours8Bit.insert({ 51, wxColour("#00ffff") });
    LightThemeColours8Bit.insert({ 52, wxColour("#5f0000") });
    LightThemeColours8Bit.insert({ 53, wxColour("#5f005f") });
    LightThemeColours8Bit.insert({ 54, wxColour("#5f0087") });
    LightThemeColours8Bit.insert({ 55, wxColour("#5f00af") });
    LightThemeColours8Bit.insert({ 56, wxColour("#5f00d7") });
    LightThemeColours8Bit.insert({ 57, wxColour("#5f00ff") });
    LightThemeColours8Bit.insert({ 58, wxColour("#5f5f00") });
    LightThemeColours8Bit.insert({ 59, wxColour("#5f5f5f") });
    LightThemeColours8Bit.insert({ 60, wxColour("#5f5f87") });
    LightThemeColours8Bit.insert({ 61, wxColour("#5f5faf") });
    LightThemeColours8Bit.insert({ 62, wxColour("#5f5fd7") });
    LightThemeColours8Bit.insert({ 63, wxColour("#5f5fff") });
    LightThemeColours8Bit.insert({ 64, wxColour("#5f8700") });
    LightThemeColours8Bit.insert({ 65, wxColour("#5f875f") });
    LightThemeColours8Bit.insert({ 66, wxColour("#5f8787") });
    LightThemeColours8Bit.insert({ 67, wxColour("#5f87af") });
    LightThemeColours8Bit.insert({ 68, wxColour("#5f87d7") });
    LightThemeColours8Bit.insert({ 69, wxColour("#5f87ff") });
    LightThemeColours8Bit.insert({ 70, wxColour("#5faf00") });
    LightThemeColours8Bit.insert({ 71, wxColour("#5faf5f") });
    LightThemeColours8Bit.insert({ 72, wxColour("#5faf87") });
    LightThemeColours8Bit.insert({ 73, wxColour("#5fafaf") });
    LightThemeColours8Bit.insert({ 74, wxColour("#5fafd7") });
    LightThemeColours8Bit.insert({ 75, wxColour("#5fafff") });
    LightThemeColours8Bit.insert({ 76, wxColour("#5fd700") });
    LightThemeColours8Bit.insert({ 77, wxColour("#5fd75f") });
    LightThemeColours8Bit.insert({ 78, wxColour("#5fd787") });
    LightThemeColours8Bit.insert({ 79, wxColour("#5fd7af") });
    LightThemeColours8Bit.insert({ 80, wxColour("#5fd7d7") });
    LightThemeColours8Bit.insert({ 81, wxColour("#5fd7ff") });
    LightThemeColours8Bit.insert({ 82, wxColour("#5fff00") });
    LightThemeColours8Bit.insert({ 83, wxColour("#5fff5f") });
    LightThemeColours8Bit.insert({ 84, wxColour("#5fff87") });
    LightThemeColours8Bit.insert({ 85, wxColour("#5fffaf") });
    LightThemeColours8Bit.insert({ 86, wxColour("#5fffd7") });
    LightThemeColours8Bit.insert({ 87, wxColour("#5fffff") });
    LightThemeColours8Bit.insert({ 88, wxColour("#870000") });
    LightThemeColours8Bit.insert({ 89, wxColour("#87005f") });
    LightThemeColours8Bit.insert({ 90, wxColour("#870087") });
    LightThemeColours8Bit.insert({ 91, wxColour("#8700af") });
    LightThemeColours8Bit.insert({ 92, wxColour("#8700d7") });
    LightThemeColours8Bit.insert({ 93, wxColour("#8700ff") });
    LightThemeColours8Bit.insert({ 94, wxColour("#875f00") });
    LightThemeColours8Bit.insert({ 95, wxColour("#875f5f") });
    LightThemeColours8Bit.insert({ 96, wxColour("#875f87") });
    LightThemeColours8Bit.insert({ 97, wxColour("#875faf") });
    LightThemeColours8Bit.insert({ 98, wxColour("#875fd7") });
    LightThemeColours8Bit.insert({ 99, wxColour("#875fff") });
    LightThemeColours8Bit.insert({ 100, wxColour("#878700") });
    LightThemeColours8Bit.insert({ 101, wxColour("#87875f") });
    LightThemeColours8Bit.insert({ 102, wxColour("#878787") });
    LightThemeColours8Bit.insert({ 103, wxColour("#8787af") });
    LightThemeColours8Bit.insert({ 104, wxColour("#8787d7") });
    LightThemeColours8Bit.insert({ 105, wxColour("#8787ff") });
    LightThemeColours8Bit.insert({ 106, wxColour("#87af00") });
    LightThemeColours8Bit.insert({ 107, wxColour("#87af5f") });
    LightThemeColours8Bit.insert({ 108, wxColour("#87af87") });
    LightThemeColours8Bit.insert({ 109, wxColour("#87afaf") });
    LightThemeColours8Bit.insert({ 110, wxColour("#87afd7") });
    LightThemeColours8Bit.insert({ 111, wxColour("#87afff") });
    LightThemeColours8Bit.insert({ 112, wxColour("#87d700") });
    LightThemeColours8Bit.insert({ 113, wxColour("#87d75f") });
    LightThemeColours8Bit.insert({ 114, wxColour("#87d787") });
    LightThemeColours8Bit.insert({ 115, wxColour("#87d7af") });
    LightThemeColours8Bit.insert({ 116, wxColour("#87d7d7") });
    LightThemeColours8Bit.insert({ 117, wxColour("#87d7ff") });
    LightThemeColours8Bit.insert({ 118, wxColour("#87ff00") });
    LightThemeColours8Bit.insert({ 119, wxColour("#87ff5f") });
    LightThemeColours8Bit.insert({ 120, wxColour("#87ff87") });
    LightThemeColours8Bit.insert({ 121, wxColour("#87ffaf") });
    LightThemeColours8Bit.insert({ 122, wxColour("#87ffd7") });
    LightThemeColours8Bit.insert({ 123, wxColour("#87ffff") });
    LightThemeColours8Bit.insert({ 124, wxColour("#af0000") });
    LightThemeColours8Bit.insert({ 125, wxColour("#af005f") });
    LightThemeColours8Bit.insert({ 126, wxColour("#af0087") });
    LightThemeColours8Bit.insert({ 127, wxColour("#af00af") });
    LightThemeColours8Bit.insert({ 128, wxColour("#af00d7") });
    LightThemeColours8Bit.insert({ 129, wxColour("#af00ff") });
    LightThemeColours8Bit.insert({ 130, wxColour("#af5f00") });
    LightThemeColours8Bit.insert({ 131, wxColour("#af5f5f") });
    LightThemeColours8Bit.insert({ 132, wxColour("#af5f87") });
    LightThemeColours8Bit.insert({ 133, wxColour("#af5faf") });
    LightThemeColours8Bit.insert({ 134, wxColour("#af5fd7") });
    LightThemeColours8Bit.insert({ 135, wxColour("#af5fff") });
    LightThemeColours8Bit.insert({ 136, wxColour("#af8700") });
    LightThemeColours8Bit.insert({ 137, wxColour("#af875f") });
    LightThemeColours8Bit.insert({ 138, wxColour("#af8787") });
    LightThemeColours8Bit.insert({ 139, wxColour("#af87af") });
    LightThemeColours8Bit.insert({ 140, wxColour("#af87d7") });
    LightThemeColours8Bit.insert({ 141, wxColour("#af87ff") });
    LightThemeColours8Bit.insert({ 142, wxColour("#afaf00") });
    LightThemeColours8Bit.insert({ 143, wxColour("#afaf5f") });
    LightThemeColours8Bit.insert({ 144, wxColour("#afaf87") });
    LightThemeColours8Bit.insert({ 145, wxColour("#afafaf") });
    LightThemeColours8Bit.insert({ 146, wxColour("#afafd7") });
    LightThemeColours8Bit.insert({ 147, wxColour("#afafff") });
    LightThemeColours8Bit.insert({ 148, wxColour("#afd700") });
    LightThemeColours8Bit.insert({ 149, wxColour("#afd75f") });
    LightThemeColours8Bit.insert({ 150, wxColour("#afd787") });
    LightThemeColours8Bit.insert({ 151, wxColour("#afd7af") });
    LightThemeColours8Bit.insert({ 152, wxColour("#afd7d7") });
    LightThemeColours8Bit.insert({ 153, wxColour("#afd7ff") });
    LightThemeColours8Bit.insert({ 154, wxColour("#afff00") });
    LightThemeColours8Bit.insert({ 155, wxColour("#afff5f") });
    LightThemeColours8Bit.insert({ 156, wxColour("#afff87") });
    LightThemeColours8Bit.insert({ 157, wxColour("#afffaf") });
    LightThemeColours8Bit.insert({ 158, wxColour("#afffd7") });
    LightThemeColours8Bit.insert({ 159, wxColour("#afffff") });
    LightThemeColours8Bit.insert({ 160, wxColour("#d70000") });
    LightThemeColours8Bit.insert({ 161, wxColour("#d7005f") });
    LightThemeColours8Bit.insert({ 162, wxColour("#d70087") });
    LightThemeColours8Bit.insert({ 163, wxColour("#d700af") });
    LightThemeColours8Bit.insert({ 164, wxColour("#d700d7") });
    LightThemeColours8Bit.insert({ 165, wxColour("#d700ff") });
    LightThemeColours8Bit.insert({ 166, wxColour("#d75f00") });
    LightThemeColours8Bit.insert({ 167, wxColour("#d75f5f") });
    LightThemeColours8Bit.insert({ 168, wxColour("#d75f87") });
    LightThemeColours8Bit.insert({ 169, wxColour("#d75faf") });
    LightThemeColours8Bit.insert({ 170, wxColour("#d75fd7") });
    LightThemeColours8Bit.insert({ 171, wxColour("#d75fff") });
    LightThemeColours8Bit.insert({ 172, wxColour("#d78700") });
    LightThemeColours8Bit.insert({ 173, wxColour("#d7875f") });
    LightThemeColours8Bit.insert({ 174, wxColour("#d78787") });
    LightThemeColours8Bit.insert({ 175, wxColour("#d787af") });
    LightThemeColours8Bit.insert({ 176, wxColour("#d787d7") });
    LightThemeColours8Bit.insert({ 177, wxColour("#d787ff") });
    LightThemeColours8Bit.insert({ 178, wxColour("#d7af00") });
    LightThemeColours8Bit.insert({ 179, wxColour("#d7af5f") });
    LightThemeColours8Bit.insert({ 180, wxColour("#d7af87") });
    LightThemeColours8Bit.insert({ 181, wxColour("#d7afaf") });
    LightThemeColours8Bit.insert({ 182, wxColour("#d7afd7") });
    LightThemeColours8Bit.insert({ 183, wxColour("#d7afff") });
    LightThemeColours8Bit.insert({ 184, wxColour("#d7d700") });
    LightThemeColours8Bit.insert({ 185, wxColour("#d7d75f") });
    LightThemeColours8Bit.insert({ 186, wxColour("#d7d787") });
    LightThemeColours8Bit.insert({ 187, wxColour("#d7d7af") });
    LightThemeColours8Bit.insert({ 188, wxColour("#d7d7d7") });
    LightThemeColours8Bit.insert({ 189, wxColour("#d7d7ff") });
    LightThemeColours8Bit.insert({ 190, wxColour("#d7ff00") });
    LightThemeColours8Bit.insert({ 191, wxColour("#d7ff5f") });
    LightThemeColours8Bit.insert({ 192, wxColour("#d7ff87") });
    LightThemeColours8Bit.insert({ 193, wxColour("#d7ffaf") });
    LightThemeColours8Bit.insert({ 194, wxColour("#d7ffd7") });
    LightThemeColours8Bit.insert({ 195, wxColour("#d7ffff") });
    LightThemeColours8Bit.insert({ 196, wxColour("#ff0000") });
    LightThemeColours8Bit.insert({ 197, wxColour("#ff005f") });
    LightThemeColours8Bit.insert({ 198, wxColour("#ff0087") });
    LightThemeColours8Bit.insert({ 199, wxColour("#ff00af") });
    LightThemeColours8Bit.insert({ 200, wxColour("#ff00d7") });
    LightThemeColours8Bit.insert({ 201, wxColour("#ff00ff") });
    LightThemeColours8Bit.insert({ 202, wxColour("#ff5f00") });
    LightThemeColours8Bit.insert({ 203, wxColour("#ff5f5f") });
    LightThemeColours8Bit.insert({ 204, wxColour("#ff5f87") });
    LightThemeColours8Bit.insert({ 205, wxColour("#ff5faf") });
    LightThemeColours8Bit.insert({ 206, wxColour("#ff5fd7") });
    LightThemeColours8Bit.insert({ 207, wxColour("#ff5fff") });
    LightThemeColours8Bit.insert({ 208, wxColour("#ff8700") });
    LightThemeColours8Bit.insert({ 209, wxColour("#ff875f") });
    LightThemeColours8Bit.insert({ 210, wxColour("#ff8787") });
    LightThemeColours8Bit.insert({ 211, wxColour("#ff87af") });
    LightThemeColours8Bit.insert({ 212, wxColour("#ff87d7") });
    LightThemeColours8Bit.insert({ 213, wxColour("#ff87ff") });
    LightThemeColours8Bit.insert({ 214, wxColour("#ffaf00") });
    LightThemeColours8Bit.insert({ 215, wxColour("#ffaf5f") });
    LightThemeColours8Bit.insert({ 216, wxColour("#ffaf87") });
    LightThemeColours8Bit.insert({ 217, wxColour("#ffafaf") });
    LightThemeColours8Bit.insert({ 218, wxColour("#ffafd7") });
    LightThemeColours8Bit.insert({ 219, wxColour("#ffafff") });
    LightThemeColours8Bit.insert({ 220, wxColour("#ffd700") });
    LightThemeColours8Bit.insert({ 221, wxColour("#ffd75f") });
    LightThemeColours8Bit.insert({ 222, wxColour("#ffd787") });
    LightThemeColours8Bit.insert({ 223, wxColour("#ffd7af") });
    LightThemeColours8Bit.insert({ 224, wxColour("#ffd7d7") });
    LightThemeColours8Bit.insert({ 225, wxColour("#ffd7ff") });
    LightThemeColours8Bit.insert({ 226, wxColour("#ffff00") });
    LightThemeColours8Bit.insert({ 227, wxColour("#ffff5f") });
    LightThemeColours8Bit.insert({ 228, wxColour("#ffff87") });
    LightThemeColours8Bit.insert({ 229, wxColour("#ffffaf") });
    LightThemeColours8Bit.insert({ 230, wxColour("#ffffd7") });
    LightThemeColours8Bit.insert({ 231, wxColour("#ffffff") });
    LightThemeColours8Bit.insert({ 232, wxColour("#080808") });
    LightThemeColours8Bit.insert({ 233, wxColour("#121212") });
    LightThemeColours8Bit.insert({ 234, wxColour("#1c1c1c") });
    LightThemeColours8Bit.insert({ 235, wxColour("#262626") });
    LightThemeColours8Bit.insert({ 236, wxColour("#303030") });
    LightThemeColours8Bit.insert({ 237, wxColour("#3a3a3a") });
    LightThemeColours8Bit.insert({ 238, wxColour("#444444") });
    LightThemeColours8Bit.insert({ 239, wxColour("#4e4e4e") });
    LightThemeColours8Bit.insert({ 240, wxColour("#585858") });
    LightThemeColours8Bit.insert({ 241, wxColour("#626262") });
    LightThemeColours8Bit.insert({ 242, wxColour("#6c6c6c") });
    LightThemeColours8Bit.insert({ 243, wxColour("#767676") });
    LightThemeColours8Bit.insert({ 244, wxColour("#808080") });
    LightThemeColours8Bit.insert({ 245, wxColour("#8a8a8a") });
    LightThemeColours8Bit.insert({ 246, wxColour("#949494") });
    LightThemeColours8Bit.insert({ 247, wxColour("#9e9e9e") });
    LightThemeColours8Bit.insert({ 248, wxColour("#a8a8a8") });
    LightThemeColours8Bit.insert({ 249, wxColour("#b2b2b2") });
    LightThemeColours8Bit.insert({ 250, wxColour("#bcbcbc") });
    LightThemeColours8Bit.insert({ 251, wxColour("#c6c6c6") });
    LightThemeColours8Bit.insert({ 252, wxColour("#d0d0d0") });
    LightThemeColours8Bit.insert({ 253, wxColour("#dadada") });
    LightThemeColours8Bit.insert({ 254, wxColour("#e4e4e4") });
    LightThemeColours8Bit.insert({ 255, wxColour("#eeeeee") });

    DarkThemeColours8Bit = LightThemeColours8Bit;
    // lighten the colours a bit
    for(auto& [_, colour] : LightThemeColours8Bit) {
        colour = colour.ChangeLightness(150);
    }

    // darken the colours for light theme
    for(auto& [_, colour] : LightThemeColours8Bit) {
        colour = colour.ChangeLightness(80);
    }

    DarkThemeColours = LightThemeColours;

    // lighten the colours a bit
    for(auto& [_, colour] : DarkThemeColours) {
        colour = colour.ChangeLightness(150);
    }

    // darken the colours for light theme
    for(auto& [_, colour] : LightThemeColours) {
        colour = colour.ChangeLightness(80);
    }

    pColours = &LightThemeColours;
}

const wxColour& find_colour_by_number(ColoursMap_t* coloursMap, int num)
{
    if(!coloursMap || pColours->count(num) == 0) {
        return wxNullColour;
    }
    return pColours->find(num)->second;
}

/// safely get char at pos. return 0 if out-of-index
inline wxChar safe_get_char(wxStringView buffer, size_t pos)
{
    if(pos < buffer.length()) {
        return buffer[pos];
    }
    return 0;
}

enum AnsiControlCode {
    BELL = 0x07, // Makes an audible noise.
    BS = 0x08,   // Backspace  Moves the cursor left (but may "backwards wrap" if cursor is at start of line).
    HT = 0x09,   // Tab
    LF = 0x0A,   // Line Feed
    FF = 0x0C,   // Form Feed
    CR = 0x0D,   // Carriage Return
    ESC = 0x1B,  // Escape
};

/// takes an input sv and render its content by taking CR, LF and BS into consideration
void render_string(wxStringView sv, wxTerminalAnsiRendererInterface* renderer)
{
    enum class States {
        STATE_NORMAL = 0,
        STATE_CR = 1,
    } state;

#define RENDER_STRING(end_pos)                                    \
    if(end_pos > anchor) {                                        \
        renderer->AddString(sv.substr(anchor, end_pos - anchor)); \
    }

    size_t anchor = 0;
    state = States::STATE_NORMAL;
    size_t curpos = 0;
    for(; curpos < sv.length(); ++curpos) {
        switch(state) {
        case States::STATE_NORMAL:
            switch(sv[curpos]) {
            case AnsiControlCode::BS:
                RENDER_STRING(curpos);
                renderer->Backspace();
                anchor = curpos + 1; // Skip the BackSpace
                break;
            case '\r':
                state = States::STATE_CR;
                break;
            default:
                break;
            }
            break; // STATE_NORMAL
        case States::STATE_CR:
            switch(sv[curpos]) {
            case '\n':
                // a case of \r\n
                // render any string seen until the \r
                RENDER_STRING(curpos - 1);
                // We handle \r\n as LF
                renderer->LineFeed();
                // set the new string anchor
                anchor = curpos + 1; // Skip the LF
                state = States::STATE_NORMAL;
                break;
            default:
                // it was \r only
                RENDER_STRING(curpos - 1);
                renderer->CarriageReturn();
                anchor = curpos;
                state = States::STATE_NORMAL;
                break;
            }
            break;
        }
    }
    RENDER_STRING(curpos);
#undef RENDER_STRING
}

enum AnsiSequenceType {
    NEED_MORE_DATA = -3,
    NOT_ESCAPE = -2,
    UNKNOWN_SEQUENCE_TYPE = -1,
    SS2 = 'N', // Single Shift Two
    SS3 = 'O', // Single Shift Three
    DCS = 'P', // Device Control String
    CSI = '[', // Control Sequence Introducer
    ST = '\\', // String Terminator
    OSC = ']', // Operating System Command
    SOS = 'X', // Start of String
    PM = '^',  // Privacy Message
    APC = '_', // Application Program Command
};

/// Common sequences + private sequences
enum AnsiControlSequence {
    UnknownControlSequence = -1,
    CursorUp = 'A', // Cursor Up, CSI n A. Moves the cursor n (default 1) cells in the given direction. If the cursor is
                    // already at the edge of the screen, this has no effect.
    CursorDown = 'B',    // Cursor down, CSI n B. Moves the cursor n (default 1) cells in the given direction. If the
                         // cursor is already at the edge of the screen, this has no effect.
    CursorForward = 'C', // Cursor Forward, CSI n C. Moves the cursor n (default 1) cells in the given direction. If the
                         // cursor is already at the edge of the screen, this has no effect.
    CursorBackward = 'D', // Cursor Backward, CSI n D. Moves the cursor n (default 1) cells in the given direction. If
                          // the cursor is already at the edge of the screen, this has no effect.
    CursorNextLine = 'E', // Cursor Next Line, CSI n E. Moves cursor to beginning of the line n (default 1) lines down
    CursorPreviousLine =
        'F', // Cursor Previous Line, CSI n F. Moves cursor to beginning of the line n (default 1) lines up
    CursorHorizontalAbsolute = 'G', // Cursor Horizontal Absolute, CSI n G. Moves the cursor to column n (default 1)
    CursorPosition = 'H', // Cursor Position, CSI n ; m H. Moves the cursor to row n, column m. The values are 1-based,
                          // and default to 1 (top left corner) if omitted. A sequence such as CSI ;5H is a synonym for
                          // CSI 1;5H as well as CSI 17;H is the same as CSI 17H and CSI 17;1H
    EraseInDisplay = 'J', // Erase in Display,  CSI n J. Clears part of the screen. If n is 0 (or missing), clear from
                          // cursor to end of screen. If n is 1, clear from cursor to beginning of the screen. If n is
                          // 2, clear entire screen (and moves cursor to upper left on DOS ANSI.SYS). If n is 3, clear
                          // entire screen and delete all lines saved in the scrollback buffer (this feature was added
                          // for xterm and is supported by other terminal applications).
    EraseInLine = 'K', // Erase in Line. CSI n K. Erases part of the line. If n is 0 (or missing), clear from cursor to
                       // the end of the line. If n is 1, clear from cursor to beginning of the line. If n is 2, clear
                       // entire line. Cursor position does not change.
    ScrollUp =
        'S', // Scroll Up. CSI n S. Scroll whole page up by n (default 1) lines. New lines are added at the bottom
    ScrollDown =
        'T', // Scroll Down. CSI n T. Scroll whole page down by n (default 1) lines. New lines are added at the top
    HorizontalVerticalPosition =
        'f', // Horizontal Vertical Position. CSI n ; m f. Same as CursorPosition, but counts as a format effector
             // function (like CR or LF) rather than an editor function (like CursorDown or CursorNextLine). This can
             // lead to different handling in certain terminal modes
    SelectGraphicRendition =
        'm',      // Select Graphic Rendition. CSI n m. Sets colors and style of the characters following this code
    AuxOn = 'i',  // CSI 5i. Enable aux serial port usually for local serial printer
    AuxOff = 'i', // CSI 4i. Disable aux serial port usually for local serial printer
    DeviceStatusReport = 'n', // CSI 6n. Device Status Report. Reports the cursor position (CPR) by transmitting
                              // ESC[n;mR, where n is the row and m is the column.

    // private sequences, we support them so we can ignore them
    SaveCurrentCursorPosition = 's',
    RestoreSavedCursorPosition = 'u',
    ActionEnable = 'h',  // Enable action, the action is determinted by the content between CSI <action> <h>
    ActionDisable = 'l', // Disable action, the action is determinted by the content between CSI <action> <l>
    EraseCharacter =
        'X', // Erase <n> characters from the current cursor position by overwriting them with a space character.
};

struct ColsRows {
    long n = 1;
    long m = 1;
};

struct AnsiControlSequenceValue {
    AnsiControlSequence seq = AnsiControlSequence::UnknownControlSequence;
    wxAny value;

    ColsRows value_cols_rows() const
    {
        ColsRows v;
        if(value.GetAs(&v)) {
            return v;
        }
        return {};
    }

    long value_long() const
    {
        long v;
        if(value.GetAs(&v)) {
            return v;
        }
        return 0;
    }

    long value_int() const
    {
        int v;
        if(value.GetAs(&v)) {
            return v;
        }
        return 0;
    }

    wxString value_string() const
    {
        wxString v;
        if(value.GetAs(&v)) {
            return v;
        }
        return wxEmptyString;
    }
};

/// read from the buffer until we find code
inline size_t find_control_code(wxStringView buffer, int code)
{
    size_t skip = 0;
    for(; skip < buffer.length(); ++skip) {
        if(buffer[skip] == code) {
            return skip;
        }
    }
    return wxStringView::npos;
}

/// read from the buffer until we find code
inline size_t find_st(wxStringView buffer, size_t* stlen)
{
    size_t pos = 0;
    for(; pos < buffer.length(); ++pos) {
        if(buffer[pos] == AnsiControlCode::ESC && safe_get_char(buffer, pos + 1) == AnsiSequenceType::ST) {
            *stlen = 2;
            return pos;
        } else if(buffer[pos] == AnsiControlCode::BELL) {
            *stlen = 1;
            return pos;
        }
    }
    return wxStringView::npos;
}

/// read from the buffer until we find the first code1 or code2 (whichever comes first)
inline size_t find_first_control_code(wxStringView buffer, int code1, int code2)
{
    size_t skip = 0;
    for(; skip < buffer.length(); ++skip) {
        if(buffer[skip] == code1 || buffer[skip] == code2) {
            break;
        }
    }
    return skip;
}

/// Starting from the start of the buffer, check the ANSI sequence type
inline AnsiSequenceType ansi_sequence_type(wxStringView buffer)
{
    wxChar ch0 = buffer[0];
    if(ch0 != AnsiControlCode::ESC) {
        return AnsiSequenceType::NOT_ESCAPE;
    }

    if(buffer.length() < 2) {
        return AnsiSequenceType::NEED_MORE_DATA;
    }

    wxChar ch1 = buffer[1];
    switch(ch1) {
    case AnsiSequenceType::SS2:
    case AnsiSequenceType::SS3:
    case AnsiSequenceType::DCS:
    case AnsiSequenceType::CSI:
    case AnsiSequenceType::ST:
    case AnsiSequenceType::OSC:
    case AnsiSequenceType::SOS:
    case AnsiSequenceType::PM:
    case AnsiSequenceType::APC:
        return (AnsiSequenceType)ch1;
    default:
        return AnsiSequenceType::UNKNOWN_SEQUENCE_TYPE;
    }
}

/// Parse wxStringView -> long
inline long wxStringViewAtol(wxStringView sv, long default_value)
{
    if(sv.empty()) {
        return default_value;
    }

    wxString str(sv.data(), sv.length());
    long v = default_value;
    if(str.ToCLong(&v)) {
        return v;
    }
    return default_value;
}

/// Parse "n;m" and return a `ColsRows` struct
inline ColsRows parse_cols_rows(wxStringView s, long default_value)
{
    ColsRows res{ default_value, default_value };
    auto where = s.find(';');
    if(where == wxStringView::npos) {
        res.n = wxStringViewAtol(s, default_value);
    } else {
        res.n = wxStringViewAtol(s.substr(0, where), default_value);
        res.m = wxStringViewAtol(s.substr(where + 1), default_value);
    }
    return res;
}

/// This function should be called after a successful call to
/// ansi_sequence_type() == CSI
/// An example for accepted sequence: 0x1B 0x9B 1 A
///                                     \   /  /| |
///                                      CSI  n | A => CursorUp (Cursor Up)
///                                             |
///                                          buffer
/// Loop until we find the terminator
inline wxHandlResultStringView ansi_control_sequence(wxStringView buffer, AnsiControlSequenceValue* value)
{
    for(size_t i = 0; i < buffer.length(); ++i) {
        switch(buffer[i]) {
        case AnsiControlSequence::SaveCurrentCursorPosition:
        case AnsiControlSequence::RestoreSavedCursorPosition: {
            // just skip it
            return buffer.substr(i + 1);
        } break;
        case AnsiControlSequence::CursorUp:                 // default 1
        case AnsiControlSequence::CursorDown:               // default 1
        case AnsiControlSequence::CursorForward:            // default 1
        case AnsiControlSequence::CursorBackward:           // default 1
        case AnsiControlSequence::CursorNextLine:           // default 1
        case AnsiControlSequence::CursorPreviousLine:       // default 1
        case AnsiControlSequence::CursorHorizontalAbsolute: // default 1
        case AnsiControlSequence::ScrollUp:                 // default 1
        case AnsiControlSequence::ScrollDown:               // default 1
        //   AnsiControlSequence::AuxOff
        case AnsiControlSequence::AuxOn:              // NA
        case AnsiControlSequence::DeviceStatusReport: // NA
        {
            // CSI <number> <Terminator>
            auto sv = buffer.substr(0, i);
            long n = wxStringViewAtol(sv, 1 /* default value */);
            value->value = n;
            value->seq = (AnsiControlSequence)buffer[i];
            // return the remainder string
            return buffer.substr(i + 1);
        } break;
        case AnsiControlSequence::EraseInDisplay: // default 0
        case AnsiControlSequence::EraseInLine:    // default 0
        case AnsiControlSequence::EraseCharacter: // default 0
        {
            // CSI <number> <Terminator>
            auto sv = buffer.substr(0, i);
            long n = wxStringViewAtol(sv, 0 /* default value */);
            value->value = n;
            value->seq = (AnsiControlSequence)buffer[i];
            // return the remainder string
            return buffer.substr(i + 1);
        } break;
        case AnsiControlSequence::CursorPosition:
        case AnsiControlSequence::HorizontalVerticalPosition: {
            // CSI<number>;<number><Terminator>
            auto nm = parse_cols_rows(wxStringView(buffer.data(), i), 1);
            value->value = nm;
            value->seq = (AnsiControlSequence)buffer[i];
            return buffer.substr(i + 1);
        } break;
        case AnsiControlSequence::ActionEnable:
        case AnsiControlSequence::ActionDisable:
        case AnsiControlSequence::SelectGraphicRendition: {
            // Select Graphic Rendition. CSI n m. Sets colors and style of the characters
            // following this code
            value->value = wxString(buffer.data(), i);
            value->seq = (AnsiControlSequence)buffer[i];
            return buffer.substr(i + 1);
        } break;
        }
    }
    return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
}

} // namespace

wxTerminalAnsiEscapeHandler::wxTerminalAnsiEscapeHandler() { initialise_colours(); }

wxTerminalAnsiEscapeHandler::~wxTerminalAnsiEscapeHandler() {}

size_t wxTerminalAnsiEscapeHandler::ProcessBuffer(wxStringView input, wxTerminalAnsiRendererInterface* renderer)
{
    wxStringView sv = input;
    size_t consumed = 0;
    while(!sv.empty()) {
        switch(ansi_sequence_type(sv)) {
        case AnsiSequenceType::NEED_MORE_DATA:
            // found the ESC but not enough in the buffer to parse the rest
            // keep the ESC and request more data
            return input.length() - sv.length();

        case AnsiSequenceType::UNKNOWN_SEQUENCE_TYPE:
            // bug in the protocol, remove the 2 chars that we parsed
            sv.remove_prefix(2);
            break;

        case AnsiSequenceType::NOT_ESCAPE: {
            // no ansi escape found
            size_t len = find_control_code(sv, AnsiControlCode::ESC);
            if(len != wxStringView::npos) {
                wxStringView str = sv.substr(0, len);
                render_string(str, renderer);
                sv.remove_prefix(len);
            } else {
                // add the entire sv
                render_string(sv, renderer);
                sv = {};
            }
        } break;
        case AnsiSequenceType::CSI: {
            wxStringView buf = sv.substr(2);
            auto res = handle_csi(buf, renderer);
            if(!res) {
                switch(res.error()) {
                case wxHandleError::kProtocolError:
                    // recover by skipping these 2 chars
                    sv.remove_prefix(2);
                    break;
                case wxHandleError::kNeedMoreData:
                    return input.length() - sv.length();
                default:
                    break;
                }
            } else {
                // success, update the string view
                sv = res.success();
            }
        } break;
        case AnsiSequenceType::SS2:
        case AnsiSequenceType::SS3:
            // skip these sequences
            sv.remove_prefix(2);
            break;
        case AnsiSequenceType::DCS:
        case AnsiSequenceType::SOS:
        case AnsiSequenceType::PM:
        case AnsiSequenceType::APC: {
            // read until we find the ST, if we can't find it
            // return to the caller without consuming the bytes
            wxStringView buf{ sv };
            auto res = loop_until_st(buf);
            if(res) {
                sv = res.success();
            } else {
                return input.length() - sv.length();
            }
        } break;
        case AnsiSequenceType::OSC: {
            wxStringView buf = sv.substr(2);
            auto res = handle_osc(buf, renderer);
            if(!res) {
                switch(res.error()) {
                case wxHandleError::kProtocolError:
                    // recover
                    sv.remove_prefix(2);
                    break;
                case wxHandleError::kNeedMoreData:
                    return input.length() - sv.length();
                default:
                    break;
                }
            } else {
                // success, update the string view
                sv = res.success();
            }
        } break;
        case AnsiSequenceType::ST:
            // we shouldn't find this here, it should be handled as part of the loop_until_st() method called
            // from the other cases
            sv.remove_prefix(2);
            break;
        }
    }
    return input.length();
}

wxHandlResultStringView wxTerminalAnsiEscapeHandler::loop_until_st(wxStringView sv)
{
    size_t len = 0;
    size_t pos = find_st(sv, &len);
    if(pos == wxStringView::npos) {
        return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
    }
    return sv.substr(pos + len);
}

wxHandlResultStringView wxTerminalAnsiEscapeHandler::handle_osc(wxStringView sv,
                                                                wxTerminalAnsiRendererInterface* renderer)
{
    if(sv.empty()) {
        return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
    }

    // ESC ]0;this is the window title <BEL>
    // ESC ]2;this is the window title <BEL>
    // ESC ]0;this is the window title <ST>
    // ESC ]2;this is the window title <ST>
    // ESC ]8;;link <ST>
    wxChar ch = sv[0];
    sv.remove_prefix(1); // remove the `0` | `8`
    switch(ch) {
    case '2': // Windows also allows `2` for the setting the title
    case '0': {
        size_t pos = 0;
        // expecting ';'
        pos = find_control_code(sv, ';');
        if(pos == wxStringView::npos) {
            // incomplete buffer
            return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
        }

        sv.remove_prefix(pos + 1); // remove, including the ;
        size_t st_len;
        pos = find_st(sv, &st_len);
        if(pos == wxStringView::npos) {
            // incomplete buffer
            return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
        }

        wxStringView window_title = sv.substr(0, pos);
        renderer->SetWindowTitle(window_title);
        sv.remove_prefix(pos + st_len); // removing everything, including the terminator
        return sv;
    } break;
    case '8': {
        size_t pos = find_control_code(sv, '\a' /* BEL */);
        if(pos == wxStringView::npos) {
            // incomplete buffer
            return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
        }
        sv.remove_prefix(pos + 1); // remove, including the BEL
        return sv;
    } break;
    default:
        return wxHandlResultStringView::make_error(wxHandleError::kProtocolError);
    }
}

wxHandlResultStringView wxTerminalAnsiEscapeHandler::handle_csi(wxStringView sv,
                                                                wxTerminalAnsiRendererInterface* renderer)
{
    AnsiControlSequenceValue value;
    auto res = ansi_control_sequence(sv, &value);
    if(!res) {
        return res;
    }

    switch(value.seq) {
    case AnsiControlSequence::UnknownControlSequence:
    case AnsiControlSequence::ScrollUp:
    case AnsiControlSequence::ScrollDown:
    case AnsiControlSequence::AuxOn: // + AuxOff
    case AnsiControlSequence::DeviceStatusReport:
    default:
        break;
    case AnsiControlSequence::CursorBackward:
        // Move caret backward
        renderer->MoveCaret(value.value_long(), wxLEFT);
        break;
    case AnsiControlSequence::CursorPreviousLine:
    case AnsiControlSequence::CursorUp:
        // Move caret backward
        renderer->MoveCaret(value.value_long(), wxUP);
        break;
    case AnsiControlSequence::CursorNextLine:
    case AnsiControlSequence::CursorDown:
        // Move caret backward
        renderer->MoveCaret(value.value_long(), wxDOWN);
        break;
    case AnsiControlSequence::CursorForward:
        // Move caret backward
        renderer->MoveCaret(value.value_long(), wxRIGHT);
        break;
    case AnsiControlSequence::CursorHorizontalAbsolute:
        // Move caret backward
        renderer->SetCaretX(value.value_long());
        break;
    case AnsiControlSequence::EraseInLine:
        // Move caret backward
        switch(value.value_long()) {
        case 0:
            // for caret -> end of line
            renderer->ClearLine(wxRIGHT);
            break;
        case 1:
            // from 0 -> caret
            renderer->ClearLine(wxLEFT);
            break;
        case 2:
        default:
            // entire line
            renderer->ClearLine(wxLEFT | wxRIGHT);
            break;
        }
        break;
    case AnsiControlSequence::EraseCharacter:
        renderer->EraseCharacter(value.value_long());
        break;
    case AnsiControlSequence::EraseInDisplay:
        // Move caret backward
        switch(value.value_long()) {
        case 0:
            // for caret -> end of screen
            renderer->ClearDisplay(wxDOWN);
            break;
        case 1:
            // from 0 -> caret
            renderer->ClearDisplay(wxUP);
            break;
        case 2:
        default:
            // entire display
            renderer->ClearDisplay(wxUP | wxDOWN);
            break;
        }
        break;
    case AnsiControlSequence::HorizontalVerticalPosition:
    case AnsiControlSequence::CursorPosition: {
        // Move caret backward
        auto pos = value.value_cols_rows();
        renderer->SetCaretX(pos.n);
        renderer->SetCaretY(pos.m);
    } break;
    case AnsiControlSequence::SelectGraphicRendition: {
        wxString s = value.value_string();
        wxStringView sv{ s.wc_str(), s.length() };
        handle_sgr(sv, renderer);
    } break;
    }

    // return the SV byeond the CSI area
    return res;
}

namespace
{
/// return the view before the first occurrence of ch.
/// return the whole string if ch is not found
wxHandlResultStringView before_first(wxStringView sv, wxChar ch)
{
    size_t pos = 0;
    for(; pos < sv.length(); ++pos) {
        if(sv[pos] == ch) {
            return sv.substr(0, pos);
        }
    }
    return wxHandlResultStringView::make_error(wxHandleError::kNotFound);
}
} // namespace

void wxTerminalAnsiEscapeHandler::handle_sgr(wxStringView sv, wxTerminalAnsiRendererInterface* renderer)
{
    LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "SGR:" << wxString(sv.data(), sv.length()) << endl; }

    if(sv.empty()) {
        // handle like reset
        renderer->ResetStyle();
        return;
    }

    std::vector<long> props;
    props.reserve(10);
    while(!sv.empty()) {
        auto res = before_first(sv, ';');
        if(res) {
            wxStringView match = res.success();
            long val = wxStringViewAtol(match, wxNOT_FOUND);
            if(val != wxNOT_FOUND) {
                props.push_back(val);
            }
            // delete the matched length + the ';'
            sv.remove_prefix(match.length() + 1);
        } else {
            // take the entire string
            wxStringView match = sv;
            sv = {};
            long val = wxStringViewAtol(match, wxNOT_FOUND);
            if(val != wxNOT_FOUND) {
                props.push_back(val);
            }
        }
    }

    if(props.empty()) {
        renderer->ResetStyle();
        return;
    }

    // ESC[...;38;2;<r>;<g>;<b>m Select RGB foreground color
    // ESC[...;38;5;<n>m Select foreground color
    // ESC[...;48;2;<r>;<g>;<b>m Select RGB background color
    // ESC[...;48;5;<n>m Select background color
    // ESC[...;0m Select RGB background color
    // where n is a number from the colours table

    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_SET_FG = 1;
    constexpr int STATE_SET_BG = 2;

#define NEXT_NUMBER(index) (index < props.size() ? props[index] : wxNOT_FOUND)
#define BETWEEN(a, b) (num >= a && num <= b)

    int state = STATE_NORMAL;
    for(size_t i = 0; i < props.size(); ++i) {
        long curnum = props[i];
        switch(state) {
        STATE_SET_FG:
            switch(curnum) {
            case 5: { // number from the list
                int num = NEXT_NUMBER(i + 1);
                if(num != wxNOT_FOUND) {
                    ++i;
                    renderer->SetTextColour(find_colour_by_number(pColours, num));
                }
            } break;
            case 2: { // r,g,b format
                wxColour::ChannelType r = NEXT_NUMBER(i + 1);
                wxColour::ChannelType g = NEXT_NUMBER(i + 2);
                wxColour::ChannelType b = NEXT_NUMBER(i + 3);
                wxColour col{ r, g, b };
                renderer->SetTextColour(col);
            } break;
            default:
                state = STATE_NORMAL;
                break;
            }
            break;
        STATE_SET_BG:
            switch(curnum) {
            case 5: { // number from the list
                int num = NEXT_NUMBER(i + 1);
                if(num != wxNOT_FOUND) {
                    ++i;
                    renderer->SetTextBgColour(find_colour_by_number(pColours, num));
                }
            } break;
            case 2: { // r,g,b format
                wxColour::ChannelType r = NEXT_NUMBER(i + 1);
                wxColour::ChannelType g = NEXT_NUMBER(i + 2);
                wxColour::ChannelType b = NEXT_NUMBER(i + 3);
                wxColour col{ r, g, b };
                renderer->SetTextBgColour(col);
            } break;
            default:
                state = STATE_NORMAL;
                break;
            }
            break;
        default:
        STATE_NORMAL:
            switch(curnum) {
            case 0:
                renderer->ResetStyle();
                break;
            case 30:
            case 31:
            case 32:
            case 33:
            case 34:
            case 35:
            case 36:
            case 37:
            case 90:
            case 91:
            case 92:
            case 93:
            case 94:
            case 95:
            case 96:
            case 97:
                pColours = renderer->IsUseDarkThemeColours() ? &DarkThemeColours : &LightThemeColours;
                renderer->SetTextColour(find_colour_by_number(pColours, curnum));
                break;
            case 38:
                state = STATE_SET_FG;
                pColours = renderer->IsUseDarkThemeColours() ? &DarkThemeColours8Bit : &LightThemeColours8Bit;
                break;
            case 39:
                // Default foreground color
                renderer->ResetStyle();
                break;
            case 40:
            case 41:
            case 42:
            case 43:
            case 44:
            case 45:
            case 46:
            case 47:
            case 100:
            case 101:
            case 102:
            case 103:
            case 104:
            case 105:
            case 106:
            case 107:
                pColours = renderer->IsUseDarkThemeColours() ? &DarkThemeColours : &LightThemeColours;
                renderer->SetTextColour(find_colour_by_number(pColours, curnum));
                break;
            case 48:
                state = STATE_SET_BG;
                pColours = renderer->IsUseDarkThemeColours() ? &DarkThemeColours8Bit : &LightThemeColours8Bit;
                break;
            case 49:
                // Default background color
                renderer->ResetStyle();
                break;
            }
            break;
        }
    }
#undef BETWEEN
#undef NEXT_NUMBER
}

const wxColour& wxTerminalAnsiEscapeHandler::GetColour(int colour_number)
{
    return find_colour_by_number(pColours, colour_number);
}