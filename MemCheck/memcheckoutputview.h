//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : memcheckoutputview.h
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

/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#ifndef _MEMCHECKOUTPUTVIEW_H_
#define _MEMCHECKOUTPUTVIEW_H_

#include <wx/valnum.h>
#include <wx/tipwin.h>

#include "memcheck.h"
#include "memcheckui.h"
#include "imemcheckprocessor.h"

class MemCheckOutputView: public MemCheckOutputViewBase
{
public:
    MemCheckOutputView(wxWindow * parent, MemCheckPlugin * plugin, IManager * mgr);
    virtual ~ MemCheckOutputView();

protected:
    virtual void OnClearOutputUpdateUI(wxUpdateUIEvent& event);
    virtual void OnClearOutput(wxCommandEvent& event);
    virtual void OnStop(wxCommandEvent& event);
    virtual void OnStopUI(wxUpdateUIEvent& event);
    virtual void OnListCtrlErrorsMouseLeave(wxMouseEvent& event);
    virtual void OnListCtrlErrorsChar(wxKeyEvent& event);
    virtual void OnListCtrlErrorsKeyDown(wxKeyEvent& event);
    virtual void OnListCtrlErrorsKeyUp(wxKeyEvent& event);
    virtual void OnListCtrlErrorsLeftDown(wxMouseEvent& event);
    virtual void OnListCtrlErrorsLeftUp(wxMouseEvent& event);
    virtual void OnListCtrlErrorsDeselected(wxListEvent& event);
    virtual void OnListCtrlErrorsSelected(wxListEvent& event);
    virtual void OnListCtrlErrorsActivated(wxListEvent& event);
    virtual void OutputViewPageChanged(wxNotebookEvent& event);
    virtual void OnSuppFileOpen(wxCommandEvent& event);
    virtual void OnErrorsPanelUI(wxUpdateUIEvent& event);
    virtual void OnSuppPanelUI(wxUpdateUIEvent& event);
    virtual void OnListCtrlErrorsMouseMotion(wxMouseEvent& event);
    virtual void OnListCtrlErrorsResize(wxSizeEvent& event);
    virtual void OnPageSelect(wxCommandEvent& event);
    virtual void OnClearFilter(wxCommandEvent& event);
    virtual void OnPageFirst(wxCommandEvent& event);
    virtual void OnPageLast(wxCommandEvent& event);
    virtual void OnPageNext(wxCommandEvent& event);
    virtual void OnPagePrev(wxCommandEvent& event);
    virtual void OnSearchNonworkspace(wxCommandEvent& event);
    virtual void OnOpenPlain(wxCommandEvent& event);
    virtual void OnFilterErrors(wxCommandEvent& event);
    virtual void OnSuppFileSelected(wxCommandEvent& event);
    virtual void OnSuppressAll(wxCommandEvent& event);
    virtual void OnSuppressSelected(wxCommandEvent& event);
    virtual void OnSelectionChanged(wxDataViewEvent& event);
    virtual void OnContextMenu(wxDataViewEvent& event);
    virtual void OnMemCheckUI(wxUpdateUIEvent& event);
    virtual void OnActivated(wxDataViewEvent& event);
    virtual void OnJumpToNext(wxCommandEvent& event);
    virtual void OnJumpToPrev(wxCommandEvent& event);
    virtual void OnExpandAll(wxCommandEvent& event);



    //common things for both notebooks
    MemCheckPlugin * m_plugin;
    IManager *m_mgr;
    wxString m_workspacePath;
    enum {
        SUPPRESS_CLICKED  = 1 << 1, ///< on tree view page supress one error
        SUPPRESS_CHECKED  = 1 << 2, ///< on tree view page supress all checked
        SUPPRESS_ALL      = 1 << 3, ///< on supp page supress all
        SUPPRESS_SELECTED = 1 << 4, ///< on supp page supress selected
    };
    bool itemsInvalidView; ///< on supp page have been some items suppressed => view page is invalid
    bool itemsInvalidSupp; ///< on tree view page have been some items suppressed => supp page is invalid
    void ResetItemsView(); ///< make tree view page valid = count items and save it to "m_totalErrorsView"
    void ResetItemsSupp(); ///< make supp page valid = count items and save it to "m_totalErrorsSupp"
    
    /**
     * @brief Perform all kinds of suppressions from both pages
     * @param mode SUPPRESS_CLICKED | SUPPRESS_CHECKED | SUPPRESS_ALL | SUPPRESS_SELECTED
     * @param dvItem is set only if mode is SUPPRESS_CLICKED
     *
     * By suppressing an item(error) is meant add suppression patern for that error into supp file
     */
    void SuppressErrors(unsigned int mode, wxDataViewItem* dvItem = NULL);


