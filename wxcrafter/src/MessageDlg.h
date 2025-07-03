#ifndef MESSAGEDLG_H
#define MESSAGEDLG_H

#include "wxcrafter.h"

class MessageDlg : public MessageDlgBaseClass
{
public:
    MessageDlg(wxWindow* parent, const wxString& msg, const wxString& title);
    ~MessageDlg() override = default;

    // Accessors
    void SetMessage(const wxString& msg);
    void SetTitle(const wxString& title) override;
    bool IsDontAnnoyChecked() const;
};
#endif // MESSAGEDLG_H
