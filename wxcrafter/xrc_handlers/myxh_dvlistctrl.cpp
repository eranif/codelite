#include "myxh_dvlistctrl.h"
#include <wx/dataview.h>
#include <wx/imaglist.h>
#include <wx/tokenzr.h>

MyWxDataViewListCtrlHandler::MyWxDataViewListCtrlHandler()
    : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxDV_SINGLE);
    XRC_ADD_STYLE(wxDV_MULTIPLE);
    XRC_ADD_STYLE(wxDV_ROW_LINES);
    XRC_ADD_STYLE(wxDV_HORIZ_RULES);
    XRC_ADD_STYLE(wxDV_VERT_RULES);
    XRC_ADD_STYLE(wxDV_VARIABLE_LINE_HEIGHT);
    XRC_ADD_STYLE(wxDV_NO_HEADER);
    AddWindowStyles();
}

wxObject* MyWxDataViewListCtrlHandler::DoCreateResource()
{
    if(m_class == "wxDataViewColumn") {
        HandleListCol();

    } else {
        wxASSERT_MSG(m_class == "wxDataViewListCtrl", wxT("can't handle unknown node"));

        return HandleListCtrl();
    }

    return m_parentAsWindow;
}

bool MyWxDataViewListCtrlHandler::CanHandle(wxXmlNode* node)
{
    return IsOfClass(node, "wxDataViewListCtrl") || IsOfClass(node, "wxDataViewColumn");
}

void MyWxDataViewListCtrlHandler::HandleListCol()
{
    // Add column to the wxDataViewListCtrl
    wxDataViewListCtrl* const list = wxDynamicCast(m_parentAsWindow, wxDataViewListCtrl);
    wxCHECK_RET(list, wxT("must have wxDataViewListCtrl parent"));

    if(!HasParam(wxT("coltype"))) { return; }

    // Column properties
    wxString coltype = GetText("coltype");
    int colwidth = GetLong("width", -1);
    wxString label = GetText("label");
    wxString salign = GetText("align", false);
    int style = GetLong("style", 0);
    wxString cellmode = GetText("cellmode", false);

    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT;
    if(cellmode == "wxDATAVIEW_CELL_ACTIVATABLE")
        mode = wxDATAVIEW_CELL_ACTIVATABLE;
    else if(cellmode == "")
        mode = wxDATAVIEW_CELL_EDITABLE;

    wxAlignment al = wxALIGN_LEFT;
    if(salign == "wxALIGN_RIGHT")
        al = wxALIGN_RIGHT;
    else if(salign == "wxALIGN_CENTER")
        al = wxALIGN_CENTER;

    if(coltype == "bitmap") {
        list->AppendBitmapColumn(label, list->GetColumnCount(), mode, colwidth, al, style);

    } else if(coltype == "check") {
        list->AppendToggleColumn(label, mode, colwidth, al, style);

    } else if(coltype == "text") {
        list->AppendTextColumn(label, mode, colwidth, al, style);

    } else if(coltype == "icontext") {
        list->AppendIconTextColumn(label, mode, colwidth, al, style);

    } else if(coltype == "progress") {
        list->AppendProgressColumn(label, mode, colwidth, al, style);

    } else if(coltype == "choice") {
        wxString content = GetText("choices", false);
        wxArrayString choices = ::wxStringTokenize(content, ",", wxTOKEN_STRTOK);
        list->AppendColumn(new wxDataViewColumn(label,
                                                new wxDataViewChoiceRenderer(choices, mode, wxDVR_DEFAULT_ALIGNMENT),
                                                list->GetColumnCount(), colwidth, al, style));
    }
}

wxDataViewListCtrl* MyWxDataViewListCtrlHandler::HandleListCtrl()
{
    XRC_MAKE_INSTANCE(list, wxDataViewListCtrl)

    list->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());
    list->SetName(GetName());
    CreateChildrenPrivately(list);
    SetupWindow(list);
    return list;
}
