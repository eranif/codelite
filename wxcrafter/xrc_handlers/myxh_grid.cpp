/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_grid.cpp
// Purpose:     XML resource handler for the grid control
// Author:      Agron Selimaj
// Created:     2005/08/11
// RCS-ID:      $Id: xh_grid.cpp 67254 2011-03-20 00:14:35Z DS $
// Copyright:   (c) 2005 Agron Selimaj, Freepour Controls Inc.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wxgui_helpers.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifdef wxOVERRIDE
#undef wxOVERRIDE
#define wxOVERRIDE
#endif

#include "wx/grid.h"

#undef wxOVERRIDE
#define wxOVERRIDE override

#include "myxh_grid.h"
#include <wx/xml/xml.h>

MyWxGridXmlHandler::MyWxGridXmlHandler()
    : wxXmlResourceHandler()
{
    AddWindowStyles();
}

struct GridColInfo {
    typedef std::vector<GridColInfo> Vector_t;

    wxString label;
    long size;

    GridColInfo()
        : size(-1)
    {
    }
};

struct GridRowInfo {
    typedef std::vector<GridRowInfo> Vector_t;

    wxString label;
    long size;

    GridRowInfo()
        : size(-1)
    {
    }
};

wxObject* MyWxGridXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(grid, wxGrid);

    grid->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle(wxT("style")), GetName());

    // Extract columns info
    GridColInfo::Vector_t columns;
    wxXmlNode* columnsNode = GetParamNode(wxT("columns"));
    if(columnsNode) {
        wxXmlNode* colnode = columnsNode->GetChildren();
        while(colnode) {
            wxXmlNode* colAttr = colnode->GetChildren();
            GridColInfo col;
            while(colAttr) {
                if(colAttr->GetName() == "name") {
                    col.label = colAttr->GetNodeContent();

                } else if(colAttr->GetName() == "colsize") {
                    colAttr->GetNodeContent().ToCLong(&col.size);
                }
                colAttr = colAttr->GetNext();
            }
            columns.push_back(col);
            colnode = colnode->GetNext();
        }
    }

    // Extract rows info
    GridRowInfo::Vector_t rows;
    wxXmlNode* rowsNode = GetParamNode(wxT("rows"));
    if(rowsNode) {
        wxXmlNode* rownode = rowsNode->GetChildren();
        while(rownode) {
            wxXmlNode* rowAttr = rownode->GetChildren();
            GridRowInfo row;
            while(rowAttr) {
                if(rowAttr->GetName() == "name") {
                    row.label = rowAttr->GetNodeContent();

                } else if(rowAttr->GetName() == "rowsize") {
                    rowAttr->GetNodeContent().ToCLong(&row.size);
                }
                rowAttr = rowAttr->GetNext();
            }
            rows.push_back(row);
            rownode = rownode->GetNext();
        }
    }

    grid->CreateGrid(0, columns.size());
    for(size_t i = 0; i < columns.size(); ++i) {
        grid->SetColLabelValue(i, columns.at(i).label);

        if(columns.at(i).size != -1) { grid->SetColSize(i, columns.at(i).size); }
    }

    if(HasParam(wxT("autosizecol"))) {
        if(GetLong(wxT("autosizecol")) == 1) { grid->AutoSizeColumns(); }
    }

    if(HasParam(wxT("eidtable"))) { grid->EnableEditing((GetLong(wxT("editable")) == 1)); }

    // Append the rows
    for(size_t i = 0; i < rows.size(); ++i) {
        int rowid = grid->GetNumberRows();
        grid->AppendRows();
        grid->SetRowLabelValue(rowid, rows.at(i).label);
        if(rows.at(i).size != -1) { grid->SetRowSize(rowid, rows.at(i).size); }
    }

    // Alignment
    wxString rowLabelVAlgn = GetNodeContent("row-label-v-alignment");
    wxString rowLabelHAlgn = GetNodeContent("row-label-h-alignment");

    wxString colLabelVAlgn = GetNodeContent("col-label-v-alignment");
    wxString colLabelHAlgn = GetNodeContent("col-label-h-alignment");

    grid->SetRowLabelAlignment(wxCrafter::ToAligment(rowLabelHAlgn), wxCrafter::ToAligment(rowLabelVAlgn));
    grid->SetColLabelAlignment(wxCrafter::ToAligment(colLabelHAlgn), wxCrafter::ToAligment(colLabelVAlgn));

    grid->UseNativeColHeader(GetBool("native-header", true));
    // grid->SetUseNativeColLabels(GetBool("native-col-labels", true));

    long colLabelHeight = GetLong("col-label-size");
    long rowLabelWidth = GetLong("row-label-size");

    if(colLabelHeight != -1) { grid->SetColLabelSize(colLabelHeight); }

    if(rowLabelWidth != -1) { grid->SetRowLabelSize(rowLabelWidth); }

    SetupWindow(grid);
    return grid;
}

bool MyWxGridXmlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxT("wxGrid")); }

wxString MyWxGridXmlHandler::GetNodeContent(const wxString& name)
{
    wxXmlNode* node = GetParamNode(name);
    if(node) { return node->GetNodeContent(); }
    return wxEmptyString;
}
