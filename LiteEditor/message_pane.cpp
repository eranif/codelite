#include "message_pane.h"
#include <wx/xrc/xmlres.h>
#include <wx/dcbuffer.h>

MessagePane::MessagePane( wxWindow* parent )
		: MessagePaneBase( parent )
{
	m_bpButtonClose->SetBitmapLabel(wxXmlResource::Get()->LoadBitmap(wxT("page_close16")));
	m_bitmap1->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("message_pane_inf")));
	m_buttonAction->Hide();
	Hide();
}

void MessagePane::OnKeyDown( wxKeyEvent& event )
{
	if (event.GetKeyCode() == WXK_ESCAPE) {
		DoHide();

	} else
		event.Skip();
}

void MessagePane::OnButtonClose( wxCommandEvent& event )
{
	wxUnusedVar(event);
	DoShowNextMessage();
}

void MessagePane::DoHide()
{
	if (IsShown()) {
		// Hide all default controls
		m_buttonAction->Hide();
		m_messages.Clear();
		Hide();
		GetParent()->GetSizer()->Layout();
	}
}

void MessagePane::DoShowCurrentMessage()
{
	int commandId = m_messages.CurrentMessage().commandId;
	wxString txt  = m_messages.CurrentMessage().message;
	wxEvtHandler* window = m_messages.CurrentMessage().window;
	wxString buttonLabel = m_messages.CurrentMessage().buttonLabel;

	if (commandId != wxNOT_FOUND && window) {
		m_buttonAction->SetLabel(buttonLabel);
		m_buttonAction->SetDefault();
		m_buttonAction->Show();

	} else {
		m_buttonAction->Hide();

	}

	m_staticTextMessage->SetLabel(txt);
	if (IsShown() == false) {
		Show();
	}
	GetParent()->GetSizer()->Layout();
	Refresh();
}

void MessagePane::ShowMessage(const wxString& message, const wxString &buttonLabel, int commandId, wxEvtHandler *window)
{
	MessageDetails msg;
	msg.buttonLabel = buttonLabel;
	msg.commandId   = commandId;
	msg.message     = message;
	msg.window      = window;

	m_messages.PushMessage(msg);
	DoShowCurrentMessage();
}

void MessagePane::OnActionButton(wxCommandEvent& event)
{
	MessageDetails msg = m_messages.CurrentMessage();

	if (msg.commandId != wxNOT_FOUND && msg.window) {
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, msg.commandId);
		msg.window->AddPendingEvent(evt);
	}

	DoShowNextMessage();
}

void MessagePane::DoShowNextMessage()
{
	m_messages.PopMessage();
	if (m_messages.IsEmpty()) {
		DoHide();
		return;
	}

	DoShowCurrentMessage();
}

/////////////////////////////////////////////////////////////////////////////
//

void MessagePaneData::Clear()
{
	m_queue.clear();
}

MessageDetails MessagePaneData::CurrentMessage()
{
	if (m_queue.empty())
		return MessageDetails();

	return m_queue.back();
}

bool MessagePaneData::IsEmpty()
{
	return m_queue.empty();
}

void MessagePaneData::PopMessage()
{
	if (m_queue.empty())
		return;

	m_queue.pop_back();
}

void MessagePaneData::PushMessage(const MessageDetails& msg)
{
	m_queue.push_back(msg);
}

void MessagePane::OnEraseBG(wxEraseEvent& event)
{
	wxUnusedVar(event);
}

void MessagePane::OnPaint(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);
	wxRect rr = GetClientRect();
	
	dc.SetPen(  wxPen  (wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
	dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
	dc.DrawRectangle(rr);
	
	rr.Deflate(2);
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW)));
	dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK)));
	
	dc.DrawRectangle(rr);
}
