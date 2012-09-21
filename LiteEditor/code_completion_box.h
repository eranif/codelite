#ifndef CODECOMPLETIONBOX_H
#define CODECOMPLETIONBOX_H

#include "cl_editor.h"
#include <wx/event.h>
#include <wx/bitmap.h>

class CCBoxTipWindow;
class CCBox;
class CodeCompletionBox : public wxEvtHandler
{
    typedef std::map<wxString, wxBitmap> BitmapMap_t;
    
    CCBox*          m_ccBox;
    CCBoxTipWindow* m_tip;
    BitmapMap_t     m_bitmaps;
    
private:
    CodeCompletionBox();
    
protected:
    void OnCCBoxDismissed(wxCommandEvent &e);
    void DoCreateBox(LEditor *editor);
    
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
    /**
     * @brief display a tooltip at the caret position of the editor
     */
    void ShowTip(const wxString &msg, LEditor* editor);
    
    /**
     * @brief display a tooltip at a given point
     * 'pt' is in screen coordinates
     */
    void ShowTip(const wxString &msg, const wxPoint& pt);
    
    /**
     * @brief dismiss the last calltip shown
     */
    void CancelTip();
    
    /**
     * @brief add an additional image to the code completion box for a given 
     * "kind"
     */
    void RegisterImage(const wxString &kind, const wxBitmap& bmp);
};

#endif // CODECOMPLETIONBOX_H
