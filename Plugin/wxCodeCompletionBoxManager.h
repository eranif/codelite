#ifndef WXCODECOMPLETIONBOXMANAGER_H
#define WXCODECOMPLETIONBOXMANAGER_H

#include <wx/event.h>
#include "wxCodeCompletionBox.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK wxCodeCompletionBoxManager : public wxEvtHandler
{
    friend class wxCodeCompletionBox;

protected:
    wxCodeCompletionBox* m_box;

private:
    wxCodeCompletionBoxManager();
    ~wxCodeCompletionBoxManager();

protected:
    void WindowDestroyed();
    void DestroyCurrent();
    
public:
    static wxCodeCompletionBoxManager& Get();

    /**
     * @brief show the completion box
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl,
                           const wxCodeCompletionBoxEntry::Vec_t& entries,
                           wxEvtHandler* eventObject = NULL);
    /**
     * @brief show the completion box
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl,
                           const wxCodeCompletionBoxEntry::Vec_t& entries,
                           const wxCodeCompletionBox::BmpVec_t& bitmaps,
                           wxEvtHandler* eventObject = NULL);

    /**
     * @brief show the completion box
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrVector_t& tags, wxEvtHandler* eventObject = NULL);

    /**
     * @brief do we have a completion box shown?
     */
    bool IsShown() const { return m_box != NULL; }
};

#endif // WXCODECOMPLETIONBOXMANAGER_H
