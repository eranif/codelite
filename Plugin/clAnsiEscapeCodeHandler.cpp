#include "clAnsiEscapeCodeHandler.hpp"

#include "ColoursAndFontsManager.h"
#include "drawingutils.h"
#include "file_logger.h"

#include <wx/tokenzr.h>

namespace
{
constexpr int X_MARGIN = 5;
}

clAnsiEscapeCodeHandler::clAnsiEscapeCodeHandler()
{
    // Text colours
    m_colours_normal.insert({ 30, wxColour(1, 1, 1) });
    m_colours_normal.insert({ 31, wxColour(222, 56, 43) });
    m_colours_normal.insert({ 32, wxColour(57, 181, 74) });
    m_colours_normal.insert({ 33, wxColour(255, 199, 6) });
    m_colours_normal.insert({ 34, wxColour(0, 111, 184) });
    m_colours_normal.insert({ 35, wxColour(118, 38, 113) });
    m_colours_normal.insert({ 36, wxColour(44, 181, 233) });
    m_colours_normal.insert({ 37, wxColour(204, 204, 204) });
    m_colours_normal.insert({ 90, wxColour(128, 128, 128) });
    m_colours_normal.insert({ 91, wxColour(255, 0, 0) });
    m_colours_normal.insert({ 92, wxColour(0, 255, 0) });
    m_colours_normal.insert({ 93, wxColour(255, 255, 0) });
    m_colours_normal.insert({ 94, wxColour(0, 0, 255) });
    m_colours_normal.insert({ 95, wxColour(255, 0, 255) });
    m_colours_normal.insert({ 96, wxColour(0, 255, 255) });
    m_colours_normal.insert({ 97, wxColour(255, 255, 255) });

    // Background colours
    m_colours_normal.insert({ 40, wxColour(1, 1, 1) });
    m_colours_normal.insert({ 41, wxColour(222, 56, 43) });
    m_colours_normal.insert({ 42, wxColour(57, 181, 74) });
    m_colours_normal.insert({ 43, wxColour(255, 199, 6) });
    m_colours_normal.insert({ 44, wxColour(0, 111, 184) });
    m_colours_normal.insert({ 45, wxColour(118, 38, 113) });
    m_colours_normal.insert({ 46, wxColour(44, 181, 233) });
    m_colours_normal.insert({ 47, wxColour(204, 204, 204) });
    m_colours_normal.insert({ 100, wxColour(128, 128, 128) });
    m_colours_normal.insert({ 101, wxColour(255, 0, 0) });
    m_colours_normal.insert({ 102, wxColour(0, 255, 0) });
    m_colours_normal.insert({ 103, wxColour(255, 255, 0) });
    m_colours_normal.insert({ 104, wxColour(0, 0, 255) });
    m_colours_normal.insert({ 105, wxColour(255, 0, 255) });
    m_colours_normal.insert({ 106, wxColour(0, 255, 255) });
    m_colours_normal.insert({ 107, wxColour(255, 255, 255) });

    // 8 bit colours
    // ESC[35;5;<fg colour>
    // ESC[48;5;<bg colour>
    m_8_bit_colours_normal.insert({ 0, wxColour("#000000") });
    m_8_bit_colours_normal.insert({ 1, wxColour("#800000") });
    m_8_bit_colours_normal.insert({ 2, wxColour("#008000") });
    m_8_bit_colours_normal.insert({ 3, wxColour("#808000") });
    m_8_bit_colours_normal.insert({ 4, wxColour("#000080") });
    m_8_bit_colours_normal.insert({ 5, wxColour("#800080") });
    m_8_bit_colours_normal.insert({ 6, wxColour("#008080") });
    m_8_bit_colours_normal.insert({ 7, wxColour("#c0c0c0") });
    m_8_bit_colours_normal.insert({ 8, wxColour("#808080") });
    m_8_bit_colours_normal.insert({ 9, wxColour("#ff0000") });
    m_8_bit_colours_normal.insert({ 10, wxColour("#00ff00") });
    m_8_bit_colours_normal.insert({ 11, wxColour("#ffff00") });
    m_8_bit_colours_normal.insert({ 12, wxColour("#0000ff") });
    m_8_bit_colours_normal.insert({ 13, wxColour("#ff00ff") });
    m_8_bit_colours_normal.insert({ 14, wxColour("#00ffff") });
    m_8_bit_colours_normal.insert({ 15, wxColour("#ffffff") });
    m_8_bit_colours_normal.insert({ 16, wxColour("#000000") });
    m_8_bit_colours_normal.insert({ 17, wxColour("#00005f") });
    m_8_bit_colours_normal.insert({ 18, wxColour("#000087") });
    m_8_bit_colours_normal.insert({ 19, wxColour("#0000af") });
    m_8_bit_colours_normal.insert({ 20, wxColour("#0000d7") });
    m_8_bit_colours_normal.insert({ 21, wxColour("#0000ff") });
    m_8_bit_colours_normal.insert({ 22, wxColour("#005f00") });
    m_8_bit_colours_normal.insert({ 23, wxColour("#005f5f") });
    m_8_bit_colours_normal.insert({ 24, wxColour("#005f87") });
    m_8_bit_colours_normal.insert({ 25, wxColour("#005faf") });
    m_8_bit_colours_normal.insert({ 26, wxColour("#005fd7") });
    m_8_bit_colours_normal.insert({ 27, wxColour("#005fff") });
    m_8_bit_colours_normal.insert({ 28, wxColour("#008700") });
    m_8_bit_colours_normal.insert({ 29, wxColour("#00875f") });
    m_8_bit_colours_normal.insert({ 30, wxColour("#008787") });
    m_8_bit_colours_normal.insert({ 31, wxColour("#0087af") });
    m_8_bit_colours_normal.insert({ 32, wxColour("#0087d7") });
    m_8_bit_colours_normal.insert({ 33, wxColour("#0087ff") });
    m_8_bit_colours_normal.insert({ 34, wxColour("#00af00") });
    m_8_bit_colours_normal.insert({ 35, wxColour("#00af5f") });
    m_8_bit_colours_normal.insert({ 36, wxColour("#00af87") });
    m_8_bit_colours_normal.insert({ 37, wxColour("#00afaf") });
    m_8_bit_colours_normal.insert({ 38, wxColour("#00afd7") });
    m_8_bit_colours_normal.insert({ 39, wxColour("#00afff") });
    m_8_bit_colours_normal.insert({ 40, wxColour("#00d700") });
    m_8_bit_colours_normal.insert({ 41, wxColour("#00d75f") });
    m_8_bit_colours_normal.insert({ 42, wxColour("#00d787") });
    m_8_bit_colours_normal.insert({ 43, wxColour("#00d7af") });
    m_8_bit_colours_normal.insert({ 44, wxColour("#00d7d7") });
    m_8_bit_colours_normal.insert({ 45, wxColour("#00d7ff") });
    m_8_bit_colours_normal.insert({ 46, wxColour("#00ff00") });
    m_8_bit_colours_normal.insert({ 47, wxColour("#00ff5f") });
    m_8_bit_colours_normal.insert({ 48, wxColour("#00ff87") });
    m_8_bit_colours_normal.insert({ 49, wxColour("#00ffaf") });
    m_8_bit_colours_normal.insert({ 50, wxColour("#00ffd7") });
    m_8_bit_colours_normal.insert({ 51, wxColour("#00ffff") });
    m_8_bit_colours_normal.insert({ 52, wxColour("#5f0000") });
    m_8_bit_colours_normal.insert({ 53, wxColour("#5f005f") });
    m_8_bit_colours_normal.insert({ 54, wxColour("#5f0087") });
    m_8_bit_colours_normal.insert({ 55, wxColour("#5f00af") });
    m_8_bit_colours_normal.insert({ 56, wxColour("#5f00d7") });
    m_8_bit_colours_normal.insert({ 57, wxColour("#5f00ff") });
    m_8_bit_colours_normal.insert({ 58, wxColour("#5f5f00") });
    m_8_bit_colours_normal.insert({ 59, wxColour("#5f5f5f") });
    m_8_bit_colours_normal.insert({ 60, wxColour("#5f5f87") });
    m_8_bit_colours_normal.insert({ 61, wxColour("#5f5faf") });
    m_8_bit_colours_normal.insert({ 62, wxColour("#5f5fd7") });
    m_8_bit_colours_normal.insert({ 63, wxColour("#5f5fff") });
    m_8_bit_colours_normal.insert({ 64, wxColour("#5f8700") });
    m_8_bit_colours_normal.insert({ 65, wxColour("#5f875f") });
    m_8_bit_colours_normal.insert({ 66, wxColour("#5f8787") });
    m_8_bit_colours_normal.insert({ 67, wxColour("#5f87af") });
    m_8_bit_colours_normal.insert({ 68, wxColour("#5f87d7") });
    m_8_bit_colours_normal.insert({ 69, wxColour("#5f87ff") });
    m_8_bit_colours_normal.insert({ 70, wxColour("#5faf00") });
    m_8_bit_colours_normal.insert({ 71, wxColour("#5faf5f") });
    m_8_bit_colours_normal.insert({ 72, wxColour("#5faf87") });
    m_8_bit_colours_normal.insert({ 73, wxColour("#5fafaf") });
    m_8_bit_colours_normal.insert({ 74, wxColour("#5fafd7") });
    m_8_bit_colours_normal.insert({ 75, wxColour("#5fafff") });
    m_8_bit_colours_normal.insert({ 76, wxColour("#5fd700") });
    m_8_bit_colours_normal.insert({ 77, wxColour("#5fd75f") });
    m_8_bit_colours_normal.insert({ 78, wxColour("#5fd787") });
    m_8_bit_colours_normal.insert({ 79, wxColour("#5fd7af") });
    m_8_bit_colours_normal.insert({ 80, wxColour("#5fd7d7") });
    m_8_bit_colours_normal.insert({ 81, wxColour("#5fd7ff") });
    m_8_bit_colours_normal.insert({ 82, wxColour("#5fff00") });
    m_8_bit_colours_normal.insert({ 83, wxColour("#5fff5f") });
    m_8_bit_colours_normal.insert({ 84, wxColour("#5fff87") });
    m_8_bit_colours_normal.insert({ 85, wxColour("#5fffaf") });
    m_8_bit_colours_normal.insert({ 86, wxColour("#5fffd7") });
    m_8_bit_colours_normal.insert({ 87, wxColour("#5fffff") });
    m_8_bit_colours_normal.insert({ 88, wxColour("#870000") });
    m_8_bit_colours_normal.insert({ 89, wxColour("#87005f") });
    m_8_bit_colours_normal.insert({ 90, wxColour("#870087") });
    m_8_bit_colours_normal.insert({ 91, wxColour("#8700af") });
    m_8_bit_colours_normal.insert({ 92, wxColour("#8700d7") });
    m_8_bit_colours_normal.insert({ 93, wxColour("#8700ff") });
    m_8_bit_colours_normal.insert({ 94, wxColour("#875f00") });
    m_8_bit_colours_normal.insert({ 95, wxColour("#875f5f") });
    m_8_bit_colours_normal.insert({ 96, wxColour("#875f87") });
    m_8_bit_colours_normal.insert({ 97, wxColour("#875faf") });
    m_8_bit_colours_normal.insert({ 98, wxColour("#875fd7") });
    m_8_bit_colours_normal.insert({ 99, wxColour("#875fff") });
    m_8_bit_colours_normal.insert({ 100, wxColour("#878700") });
    m_8_bit_colours_normal.insert({ 101, wxColour("#87875f") });
    m_8_bit_colours_normal.insert({ 102, wxColour("#878787") });
    m_8_bit_colours_normal.insert({ 103, wxColour("#8787af") });
    m_8_bit_colours_normal.insert({ 104, wxColour("#8787d7") });
    m_8_bit_colours_normal.insert({ 105, wxColour("#8787ff") });
    m_8_bit_colours_normal.insert({ 106, wxColour("#87af00") });
    m_8_bit_colours_normal.insert({ 107, wxColour("#87af5f") });
    m_8_bit_colours_normal.insert({ 108, wxColour("#87af87") });
    m_8_bit_colours_normal.insert({ 109, wxColour("#87afaf") });
    m_8_bit_colours_normal.insert({ 110, wxColour("#87afd7") });
    m_8_bit_colours_normal.insert({ 111, wxColour("#87afff") });
    m_8_bit_colours_normal.insert({ 112, wxColour("#87d700") });
    m_8_bit_colours_normal.insert({ 113, wxColour("#87d75f") });
    m_8_bit_colours_normal.insert({ 114, wxColour("#87d787") });
    m_8_bit_colours_normal.insert({ 115, wxColour("#87d7af") });
    m_8_bit_colours_normal.insert({ 116, wxColour("#87d7d7") });
    m_8_bit_colours_normal.insert({ 117, wxColour("#87d7ff") });
    m_8_bit_colours_normal.insert({ 118, wxColour("#87ff00") });
    m_8_bit_colours_normal.insert({ 119, wxColour("#87ff5f") });
    m_8_bit_colours_normal.insert({ 120, wxColour("#87ff87") });
    m_8_bit_colours_normal.insert({ 121, wxColour("#87ffaf") });
    m_8_bit_colours_normal.insert({ 122, wxColour("#87ffd7") });
    m_8_bit_colours_normal.insert({ 123, wxColour("#87ffff") });
    m_8_bit_colours_normal.insert({ 124, wxColour("#af0000") });
    m_8_bit_colours_normal.insert({ 125, wxColour("#af005f") });
    m_8_bit_colours_normal.insert({ 126, wxColour("#af0087") });
    m_8_bit_colours_normal.insert({ 127, wxColour("#af00af") });
    m_8_bit_colours_normal.insert({ 128, wxColour("#af00d7") });
    m_8_bit_colours_normal.insert({ 129, wxColour("#af00ff") });
    m_8_bit_colours_normal.insert({ 130, wxColour("#af5f00") });
    m_8_bit_colours_normal.insert({ 131, wxColour("#af5f5f") });
    m_8_bit_colours_normal.insert({ 132, wxColour("#af5f87") });
    m_8_bit_colours_normal.insert({ 133, wxColour("#af5faf") });
    m_8_bit_colours_normal.insert({ 134, wxColour("#af5fd7") });
    m_8_bit_colours_normal.insert({ 135, wxColour("#af5fff") });
    m_8_bit_colours_normal.insert({ 136, wxColour("#af8700") });
    m_8_bit_colours_normal.insert({ 137, wxColour("#af875f") });
    m_8_bit_colours_normal.insert({ 138, wxColour("#af8787") });
    m_8_bit_colours_normal.insert({ 139, wxColour("#af87af") });
    m_8_bit_colours_normal.insert({ 140, wxColour("#af87d7") });
    m_8_bit_colours_normal.insert({ 141, wxColour("#af87ff") });
    m_8_bit_colours_normal.insert({ 142, wxColour("#afaf00") });
    m_8_bit_colours_normal.insert({ 143, wxColour("#afaf5f") });
    m_8_bit_colours_normal.insert({ 144, wxColour("#afaf87") });
    m_8_bit_colours_normal.insert({ 145, wxColour("#afafaf") });
    m_8_bit_colours_normal.insert({ 146, wxColour("#afafd7") });
    m_8_bit_colours_normal.insert({ 147, wxColour("#afafff") });
    m_8_bit_colours_normal.insert({ 148, wxColour("#afd700") });
    m_8_bit_colours_normal.insert({ 149, wxColour("#afd75f") });
    m_8_bit_colours_normal.insert({ 150, wxColour("#afd787") });
    m_8_bit_colours_normal.insert({ 151, wxColour("#afd7af") });
    m_8_bit_colours_normal.insert({ 152, wxColour("#afd7d7") });
    m_8_bit_colours_normal.insert({ 153, wxColour("#afd7ff") });
    m_8_bit_colours_normal.insert({ 154, wxColour("#afff00") });
    m_8_bit_colours_normal.insert({ 155, wxColour("#afff5f") });
    m_8_bit_colours_normal.insert({ 156, wxColour("#afff87") });
    m_8_bit_colours_normal.insert({ 157, wxColour("#afffaf") });
    m_8_bit_colours_normal.insert({ 158, wxColour("#afffd7") });
    m_8_bit_colours_normal.insert({ 159, wxColour("#afffff") });
    m_8_bit_colours_normal.insert({ 160, wxColour("#d70000") });
    m_8_bit_colours_normal.insert({ 161, wxColour("#d7005f") });
    m_8_bit_colours_normal.insert({ 162, wxColour("#d70087") });
    m_8_bit_colours_normal.insert({ 163, wxColour("#d700af") });
    m_8_bit_colours_normal.insert({ 164, wxColour("#d700d7") });
    m_8_bit_colours_normal.insert({ 165, wxColour("#d700ff") });
    m_8_bit_colours_normal.insert({ 166, wxColour("#d75f00") });
    m_8_bit_colours_normal.insert({ 167, wxColour("#d75f5f") });
    m_8_bit_colours_normal.insert({ 168, wxColour("#d75f87") });
    m_8_bit_colours_normal.insert({ 169, wxColour("#d75faf") });
    m_8_bit_colours_normal.insert({ 170, wxColour("#d75fd7") });
    m_8_bit_colours_normal.insert({ 171, wxColour("#d75fff") });
    m_8_bit_colours_normal.insert({ 172, wxColour("#d78700") });
    m_8_bit_colours_normal.insert({ 173, wxColour("#d7875f") });
    m_8_bit_colours_normal.insert({ 174, wxColour("#d78787") });
    m_8_bit_colours_normal.insert({ 175, wxColour("#d787af") });
    m_8_bit_colours_normal.insert({ 176, wxColour("#d787d7") });
    m_8_bit_colours_normal.insert({ 177, wxColour("#d787ff") });
    m_8_bit_colours_normal.insert({ 178, wxColour("#d7af00") });
    m_8_bit_colours_normal.insert({ 179, wxColour("#d7af5f") });
    m_8_bit_colours_normal.insert({ 180, wxColour("#d7af87") });
    m_8_bit_colours_normal.insert({ 181, wxColour("#d7afaf") });
    m_8_bit_colours_normal.insert({ 182, wxColour("#d7afd7") });
    m_8_bit_colours_normal.insert({ 183, wxColour("#d7afff") });
    m_8_bit_colours_normal.insert({ 184, wxColour("#d7d700") });
    m_8_bit_colours_normal.insert({ 185, wxColour("#d7d75f") });
    m_8_bit_colours_normal.insert({ 186, wxColour("#d7d787") });
    m_8_bit_colours_normal.insert({ 187, wxColour("#d7d7af") });
    m_8_bit_colours_normal.insert({ 188, wxColour("#d7d7d7") });
    m_8_bit_colours_normal.insert({ 189, wxColour("#d7d7ff") });
    m_8_bit_colours_normal.insert({ 190, wxColour("#d7ff00") });
    m_8_bit_colours_normal.insert({ 191, wxColour("#d7ff5f") });
    m_8_bit_colours_normal.insert({ 192, wxColour("#d7ff87") });
    m_8_bit_colours_normal.insert({ 193, wxColour("#d7ffaf") });
    m_8_bit_colours_normal.insert({ 194, wxColour("#d7ffd7") });
    m_8_bit_colours_normal.insert({ 195, wxColour("#d7ffff") });
    m_8_bit_colours_normal.insert({ 196, wxColour("#ff0000") });
    m_8_bit_colours_normal.insert({ 197, wxColour("#ff005f") });
    m_8_bit_colours_normal.insert({ 198, wxColour("#ff0087") });
    m_8_bit_colours_normal.insert({ 199, wxColour("#ff00af") });
    m_8_bit_colours_normal.insert({ 200, wxColour("#ff00d7") });
    m_8_bit_colours_normal.insert({ 201, wxColour("#ff00ff") });
    m_8_bit_colours_normal.insert({ 202, wxColour("#ff5f00") });
    m_8_bit_colours_normal.insert({ 203, wxColour("#ff5f5f") });
    m_8_bit_colours_normal.insert({ 204, wxColour("#ff5f87") });
    m_8_bit_colours_normal.insert({ 205, wxColour("#ff5faf") });
    m_8_bit_colours_normal.insert({ 206, wxColour("#ff5fd7") });
    m_8_bit_colours_normal.insert({ 207, wxColour("#ff5fff") });
    m_8_bit_colours_normal.insert({ 208, wxColour("#ff8700") });
    m_8_bit_colours_normal.insert({ 209, wxColour("#ff875f") });
    m_8_bit_colours_normal.insert({ 210, wxColour("#ff8787") });
    m_8_bit_colours_normal.insert({ 211, wxColour("#ff87af") });
    m_8_bit_colours_normal.insert({ 212, wxColour("#ff87d7") });
    m_8_bit_colours_normal.insert({ 213, wxColour("#ff87ff") });
    m_8_bit_colours_normal.insert({ 214, wxColour("#ffaf00") });
    m_8_bit_colours_normal.insert({ 215, wxColour("#ffaf5f") });
    m_8_bit_colours_normal.insert({ 216, wxColour("#ffaf87") });
    m_8_bit_colours_normal.insert({ 217, wxColour("#ffafaf") });
    m_8_bit_colours_normal.insert({ 218, wxColour("#ffafd7") });
    m_8_bit_colours_normal.insert({ 219, wxColour("#ffafff") });
    m_8_bit_colours_normal.insert({ 220, wxColour("#ffd700") });
    m_8_bit_colours_normal.insert({ 221, wxColour("#ffd75f") });
    m_8_bit_colours_normal.insert({ 222, wxColour("#ffd787") });
    m_8_bit_colours_normal.insert({ 223, wxColour("#ffd7af") });
    m_8_bit_colours_normal.insert({ 224, wxColour("#ffd7d7") });
    m_8_bit_colours_normal.insert({ 225, wxColour("#ffd7ff") });
    m_8_bit_colours_normal.insert({ 226, wxColour("#ffff00") });
    m_8_bit_colours_normal.insert({ 227, wxColour("#ffff5f") });
    m_8_bit_colours_normal.insert({ 228, wxColour("#ffff87") });
    m_8_bit_colours_normal.insert({ 229, wxColour("#ffffaf") });
    m_8_bit_colours_normal.insert({ 230, wxColour("#ffffd7") });
    m_8_bit_colours_normal.insert({ 231, wxColour("#ffffff") });
    m_8_bit_colours_normal.insert({ 232, wxColour("#080808") });
    m_8_bit_colours_normal.insert({ 233, wxColour("#121212") });
    m_8_bit_colours_normal.insert({ 234, wxColour("#1c1c1c") });
    m_8_bit_colours_normal.insert({ 235, wxColour("#262626") });
    m_8_bit_colours_normal.insert({ 236, wxColour("#303030") });
    m_8_bit_colours_normal.insert({ 237, wxColour("#3a3a3a") });
    m_8_bit_colours_normal.insert({ 238, wxColour("#444444") });
    m_8_bit_colours_normal.insert({ 239, wxColour("#4e4e4e") });
    m_8_bit_colours_normal.insert({ 240, wxColour("#585858") });
    m_8_bit_colours_normal.insert({ 241, wxColour("#626262") });
    m_8_bit_colours_normal.insert({ 242, wxColour("#6c6c6c") });
    m_8_bit_colours_normal.insert({ 243, wxColour("#767676") });
    m_8_bit_colours_normal.insert({ 244, wxColour("#808080") });
    m_8_bit_colours_normal.insert({ 245, wxColour("#8a8a8a") });
    m_8_bit_colours_normal.insert({ 246, wxColour("#949494") });
    m_8_bit_colours_normal.insert({ 247, wxColour("#9e9e9e") });
    m_8_bit_colours_normal.insert({ 248, wxColour("#a8a8a8") });
    m_8_bit_colours_normal.insert({ 249, wxColour("#b2b2b2") });
    m_8_bit_colours_normal.insert({ 250, wxColour("#bcbcbc") });
    m_8_bit_colours_normal.insert({ 251, wxColour("#c6c6c6") });
    m_8_bit_colours_normal.insert({ 252, wxColour("#d0d0d0") });
    m_8_bit_colours_normal.insert({ 253, wxColour("#dadada") });
    m_8_bit_colours_normal.insert({ 254, wxColour("#e4e4e4") });
    m_8_bit_colours_normal.insert({ 255, wxColour("#eeeeee") });

    m_8_bit_colours_for_dark_theme = m_8_bit_colours_normal;
    // lighten the colours a bit
    for(auto& [_, colour] : m_8_bit_colours_normal) {
        colour = colour.ChangeLightness(150);
    }

    // darken the colours for light theme
    for(auto& [_, colour] : m_8_bit_colours_normal) {
        colour = colour.ChangeLightness(80);
    }

    m_colours_for_dark_theme = m_colours_normal;

    // lighten the colours a bit
    for(auto& [_, colour] : m_colours_for_dark_theme) {
        colour = colour.ChangeLightness(150);
    }

    // darken the colours for light theme
    for(auto& [_, colour] : m_colours_normal) {
        colour = colour.ChangeLightness(80);
    }

    m_8_bit_colours = &m_8_bit_colours_normal;
    m_colours = &m_colours_normal;
}

