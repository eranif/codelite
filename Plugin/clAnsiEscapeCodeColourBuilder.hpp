#ifndef CLASCIIESCAPECOLOURBUILDER_HPP
#define CLASCIIESCAPECOLOURBUILDER_HPP

#include "codelite_exports.h"

#include <unordered_map>
#include <wx/colour.h>
#include <wx/string.h>

/// Helper class for getting proper common colour code
struct WXDLLIMPEXP_SDK AnsiColours {
    static bool dark_theme;
    static void SetDarkTheme(bool b) { dark_theme = b; }
    inline static int OneOf(int light, int dark) { return dark_theme ? dark : light; }
    inline static int Red() { return OneOf(160, 160); }
    inline static int Yellow() { return OneOf(220, 214); }
    inline static int Green() { return OneOf(28, 40); }
    inline static int NormalText() { return OneOf(0, 255); }
    inline static int Cyan() { return OneOf(45, 117); }
    inline static int Magenta() { return OneOf(162, 200); }
    inline static int Gray() { return OneOf(247, 250); }
};

enum class eColourTheme : int {
    DARK,
    LIGHT,
};

class WXDLLIMPEXP_SDK clAnsiEscapeCodeColourBuilder
{
private:
    wxString* m_string = nullptr;
    wxString m_internalBuffer;

protected:
    void DoAddTextToBuffer(wxString* buffer, const wxString& text, int textColour, bool bold) const;

public:
    clAnsiEscapeCodeColourBuilder(wxString* string);
    clAnsiEscapeCodeColourBuilder();
    ~clAnsiEscapeCodeColourBuilder();

    clAnsiEscapeCodeColourBuilder& SetTheme(eColourTheme theme);
    clAnsiEscapeCodeColourBuilder& Add(const wxString& text, int textColour, bool bold = false);
    clAnsiEscapeCodeColourBuilder& Add(const wxString& text, const wxColour& colour, bool bold = false);

    /**
     * @brief wrap "line" with colour and optionally, bold font
     * @return reference to `line`
     */
    wxString& WrapWithColour(wxString& line, int colour, bool bold_font = false) const;

    /**
     * @brief should be used when working with the default constructor
     * @return
     */
    const wxString& GetString() const { return m_internalBuffer; }

    /**
     * @brief clear the internal buffer
     */
    void Clear() { m_internalBuffer.clear(); }
};

#endif // CLASCIIESCAPECOLOURBUILDER_HPP
