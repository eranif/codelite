#ifndef CLABOUTDIALOG_H
#define CLABOUTDIALOG_H

#include "clAboutDialogBase.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clAboutDialog : public clAboutDialogBase
{
public:
    clAboutDialog(wxWindow* parent, const wxString& version);
    ~clAboutDialog() override;
};
#endif // CLABOUTDIALOG_H
