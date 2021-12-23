#include "clSTCLineKeeper.h"

#include "clEditorStateLocker.h"

clSTCLineKeeper::clSTCLineKeeper(IEditor* editor) { m_locker = new clEditorStateLocker(editor->GetCtrl()); }

clSTCLineKeeper::clSTCLineKeeper(wxStyledTextCtrl* stc) { m_locker = new clEditorStateLocker(stc); }

clSTCLineKeeper::~clSTCLineKeeper() { wxDELETE(m_locker); }
