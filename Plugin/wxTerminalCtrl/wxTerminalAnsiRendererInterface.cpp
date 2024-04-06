#include "wxTerminalAnsiRendererInterface.hpp"

#include "clModuleLogger.hpp"
#include "file_logger.h"

INITIALISE_MODULE_LOG(LOG, "AnsiEscapeHandler", "ansi_escape_parser.log");

wxTerminalAnsiRendererInterface::wxTerminalAnsiRendererInterface() {}

wxTerminalAnsiRendererInterface::~wxTerminalAnsiRendererInterface() {}

void wxTerminalAnsiRendererInterface::Bell() { LOG_DEBUG(LOG()) << "Bell!" << endl; }
void wxTerminalAnsiRendererInterface::Backspace() { LOG_DEBUG(LOG()) << "Backspace!" << endl; }
void wxTerminalAnsiRendererInterface::Tab() { LOG_DEBUG(LOG()) << "Tab!" << endl; }
void wxTerminalAnsiRendererInterface::LineFeed() { LOG_DEBUG(LOG()) << "Line Feed!" << endl; }
void wxTerminalAnsiRendererInterface::FormFeed() { LOG_DEBUG(LOG()) << "Form Feed!" << endl; }
void wxTerminalAnsiRendererInterface::CarriageReturn() { LOG_DEBUG(LOG()) << "CR!" << endl; }
void wxTerminalAnsiRendererInterface::AddString(wxStringView str) {}
void wxTerminalAnsiRendererInterface::EraseCharacter(int) {}

void wxTerminalAnsiRendererInterface::MoveCaret(long n, wxDirection direction)
{
    LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "MoveCaret:" << n << ". Direction:" << (int)direction << endl; }
}

void wxTerminalAnsiRendererInterface::SetCaretX(long n)
{
    m_pos.x = (n - 1); // we are 0 based
    m_pos.x = wxMax(m_pos.x, 0);
    LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "SetCaretX(" << n << ")" << endl; }
}

void wxTerminalAnsiRendererInterface::SetCaretY(long n)
{
    m_pos.y = (n - 1); // we are 0 based
    m_pos.y = wxMax(m_pos.y, 0);
    LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "SetCaretY(" << n << ")" << endl; }
}

void wxTerminalAnsiRendererInterface::ClearLine(size_t dir)
{
    LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "ClearLine" << endl; }
}
void wxTerminalAnsiRendererInterface::ClearDisplay(size_t dir)
{
    LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "ClearDisplay" << endl; }
}
void wxTerminalAnsiRendererInterface::SetWindowTitle(wxStringView window_title)
{
    LOG_IF_DEBUG
    {
        LOG_DEBUG(LOG()) << "SetWindowTitle(" << wxString(window_title.data(), window_title.length()) << ")" << endl;
    }
}

void wxTerminalAnsiRendererInterface::ResetStyle()
{
    m_curAttr = m_defaultAttr;
    LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "ResetStyle" << endl; }
}

void wxTerminalAnsiRendererInterface::SetTextColour(const wxColour& col)
{
    if(col.IsOk()) {
        LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "SetTextColour(" << col << ")" << endl; }
        m_curAttr.SetTextColour(col);
    } else {
        LOG_IF_DEBUG { LOG_DEBUG(LOG()) << "SetTextColour(NullColour)" << endl; }
        m_curAttr.SetTextColour(m_defaultAttr.GetTextColour());
    }
}

void wxTerminalAnsiRendererInterface::SetTextBgColour(const wxColour& col)
{
#if 0
    if(col.IsOk()) {
        LOG_IF_DEBUG { LOG_DEBUG(LOG) << "SetTextBgColour(" << col << ")" << endl; }
        m_curAttr.SetBackgroundColour(col);
    } else {
        LOG_IF_DEBUG { LOG_DEBUG(LOG) << "SetTextBgColour(NullColour)" << endl; }
        m_curAttr.SetBackgroundColour(m_defaultAttr.GetBackgroundColour());
    }
#endif
}

void wxTerminalAnsiRendererInterface::SetDefaultAttributes(const wxTextAttr& attr) { wxUnusedVar(attr); }

void wxTerminalAnsiRendererInterface::SetTextFont(const wxFont& font) { m_curAttr.SetFont(font); }
