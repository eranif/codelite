#ifndef __main_panel__
#define __main_panel__

#include "aui_pane_info_list_view.h"
#include "events_database.h"
#include "gui.h"
#include "properties_sheet.h"
#include "sizer_flags_list_view.h"
#include "styles_sheet_list_view.h"
#include "wxc_edit_manager.h"
#include "wxc_project_metadata.h"
#include "wxc_widget.h"
#include <cl_command_event.h>
#include <wx/aui/auibar.h>
#include <wx/treectrl.h>
#include <clTreeCtrl.h>

class EventsEditorPane;
class EventsEditorDlg;
extern const wxEventType wxEVT_UPDATE_PREVIEW;
extern const wxEventType wxEVT_UPDATE_EVENTSEDITORPANE;
wxDECLARE_EVENT(wxEVT_EVENTS_PROPERTIES_UPDATED, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_WXC_CMD_BATCH_GENERATE_CODE, wxCommandEvent);
extern const wxEventType wxEVT_PROPERTIES_MODIFIED;
extern const wxEventType wxEVT_PROJECT_METADATA_MODIFIED;
extern const wxEventType wxEVT_WXGUI_PROJECT_CLOSED;
extern const wxEventType wxEVT_WXGUI_PROJECT_LOADED;
extern const wxEventType wxEVT_WXGUI_PROJECT_SAVED;
extern const wxEventType wxEVT_CLOSE_PREVIEW;
extern const wxEventType wxEVT_PREVIEW_CLOSED;
extern const wxEventType wxEVT_PREVIEW_CTRL_SELECTED;
extern const wxEventType wxEVT_PREVIEW_BOOKPAGE_SELECTED;
extern const wxEventType wxEVT_PREVIEW_RIBBON_PAGE_SELECTED;
extern const wxEventType wxEVT_DESIGNER_DELETED;
extern const wxEventType wxEVT_REFRESH_DESIGNER;
extern const wxEventType wxEVT_REFRESH_PROPERTIES_VIEW;
extern const wxEventType wxEVT_WXC_PROJECT_LOADED;
extern const wxEventType wxEVT_WXC_SELECT_TREE_TLW;
extern const wxEventType wxEVT_WXC_CODE_PREVIEW_PAGE_CHANGED;

extern const wxEventType wxEVT_WXC_CMD_GENERATE_CODE;
extern const wxEventType wxEVT_WXC_OPEN_PROJECT;
extern const wxEventType wxEVT_WXC_SAVE_PROJECT;
extern const wxEventType wxEVT_WXC_CLOSE_PROJECT;
extern const wxEventType wxEVT_TREE_ITEM_SELECTED;
extern const wxEventType wxEVT_SHOW_CONTEXT_MENU;
extern const wxEventType wxEVT_DELETE_CONTROL;

class GUICraftItemData : public wxTreeItemData
{
public:
    wxcWidget* m_wxcWidget;

    GUICraftItemData(wxcWidget* w)
        : m_wxcWidget(w)
    {
    }
    virtual ~GUICraftItemData()
    {
        if(m_wxcWidget) {
            delete m_wxcWidget;
            m_wxcWidget = NULL;
        }
    }
};

class wxCrafterPlugin;
class GUICraftMainPanel : public GUICraftMainPanelBase
{
    friend class PropertiesSheet;
    friend class wxcEditManager;

    PropertiesSheet* m_propertiesPage;
    wxAuiToolBar* m_mainToolBar;
    wxcWidget* m_clipboardItem;
    wxTreeItemId m_draggedItem;
    bool m_previewAlive;
    clTreeCtrl* m_treeControls;
    wxCrafterPlugin* m_plugin;
    StylesSheetListView m_styles;
    SizerFlagsListView m_sizerFlags;
    AuiPaneInfoListView m_auiPaneInfo;

public:
    enum { kGenCodeForPreview = 0x00000001, kGenCodeSelectionOnly = 0x00000002 };

    static GUICraftMainPanel* m_MainPanel;
    wxWindow* GetTopFrame() const;

    bool IsPreviewAlive() const { return m_previewAlive; }

    DesignerPanel* GetDesignerPanel() const { return m_dp; }

