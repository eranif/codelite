#ifndef SFTPGREPSTYLER_H
#define SFTPGREPSTYLER_H

#include "clFindResultsStyler.h"

class SFTPGrepStyler : public clFindResultsStyler
{
public:
    SFTPGrepStyler(wxStyledTextCtrl* stc);
    SFTPGrepStyler();
    virtual ~SFTPGrepStyler();

    void StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasScope);
    void Reset();
};

#endif // SFTPGREPSTYLER_H