clAnsiEscapeCodeHandler::~clAnsiEscapeCodeHandler() {}

#define NEXT(__is_eol)                               \
    chunk->is_eol = __is_eol;                        \
    if(chunk->is_style_reset || !chunk->d.empty()) { \
        chunk->is_completed = true;                  \
    }                                                \
    EnsureCurrent();                                 \
    chunk = &m_chunks.back().back();

namespace
{
wxChar look_ahead(const wxString& buffer, size_t curpos, size_t count)
{
    if((curpos + count) < buffer.length()) {
        return buffer[curpos + count];
    }
    return wxChar(0);
}

bool check_string_from_curpos_ahead(const wxString& buffer, size_t curpos, const wxString& str)
{
    for(size_t i = 0; i < str.length(); ++i) {
        if(curpos >= buffer.length() || buffer[curpos] != str[i]) {
            return false;
        }
        curpos += 1;
    }
    return true;
}
} // namespace

void clAnsiEscapeCodeHandler::Parse(const wxString& buffer)
{
    EnsureCurrent();
    eColourHandlerState kCR_prev_state = eColourHandlerState::kNormal;
    auto chunk = &m_chunks.back().back();
    for(size_t i = 0; i < buffer.length(); ++i) {
        wxChar ch = buffer[i];
        switch(m_state) {
        case eColourHandlerState::kCR:
            switch(ch) {
            case '\n':
                // "\r\n"
                --i;
                m_state = kCR_prev_state;
                break;
            default:
                // any other char, we delete this entire line
                chunk->d.clear();
                --i;
                m_state = kCR_prev_state;
                break;
            }
            break;
        case eColourHandlerState::kNormal:
            switch(ch) {
            case 0x1B: // ESC
                NEXT(false);
                // remove the text flag
                chunk->is_text = false;
                m_state = eColourHandlerState::kInEscape;
                break;
            case '\r':
                kCR_prev_state = m_state;
                m_state = eColourHandlerState::kCR;
                break;
            case '\n':
                NEXT(true);
                break;
            default:
                chunk->d.append(1, ch);
                break;
            }
            break;
        case eColourHandlerState::kInEscape:
            switch(ch) {
            // CSI - Control Sequence Introducer
            case '[':
                m_state = eColourHandlerState::kInCsi;
                break;
            case ']':
                m_state = eColourHandlerState::kInOsc;
                break;
            default:
                // do nothing
                break;
            }
            break;
        case eColourHandlerState::kInOsc:
            // ESC ]
            switch(ch) {
            case 0x07: // BELL
                // bell, leave the current state
                chunk->is_completed = true;
                NEXT(false);
                m_state = eColourHandlerState::kNormal;
                break;
            case '0':
            case '2':
                m_state = eColourHandlerState::kOSC_Title;
                break;
            case '8':
                m_state = eColourHandlerState::kOSC_Url;
                break;
            default:
                chunk->d.append(1, ch);
                break;
            }
            break;
        case eColourHandlerState::kOSC_Title: {
            // In this state we skip the first char (';')
            // and read everything until we find the BELL or EOF
            chunk->is_title = true;
            wxChar next_char = look_ahead(buffer, i, 1);
            while(next_char != 0x07 && next_char != 0) { // while NEXT is NOT BELL
                chunk->d.append(1, next_char);
                ++i;
                next_char = look_ahead(buffer, i, 1);
            }
            m_state = eColourHandlerState::kInOsc;
        } break;
        case eColourHandlerState::kOSC_Url: {
            // In this state we skip the first char (';')
            // and read everything until we find the BELL or EOF, but we don't collect it!
            wxChar next_char = look_ahead(buffer, i, 1);
            while(next_char != 0x07 && next_char != 0) { // while NEXT is NOT BELL
                ++i;
                next_char = look_ahead(buffer, i, 1);
            }
            m_state = eColourHandlerState::kInOsc;
        } break;
        case eColourHandlerState::kInCsi:
            // found ESC[
            switch(ch) {
            case 'K':
                // clear the current line
                chunk->d.clear();
                m_state = eColourHandlerState::kNormal;
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'J':
            case 'S':
            case 'T':
            case 'f':
            case 'i':
            case 'n':
            case 'u':
            case 's':
                // exit the CSI mode back to normal mode
                NEXT(false);
                m_state = eColourHandlerState::kNormal;
                break;
            case '?':
                // Some popular private sequences
                m_state = eColourHandlerState::kInPrivateSequence;
                break;
            case 'm':
                // update the style
                if(chunk->d.empty() || chunk->d == "0") {
                    chunk->is_style_reset = true;
                }
                NEXT(false);
                m_state = eColourHandlerState::kNormal;
                break;
            default:
                chunk->d.append(1, ch);
                break;
            }
            break;
        case eColourHandlerState::kInPrivateSequence:
            // https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_(Control_Sequence_Introducer)_sequences
            // (Some popular private sequences)
            switch(ch) {
            case '?':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                // consume [digits]
                break;
            case '\n':
                NEXT(true);
                m_state = eColourHandlerState::kNormal;
                break;
            case 'h':
            case 'l':
                NEXT(false);
                m_state = eColourHandlerState::kNormal;
                break;
            case '\r':
                *chunk = Chunk{};
                m_state = eColourHandlerState::kNormal;
                break;
            default:
                // get back to the prev state
                m_state = eColourHandlerState::kInCsi;
                break;
            }
            break;
        }
    }

    chunk->is_completed = true;
    // remove the last chunk if it is empty
    if(!m_chunks.empty() && m_chunks.back().back().is_empty()) {
        m_chunks.back().pop_back();
    }
}