    wxStyledTextCtrl* GetPreviewEditor() const;

protected:
    virtual void OnSizerFlagsUpdateUI(wxUpdateUIEvent& event);
    virtual void OnAuiPaneInfoChanged(wxPropertyGridEvent& event);
    virtual void OnSizerFlagsChanged(wxPropertyGridEvent& event);
    virtual void OnStylesChanged(wxPropertyGridEvent& event);
    virtual void OnNewFormUI(wxUpdateUIEvent& event);
    virtual void OnCppBookPageChanged(wxBookCtrlEvent& event);
    virtual void OnRegisterWxCrafter(wxCommandEvent& event);
    virtual void OnCloseLicenseMessage(wxCommandEvent& event);
    virtual void OnPageChanged(wxBookCtrlEvent& event);

    void DoDismissFindBar();
    bool IsTreeViewSelected() const;
    bool IsPropertyGridPropertySelected() const;
    void DoSelectItemByName(const wxString& name, const wxString& parentTLW);
    void DoRefresh(int eventType);
    void DoUpdatePropertiesView();
    void DoUpdateSizerFlags(wxcWidget* data);
    void DoUpdateStyleFlags(wxcWidget* data);
    void DoUpdatPropertiesFlags(wxcWidget* data);
    void DoUnsetItemData(const wxTreeItemId& item);
    void DoFindName(const wxTreeItemId& parent, const wxString& name, wxTreeItemId& item);
    void DoInsertControl(wxcWidget* control, wxcWidget* parent, int insertType, int imgId);
    void DoPasteOrDuplicate(wxcWidget* source, wxcWidget* target, bool duplicating);
    void DoMoveToplevelWindow(wxcWidget* tlw, int direction);
    size_t DoFindPropertiesPage(wxWindow* win);
    void DoShowPropertiesPage(wxWindow* win, const wxString& text, bool show);
    void DoUpdateCppPreview();

    /**
     * @brief check if we can place source inside target (or next to it)
     */
    bool DoCheckInsert(wxcWidget* source, wxcWidget* target) const;
    wxTreeItemId DoGetTopLevelTreeItem() const;
    void NotifyPreviewChanged(int eventType = wxEVT_UPDATE_PREVIEW);
    wxString GetStyleFromGuiID(int guiId) const;

    void Clear();
    void DoBuildTree(wxTreeItemId& itemToSelect, wxcWidget* wrapper, const wxTreeItemId& parent,
                     const wxTreeItemId& beforeItem = wxTreeItemId(), bool insertBefore = true);
    void ExpandToItem(const wxString& name, wxTreeItemId& searchFromItem); // Find and select the named item, starting
                                                                           // looking from branch searchFromItem (in
                                                                           // case of a name-clash)
    wxTreeItemId DoFindBestSelection(const wxTreeItemId& item);
    bool DoUpdateNotebookSelection(const wxTreeItemId& item);

    void DoInsertBefore(const wxTreeItemId& sourceItem, const wxTreeItemId& targetItem,
                        wxcWidget* sourceItemData = NULL, bool insertBefore = true);
    void DoAppendItem(const wxTreeItemId& sourceItem, const wxTreeItemId& targetItem, wxcWidget* sourceItemData = NULL);
    void DoChangeOrInsertIntoSizer(int id);
    wxArrayString GetCustomControlsUsed() const;
    bool DoCheckLicense(int controlType) const;
    wxTreeItemId DoFindItemByWxcWidget(wxcWidget* widget, const wxTreeItemId& item) const;

    /**
     * @brief generate the code for the current project
     * @param silent if set to true, disable all dialog messages
     */
    void DoGenerateCode(bool silent);

public:
    GUICraftMainPanel(wxWindow* parent, wxCrafterPlugin* plugin, clTreeCtrl* treeView);
    virtual ~GUICraftMainPanel();
    /**
     * @brief batch generate the code for a list of wxC files
     * @param files
     */
    void BatchGenerate(const wxArrayString& files);

