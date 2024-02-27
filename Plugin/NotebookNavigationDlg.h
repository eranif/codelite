//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NotebookNavigationDlg.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef NOTEBOOKNAVIGATIONDLG_H
#define NOTEBOOKNAVIGATIONDLG_H

#include "Notebook.h"
#include "clAuiBook.hpp"
#include "clTab.h"
#include "globals.h"
#include "wxcrafter_plugin.h"

struct TabData {
    wxString label;
    wxBitmap bmp;
    wxFileName filename;
    int index;
    bool isFile;

    TabData()
        : index(wxNOT_FOUND)
        , isFile(false)
    {
    }
};

class WXDLLIMPEXP_SDK NotebookNavigationDlg : public NotebookNavigationDlgBase
{
    Notebook* m_notebook = nullptr;
    clAuiBook* m_aui_notebook = nullptr;
    int m_selection = wxNOT_FOUND;

public:
    NotebookNavigationDlg(wxWindow* parent, Notebook* book);
    NotebookNavigationDlg(wxWindow* parent, clAuiBook* book);
    ~NotebookNavigationDlg() override;
    void CloseDialog();

    int GetSelection() const { return m_selection; }

protected:
    void OnItemActivated(wxDataViewEvent& event) override;
    void OnKeyDown(wxKeyEvent& event) override;
    void OnKeyUp(wxKeyEvent& event) override;

    void SelectNext();
    void SelectPrev();

    void OnHotKeyNext(wxKeyEvent& event);
    void OnHotKeyPrev(wxKeyEvent& event);
    void FinalizeCtor();

    template <typename Book>
    void Initialise(Book* book);
};

template <typename Book>
void NotebookNavigationDlg::Initialise(Book* book)
{
    m_dvListCtrl->SetSortFunction(nullptr);
    clTab::Vec_t allTabs;
    clGetManager()->GetAllTabs(allTabs);
    std::map<void*, clTab> tabsInfoMap;
    for (size_t i = 0; i < allTabs.size(); ++i) {
        tabsInfoMap.insert(std::make_pair((void*)allTabs.at(i).window, allTabs.at(i)));
    }
    m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    m_dvListCtrl->SetColumnWidth(0, wxCOL_WIDTH_AUTOSIZE);
    m_dvListCtrl->SetColumnWidth(1, wxCOL_WIDTH_AUTOSIZE);
    clTabHistory::Ptr_t history = book->GetHistory();
    const std::vector<wxWindow*>& windows = history->GetHistory();
    // Populate the list
    for (size_t i = 0; i < windows.size(); ++i) {
        int index = book->GetPageIndex(windows[i]);
        if (index != wxNOT_FOUND) {
            wxString label = book->GetPageText(index);
            wxBitmap bmp = book->GetPageBitmap(index);

            wxVector<wxVariant> cols;
            wxVariant colPath;
            wxVariant colSaveIndicator;
            TabData* d = new TabData;
            d->bmp = bmp;
            d->label = label;
            d->index = index;

            // add extra info
            std::map<void*, clTab>::iterator iter = tabsInfoMap.find(windows[i]);
            wxVariant mod;
            int saveImgId = wxNOT_FOUND;
            if (iter != tabsInfoMap.end()) {
                d->isFile = iter->second.isFile;
                d->filename = iter->second.filename;
                if (iter->second.isModified) {
                    saveImgId = clGetManager()->GetStdIcons()->GetImageIndex(BitmapLoader::kSave);
                }
            }
            colSaveIndicator = ::MakeBitmapIndexText("", saveImgId);

            // Prepare the display item
            wxString text;
            if (d->isFile && d->filename.GetDirCount()) {
                wxFileName fn(d->filename.GetFullName());
                fn.AppendDir(d->filename.GetDirs().Last());
                text << fn.GetFullPath();
            } else {
                text << d->label;
            }

            // If the tab has a bitmap - use it
            // otherwise, try to assign one
            int imgId = clGetManager()->GetStdIcons()->GetMimeImageId(d->filename.GetFullName());
            colPath = ::MakeBitmapIndexText(text, imgId);
            cols.push_back(colPath);
            cols.push_back(colSaveIndicator);
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr)d);
        }
    }
}
#endif // NOTEBOOKNAVIGATIONDLG_H
