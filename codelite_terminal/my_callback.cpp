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
    wxString message;
    message << "[" << m_frame->m_process->GetPid() << "] Done\n";
    wxStyledTextCtrl *stc = m_frame->m_stc;
    stc->AppendText( message );
    
    wxDELETE(m_frame->m_process);
    m_frame->SetCartAtEnd();
    
    if ( m_frame->GetOptions().HasFlag( TerminalOptions::kExitWhenInfiriorTerminates ) ) {
        m_frame->CallAfter( &MainFrame::Exit );
    }
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------

void PtyCallback::OnProcessOutput(const wxString& str)
{
    wxStyledTextCtrl *stc = m_frame->m_stc;
    stc->AppendText( str );
    m_frame->SetCartAtEnd();
}

void PtyCallback::OnProcessTerminated()
{
    wxString message;
    message << "[" << m_frame->m_process->GetPid() << "] Done\n";
    wxStyledTextCtrl *stc = m_frame->m_stc;
    stc->AppendText( message );
    m_frame->SetCartAtEnd();
}

PtyCallback::PtyCallback(MainFrame* frame)
    : m_frame(frame)
{
}

PtyCallback::~PtyCallback()
{
}
