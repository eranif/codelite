#include "wxTerminalColourHandler.h"

#include "TextView.h"
#include "wxTerminalCtrl.h"

#ifdef __WXMSW__
#include "wx/msw/wrapwin.h" // includes windows.h
#endif

#include "fileutils.h"

#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

wxTerminalColourHandler::wxTerminalColourHandler() {}

wxTerminalColourHandler::~wxTerminalColourHandler() {}

void wxTerminalColourHandler::Append(const std::string& buffer)
{
    if(buffer == "\n") {
        // small optimization:
        // this is usually the case where the user simply hit ENTER after typing a command
        // No need to do fancy stuff here
        m_ctrl->AppendText(buffer);
        return;
    }

    wxString curline;
    // we start were left (at the end of the buffer)
    long lastPos = m_ctrl->GetLastPosition();
    if(lastPos > 0) {
        // test the last char
        wxChar lastCh = m_ctrl->GetLastChar();
        if(lastCh != '\n') {
            // we dont have a complete line here
            // read the last line into the buffer and clear the line
            long x, y;
            m_ctrl->PositionToXY(lastPos, &x, &y);
            long newpos = m_ctrl->XYToPosition(0, y);
            if(newpos == wxNOT_FOUND) {
                newpos = 0;
            }
            curline = m_ctrl->GetRange(newpos, lastPos);
            m_ctrl->Remove(newpos, lastPos);
            m_ctrl->SetInsertionPoint(newpos);
        }
    }

    m_ctrl->SelectNone();
    m_ctrl->SetInsertionPointEnd();

    wxWindowUpdateLocker locker{ m_ctrl };
    clAnsiEscapeCodeHandler handler;
    handler.Parse(FileUtils::ToStdString(curline) + buffer);
    handler.Render(static_cast<wxTextCtrl*>(m_ctrl->GetCtrl()), m_defaultAttr, false);
    SetCaretEnd();
}

wxTerminalColourHandler& wxTerminalColourHandler::operator<<(const std::string& buffer)
{
    Append(buffer);
    return *this;
}

void wxTerminalColourHandler::SetCtrl(TextView* ctrl)
{
    Clear();
    m_ctrl = ctrl;
    m_defaultAttr = m_ctrl->GetDefaultStyle();
}

void wxTerminalColourHandler::Clear() {}

void wxTerminalColourHandler::SetDefaultStyle(const wxTextAttr& attr)
{
    if(m_ctrl) {
        m_ctrl->SetInsertionPointEnd();
        if(attr.GetBackgroundColour().IsOk()) {
            m_ctrl->SetBackgroundColour(attr.GetBackgroundColour());
            m_defaultAttr.SetBackgroundColour(attr.GetBackgroundColour());
        }
        if(attr.GetTextColour().IsOk()) {
            m_defaultAttr.SetTextColour(attr.GetTextColour());
            m_ctrl->SetForegroundColour(attr.GetTextColour());
        }
        if(attr.GetFont().IsOk()) {
            m_defaultAttr.SetFont(attr.GetFont());
        }
        m_ctrl->SetDefaultStyle(m_defaultAttr);
        m_ctrl->Refresh();
    }
}

void wxTerminalColourHandler::SetCaretEnd()
{
    // And ensure that the last line is visible
    m_ctrl->ShowCommandLine();
    m_ctrl->CallAfter(&TextView::Focus);
}
