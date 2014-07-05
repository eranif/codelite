#include "clSTCLineKeeper.h"

clSTCLineKeeper::clSTCLineKeeper(IEditor* editor)
    : m_stc(editor->GetSTC())
{
    m_firstVisibleLine = m_stc->GetFirstVisibleLine();
}

clSTCLineKeeper::clSTCLineKeeper(wxStyledTextCtrl* stc)
    : m_stc(stc)
{
    m_firstVisibleLine = m_stc->GetFirstVisibleLine();
}

clSTCLineKeeper::~clSTCLineKeeper()
{
    if ( m_stc ) {
        m_stc->SetFirstVisibleLine( m_firstVisibleLine );
    }
}

