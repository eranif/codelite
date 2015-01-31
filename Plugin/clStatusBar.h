#ifndef CLSTATUSBAR_H
#define CLSTATUSBAR_H

#include "wxCustomStatusBar.h" // Base class: wxCustomStatusBar
#include "codelite_exports.h"

class IManager;
class WXDLLIMPEXP_SDK clStatusBar : public wxCustomStatusBar
{
    IManager* m_mgr;

protected:
    void OnPageChanged(wxCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);
    void DoUpdateColour();
    void DoSetLinePosColumn(const wxString &message);
    
public:
    clStatusBar(wxWindow* parent, IManager* mgr);
    virtual ~clStatusBar();

    /**
     * @brief set a status bar message
     */
    void SetMessage(const wxString& message);

    /**
     * @brief update the file name part of the status bar
     * @param filename
     */
    void SetFileName(const wxString& filename);

    /**
     * @brief update the language field
     */
    void SetLanguage(const wxString& lang);

    /**
     * @brief update the line / column / pos field
     */
    void SetLinePosColumn(const wxString& lineCol);
};

#endif // CLSTATUSBAR_H