void clAnsiEscapeCodeHandler::Reset()
{
    m_chunks.clear();
    m_windowTitle.clear();
    m_state = eColourHandlerState::kNormal;
}

void clAnsiEscapeCodeHandler::EnsureCurrent()
{
    if(m_chunks.empty() || m_chunks.back().back().is_eol) {
        // add new row
        m_chunks.emplace_back(Chunk::Vec_t{});
        // make sure we have at least 1 element in that row
        m_chunks.back().push_back(Chunk{});
    } else {
        // start new chunk for the current line
        m_chunks.back().push_back(Chunk{});
    }
}

void clAnsiEscapeCodeHandler::RenderNoStyle(wxDC& dc, const clRenderDefaultStyle& defaultStyle, int line,
                                            const wxRect& rect, bool isLightTheme)
{
    // find the line chunks
    if(line >= (int)m_chunks.size()) {
        return;
    }

    const auto& v = m_chunks[line];

    dc.SetFont(defaultStyle.font);
    int text_height = dc.GetTextExtent("Tp").GetHeight();

    wxRect tmpRect(0, 0, 20, text_height);
    tmpRect = tmpRect.CenterIn(rect, wxVERTICAL);
    dc.SetTextForeground(defaultStyle.fg_colour);
    int yy = rect.y;
    int xx = X_MARGIN;
    dc.SetClippingRegion(rect);
    for(const auto& chunk : v) {
        // ensure to restore the dont once we are done with this line
        wxDCFontChanger font_changer(dc);
        if(chunk.is_text) {
            // draw the text
            wxSize text_size = dc.GetTextExtent(chunk.d);
            dc.DrawText(chunk.d, xx, yy);
            xx += text_size.GetWidth();
        }

        // if this chunk was EOL, reset the style here
        if(chunk.is_eol) {
            defaultStyle.ResetDC(dc);
        }
    }
    dc.DestroyClippingRegion();
}

