#ifndef CLASCIIESCAPCODEHANDLER_HPP
#define CLASCIIESCAPCODEHANDLER_HPP

#include <codelite_exports.h>
#include <unordered_map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/dc.h>
#include <wx/string.h>
#include <wx/window.h>

enum class eColourHandlerState {
    kNormal = 0,
    kInEscape, // found ESC char
    kInOsc,    // Operating System Command
    kInCsi,    // Control Sequence Introducer
};

enum eChunkFlags {
    kCompleted = (1 << 0),
    kEndOfLine = (1 << 1),
    kTextChunk = (1 << 2),
    kTitleChunk = (1 << 3),
    kResetStyle = (1 << 4),
};

struct WXDLLIMPEXP_SDK Chunk {
    wxString d;

    // chunk flags
    size_t flags = kTextChunk;

    // the line number this chunk belongs to
    int line_number = wxNOT_FOUND;

    // set or remove a flag
    void set_flag(eChunkFlags flag, bool b = true)
    {
        if(b) {
            flags |= flag;
        } else {
            flags &= ~flag;
        }
    }

    bool is_text() const { return flags & kTextChunk; }
    bool is_eol() const { return flags & kEndOfLine; }
    bool is_style_reset() const { return flags & kResetStyle; }
    bool is_window_title() const { return flags & kTitleChunk; }
    bool is_empty() const { return flags == 0 && d.empty(); }
};

struct WXDLLIMPEXP_SDK clRenderDefaultStyle {
    wxColour bg_colour; // background colour
    wxColour fg_colour; // text colour
    wxFont font;        // default font

    void ResetDC(wxDC& dc) const
    {
        dc.SetFont(font);
        dc.SetTextBackground(bg_colour);
        dc.SetTextForeground(fg_colour);
    }
};

class WXDLLIMPEXP_SDK clAnsiEscapeCodeHandler
{
    typedef std::unordered_map<int, wxColour> ColoursMap_t;
    ColoursMap_t m_8_bit_colours_normal;
    ColoursMap_t m_8_bit_colours_for_dark_theme;
    ColoursMap_t m_colours_normal;
    ColoursMap_t m_colours_for_dark_theme;
    ColoursMap_t* m_8_bit_colours = nullptr;
    ColoursMap_t* m_colours = nullptr;

    std::vector<Chunk> m_chunks;
    eColourHandlerState m_state = eColourHandlerState::kNormal;
    int m_lineNumber = 0;
    // map between chunks and line numbers (index in the m_chunk vector)
    std::unordered_map<int, std::vector<size_t>> m_lines;

private:
    void EnsureCurrent();
    void UpdateStyle(const Chunk& chunk, wxDC& dc, const clRenderDefaultStyle& defaultStyle);
    const wxColour& GetColour(const ColoursMap_t& m, int num) const;

public:
    clAnsiEscapeCodeHandler();
    ~clAnsiEscapeCodeHandler();

    void Parse(const wxString& buffer);
    void Reset();

    /**
     * @brief draw line using device context using rect as the bounding area
     */
    void Render(wxDC& dc, const clRenderDefaultStyle& defaultStyle, int line, const wxRect& rect, bool isLightTheme);

    /**
     * @brief render line without style
     */
    void RenderNoStyle(wxDC& dc, const clRenderDefaultStyle& defaultStyle, int line, const wxRect& rect,
                       bool isLightTheme);

    size_t GetLineCount() const { return m_lines.size(); }
};

#endif // CLASCIIESCAPCODEHANDLER_HPP
