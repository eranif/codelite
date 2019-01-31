#ifndef MYXH_TREELIST_H
#define MYXH_TREELIST_H

#include <wx/xrc/xmlres.h>

class wxTreeListCtrl;
class MyTreeListCtrl : public wxXmlResourceHandler
{
public:
    MyTreeListCtrl();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);

private:
    // handlers for wxListCtrl itself and its listcol and listitem children
    wxTreeListCtrl* HandleListCtrl();
    void HandleListCol();
};

#endif // MYXH_TREELIST_H
