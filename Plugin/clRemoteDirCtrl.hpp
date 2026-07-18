#ifndef CLREMOTEDIRCTRL_HPP
#define CLREMOTEDIRCTRL_HPP
#include "DataViewTypeHelper.h"
#if USE_SFTP

#include "codelite_events.h"
#include "ssh/cl_sftp.h"
#include "ssh/cl_ssh.h"
#include "ssh/ssh_account_info.h"

#include <vector>
#include <wx/dataview.h>
#include <wx/panel.h>

/// A thin native wxDataViewTreeCtrl subclass exposing the (invisible) root item,
/// mirroring clFileViewerTreeCtrl::GetRootItem() (see Plugin/clFileViwerTreeCtrl.h).
class WXDLLIMPEXP_SDK clRemoteDirTreeCtrl : public wxDataViewTreeCtrl
{
public:
    clRemoteDirTreeCtrl(wxWindow* parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDV_MULTIPLE | wxDV_ROW_LINES | wxDV_NO_HEADER)
        : wxDataViewTreeCtrl(parent, id, pos, size, style)
    {
        // wxDataViewTreeCtrl::Create() sets up its only column as wxDATAVIEW_CELL_EDITABLE.
        // We don't support inline rename here, so force the cell back to non-editable.
        if (wxDataViewColumn* col = GetColumn(0)) {
            if (wxDataViewRenderer* renderer = col->GetRenderer()) {
                renderer->SetMode(wxDATAVIEW_CELL_INERT);
            }
        }

        // See clFileViewerTreeCtrl::OnLeftDown (Plugin/clFileViwerTreeCtrl.cpp) for why this is needed.
        Bind(wxEVT_LEFT_DOWN, &clRemoteDirTreeCtrl::OnLeftDown, this);
    }
    virtual ~clRemoteDirTreeCtrl() = default;

    wxDataViewItem GetRootItem() const { return wxDataViewItem(); }

    /**
     * @brief associate bitmap vector with this tree, used as the source for icon indices
     */
    void SetBitmaps(std::vector<wxBitmap>* bitmaps)
    {
        wxWithImages::Images images;
        if (bitmaps) {
            images.reserve(bitmaps->size());
            for (const auto& bmp : *bitmaps) {
                images.push_back(wxBitmapBundle::FromBitmap(bmp));
            }
        }
        SetImages(images);
    }

private:
    void OnLeftDown(wxMouseEvent& event)
    {
        if (event.CmdDown() || event.ShiftDown() || event.AltDown()) {
            // let modifier-driven range/toggle selection go through the native control
            event.Skip();
            return;
        }

        wxDataViewItem item;
        wxDataViewColumn* column = nullptr;
        HitTest(event.GetPosition(), item, column);
        if (!item.IsOk()) {
            event.Skip();
            return;
        }

        if (!IsSelected(item) || GetSelectedItemsCount() <= 1) {
            // nothing ambiguous here - let the native control handle it normally
            event.Skip();
            return;
        }

        // The clicked item is already part of a larger selection: collapse the
        // selection to just this item ourselves, and consume the event so it never
        // reaches the native control's own (slow, in this scenario) click handling.
        UnselectAll();
        Select(item);
        SetCurrentItem(item);

        wxDataViewEvent selectionEvent(wxEVT_DATAVIEW_SELECTION_CHANGED, this, item);
        ProcessWindowEvent(selectionEvent);

        SetFocus();
    }
};

class clRemoteDirCtrlItemData;
class WXDLLIMPEXP_SDK clRemoteDirCtrl : public wxPanel
{
    clRemoteDirTreeCtrl* m_treeCtrl = nullptr;
    SSHAccountInfo m_account;
    std::unique_ptr<DataViewTypeHelper> m_typeHelper;

protected:
    enum eDownloadAction {
        kOpenInCodeLite = 0,
        kOpenInExplorer,
    };

protected:
    void OnItemActivated(wxDataViewEvent& event);
    void OnItemExpanding(wxDataViewEvent& event);
    void OnContextMenu(wxDataViewEvent& event);

protected:
    clRemoteDirCtrlItemData* GetItemData(const wxDataViewItem& item) const;
    void DoExpandItem(const wxDataViewItem& item);
    wxDataViewItemArray GetSelections() const;
    void DoOpenItem(clRemoteDirCtrlItemData itemData, eDownloadAction action);
    void DoOpenItemMulti(std::vector<clRemoteDirCtrlItemData> itemData, eDownloadAction action);
    void DoCreateFolder(const wxDataViewItem& item, const wxString& name);
    void DoCreateFile(const wxDataViewItem& item, const wxString& name);
    void DoRename(const wxDataViewItem& item);
    void DoDelete(const wxDataViewItem& item);
    wxDataViewItem InsertSorted(const wxDataViewItem& parent,
                                const wxString& text,
                                int icon,
                                int expandedIcon,
                                bool isContainer,
                                wxClientData* data);

public:
    clRemoteDirCtrl(wxWindow* parent);
    virtual ~clRemoteDirCtrl();

    // Public API
    clRemoteDirTreeCtrl* GetTree() { return m_treeCtrl; }
    /**
     * @brief Open the tree view for a given path + ssh
     */
    bool Open(const wxString& path, const SSHAccountInfo& account);
    /**
     * @brief close the view and free all resources
     */
    bool Close(bool promptUser);

    /**
     * @brief return true if the tree is "connected" to an account
     * @return
     */
    bool IsConnected() const;

    /**
     * @brief return the selected path
     */
    wxString GetSelectedFolder() const;

    /**
     * @brief return the selected folders
     */
    size_t GetSelectedFolders(wxArrayString& paths) const;

    /**
     * @brief change the new root folder
     */
    bool SetNewRoot(const wxString& remotePath);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, clContextMenuEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, clContextMenuEvent);
#endif
#endif // CLREMOTEDIRCTRL_HPP
