#pragma once

#include "wxcrafter.hpp"

class ImportThemesDialog : public ImportThemesDialogBase
{
public:
    ImportThemesDialog(wxWindow* parent);
    virtual ~ImportThemesDialog();

    wxString GetInputDirectory() const;
    wxString GetOutputDirectory() const;
};
