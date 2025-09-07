#pragma once

#include "clSTCContainerStylerBase.hpp"
#include "codelite_exports.h"

#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK MarkdownStyler : public clSTCContainerStylerBase
{
public:
    MarkdownStyler(wxStyledTextCtrl* ctrl);
    virtual ~MarkdownStyler() = default;
    void InitStyles() override;
    void Reset() override;

private:
    void OnStyle(clSTCAccessor& accessor);
};
