#pragma once

#include <wx/xrc/xmlreshandler.h>

class wxDataViewTreeCtrl;

class MyWxDataViewTreeCtrlHandler : public wxXmlResourceHandler
{
public:
    MyWxDataViewTreeCtrlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    // handlers for wxListCtrl itself and its listcol and listitem children
    wxDataViewTreeCtrl* HandleListCtrl();
    void HandleListCol();
};
