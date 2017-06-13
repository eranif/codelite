#include "clStatusBarMessage.h"
#include "globals.h"
#include "imanager.h"

clStatusBarMessage::clStatusBarMessage(const wxString& startMessage, const wxString& endMessage)
    : m_startMessage(startMessage)
    , m_endMessage(endMessage)
{
    if(m_endMessage.IsEmpty()) {
        m_endMessage = clGetManager()->GetStatusBar()->GetText();
    }
    clGetManager()->SetStatusMessage(m_startMessage);
}

clStatusBarMessage::~clStatusBarMessage() { clGetManager()->SetStatusMessage(m_endMessage); }
