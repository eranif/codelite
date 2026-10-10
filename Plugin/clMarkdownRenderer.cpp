#include "clMarkdownRenderer.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "file_logger.h"
#include "mdparser.hpp"

#include <wx/settings.h>

namespace
{
struct DCFontLocker {
    wxDC& m_dc;
    wxFont m_old_font;
    DCFontLocker(wxDC& dc)
        : m_dc(dc)
        , m_old_font(dc.GetFont())
    {
    }
    ~DCFontLocker() { m_dc.SetFont(m_old_font); }
};
} // namespace

void clMarkdownRenderer::UpdateFont(wxDC& dc, const mdparser::Style& style)
{
    // we always use code font, so we don't change it
    wxFont f = dc.GetFont();
    double point_size = f.GetPointSize();
    switch (style.font_size) {
    case mdparser::Style::FONTSIZE_H1:
        point_size += 4; // it will receive a different colour
        break;
    case mdparser::Style::FONTSIZE_H2:
        point_size += 4;
        break;
    case mdparser::Style::FONTSIZE_H3:
        point_size += 2;
        break;
    default:
        break;
    }
    f.SetPointSize(point_size);
    f.SetWeight(style.font_weight == mdparser::Style::FONTWEIGHT_BOLD ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    f.SetStyle(style.font_style == mdparser::Style::FONTSTYLE_ITALIC ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    f.SetStrikethrough(style.font_strikethrough);
    dc.SetFont(f);
}

wxSize clMarkdownRenderer::Render(wxWindow* win, wxDC& dc, const wxString& text, const wxRect& rect)
{
    return DoRender(win, dc, text, rect, true);
}

wxSize clMarkdownRenderer::DoRender(wxWindow* win, wxDC& dc, const wxString& text, const wxRect& rect, bool do_draw)
{
    wxUnusedVar(win);

    constexpr int X_MARGIN = 8;
    constexpr int Y_MARGIN = 6;
    constexpr int CODEBLOCK_PADDING = 4;

    const int left = rect.GetTopLeft().x + X_MARGIN;
    int xx = left;
    int yy = rect.GetTopLeft().y + Y_MARGIN;

    // prose uses the GUI font, code uses the fixed font
    wxFont text_font = DrawingUtils::GetDefaultGuiFont();
    wxFont code_font = ColoursAndFontsManager::Get().GetFixedFont(true);

    // use the same line height for both fonts, so mixed lines line up
    dc.SetFont(code_font);
    int base_line_height = dc.GetTextExtent("Tp").GetHeight();
    dc.SetFont(text_font);
    base_line_height = wxMax(base_line_height, dc.GetTextExtent("Tp").GetHeight());

    // clear the area
    wxColour pen_colour = clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    wxColour bg_colour = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    bool is_dark = DrawingUtils::IsDark(bg_colour); //.GetLuminance() < 128;
    if (do_draw) {
        wxRect bgRect = rect;
#ifdef __WXMAC__
        bgRect.Inflate(1);
#endif
        dc.SetPen(pen_colour);
        dc.SetBrush(bg_colour);
        dc.DrawRectangle(bgRect);
    }

    wxColour code_bg_colour = bg_colour.ChangeLightness(is_dark ? 110 : 150);
    int max_x = left;
    // height of the current (open) line, 0 if nothing was written on it yet
    int line_height = 0;
    bool in_codeblock = false;

    // fill a full-width row with the code block background
    auto draw_codeblock_bg = [&](int y, int h) {
        if (!do_draw) {
            return;
        }
        wxRect code_rect = wxRect(rect.GetX(), y, rect.GetWidth(), h);
        code_rect.Deflate(1, 0);
        dc.SetPen(code_bg_colour);
        dc.SetBrush(code_bg_colour);
        dc.DrawRectangle(code_rect);
    };

    auto end_codeblock = [&]() {
        if (in_codeblock) {
            draw_codeblock_bg(yy, CODEBLOCK_PADDING);
            yy += CODEBLOCK_PADDING;
            in_codeblock = false;
        }
    };

    auto on_write = [&](const wxString& buffer, const mdparser::Style& style, bool is_eol) {
        DCFontLocker font_locker(dc);
        if (style.is_horizontal_rule()) {
            end_codeblock();
            yy += base_line_height / 2;
            if (do_draw) {
                dc.SetPen(pen_colour);
                dc.DrawLine(left, yy, rect.GetRight() - X_MARGIN, yy);
            }
            xx = left;
            yy += base_line_height / 2;
            line_height = 0;
            return;
        }

        if (style.is_codeblock()) {
            if (!in_codeblock) {
                draw_codeblock_bg(yy, CODEBLOCK_PADDING);
                yy += CODEBLOCK_PADDING;
                in_codeblock = true;
            }
        } else {
            end_codeblock();
        }

        if (buffer.empty() && is_eol && xx == left && !style.is_codeblock()) {
            // empty line between paragraphs: use a smaller gap
            yy += base_line_height / 2;
            line_height = 0;
            return;
        }

        dc.SetFont(style.font_family == mdparser::Style::FONTFAMILY_CODE ? code_font : text_font);
        UpdateFont(dc, style);
        wxSize text_size = dc.GetTextExtent(buffer);

        // even if text is empty, we still need to have a valid line height
        // so use a dummy "Tp" text for this purpose
        int segment_height = dc.GetTextExtent("Tp").GetHeight();
        int row_height = wxMax(base_line_height, segment_height);
        line_height = wxMax(line_height, row_height);

        wxColour text_colour = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        if (style.is_code()) {
            text_colour = is_dark ? wxColour("#cc99ff") : wxColour("#cc0000");
        } else if (style.has_flag(mdparser::T_H1)) {
            text_colour = is_dark ? wxColour("#ff9999") : wxColour("#3399cc");
        }

        if (do_draw) {
            // centre the text vertically within the row
            int text_y = yy + (row_height - segment_height) / 2;
            if (style.is_code()) {
                wxRect code_rect = wxRect({xx, text_y}, text_size);
                dc.SetPen(code_bg_colour);
                dc.SetBrush(code_bg_colour);
                dc.DrawRoundedRectangle(code_rect, 1.0);

            } else if (style.is_codeblock()) {
                // colour the entire row
                draw_codeblock_bg(yy, row_height);
            }
            dc.SetTextForeground(text_colour);
            dc.DrawText(buffer, xx, text_y);
        }
        xx += text_size.GetWidth();
        max_x = wxMax(max_x, xx);

        if (is_eol) {
            xx = left;
            yy += line_height;
            line_height = 0;
        }
    };

    mdparser::Parser parser;
    parser.parse(text, on_write);

    // close the last line, if it is still open
    yy += line_height;
    end_codeblock();

    return {max_x + X_MARGIN - rect.GetX(), yy + Y_MARGIN - rect.GetY()};
}

wxSize clMarkdownRenderer::GetSize(wxWindow* win, wxDC& dc, const wxString& text)
{
    return DoRender(win, dc, text, {}, false);
}
