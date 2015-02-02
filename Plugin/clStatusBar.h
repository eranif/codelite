#ifndef CLSTATUSBAR_H
#define CLSTATUSBAR_H

#include "wxCustomStatusBar.h" // Base class: wxCustomStatusBar
#include "codelite_exports.h"
#include <wx/bitmap.h>

class IManager;
class WXDLLIMPEXP_SDK clStatusBar : public wxCustomStatusBar
{
    IManager* m_mgr;
    wxBitmap m_bmpBuild;
    wxBitmap m_bmpBuildError;
    wxBitmap m_bmpBuildWarnings;
    
protected:
    void OnPageChanged(wxCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void OnBuildStarted(clBuildEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    void OnWorkspaceClosed(wxCommandEvent &event);
    
    void DoUpdateColour();
    void DoSetLinePosColumn(const wxString &message);
    
    void SetBuildBitmap(const wxBitmap& bmp);
    
public:
    clStatusBar(wxWindow* parent, IManager* mgr);
    virtual ~clStatusBar();
    
    /**
     * @brief clear all text fields from the status bar
     */
    void Clear();
    
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
