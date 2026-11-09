#pragma once

#include "clFindResultsStyler.h"

class SFTPGrepStyler : public clFindResultsStyler
{
public:
    SFTPGrepStyler(wxStyledTextCtrl* stc);
    SFTPGrepStyler() = default;
    virtual ~SFTPGrepStyler() = default;

    void StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasScope);
    void Reset();
};
