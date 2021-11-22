#ifndef GDBMI_HPP
#define GDBMI_HPP

#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <wx/string.h>

namespace mdparser
{
enum Type : int {
    // See also: StringUtils::DisableMarkdownStyling()
    T_TEXT = (1 << 0),       // normal state
    T_H1 = (1 << 1),         // #
    T_H2 = (1 << 2),         // ##
    T_H3 = (1 << 3),         // ###
    T_LI = (1 << 4),         // -
    T_HR = (1 << 5),         // === or ---
    T_BOLD = (1 << 6),       // **
    T_ITALIC = (1 << 7),     // *
    T_STRIKE = (1 << 8),     // ~~
    T_CODE = (1 << 9),       // `
    T_CODEBLOCK = (1 << 10), // ```
    T_EOL = (1 << 11),       // \n
    T_EOF = 0,               // End of file
};

class Tokenizer
{
    const wxString& m_text;
    size_t m_pos = 0;
    int m_text_sequence = 0;
    bool m_enable_backslash_esc = true;

private:
    wxChar safe_get_char(size_t pos) const;

public:
    Tokenizer(const wxString& text)
        : m_text(text)
    {
    }
    std::pair<Type, wxString> next();
    void consume_until(wxChar ch);
    void enable_backslash_esc(bool enable) { m_enable_backslash_esc = enable; }
};

// font definition
struct Style {
private:
    size_t font_flags = 0;
    bool horizontal_rule = false;

public:
    bool has_flag(Type prop) const { return font_flags & prop; }
    enum FontSize {
        FONTSIZE_NORMAL,
        FONTSIZE_H1,
        FONTSIZE_H2,
        FONTSIZE_H3,
    };

    enum FontFamily {
        FONTFAMILY_NORMAL,
        FONTFAMILY_CODE,
    };

    enum FontWeight {
        FONTWEIGHT_NORMAL,
        FONTWEIGHT_BOLD,
    };

    enum FontStyle {
        FONTSTYLE_NORMAL,
        FONTSTYLE_ITALIC,
    };

    FontSize font_size = FONTSIZE_NORMAL;
    FontFamily font_family = FONTFAMILY_NORMAL;
    FontWeight font_weight = FONTWEIGHT_NORMAL;
    FontStyle font_style = FONTSTYLE_NORMAL;
    bool font_strikethrough = false;

    void toggle_property(Type prop)
    {
        font_flags ^= prop;
        update();
    }
    void clear_property(Type prop)
    {
        font_flags &= ~prop;
        update();
    }

    void update()
    {
        font_size = FONTSIZE_NORMAL;
        font_family = FONTFAMILY_NORMAL;
        font_weight = FONTWEIGHT_NORMAL;
        font_style = FONTSTYLE_NORMAL;
        font_strikethrough = has_flag(T_STRIKE);
        horizontal_rule = has_flag(T_HR);

        if(has_flag(T_CODE) || has_flag(T_CODEBLOCK)) {
            font_family = FONTFAMILY_CODE;
        }

        if(has_flag(T_ITALIC)) {
            font_style = FONTSTYLE_ITALIC;
        }

        if(has_flag(T_BOLD)) {
            font_weight = FONTWEIGHT_BOLD;
        }

        if(has_flag(T_H1)) {
            font_size = FONTSIZE_H1;
        }

        if(has_flag(T_H2)) {
            font_size = FONTSIZE_H2;
        }

        if(has_flag(T_H3)) {
            font_size = FONTSIZE_H3;
        }
    }

    wxString to_string() const
    {
        wxString str;
        str << "{FONT";
        if(font_flags & mdparser::T_CODE || font_flags & mdparser::T_CODEBLOCK) {
            str << ":CODE";
        }
        if(font_flags & mdparser::T_ITALIC) {
            str << ":ITALIC";
        }
        if(font_flags & mdparser::T_BOLD) {
            str << ":BOLD";
        }
        if(font_flags & mdparser::T_STRIKE) {
            str << ":STRIKE";
        }
        if(font_flags & mdparser::T_H1) {
            str << ":H1";
        }
        if(font_flags & mdparser::T_H2) {
            str << ":H2";
        }
        if(font_flags & mdparser::T_H3) {
            str << ":H3";
        }
        str << "}";
        return str;
    }

    bool is_code() const { return has_flag(T_CODE); }
    bool is_codeblock() const { return has_flag(T_CODEBLOCK); }
    bool is_horizontal_rule() const { return horizontal_rule; }
};

typedef std::function<void(const wxString&, const Style&, bool)> write_callback_t;
class Parser
{
    write_callback_t write_cb = nullptr;

private:
    void flush_buffer(wxString& buffer, const Style& font, bool is_eol);
    void notify_hr();

public:
    void parse(const wxString& input_str, write_callback_t on_write);
};
} // namespace mdparser

#endif // GDBMI_HPP