void clAnsiEscapeCodeHandler::Render(wxSTCStyleProvider* style_provider, bool isLightTheme)
{
    // find the line chunks
    if(m_chunks.empty()) {
        return;
    }

    wxStyledTextCtrl* stc = style_provider->m_ctrl;
    // Check if this is a dark theme
    if(isLightTheme) {
        // normal
        m_8_bit_colours = &m_8_bit_colours_normal;
        m_colours = &m_colours_normal;
    } else {
        // dark theme background
        m_8_bit_colours = &m_8_bit_colours_for_dark_theme;
        m_colours = &m_colours_for_dark_theme;
    }

    // render everything
    int curstyle = 0;
    for(const auto& v : m_chunks) {
        for(const auto& chunk : v) {
            // ensure to restore the dont once we are done with this line
            if(chunk.is_style_reset) {
                // reset the style
                curstyle = 0;
            } else if(chunk.is_text) {
                if(!chunk.d.empty()) {
                    // append and style the next text
                    int pos = stc->GetLength();
                    int curline = stc->LineFromPosition(pos);
                    clDEBUG() << "styling from line:" << curline << "with style:" << curstyle << endl;
                    stc->AppendText(chunk.d);
                    stc->StartStyling(pos);
                    stc->SetStyling(chunk.d.length(), curstyle);
                }
            } else if(chunk.is_title) {
                m_windowTitle = chunk.d;
            } else if(chunk.is_empty()) {
                // skip it
            } else {
                wxTextAttr result;
                UpdateStyle(chunk, style_provider->GetDefaultStyle(), &result);
                curstyle = style_provider->GetStyle(result.GetTextColour(), result.GetBackgroundColour());
            }

            // if this chunk was EOL, reset the style here
            if(chunk.is_eol) {
                stc->AppendText("\n");
            }
        }
    }
    m_chunks.clear();
}

