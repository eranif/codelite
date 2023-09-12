#include "clAnsiEscapeCodeColourBuilder.hpp"

#include "wxTerminalCtrl/wxTerminalAnsiEscapeHandler.hpp"

bool AnsiColours::dark_theme = false;

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
}

clAnsiEscapeCodeColourBuilder::~clAnsiEscapeCodeColourBuilder() {}

clAnsiEscapeCodeColourBuilder& clAnsiEscapeCodeColourBuilder::Add(const wxString& text, int textColour, bool bold)
{
    DoAddTextToBuffer(m_string, text, textColour, bold);
    return *this;
}
clAnsiEscapeCodeColourBuilder& clAnsiEscapeCodeColourBuilder::Add(const wxString& text, const wxColour& colour,
                                                                  bool bold)
{
    if(!colour.IsOk()) {
        return Add(text, AnsiColours::NormalText(), bold);
    }

    wxString prefix;
    wxString suffix;

    // prepare the prefix
    prefix << ESC << wxT("[");
    prefix << wxT("38;2;") << (int)colour.Red() << wxT(";") << (int)colour.Green() << wxT(";") << (int)colour.Blue()
           << wxT("m");

    // and suffix
    suffix << ESC << wxT("[0m");

    (*m_string) << prefix << text << suffix;
    return *this;
}

clAnsiEscapeCodeColourBuilder& clAnsiEscapeCodeColourBuilder::SetTheme(eColourTheme theme)
{
    AnsiColours::SetDarkTheme(theme == eColourTheme::DARK);
    return *this;
}

wxString& clAnsiEscapeCodeColourBuilder::WrapWithColour(wxString& line, int colour, bool bold_font) const
{
    wxString buffer;
    DoAddTextToBuffer(&buffer, line, colour, bold_font);
    line.swap(buffer);
    return line;
}

void clAnsiEscapeCodeColourBuilder::DoAddTextToBuffer(wxString* buffer, const wxString& text, int textColour,
                                                      bool bold) const
{
    wxString prefix;
    wxString suffix;

    prefix << ESC << wxT("[");
    if(bold) {
        prefix << wxT("1;");
    }
    prefix << wxT("38;5;") << textColour << wxT("m");
    suffix << ESC << wxT("[0m");
    (*buffer) << prefix << text << suffix;
}
