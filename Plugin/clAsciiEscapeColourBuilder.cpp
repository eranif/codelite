#include "clAsciiEscapeColourBuilder.hpp"

namespace
{
constexpr wxChar ESC = 0x1B;
};

clAsciiEscapeColourBuilder::clAsciiEscapeColourBuilder(wxString& string)
    : m_string(string)
{
    m_activeColours = &m_lightThemeColours;

    m_lightThemeColours.insert({ eAsciiColours::RED, 124 });
    m_lightThemeColours.insert({ eAsciiColours::YELLOW, 136 });
    m_lightThemeColours.insert({ eAsciiColours::GREEN, 28 });
    m_lightThemeColours.insert({ eAsciiColours::GRAY, 251 });

    m_darkThemeColours.insert({ eAsciiColours::RED, 196 });
    m_darkThemeColours.insert({ eAsciiColours::YELLOW, 214 });
    m_darkThemeColours.insert({ eAsciiColours::GREEN, 34 });
    m_darkThemeColours.insert({ eAsciiColours::GRAY, 245 });
}

clAsciiEscapeColourBuilder::~clAsciiEscapeColourBuilder() {}

clAsciiEscapeColourBuilder& clAsciiEscapeColourBuilder::Add(const wxString& text, eAsciiColours textColour, bool bold)
{
    if(m_activeColours->count(textColour) == 0) {
        return *this;
    }

    int colour_number = m_activeColours->find(textColour)->second;
    return Add(text, colour_number, bold);
}

clAsciiEscapeColourBuilder& clAsciiEscapeColourBuilder::Add(const wxString& text, int textColour, bool bold)
{
    wxString prefix;
    wxString suffix;

    prefix << ESC << "[";
    if(bold) {
        prefix << "1;";
    }
    prefix << "38;5;" << textColour << "m";
    suffix << ESC << "[0m";
    m_string << prefix << text << suffix;
    return *this;
}

clAsciiEscapeColourBuilder& clAsciiEscapeColourBuilder::SetTheme(eAsciiTheme theme)
{
    if(theme == eAsciiTheme::DARK) {
        m_activeColours = &m_darkThemeColours;
    } else {
        m_activeColours = &m_lightThemeColours;
    }
    return *this;
}
