#pragma once

#include "CustomControls.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK TextPreviewDialog : public TextPreviewDialogBase
{
public:
    TextPreviewDialog(wxWindow* parent);
    ~TextPreviewDialog() override;
    void SetPreviewText(const wxString& text);

protected:
    void OnCopy(wxCommandEvent& event) override;
    void OnCopyUI(wxUpdateUIEvent& event) override;
};
