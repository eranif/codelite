#ifndef MESSAGEDLG_H
#define MESSAGEDLG_H

#include "wxcrafter.h"

class MessageDlg : public MessageDlgBaseClass
{
public:
    MessageDlg(wxWindow* parent, const wxString& msg, const wxString& title);
    virtual ~MessageDlg();

    // Accessors
    void SetMessage(const wxString& msg);
    void SetTitle(const wxString& title);
    bool IsDontAnnoyChecked() const;
};
#endif // MESSAGEDLG_H
