#include "clMarkdownRenderer.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "file_logger.h"
#include "mdparser.hpp"
#include "wx/settings.h"

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
    // we always use code font, so we dont change it
    wxFont f = dc.GetFont();
    double point_size = f.GetPointSize();
    switch(style.font_size) {
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

clMarkdownRenderer::clMarkdownRenderer() {}

clMarkdownRenderer::~clMarkdownRenderer() {}

wxSize clMarkdownRenderer::Render(wxWindow* win, wxDC& dc, const wxString& text, const wxRect& rect)
{
    return DoRender(win, dc, text, rect, true);
}

wxSize clMarkdownRenderer::DoRender(wxWindow* win, wxDC& dc, const wxString& text, const wxRect& rect, bool do_draw)
{
    wxUnusedVar(win);

    constexpr int X_MARGIN = 5;
    constexpr int Y_MARGIN = 5;

    int xx = rect.GetTopLeft().x + X_MARGIN;
    int yy = rect.GetTopLeft().y + Y_MARGIN;

    wxFont default_font = ColoursAndFontsManager::Get().GetFixedFont(true);
    dc.SetFont(default_font);

    // clear the area
    wxColour pen_colour = clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    wxColour bg_colour = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    bool is_dark = DrawingUtils::IsDark(bg_colour); //.GetLuminance() < 128;
    if(do_draw) {
        wxRect bgRect = rect;
#ifdef __WXMAC__
        bgRect.Inflate(1);
#endif
        dc.SetPen(pen_colour);
        dc.SetBrush(bg_colour);
        dc.DrawRectangle(bgRect);
    }

    int height = X_MARGIN;
    int width = Y_MARGIN;
    int line_height = wxNOT_FOUND;

    auto on_write = [&](const wxString& buffer, const mdparser::Style& style, bool is_eol) {
        DCFontLocker font_locker(dc);
        if(style.is_horizontal_rule()) {
            wxSize text_size = dc.GetTextExtent("Tp");

            yy += text_size.GetHeight() / 2;
            if(do_draw) {
                dc.DrawLine(xx, yy, rect.GetRight() - X_MARGIN, yy);
            }
            xx = X_MARGIN;
            yy += text_size.GetHeight() / 2;

            height += text_size.GetHeight();

        } else {
            UpdateFont(dc, style);
            wxSize text_size = dc.GetTextExtent(buffer);

            // even if text is empty, we still need to have a valid line height
            // so use a dummy "Tp" text for this purpose
            line_height = dc.GetTextExtent("Tp").GetHeight();

            wxColour code_bg_colour = bg_colour.ChangeLightness(is_dark ? 110 : 150);
            wxColour text_colour = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

            if(style.is_code()) {
                text_colour = is_dark ? wxColour("#cc99ff") : wxColour("#cc0000");
            } else if(style.has_flag(mdparser::T_H1)) {
                text_colour = is_dark ? wxColour("#ff9999") : wxColour("#3399cc");
            }

            if(do_draw) {
                if(style.is_code()) {
                    wxRect code_rect = wxRect({ xx, yy }, text_size);
                    dc.SetPen(code_bg_colour);
                    dc.SetBrush(code_bg_colour);
                    dc.DrawRoundedRectangle(code_rect, 1.0);

                } else if(style.is_codeblock()) {
                    // colour the entire row
                    wxRect code_rect = wxRect(0, yy, rect.GetWidth(), line_height);
                    code_rect.Deflate(1, 0);
                    dc.SetPen(code_bg_colour);
                    dc.SetBrush(code_bg_colour);
                    dc.DrawRectangle(code_rect);
                }
                dc.SetTextForeground(text_colour);
                dc.DrawText(buffer, xx, yy);
            }
            xx += text_size.GetWidth();

            if(is_eol) {
                width = wxMax(xx, width);
                xx = X_MARGIN;
                yy += line_height;
                height = yy;
            }
        }
    };

    mdparser::Parser parser;
    parser.parse(text, on_write);
    width = wxMax(width, xx);
    height += line_height;

    return { width, height };
}

wxSize clMarkdownRenderer::GetSize(wxWindow* win, wxDC& dc, const wxString& text)
{
    return DoRender(win, dc, text, {}, false);
}
