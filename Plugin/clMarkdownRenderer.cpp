#include "clMarkdownRenderer.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "mdparser.hpp"

#include <map>
#include <optional>
#include <vector>
#include <wx/settings.h>
#include <wx/stc/stc.h>

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

/// a piece of a code block line that uses a single lexer style
struct CodeRun {
    wxString text;
    wxColour colour;
    bool bold = false;
    bool italic = false;
};

struct HighlightedCode {
    wxColour bg_colour;
    std::vector<std::vector<CodeRun>> lines;
};

/// find the lexer for a code block fence language (e.g. "php", "cpp", "rs")
LexerConf::Ptr_t GetLexerForLanguage(const wxString& fence_lang)
{
    // the fence may contain more than the language, e.g. ```php title="a.php"
    wxString lang = fence_lang.BeforeFirst(' ').Lower();
    if (lang.empty()) {
        return nullptr;
    }

    static const std::map<wxString, wxString> aliases = {
        {"c", "c++"},
        {"cc", "c++"},
        {"cpp", "c++"},
        {"cxx", "c++"},
        {"h", "c++"},
        {"hpp", "c++"},
        {"objc", "c++"},
        {"objective-c", "c++"},
        {"js", "javascript"},
        {"jsx", "javascript"},
        {"ts", "javascript"},
        {"tsx", "javascript"},
        {"typescript", "javascript"},
        {"py", "python"},
        {"rs", "rust"},
        {"sh", "script"},
        {"bash", "script"},
        {"shell", "script"},
        {"zsh", "script"},
        {"yml", "yaml"},
        {"golang", "go"},
    };
    auto alias = aliases.find(lang);
    if (alias != aliases.end()) {
        lang = alias->second;
    }

    auto& manager = ColoursAndFontsManager::Get();
    if (manager.GetAllLexersNames().Index(lang) != wxNOT_FOUND) {
        return manager.GetLexer(lang);
    }

    // treat the language as a file extension
    auto lexer = manager.GetLexerForFileType(FileExtManager::GetType("file." + lang));
    return lexer.value_or(nullptr);
}

/// colour the code with the lexer for `lang`, using a hidden wxStyledTextCtrl
std::optional<HighlightedCode> HighlightCode(wxWindow* parent, const wxString& lang, const std::vector<wxString>& lines)
{
    if (parent == nullptr || lines.empty()) {
        return std::nullopt;
    }

    LexerConf::Ptr_t lexer = GetLexerForLanguage(lang);
    if (!lexer || lexer->GetName() == "text") {
        return std::nullopt;
    }

    wxString code;
    for (const auto& line : lines) {
        code << line << "\n";
    }

    // the tip is rendered on every paint, so keep the result
    static std::map<wxString, HighlightedCode> cache;
    wxString cache_key;
    cache_key << lexer->GetName() << "\n" << lexer->GetThemeName() << "\n" << code;
    auto iter = cache.find(cache_key);
    if (iter != cache.end()) {
        return iter->second;
    }

    wxStyledTextCtrl* stc = new wxStyledTextCtrl();
    stc->Hide();
    stc->Create(parent, wxID_ANY);
    lexer->Apply(stc, true);
    stc->SetText(code);
    stc->Colourise(0, wxSTC_INVALID_POSITION);

    HighlightedCode result;
    result.bg_colour = stc->StyleGetBackground(0);
    for (size_t i = 0; i < lines.size(); ++i) {
        std::vector<CodeRun> runs;
        int pos = stc->PositionFromLine(i);
        int end_pos = stc->GetLineEndPosition(i);
        // positions are in bytes, but the style only changes at character boundaries
        while (pos < end_pos) {
            int style = stc->GetStyleAt(pos);
            int run_end = pos + 1;
            while (run_end < end_pos && stc->GetStyleAt(run_end) == style) {
                ++run_end;
            }
            CodeRun run;
            run.text = stc->GetTextRange(pos, run_end);
            run.colour = stc->StyleGetForeground(style);
            run.bold = stc->StyleGetBold(style);
            run.italic = stc->StyleGetItalic(style);
            runs.push_back(std::move(run));
            pos = run_end;
        }
        result.lines.push_back(std::move(runs));
    }
    stc->Destroy();

    if (cache.size() > 50) {
        cache.clear();
    }
    cache.insert({cache_key, result});
    return result;
}
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
    // code block lines are collected, so the whole block can be highlighted at once
    std::vector<wxString> codeblock_lines;
    wxString codeblock_line;
    mdparser::Parser parser;

    // fill a full-width row with the code block background
    auto draw_codeblock_bg = [&](int y, int h, const wxColour& colour) {
        if (!do_draw) {
            return;
        }
        wxRect code_rect = wxRect(rect.GetX(), y, rect.GetWidth(), h);
        code_rect.Deflate(1, 0);
        dc.SetPen(colour);
        dc.SetBrush(colour);
        dc.DrawRectangle(code_rect);
    };

    auto end_codeblock = [&]() {
        if (!in_codeblock) {
            return;
        }
        in_codeblock = false;
        if (!codeblock_line.empty()) {
            codeblock_lines.push_back(codeblock_line);
            codeblock_line.clear();
        }

        auto highlighted = HighlightCode(win, parser.codeblock_lang(), codeblock_lines);
        wxColour block_bg_colour = highlighted ? highlighted->bg_colour : code_bg_colour;
        wxColour default_colour = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

        DCFontLocker font_locker(dc);
        dc.SetFont(code_font);
        int code_height = dc.GetTextExtent("Tp").GetHeight();
        int row_height = wxMax(base_line_height, code_height);
        // centre the text vertically within the row
        int text_offset = (row_height - code_height) / 2;

        draw_codeblock_bg(yy, CODEBLOCK_PADDING, block_bg_colour);
        yy += CODEBLOCK_PADDING;
        for (size_t i = 0; i < codeblock_lines.size(); ++i) {
            draw_codeblock_bg(yy, row_height, block_bg_colour);
            std::vector<CodeRun> runs;
            if (highlighted) {
                runs = highlighted->lines[i];
            } else {
                runs.push_back({codeblock_lines[i], default_colour});
            }

            xx = left;
            for (const auto& run : runs) {
                wxFont f = code_font;
                f.SetWeight(run.bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
                f.SetStyle(run.italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
                dc.SetFont(f);
                if (do_draw) {
                    dc.SetTextForeground(run.colour);
                    dc.DrawText(run.text, xx, yy + text_offset);
                }
                xx += dc.GetTextExtent(run.text).GetWidth();
            }
            max_x = wxMax(max_x, xx);
            yy += row_height;
        }
        draw_codeblock_bg(yy, CODEBLOCK_PADDING, block_bg_colour);
        yy += CODEBLOCK_PADDING;

        xx = left;
        line_height = 0;
        codeblock_lines.clear();
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
            // drawn by end_codeblock()
            in_codeblock = true;
            codeblock_line << buffer;
            if (is_eol) {
                codeblock_lines.push_back(codeblock_line);
                codeblock_line.clear();
            }
            return;
        }
        end_codeblock();

        if (buffer.empty() && is_eol && xx == left) {
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
