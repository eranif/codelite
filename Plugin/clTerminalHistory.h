#ifndef CLTERMINALHISTORY_H
#define CLTERMINALHISTORY_H

#include "codelite_exports.h"

#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clTerminalHistory
{
    std::vector<wxString> m_history;
    int m_where;

protected:
    void DoReset();

public:
    clTerminalHistory();
    virtual ~clTerminalHistory();

    void Add(const wxString& command);
    const wxString& ArrowUp();
    const wxString& ArrowDown();
    wxArrayString GetItems() const;
    void SetItems(const wxArrayString& items);
};

#endif // CLTERMINALHISTORY_H
