#ifndef CSSCODECOMPLETION_H
#define CSSCODECOMPLETION_H

#include <wx/event.h> // Base class: wxEvtHandler
#include "smart_ptr.h"
#include <vector>
#include <wx/arrstr.h>

class IEditor;
class CSSCodeCompletion : public wxEvtHandler
{

public:
    typedef SmartPtr<CSSCodeCompletion> Ptr_t;
    struct Entry {
        wxString property;
        wxArrayString values;
        typedef std::vector<Entry> Vec_t;
    };

protected:
    bool m_isEnabled;
    Entry::Vec_t m_entries;

protected:
    wxString GetPreviousWord(IEditor* editor, int pos);

public:
    CSSCodeCompletion();
    virtual ~CSSCodeCompletion();

    void CssCodeComplete(IEditor* editor);
};

#endif // CSSCODECOMPLETION_H
