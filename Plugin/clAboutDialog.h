#ifndef CLABOUTDIALOG_H
#define CLABOUTDIALOG_H

#include "clAboutDialogBase.h"

#include <codelite_exports.h>

class WXDLLIMPEXP_SDK clAboutDialog : public clAboutDialogBase
{
public:
    clAboutDialog(wxWindow* parent, const wxString& version);
    virtual ~clAboutDialog();
};
#endif // CLABOUTDIALOG_H
