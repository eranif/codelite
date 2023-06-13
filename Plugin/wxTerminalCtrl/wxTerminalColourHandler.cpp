#include "wxTerminalColourHandler.h"

#include "TextView.h"
#include "drawingutils.h"
#include "wxTerminalCtrl.h"

#ifdef __WXMSW__
#include "wx/msw/wrapwin.h" // includes windows.h
#endif

#include "fileutils.h"

#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

wxTerminalColourHandler::wxTerminalColourHandler() {}

wxTerminalColourHandler::~wxTerminalColourHandler() { wxDELETE(m_style_provider); }

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
    auto stc = m_ctrl->GetCtrl();
    int last_pos = stc->GetLastPosition();
    int last_line = stc->LineFromPosition(last_pos);
    curline = stc->GetLine(last_line);
    if(curline.EndsWith("\n")) {
        // a ncomplete line
        curline.clear();
    } else {
        // remove the last line from the control
        stc->Remove(stc->PositionFromLine(last_line), last_pos);
    }

    m_ctrl->SelectNone();
    m_ctrl->SetInsertionPointEnd();

    // wxWindowUpdateLocker locker{ m_ctrl };
    clAnsiEscapeCodeHandler handler;
    handler.Parse(FileUtils::ToStdString(curline) + buffer);
    handler.Render(m_style_provider, !DrawingUtils::IsDark(m_defaultAttr.GetBackgroundColour()));
    SetCaretEnd();
}

wxTerminalColourHandler& wxTerminalColourHandler::operator<<(const std::string& buffer)
{
    Append(buffer);
    return *this;
}

void wxTerminalColourHandler::SetCtrl(TextView* ctrl)
{
    m_ctrl = ctrl;
    Clear();
}

void wxTerminalColourHandler::Clear()
{
    wxDELETE(m_style_provider);
    if(m_ctrl) {
        m_style_provider = new wxSTCStyleProvider(m_ctrl->GetCtrl());
        m_ctrl->ReloadSettings();
    }
}

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
    m_ctrl->SelectNone();
    m_ctrl->SetInsertionPointEnd();
}
