#pragma once

#include "clSTCContainerStylerBase.hpp"
#include "codelite_exports.h"

#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/stc/stc.h>

enum MarkdownStyles {
    kDefault,
    kHeader1,
    kHeader2,
    kHeader3,
    kHeader4,
    kHeader5,
    kHeader6,
    kHeaderText,
    kCodeBlockTag,
    kCodeBlockText,
    kCodeWord,
    kStrongText,
    kStrong2Text,
    kStrongTag,
    kStrong2Tag,
    kEmphasisText,
    kEmphasis2Text,
    kEmphasisTag,
    kEmphasis2Tag,
    kBacktick,
    kHorizontalLine,
    kListItem,
    kNumberedListItem,
    kNumberedListItemDot,
};

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
