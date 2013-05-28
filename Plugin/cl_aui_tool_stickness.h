#ifndef CLAUITOOLSTICKNESS_H
#define CLAUITOOLSTICKNESS_H

#include <wx/aui/auibar.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clAuiToolStickness
{
    wxAuiToolBar* m_tb;
    wxAuiToolBarItem* m_item;
    
public:
    clAuiToolStickness(wxAuiToolBar* tb, int toolId);
    virtual ~clAuiToolStickness();

};

#endif // CLAUITOOLSTICKNESS_H
