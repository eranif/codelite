#include "wxTerminalColourHandler.h"
#include <wx/tokenzr.h>

wxTerminalColourHandler::wxTerminalColourHandler()
{
    // we use the Ubuntu colour scheme
    m_colours.insert({ 30, wxColour(1, 1, 1) });
    m_colours.insert({ 31, wxColour(222, 56, 43) });
    m_colours.insert({ 32, wxColour(57, 181, 74) });
    m_colours.insert({ 33, wxColour(255, 199, 6) });
    m_colours.insert({ 34, wxColour(0, 111, 184) });
    m_colours.insert({ 35, wxColour(118, 38, 113) });
    m_colours.insert({ 36, wxColour(44, 181, 233) });
    m_colours.insert({ 37, wxColour(204, 204, 204) });
    m_colours.insert({ 90, wxColour(128, 128, 128) });
    m_colours.insert({ 91, wxColour(255, 0, 0) });
    m_colours.insert({ 92, wxColour(0, 255, 0) });
    m_colours.insert({ 93, wxColour(255, 255, 0) });
    m_colours.insert({ 94, wxColour(0, 0, 255) });
    m_colours.insert({ 95, wxColour(255, 0, 255) });
    m_colours.insert({ 96, wxColour(0, 255, 255) });
    m_colours.insert({ 97, wxColour(255, 255, 255) });
    m_colours.insert({ 40, wxColour(1, 1, 1) });
    m_colours.insert({ 41, wxColour(222, 56, 43) });
    m_colours.insert({ 42, wxColour(57, 181, 74) });
    m_colours.insert({ 43, wxColour(255, 199, 6) });
    m_colours.insert({ 44, wxColour(0, 111, 184) });
    m_colours.insert({ 45, wxColour(118, 38, 113) });
    m_colours.insert({ 46, wxColour(44, 181, 233) });
    m_colours.insert({ 47, wxColour(204, 204, 204) });
    m_colours.insert({ 100, wxColour(128, 128, 128) });
    m_colours.insert({ 101, wxColour(255, 0, 0) });
    m_colours.insert({ 102, wxColour(0, 255, 0) });
    m_colours.insert({ 103, wxColour(255, 255, 0) });
    m_colours.insert({ 104, wxColour(0, 0, 255) });
    m_colours.insert({ 105, wxColour(255, 0, 255) });
    m_colours.insert({ 106, wxColour(0, 255, 255) });
    m_colours.insert({ 107, wxColour(255, 255, 255) });
}

wxTerminalColourHandler::wxTerminalColourHandler(wxTextCtrl* ctrl)
    : wxTerminalColourHandler()
{
    SetCtrl(m_ctrl);
}

wxTerminalColourHandler::~wxTerminalColourHandler() {}

void wxTerminalColourHandler::Append(const wxString& buffer)
{
    m_ctrl->SelectNone();
    m_ctrl->SetInsertionPointEnd();
    wxString m_chunk;
    for(const wxChar& ch : buffer) {
        switch(m_state) {
        case eColourHandlerState::kNormal:
            switch(ch) {
            case 0x1B: // ESC
                // Add the current chunk using the current style
                if(!m_chunk.IsEmpty()) {
                    m_ctrl->AppendText(m_chunk);
                    m_chunk.Clear();
                }
                m_state = eColourHandlerState::kInEscape;
                break;
            default:
                m_chunk << ch;
                break;
            }
            break;
        case eColourHandlerState::kInEscape:
            switch(ch) {
            // CSI - Control Sequence Introducer
            case '[':
                m_escapeSequence.Clear();
                m_state = eColourHandlerState::kInCsi;
                break;
            case ']':
                m_state = eColourHandlerState::kInOsc;
                break;
            default:
                break;
            }
            break;
        case eColourHandlerState::kInOsc:
            if(ch == '\a') {
                // bell, leave the current state
                m_state = eColourHandlerState::kNormal;
            }
            break;
        case eColourHandlerState::kInCsi:
            // found ESC[
            switch(ch) {
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'J':
            case 'K':
            case 'S':
            case 'T':
            case 'f':
            case 'i':
            case 'n':
            case 'u':
            case 's':
                // exit the CSI mode back to normal mode
                m_state = eColourHandlerState::kNormal;
                break;
            case 'm':
                // colour sequence
                SetStyleFromEscape(m_escapeSequence);
                m_state = eColourHandlerState::kNormal;
                break;
            default:
                m_escapeSequence << ch;
                break;
            }
        }
    }
    if(!m_chunk.IsEmpty()) { m_ctrl->AppendText(m_chunk); }
    m_ctrl->ScrollLines(m_ctrl->GetNumberOfLines());
    m_chunk.Clear();
}

void wxTerminalColourHandler::SetStyleFromEscape(const wxString& escape)
{
    if(escape == "0") {
        // reset to normal
        m_ctrl->SetDefaultStyle(m_defaultAttr);
    } else {
        wxArrayString colourCodes = ::wxStringTokenize(escape, ";", wxTOKEN_RET_EMPTY);
        if(colourCodes.size() == 1) {
            // foreground colur only
            wxColour fg_colour = GetColour(colourCodes.Item(0));
            if(fg_colour.IsOk()) { m_ctrl->SetDefaultStyle(wxTextAttr(fg_colour)); }
        } else if(colourCodes.size() == 2) {
            wxColour fg_colour = GetColour(colourCodes.Item(0));
            wxColour bg_colour = GetColour(colourCodes.Item(1));
            if(bg_colour.IsOk() && fg_colour.IsOk()) { m_ctrl->SetDefaultStyle(wxTextAttr(fg_colour, bg_colour)); }
        }
    }
}

wxColour wxTerminalColourHandler::GetColour(const wxString& colour)
{
    long colourNumber(0);
    if(!colour.ToCLong(&colourNumber)) { return wxNullColour; }
    if(m_colours.count(colourNumber) == 0) { return wxNullColour; }
    return m_colours.find(colourNumber)->second;
}

wxTerminalColourHandler& wxTerminalColourHandler::operator<<(const wxString& buffer)
{
    Append(buffer);
    return *this;
}

void wxTerminalColourHandler::SetCtrl(wxTextCtrl* ctrl)
{
    Clear();
    m_ctrl = ctrl;
    m_defaultAttr = m_ctrl->GetDefaultStyle();
}

void wxTerminalColourHandler::Clear() { m_escapeSequence.Clear(); }
