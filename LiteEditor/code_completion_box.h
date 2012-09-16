#ifndef CODECOMPLETIONBOX_H
#define CODECOMPLETIONBOX_H

#include "cl_editor.h"
#include <wx/event.h>

class CodeCompletionBox : public wxEvtHandler
{
    CCBox* m_ccBox;
    
private:
    CodeCompletionBox();
    
protected:
    void OnCCBoxDismissed(wxCommandEvent &e);
    
public:
    virtual ~CodeCompletionBox();
    static CodeCompletionBox& Get();
    
    void Display(LEditor* editor, const TagEntryPtrVector_t& tags, const wxString &word, bool isKeywordList, wxEvtHandler* owner = NULL);
    void Hide();
    bool IsShown() const;
    bool SelectWord(const wxString &word);
    void InsertSelection();
    void Previous();
    void Next();
    void PreviousPage();
    void NextPage();
};

#endif // CODECOMPLETIONBOX_H
