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
		m_buttonAction1->Hide();
		m_buttonAction2->Hide();
	
		m_messages.Clear();
		Hide();
		GetParent()->GetSizer()->Layout();
	}
}

void MessagePane::DoShowCurrentMessage()
{
	MessageDetails msg   = m_messages.CurrentMessage();
	wxString txt         = msg.message;

	m_buttonAction->Hide();
	m_buttonAction1->Hide();
	m_buttonAction2->Hide();
	
	if (msg.btn1.window && msg.btn1.commandId != wxNOT_FOUND) {
		m_buttonAction->SetLabel(msg.btn1.buttonLabel);
		m_buttonAction->Show();
	}
	
	if (msg.btn2.commandId != wxNOT_FOUND) {
		m_buttonAction1->SetLabel(msg.btn2.buttonLabel);
		m_buttonAction1->Show();
	}
	
	if (msg.btn3.commandId != wxNOT_FOUND) {
		m_buttonAction2->SetLabel(msg.btn3.buttonLabel);
		m_buttonAction2->Show();
	}
		
	m_staticTextMessage->SetLabel(txt);
	if (IsShown() == false) {
		Show();
	}
	GetSizer()->Fit(this);
	GetParent()->GetSizer()->Layout();
	Refresh();
}

void MessagePane::ShowMessage(const wxString &message, const ButtonDetails& btn1, const ButtonDetails& btn2, const ButtonDetails& btn3)
{
	MessageDetails msg;
	msg.message     = message;
	msg.btn1        = btn1;
	msg.btn2        = btn2;
	msg.btn3        = btn3;
	
	m_messages.PushMessage(msg);
	DoShowCurrentMessage();
}

void MessagePane::OnActionButton(wxCommandEvent& event)
{
	MessageDetails msg = m_messages.CurrentMessage();
	DoPostEvent(msg.btn1);
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

void MessagePane::OnActionButton1(wxCommandEvent& event)
{
	MessageDetails msg = m_messages.CurrentMessage();
	DoPostEvent(msg.btn1);
	DoShowNextMessage();
}

void MessagePane::OnActionButton2(wxCommandEvent& event)
{
	MessageDetails msg = m_messages.CurrentMessage();
	DoPostEvent(msg.btn2);
	DoShowNextMessage();
}

void MessagePane::DoPostEvent(ButtonDetails btn)
{
	if (btn.commandId > 0 && btn.window) {
		if(btn.menuCommand) {
			wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, btn.commandId);
			btn.window->AddPendingEvent(evt);
		} else {
			wxCommandEvent evt(btn.commandId);
			btn.window->AddPendingEvent(evt);
		}
	}
}
