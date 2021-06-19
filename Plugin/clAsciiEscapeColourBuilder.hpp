#ifndef CLASCIIESCAPECOLOURBUILDER_HPP
#define CLASCIIESCAPECOLOURBUILDER_HPP

#include "codelite_exports.h"
#include <unordered_map>
#include <wx/string.h>

enum class eAsciiColours : int {
    RED,
    YELLOW,
    GREEN,
    GRAY,
    NORMAL_TEXT,
};

enum class eAsciiTheme : int {
    DARK,
    LIGHT,
};

// provide hash for eAsciiColours
namespace std
{
template <> struct hash<eAsciiColours> {
    std::size_t operator()(eAsciiColours i) const { return hash<int>{}((int)i); }
};
} // namespace std

class WXDLLIMPEXP_SDK clAsciiEscapeColourBuilder
{
private:
    wxString* m_string = nullptr;
    wxString m_internalBuffer;
    typedef std::unordered_map<eAsciiColours, int> ColoursTable_t;
    ColoursTable_t m_lightThemeColours;
    ColoursTable_t m_darkThemeColours;
    ColoursTable_t* m_activeColours = nullptr;

protected:
    void DoAddTextToBuffer(wxString* buffer, const wxString& text, int textColour, bool bold) const;

public:
    clAsciiEscapeColourBuilder(wxString* string);
    clAsciiEscapeColourBuilder();
    ~clAsciiEscapeColourBuilder();

    clAsciiEscapeColourBuilder& SetTheme(eAsciiTheme theme);
    clAsciiEscapeColourBuilder& Add(const wxString& text, int textColour, bool bold = false);
    clAsciiEscapeColourBuilder& Add(const wxString& text, eAsciiColours textColour, bool bold = false);

    /**
     * @brief wrap "line" with colour and optionally, bold font
     * @return reference to `line`
     */
    wxString& WrapWithColour(wxString& line, eAsciiColours colour, bool bold_font = false) const;
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
