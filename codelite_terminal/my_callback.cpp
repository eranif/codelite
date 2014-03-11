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
    m_frame->AppendOutputText( str );
}

void MyCallback::OnProcessTerminated()
{
    // if the process linked to this callback object is the same as the one in the main frame
    // it means that this is the "sync" process - we need to delete it as well from the main
    // frame
    if ( m_frame->m_process == m_process ) {
        wxDELETE(m_frame->m_process);
    }
    
    m_frame->SetCartAtEnd();
    if ( m_frame->GetOptions().HasFlag( TerminalOptions::kExitWhenInfiriorTerminates ) ) {
        m_frame->CallAfter( &MainFrame::Exit );
    }
    delete this;
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