void clAnsiEscapeCodeHandler::Render(wxTextCtrl* ctrl, const wxTextAttr& defaultStyle, bool isLightTheme)
{
    // find the line chunks
    if(m_chunks.empty()) {
        return;
    }

    // Check if this is a dark theme
    if(isLightTheme) {
        // normal
        m_8_bit_colours = &m_8_bit_colours_normal;
        m_colours = &m_colours_normal;
    } else {
        // dark theme background
        m_8_bit_colours = &m_8_bit_colours_for_dark_theme;
        m_colours = &m_colours_for_dark_theme;
    }

    // render everything
    for(const auto& v : m_chunks) {
        for(const auto& chunk : v) {
            // ensure to restore the dont once we are done with this line
            if(chunk.is_style_reset) {
                // reset the style
                ctrl->SetDefaultStyle(defaultStyle);
            } else if(chunk.is_text) {
                // draw the text
                // ctrl->SetInsertionPointEnd();
                ctrl->AppendText(chunk.d);

            } else if(chunk.is_title || chunk.is_empty()) {
                m_windowTitle = chunk.d;
            } else {
                wxTextAttr result;
                UpdateStyle(chunk, defaultStyle, &result);
                ctrl->SetDefaultStyle(result);
            }

            // if this chunk was EOL, reset the style here
            if(chunk.is_eol) {
                // ctrl->SetInsertionPointEnd();
                ctrl->AppendText("\n");
                ctrl->SetDefaultStyle(defaultStyle);
            }
        }
    }
    m_chunks.clear();
}

