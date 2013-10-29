#include "my_callback.h"
#include "MainFrame.h"

MyCallback::MyCallback(MainFrame* frame)
    : m_frame(frame)
{
}

MyCallback::~MyCallback()
{
}

void MyCallback::OnProcessOutput(const wxString& str)
{
    wxStyledTextCtrl *stc = m_frame->m_stc;
    stc->AppendText( str );
    m_frame->SetCartAtEnd();
}

void MyCallback::OnProcessTerminated()
{
    wxDELETE(m_frame->m_process);
    m_frame->SetCartAtEnd();
}
