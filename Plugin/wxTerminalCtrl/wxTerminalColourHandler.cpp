#include "wxTerminalColourHandler.h"

#include "drawingutils.h"
#include "fileutils.h"
#include "wxTerminalCtrl.h"
#include "wxTerminalOutputCtrl.hpp"

#ifdef __WXMSW__
#include <wx/msw/wrapwin.h> // includes windows.h
#endif

#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

wxTerminalColourHandler::wxTerminalColourHandler() {}

wxTerminalColourHandler::~wxTerminalColourHandler() { wxDELETE(m_style_provider); }

void wxTerminalColourHandler::Append(const wxString& buffer, wxString* window_title)
{
    wxString curline;
    m_ctrl->SelectNone();
    m_ctrl->SetInsertionPointEnd();

    m_ansiEscapeHandler.Parse(curline + buffer);
    m_ansiEscapeHandler.Render(m_style_provider, !DrawingUtils::IsDark(m_defaultAttr.GetBackgroundColour()));
    SetCaretEnd();
    if(window_title) {
        *window_title = m_ansiEscapeHandler.GetWindowTitle();
    }
}

void wxTerminalColourHandler::SetCtrl(wxTerminalOutputCtrl* ctrl)
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
