#ifndef SFTPSTATUSPAGE_H
#define SFTPSTATUSPAGE_H
#include "UI.h"

class SFTPWriterThreadMessage;
class SFTPStatusPage : public SFTPStatusPageBase
{
    SFTPImages m_bitmaps;
public:
    SFTPStatusPage(wxWindow* parent);
    virtual ~SFTPStatusPage();
    
    void AddLine( SFTPWriterThreadMessage* message );
    void ShowContextMenu();
    
protected:
    virtual void OnContentMenu(wxDataViewEvent& event);
    virtual void OnClearLog(wxCommandEvent& event);
    
};
#endif // SFTPSTATUSPAGE_H
