#include "TextView.h"
#include <wx/sizer.h>
#include "wxTerminalOptions.h"

TextView::TextView(wxWindow* parent, wxWindowID winid)
    : wxWindow(parent, winid)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_ENTER | wxTE_NOHIDESEL | wxTE_PROCESS_TAB);
#ifdef __WXOSX__
    m_ctrl->OSXDisableAllSmartSubstitutions();
#endif
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Fit(this);
    ReloadSettings();
    m_colourHandler.SetCtrl(this);
}

TextView::~TextView() {}

void TextView::AppendText(const wxString& buffer) { m_ctrl->AppendText(buffer); }

long TextView::GetLastPosition() const { return m_ctrl->GetLastPosition(); }

wxString TextView::GetRange(int from, int to) const { return m_ctrl->GetRange(from, to); }

bool TextView::PositionToXY(long pos, long* x, long* y) const { return m_ctrl->PositionToXY(pos, x, y); }

long TextView::XYToPosition(long x, long y) const { return m_ctrl->XYToPosition(x, y); }

void TextView::Remove(long from, long to) { return m_ctrl->Remove(from, to); }

void TextView::SetInsertionPoint(long pos) { m_ctrl->SetInsertionPoint(pos); }

void TextView::SelectNone() { m_ctrl->SelectNone(); }

void TextView::SetInsertionPointEnd() { m_ctrl->SetInsertionPointEnd(); }

int TextView::GetNumberOfLines() const { return m_ctrl->GetNumberOfLines(); }

void TextView::SetDefaultStyle(const wxTextAttr& attr) { m_ctrl->SetDefaultStyle(attr); }

wxTextAttr TextView::GetDefaultStyle() const { return m_ctrl->GetDefaultStyle(); }

bool TextView::IsEditable() const { return m_ctrl->IsEditable(); }

long TextView::GetInsertionPoint() const { return m_ctrl->GetInsertionPoint(); }

void TextView::Replace(long from, long to, const wxString& replaceWith) { m_ctrl->Replace(from, to, replaceWith); }

wxString TextView::GetLineText(int lineNumber) const { return m_ctrl->GetLineText(lineNumber); }

void TextView::SetEditable(bool b) { m_ctrl->SetEditable(b); }

void TextView::ReloadSettings()
{
    wxFont font = wxTerminalOptions::Get().GetFont();
    wxColour textColour = wxTerminalOptions::Get().GetTextColour();
    wxColour bgColour = wxTerminalOptions::Get().GetBgColour();
    SetBackgroundColour(bgColour);
    SetForegroundColour(textColour);
    m_ctrl->SetBackgroundColour(bgColour);
    m_ctrl->SetForegroundColour(textColour);
    wxTextAttr defaultAttr = wxTextAttr(textColour, bgColour, font);
    SetDefaultStyle(defaultAttr);
    m_colourHandler.SetDefaultStyle(defaultAttr);
    m_ctrl->Refresh();
}

void TextView::StyleAndAppend(const wxString& buffer) { m_colourHandler << buffer; }

void TextView::Focus() { m_ctrl->SetFocus(); }
