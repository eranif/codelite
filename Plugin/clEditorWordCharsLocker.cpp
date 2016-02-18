#include "clEditorWordCharsLocker.h"

clEditorWordCharsLocker::clEditorWordCharsLocker(wxStyledTextCtrl* stc, const wxString& newWordChars)
    : m_stc(stc)
{
    m_origWordChars = m_stc->GetWordChars();
    m_stc->SetWordChars(newWordChars);
}

clEditorWordCharsLocker::~clEditorWordCharsLocker() { m_stc->SetWordChars(m_origWordChars); }
