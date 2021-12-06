#include "clAnsiEscapeCodeColourBuilder.hpp"

namespace
{
constexpr wxChar ESC = 0x1B;
};

clAnsiEscapeCodeColourBuilder::clAnsiEscapeCodeColourBuilder()
    : clAnsiEscapeCodeColourBuilder(&m_internalBuffer)
{
}

clAnsiEscapeCodeColourBuilder::clAnsiEscapeCodeColourBuilder(wxString* string)
    : m_string(string)
{
    m_activeColours = &m_lightThemeColours;

    m_lightThemeColours.insert({ eAsciiColours::RED, 124 });
    m_lightThemeColours.insert({ eAsciiColours::YELLOW, 136 });
    m_lightThemeColours.insert({ eAsciiColours::GREEN, 28 });
    m_lightThemeColours.insert({ eAsciiColours::GRAY, 251 });
    m_lightThemeColours.insert({ eAsciiColours::NORMAL_TEXT, 240 });

    m_darkThemeColours.insert({ eAsciiColours::RED, 196 });
    m_darkThemeColours.insert({ eAsciiColours::YELLOW, 214 });
    m_darkThemeColours.insert({ eAsciiColours::GREEN, 34 });
    m_darkThemeColours.insert({ eAsciiColours::GRAY, 243 });
    m_darkThemeColours.insert({ eAsciiColours::NORMAL_TEXT, 254 });
}

clAnsiEscapeCodeColourBuilder::~clAnsiEscapeCodeColourBuilder() {}

clAnsiEscapeCodeColourBuilder& clAnsiEscapeCodeColourBuilder::Add(const wxString& text, eAsciiColours textColour,
                                                                  bool bold)
{
    if(m_activeColours->count(textColour) == 0) {
        return *this;
    }

    int colour_number = m_activeColours->find(textColour)->second;
    return Add(text, colour_number, bold);
}

clAnsiEscapeCodeColourBuilder& clAnsiEscapeCodeColourBuilder::Add(const wxString& text, int textColour, bool bold)
{
    DoAddTextToBuffer(m_string, text, textColour, bold);
    return *this;
}
clAnsiEscapeCodeColourBuilder& clAnsiEscapeCodeColourBuilder::Add(const wxString& text, const wxColour& colour,
                                                                  bool bold)
{
    if(!colour.IsOk()) {
        return Add(text, eAsciiColours::NORMAL_TEXT, bold);
    }

    wxString prefix;
    wxString suffix;

    // prepare the prefix
    prefix << ESC << "[";
    prefix << "38;2;" << (int)colour.Red() << ";" << (int)colour.Green() << ";" << (int)colour.Blue() << "m";

    // and suffix
    suffix << ESC << "[0m";

    (*m_string) << prefix << text << suffix;
    return *this;
}

clAnsiEscapeCodeColourBuilder& clAnsiEscapeCodeColourBuilder::SetTheme(eAsciiTheme theme)
{
    if(theme == eAsciiTheme::DARK) {
        m_activeColours = &m_darkThemeColours;
    } else {
        m_activeColours = &m_lightThemeColours;
    }
    return *this;
}

wxString& clAnsiEscapeCodeColourBuilder::WrapWithColour(wxString& line, eAsciiColours colour, bool bold_font) const
{
    if(m_activeColours->count(colour) == 0) {
        return line;
    }

    int colour_number = m_activeColours->find(colour)->second;
    wxString buffer;
    DoAddTextToBuffer(&buffer, line, colour_number, bold_font);
    line.swap(buffer);
    return line;
}

void clAnsiEscapeCodeColourBuilder::DoAddTextToBuffer(wxString* buffer, const wxString& text, int textColour,
                                                      bool bold) const
{
    wxString prefix;
    wxString suffix;

    prefix << ESC << "[";
    if(bold) {
        prefix << "1;";
    }
    prefix << "38;5;" << textColour << "m";
    suffix << ESC << "[0m";
    (*buffer) << prefix << text << suffix;
}
