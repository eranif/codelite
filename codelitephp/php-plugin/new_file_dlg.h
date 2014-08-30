#ifndef __new_file_dlg__
#define __new_file_dlg__

#include "php_ui.h"

class NewFileDlg : public NewFileDlgBase
{
public:
    /** Constructor */
    NewFileDlg( wxWindow* parent, const wxString &path );
    virtual ~NewFileDlg();
    wxFileName GetPath() const;
};

#endif // __new_file_dlg__