void clAnsiEscapeCodeHandler::Render(wxDC& dc, const clRenderDefaultStyle& defaultStyle, int line, const wxRect& rect,
                                     bool isLightTheme)
{
    // find the line chunks
    if(line >= (int)m_chunks.size()) {
        return;
    }
    // Check if this is a dark theme
    if(isLightTheme) {
        // normal
        m_8_bit_colours = &m_8_bit_colours_normal;
        m_colours = &m_colours_normal;
    } else {
        // dark theme background
        m_8_bit_colours = &m_8_bit_colours_for_dark_theme;
        m_colours = &m_colours_for_dark_theme;
    }

    const auto& v = m_chunks[line];

    dc.SetFont(defaultStyle.font);
    int text_height = dc.GetTextExtent("Tp").GetHeight();

    wxRect tmpRect(0, 0, 20, text_height);
    tmpRect = tmpRect.CenterIn(rect, wxVERTICAL);
    dc.SetTextForeground(defaultStyle.fg_colour);

    int yy = rect.y;
    int xx = X_MARGIN;
    dc.SetClippingRegion(rect);
    for(const auto& chunk : v) {
        // ensure to restore the dont once we are done with this line
        wxDCFontChanger font_changer(dc);
        if(chunk.is_style_reset) {
            // reset the style
            defaultStyle.ResetDC(dc);
        } else if(chunk.is_text) {
            // draw the text
            wxSize text_size = dc.GetTextExtent(chunk.d);
            dc.DrawText(chunk.d, xx, yy);
            xx += text_size.GetWidth();

        } else if(chunk.is_title || chunk.is_empty()) {
            m_windowTitle = chunk.d;
        } else {
            UpdateStyle(chunk, dc, defaultStyle);
        }

        // if this chunk was EOL, reset the style here
        if(chunk.is_eol) {
            defaultStyle.ResetDC(dc);
        }
    }
    dc.DestroyClippingRegion();
}

