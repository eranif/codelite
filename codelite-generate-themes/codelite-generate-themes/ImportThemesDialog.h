#ifndef IMPORTTHEMESDIALOG_H
#define IMPORTTHEMESDIALOG_H
#include "wxcrafter.h"

class ImportThemesDialog : public ImportThemesDialogBase
{
public:
    ImportThemesDialog(wxWindow* parent);
    virtual ~ImportThemesDialog();

    wxString GetInputDirectory() const;
    wxString GetOutputDirectory() const;
};
#endif // IMPORTTHEMESDIALOG_H
