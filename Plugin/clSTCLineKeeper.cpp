#include "clSTCLineKeeper.h"
#include "clEditorStateLocker.h"

clSTCLineKeeper::clSTCLineKeeper(IEditor* editor)
{
    m_locker = new clEditorStateLocker(editor->GetSTC());
}

clSTCLineKeeper::clSTCLineKeeper(wxStyledTextCtrl* stc)
{
    m_locker = new clEditorStateLocker(stc);
}

clSTCLineKeeper::~clSTCLineKeeper()
{
    wxDELETE(m_locker);
}