    /////////////////////////////////////////////////////////////////////////////////////////////////


    //tree view page
    wxIntegerValidator<size_t> pageValidator; ///< validator for wxTextCtrl which changes page on page view
    bool m_currentPageIsEmptyView;
    wxDataViewItem m_currentItem;
    bool m_onValueChangedLocked; ///< if user (un)checks an item, all items in its tree must be (un)checked. This action is trigered by OnValueChanged callback. Problem is that if an item is checked is also invoked that callback. So this lock brakes the infinite loop.
    size_t m_totalErrorsView;
    size_t m_currentPage;
    size_t m_pageMax;

    wxDataViewItem GetTopParent(wxDataViewItem item); ///< get top level item for an item
    wxDataViewItem GetLeaf(const wxDataViewItem &item, bool first); ///< get deepes item for an item(error), first == true means firts from top, first==false means last.
    wxDataViewItem GetAdjacentItem(const wxDataViewItem &item, bool forward); ///< for an item(error or location) get adjecent item. this is used for next/prev functionality. Forward == true means item below, forward == false means item above.
    void ExpandAll(const wxDataViewItem & item); ///< wxDVC doesn't implenet ExpandAll, so this is it
    void SetCurrentItem(const wxDataViewItem &item); ///< marks current item with little green right arrow
    void MarkTree(const wxDataViewItem &item, bool checked); ///< (un)checks all items that belong to a particular error
    void MarkAllErrors(bool state); // (Un)Marks all errors. Called by On(un)MarkAllErrors()
    void GetStatusOfErrors(bool& unmarked, bool& marked); // Are there any unmarked, any marked errors?
    unsigned int GetColumnByName(const wxString & name); ///< Finds index of an wxDVC column by its caption
    void JumpToLocation(const wxDataViewItem &item); ///< Opens file specifieed in particular ErrorLocation in editor
    void ShowPageView(size_t page); ///< Item could be more than is good for wxDVC. So paging is implementetd. This method fills wxDVC with portion of errors.
    void AddTree(const wxDataViewItem & parentItem, MemCheckError & error); ///< Adds one error and all its location into wxDVC as tree
    void OnJumpToLocation(wxCommandEvent & event); ///< Callback from wxDVC popupmenu
    void OnMarkAllErrors(wxCommandEvent & event); ///< Callback from wxDVC popupmenu
    void OnUnmarkAllErrors(wxCommandEvent & event); ///< Callback from wxDVC popupmenu
    void OnSuppressError(wxCommandEvent & event); ///< Callback from wxDVC popupmenu
    void OnSuppressMarkedErrors(wxCommandEvent & event); ///< Callback from wxDVC popupmenu
    void OnRowToClip(wxCommandEvent& event); ///< Callback from wxDVC popupmenu. Puts row as string into clipboard.
    void OnErrorToClip(wxCommandEvent& event); ///< Callback from wxDVC popupmenu. Puts whole error as string into clipboard.
    void OnMarkedErrorsToClip(wxCommandEvent& event); ///< Callback from wxDVC popupmenu. Puts checked error as string into clipboard.




    //supp page
    enum {
        FILTER_CLEAR     = 1 << 1, ///< on supp page, clear the filter => show all errors
        FILTER_STRING    = 1 << 2, ///< on supp page, filter by string or reg exp
        FILTER_WORKSPACE = 1 << 3, ///< on supp page, filter errors which have at least on file from current workspace in its stack trace.
    };
    wxMenu* m_searchMenu; ///< wxSearchCtrl popupmenu
    size_t m_totalErrorsSupp; ///< Total items in wxListCtrl.
    std::vector<MemCheckError *> m_filterResults; ///< Contetn of wxListCtrl.
    long m_lastToolTipItem; ///< On hover over wxListCtrl tooltip is shown. It is refreshed only if user hovers another item, not if moves by one pixel.

    void ApplyFilterSupp(unsigned int mode); ///< Performs filtering errors. Searches in whole ErrorList structure. Mode is FILTER_CLEAR | FILTER_STRING | FILTER_WORKSPACE.
    void UpdateStatusSupp(); ///< Shows number of error total / filtered /selected
    void ListCtrlErrorsShowTip(long item); ///< Sets proper tooltip for wxListCtrl. Item is index in m_filterResults.

public:
    /**
     * @brief Load ErrorList into wxDVC and wxListCtrl on tree view and supp page.
     *
     * MemCheck plugin calls this method after test ends and after processor parses logfile into ErrorList.
     */
    void LoadErrors();
    /**
     * @brief clear the content
     */
    void Clear();
};

#endif // _MEMCHECKOUTPUTVIEW_H_