    State::Ptr_t CurrentState();
    GUICraftItemData* GetSelItemData();
    wxcWidget* GetActiveWizardPage() const;
    wxcWidget* GetActiveTopLevelWin() const;
    wxcWidget* DoGetItemData(const wxTreeItemId& item) const;
    bool GenerateCppOutput(wxString& cpp, wxString& header, wxArrayString& headers, wxStringMap_t& additionalFiles,
                           size_t flags = 0) const;
    void GenerateXrcOutput(wxString& output, size_t flags = 0) const;
    /**
     * @brief save the tree to the file system
     * @param filename
     */
    JSONElement ToJSON(const wxTreeItemId& fromItem = wxTreeItemId());
    void LoadProject(const wxFileName& fn, const wxString& fileContent, bool lightLoad = false);

    void OnMenu(wxTreeEvent& event);
    void OnShowContextMenu(wxCommandEvent& e);
    void AddCustomControl(int controlId);

    DECLARE_EVENT_TABLE()
    void OnRefreshPropertiesView(wxCommandEvent& e);
    void OnSaveProject(wxCommandEvent& e);
    void OnOpenProject(wxCommandEvent& e);
    void OnCloseProject(wxCommandEvent& e);
    void OnGenerateCode(wxCommandEvent& e);
    void OnBatchGenerateCode(wxCommandEvent& e);
    void OnGenerateCodeMenu(wxAuiToolBarEvent& e);
    void OnCodeGenerationTypeChanged(wxCommandEvent& event);
    void OnNewControl(wxCommandEvent& e);
    void OnChangeSizerType(wxCommandEvent& e);
    void OnInsertIntoSizer(wxCommandEvent& e);
    void OnNewCustomControlMenu(wxCommandEvent& e);
    void OnNewCustomControl(wxCommandEvent& e);
    void OnItemSelected(wxTreeEvent& event);
    void OnShowPreview(wxCommandEvent& e);
    void OnGenerateCodeUI(wxUpdateUIEvent& e);
    void OnSaveProjectUI(wxUpdateUIEvent& e);
    void OnCancelPreviewUI(wxUpdateUIEvent& e);
    void OnShowPreviewUI(wxUpdateUIEvent& e);
    void OnCancelPreview(wxCommandEvent& e);
    void OnSizerForTopLevelWinUI(wxUpdateUIEvent& e);
    void OnItemRightClick(wxTreeEvent& event);
    void OnStyleChanged(wxPropertyGridEvent& event);
    void OnPropertyChanged(wxCommandEvent& e);
    void OnSizerTool(wxCommandEvent& e);
    void OnSetSizerProp1(wxCommandEvent& e);
    void OnEventEditor(wxCommandEvent& e);
    void OnEventEditorUI(wxUpdateUIEvent& e);
    void OnEventEditorCommon(wxCommandEvent& e);
    void OnSizerToolUI(wxUpdateUIEvent& e);
    void OnSetSizerProp1UI(wxUpdateUIEvent& e);
    void OnDelete(wxCommandEvent& e);
    void OnDeleteUI(wxUpdateUIEvent& e);
    void OnMoveItem(wxCommandEvent& e);
    void OnMoveItemUI(wxUpdateUIEvent& e);
    void OnMenuItemUI(wxUpdateUIEvent& e);
    void OnPreviewItemSelected(wxCommandEvent& e);
    void OnSelectToplevelItem(wxCommandEvent& e);
    void OnBookPageSelected(wxCommandEvent& e);
    void OnRibbonPageSelected(wxCommandEvent& e);
    void OnPreviewClosed(wxCommandEvent& e);
    void OnMenuItemClicked(wxCommandEvent& e);
    void OnCut(wxCommandEvent& e);
    void OnCopy(wxCommandEvent& e);
    void OnPaste(wxCommandEvent& e);
    void OnDuplicate(wxCommandEvent& e);
    void OnRename(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& e);
    void OnRedo(wxCommandEvent& e);
    void OnRedoUI(wxUpdateUIEvent& e);
    void OnUndoUI(wxUpdateUIEvent& e);
    void OnLabelCurrentState(wxCommandEvent& e);
    void OnEditMenuOpened(wxMenuEvent& event);
    void OnLoadCurrentState(wxCommandEvent& e);
    void OnBeginDrag(wxTreeEvent& event);
    void OnEndDrag(wxTreeEvent& event);
    void OnRefreshView(wxCommandEvent& e);
    void OnBarItemSelected(wxCommandEvent& e);
    void OnUpdatePreview(wxCommandEvent& e);
    void OnFindBar(clFindEvent& e);
};

#endif // __main_panel__
