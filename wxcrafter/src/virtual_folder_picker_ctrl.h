#ifndef VFOLDER_PICKER_CTRL_H
#define VFOLDER_PICKER_CTRL_H

#include <wx/textctrl.h>

extern const wxEventType wxEVT_VIRTUAL_FOLDER_PICKED;

class VirtualFolderPickerCtrl : public wxTextCtrl
{
    wxString m_virtualFolder;

protected:
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnMouseLeft(wxMouseEvent& event);

    void DoEdit();
    void DoNotify();

public:
    VirtualFolderPickerCtrl(wxWindow* parent, const wxString& value);
    virtual ~VirtualFolderPickerCtrl();
};

#endif // VFOLDER_PICKER_CTRL_H
