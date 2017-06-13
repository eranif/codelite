#include "clStatusBarMessage.h"
#include "globals.h"
#include "imanager.h"

clStatusBarMessage::clStatusBarMessage(const wxString& startMessage, const wxString& endMessage)
    : m_startMessage(startMessage)
    , m_endMessage(endMessage)
{
    clGetManager()->SetStatusMessage(m_startMessage);
    if(m_endMessage.IsEmpty()) {
        m_endMessage = clGetManager()->GetStatusBar()->GetText();
    }
}

clStatusBarMessage::~clStatusBarMessage() { clGetManager()->SetStatusMessage(m_endMessage); }
