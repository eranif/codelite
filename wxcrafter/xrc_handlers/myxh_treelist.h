#ifndef MYXH_TREELIST_H
#define MYXH_TREELIST_H

#include <wx/xrc/xmlreshandler.h>

class wxTreeListCtrl;
class MyTreeListCtrl : public wxXmlResourceHandler
{
public:
    MyTreeListCtrl();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    // handlers for wxListCtrl itself and its listcol and listitem children
    wxTreeListCtrl* HandleListCtrl();
    void HandleListCol();
};

#endif // MYXH_TREELIST_H
