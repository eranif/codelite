#ifndef STCLINEKEEPER_H
#define STCLINEKEEPER_H

/**
 * @class STCLineKeeper
 * a helper class that restores STC first visible line
 */
#include <wx/stc/stc.h>
#include "ieditor.h"
#include "codelite_exports.h"

// For backward compatibility we keep this class,
// but now its just a wrapper to clEditorStateLocker
class clEditorStateLocker;
class WXDLLIMPEXP_SDK clSTCLineKeeper
{
    clEditorStateLocker* m_locker;
public:
    clSTCLineKeeper(IEditor* editor);
    clSTCLineKeeper(wxStyledTextCtrl* stc);
    virtual ~clSTCLineKeeper();

};

#endif // STCLINEKEEPER_H
