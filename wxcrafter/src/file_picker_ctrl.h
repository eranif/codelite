#ifndef FILEPICKERCTRL_H
#define FILEPICKERCTRL_H

#include <wx/textctrl.h>

extern const wxEventType wxEVT_FILE_PICKED;

class FilePickerCtrl : public wxTextCtrl
{
    wxString m_bmpPath;
    wxString m_projectPath;

protected:
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnMouseLeft(wxMouseEvent& event);

    void DoEdit();
    void DoNotify();

public:
    FilePickerCtrl(wxWindow* parent, const wxString& projectPath, const wxString& value);
    virtual ~FilePickerCtrl();
};

#endif // FILEPICKERCTRL_H
