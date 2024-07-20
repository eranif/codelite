#ifndef CLASCIIESCAPCODEHANDLER_HPP
#define CLASCIIESCAPCODEHANDLER_HPP

#include "codelite_exports.h"

#include <map>
#include <unordered_map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/dc.h>
#include <wx/stc/stc.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

enum class eColourHandlerState {
    kNormal = 0,
    kInEscape,          // found ESC char
    kInOsc,             // Operating System Command
    kInCsi,             // Control Sequence Introducer
    kInPrivateSequence, // Some popular private sequences
    kCR,                // Found CR in Normal
    kOSC_Title,
    kOSC_Url,
};

struct WXDLLIMPEXP_SDK wxSTCStyleProvider : public wxEvtHandler {
    struct wxSTCStyleSegment {
        int pos = wxNOT_FOUND;
        int len = wxNOT_FOUND;
        int style = wxNOT_FOUND;
    };

public:
    wxStyledTextCtrl* m_ctrl = nullptr;
    int m_curstyle = wxSTC_STYLE_LASTPREDEFINED + 1;
    std::unordered_map<wxString, int> m_styleCache;

public:
    /// Return style for a given colour
    int GetStyle(const wxColour& fg, const wxColour& bg);
    wxTextAttr GetDefaultStyle() const;
    wxSTCStyleProvider(wxStyledTextCtrl* ctrl);
    virtual ~wxSTCStyleProvider();
    void Clear();
    void OnIdle(wxIdleEvent& event);
};

struct WXDLLIMPEXP_SDK Chunk {
    wxString d;

    // chunk flags
    bool is_text = true;
    bool is_eol = false;
    bool is_completed = false;
    bool is_style_reset = false;
    bool is_title = false;

    bool is_empty() const { return is_text && d.empty(); }
    typedef std::vector<Chunk> Vec_t;
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
    typedef std::map<int, wxColour> ColoursMap_t;
    ColoursMap_t m_8_bit_colours_normal;
    ColoursMap_t m_8_bit_colours_for_dark_theme;
    ColoursMap_t m_colours_normal;
    ColoursMap_t m_colours_for_dark_theme;
    ColoursMap_t* m_8_bit_colours = nullptr;
    ColoursMap_t* m_colours = nullptr;
    eColourHandlerState m_state = eColourHandlerState::kNormal;
    wxString m_windowTitle;
    // Every entry in the below vector represents a single line, splitted into "chunks"
    std::vector<Chunk::Vec_t> m_chunks;

private:
    void EnsureCurrent();
    void UpdateStyle(const Chunk& chunk, wxDC& dc, const clRenderDefaultStyle& defaultStyle);
    void UpdateStyle(const Chunk& chunk, const wxTextAttr& defaultStyle, wxTextAttr* updatedStyle);
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
     * @brief draw the text onto the text control
     */
    void Render(wxTextCtrl* ctrl, const wxTextAttr& defaultStyle, bool isLightTheme);
    /**
     * @brief draw the text onto the wxStyledTextCtrl control
     */
    void Render(wxSTCStyleProvider* style_provider, bool isLightTheme);

    /**
     * @brief render line without style
     */
    void RenderNoStyle(wxDC& dc, const clRenderDefaultStyle& defaultStyle, int line, const wxRect& rect,
                       bool isLightTheme);

    size_t GetLineCount() const { return m_chunks.size(); }

    /**
     * @brief return the window title found
     */
    const wxString& GetWindowTitle() const { return m_windowTitle; }
};

#endif // CLASCIIESCAPCODEHANDLER_HPP