#define NEXT_ATTR(index, channel)                 \
    {                                             \
        long number_attr = wxNOT_FOUND;           \
        channel = 0;                              \
        if((index) < attrs.size()) {              \
            attrs[(index)].ToCLong(&number_attr); \
            channel = number_attr;                \
        }                                         \
    }

void clAnsiEscapeCodeHandler::UpdateStyle(const Chunk& chunk, const wxTextAttr& defaultStyle, wxTextAttr* updatedStyle)
{
    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_SET_FG = 1;
    constexpr int STATE_SET_BG = 2;

    // see: https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters
    *updatedStyle = defaultStyle;
    if(chunk.d == "0") {
        *updatedStyle = defaultStyle;
    } else {
        int state = STATE_NORMAL;
        wxArrayString attrs = ::wxStringTokenize(chunk.d, ";", wxTOKEN_RET_EMPTY);
        wxFont f = defaultStyle.GetFont();
        for(size_t i = 0; i < attrs.size(); ++i) {
            const wxString& attr = attrs[i];
            long number;
            if(!attr.ToCLong(&number)) {
                continue;
            }
            switch(state) {
            case STATE_NORMAL:
                switch(number) {
                case 0:
                    // reset attributes
                    *updatedStyle = defaultStyle;
                    break;
                case 1:
                    f.SetWeight(wxFONTWEIGHT_BOLD);
                    break;
                case 2:
                    f.SetWeight(wxFONTWEIGHT_LIGHT);
                    break;
                case 3:
                    f.SetStyle(wxFONTSTYLE_ITALIC);
                    break;
                case 4:
                    f.SetUnderlined(true);
                    break;
                case 38:
                    state = STATE_SET_FG;
                    break;
                case 48:
                    state = STATE_SET_BG;
                    break;
                default:
                    if((number >= 30 && number <= 37) || (number >= 90 && number <= 97)) {
                        // use colour table to set the text colour
                        wxColour c = GetColour(*m_colours, number);
                        if(c.IsOk()) {
                            updatedStyle->SetTextColour(c);
                        }
                    } else if((number >= 40 && number <= 47) || (number >= 100 && number <= 107)) {
                        wxColour c = GetColour(*m_colours, number);
                        if(c.IsOk()) {
                            updatedStyle->SetBackgroundColour(c);
                        }
                    }
                    break;
                }
                break;
            case STATE_SET_BG:
                switch(number) {
                case 5:
                    break;
                case 2: {
                    // ESC[38;2;R;G;B
                    wxColour::ChannelType r, g, b;
                    NEXT_ATTR(i + 1, r);
                    NEXT_ATTR(i + 2, g);
                    NEXT_ATTR(i + 3, b);
                    wxColour c{ r, g, b };
                    if(c.IsOk()) {
                        updatedStyle->SetTextColour(c);
                    }
                    state = STATE_NORMAL;
                } break;
                default: {
                    // use colour table to set the text colour
                    wxColour c = GetColour(*m_8_bit_colours, number);
                    if(c.IsOk()) {
                        updatedStyle->SetBackgroundColour(c);
                    }
                    state = STATE_NORMAL;
                } break;
                }
                break;
            case STATE_SET_FG:
                switch(number) {
                case 5:
                    break;
                case 2: {
                    // ESC[38;2;R;G;B
                    wxColour::ChannelType r, g, b;
                    NEXT_ATTR(i + 1, r);
                    NEXT_ATTR(i + 2, g);
                    NEXT_ATTR(i + 3, b);
                    wxColour c{ r, g, b };
                    if(c.IsOk()) {
                        updatedStyle->SetTextColour(c);
                    }
                    state = STATE_NORMAL;
                } break;
                default: {
                    // use colour table to set the text colour
                    wxColour c = GetColour(*m_8_bit_colours, number);
                    if(c.IsOk()) {
                        updatedStyle->SetTextColour(c);
                    }
                    state = STATE_NORMAL;
                } break;
                }
                break;
            }
        }
        updatedStyle->SetFont(f);
    }
}

