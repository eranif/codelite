#ifndef MESSAGEDLG_H
#define MESSAGEDLG_H

#include "wxcrafter.hpp"

class MessageDlg : public MessageDlgBaseClass
{
public:
    MessageDlg(wxWindow* parent, const wxString& msg, const wxString& title);
    ~MessageDlg() override = default;

    // Accessors
    void SetMessage(const wxString& msg);
    void SetTitle(const wxString& title) override;
};
#endif // MESSAGEDLG_H
