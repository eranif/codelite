#ifndef STCLINEKEEPER_H
#define STCLINEKEEPER_H

/**
 * @class STCLineKeeper
 * a helper class that restores STC first visible line
 */
#include <wx/stc/stc.h>
#include "ieditor.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clSTCLineKeeper
{
    wxStyledTextCtrl* m_stc;
    int m_firstVisibleLine;

public:
    clSTCLineKeeper(IEditor* editor);
    clSTCLineKeeper(wxStyledTextCtrl* stc);
    virtual ~clSTCLineKeeper();

};

#endif // STCLINEKEEPER_H