void clAnsiEscapeCodeHandler::UpdateStyle(const Chunk& chunk, wxDC& dc, const clRenderDefaultStyle& defaultStyle)
{
    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_SET_FG = 1;
    constexpr int STATE_SET_BG = 2;

    // see: https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters
    if(chunk.d == "0") {
        defaultStyle.ResetDC(dc);
    } else {
        int state = STATE_NORMAL;
        wxArrayString attrs = ::wxStringTokenize(chunk.d, ";", wxTOKEN_RET_EMPTY);
        wxFont f = defaultStyle.font;
        for(size_t i = 0; i < attrs.size(); ++i) {
            const wxString& attr = attrs[i];
            long number;
            if(!attr.ToCLong(&number)) {
                continue;
            }
            switch(state) {
            case STATE_NORMAL:
                switch(number) {
                case 0:
                    // reset attributes
                    defaultStyle.ResetDC(dc);
                    break;
                case 1:
                    f.SetWeight(wxFONTWEIGHT_BOLD);
                    break;
                case 2:
                    f.SetWeight(wxFONTWEIGHT_LIGHT);
                    break;
                case 3:
                    f.SetStyle(wxFONTSTYLE_ITALIC);
                    break;
                case 4:
                    f.SetUnderlined(true);
                    break;
                case 38:
                    state = STATE_SET_FG;
                    break;
                case 48:
                    state = STATE_SET_BG;
                    break;
                default:
                    if((number >= 30 && number <= 37) || (number >= 90 && number <= 97)) {
                        // use colour table to set the text colour
                        wxColour c = GetColour(*m_colours, number);
                        if(c.IsOk()) {
                            dc.SetTextForeground(c);
                        }
                    } else if((number >= 40 && number <= 47) || (number >= 100 && number <= 107)) {
                        wxColour c = GetColour(*m_colours, number);
                        if(c.IsOk()) {
                            dc.SetTextBackground(c);
                        }
                    }
                    break;
                }
                break;
            case STATE_SET_BG:
                switch(number) {
                case 5:
                    break;
                case 2: {
                    // ESC[38;2;R;G;B
                    wxColour::ChannelType r, g, b;
                    NEXT_ATTR(i + 1, r);
                    NEXT_ATTR(i + 2, g);
                    NEXT_ATTR(i + 3, b);
                    wxColour c{ r, g, b };
                    if(c.IsOk()) {
                        dc.SetTextBackground(c);
                    }
                    state = STATE_NORMAL;
                } break;
                default: {
                    // use colour table to set the text colour
                    wxColour c = GetColour(*m_8_bit_colours, number);
                    if(c.IsOk()) {
                        dc.SetTextBackground(c);
                    }
                    state = STATE_NORMAL;
                } break;
                }
                break;
            case STATE_SET_FG:
                switch(number) {
                case 5:
                    break;
                case 2: {
                    // ESC[38;2;R;G;B
                    wxColour::ChannelType r, g, b;
                    NEXT_ATTR(i + 1, r);
                    NEXT_ATTR(i + 2, g);
                    NEXT_ATTR(i + 3, b);
                    wxColour c{ r, g, b };
                    if(c.IsOk()) {
                        dc.SetTextForeground(c);
                    }
                    state = STATE_NORMAL;
                } break;
                default: {
                    // use colour table to set the text colour
                    wxColour c = GetColour(*m_8_bit_colours, number);
                    if(c.IsOk()) {
                        dc.SetTextForeground(c);
                    }
                    state = STATE_NORMAL;
                } break;
                }
                break;
            }
        }
        dc.SetFont(f);
    }
}

const wxColour& clAnsiEscapeCodeHandler::GetColour(const ColoursMap_t& m, int num) const
{
    if(m.count(num) == 0) {
        return wxNullColour;
    }
    return m.find(num)->second;
}

// ===------------------------------------------------------------------------
// ===------------------------------------------------------------------------

int wxSTCStyleProvider::GetStyle(const wxColour& fg, const wxColour& bg)
{
    // build the key for the style
    wxString key;
    key << "fg:" << fg.GetAsString(wxC2S_HTML_SYNTAX) << ";bg:" << bg.GetAsString(wxC2S_HTML_SYNTAX);
    if(m_styleCache.count(key)) {
        return m_styleCache[key];
    }

    // no such style, create it
    if(m_curstyle >= wxSTC_STYLE_MAX) {
        return 0;
    }

    int new_style = m_curstyle;
    m_ctrl->StyleSetBackground(new_style, bg);
    m_ctrl->StyleSetForeground(new_style, fg);
    m_styleCache.insert({ key, new_style });
    ++m_curstyle;
    return new_style;
}

wxTextAttr wxSTCStyleProvider::GetDefaultStyle() const
{
    wxTextAttr attr;
    attr.SetFont(m_ctrl->StyleGetFont(0));
    attr.SetTextColour(m_ctrl->StyleGetForeground(0));
    attr.SetBackgroundColour(m_ctrl->StyleGetBackground(0));
    return attr;
}

wxSTCStyleProvider::~wxSTCStyleProvider() {}

wxSTCStyleProvider::wxSTCStyleProvider(wxStyledTextCtrl* ctrl)
    : m_ctrl(ctrl)
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_ctrl);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
}

void wxSTCStyleProvider::Clear()
{
    m_styleCache.clear();
    m_curstyle = wxSTC_STYLE_LASTPREDEFINED + 1;
}

void wxSTCStyleProvider::OnIdle(wxIdleEvent& event) { event.Skip(); }
