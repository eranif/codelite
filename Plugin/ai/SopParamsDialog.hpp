#pragma once

#include "ai/SopParser.hpp"
#include "ai_UI.hpp"
#include "codelite_exports.h"

#include <vector>
#include <wx/dialog.h>
#include <wx/textctrl.h>

class WXDLLIMPEXP_SDK SopParamsDialog : public SopParamsDialogBase
{
public:
    SopParamsDialog(wxWindow* parent, const SopInfo& sop);
    ~SopParamsDialog() override = default;

    std::vector<std::pair<wxString, wxString>> GetParams() const;

private:
    std::vector<std::pair<wxString, wxTextCtrl*>> m_textEntries;
};
