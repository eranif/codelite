#ifndef VISUALIZERMGR_H
#define VISUALIZERMGR_H

#include "events_database.h"
#include "wxc_widget.h"
#include "wxgui_bitmaploader.h"

#include <map>
#include <wx/imaglist.h>

enum {
    ID_CHANGE_SIZER_TYPE = 3000,
    ID_CHANGE_SIZER_FIRST,
    ID_CHANGE_SIZER_BOXSIZER = ID_CHANGE_SIZER_FIRST,
    ID_CHANGE_SIZER_STATICBOXSIZER,
    ID_CHANGE_SIZER_FLEXGRIDSIZER,
    ID_CHANGE_SIZER_GRIDSIZER,
    ID_CHANGE_SIZER_GRIDBAGSIZER,
    ID_CHANGE_SIZER_LAST
};

enum {
    ID_INSERT_INTO_SIZER_TYPE = 3100,
    ID_INSERT_INTO_SIZER_FIRST,
    ID_INSERT_INTO_SIZER_BOXSIZER = ID_INSERT_INTO_SIZER_FIRST,
    ID_INSERT_INTO_SIZER_STATICBOXSIZER,
    ID_INSERT_INTO_SIZER_FLEXGRIDSIZER,
    ID_INSERT_INTO_SIZER_GRIDSIZER,
    ID_INSERT_INTO_SIZER_GRIDBAGSIZER,
    ID_INSERT_INTO_SIZER_LAST
};

// Possible menu types
extern FLAGS_t MT_TOP_LEVEL;
extern FLAGS_t MT_SIZERS;
extern FLAGS_t MT_CONTROLS;
extern FLAGS_t MT_CONTAINERS;
extern FLAGS_t MT_NOTEBOOK_PAGES;
extern FLAGS_t MT_SPLITTERWIN_PAGES;
extern FLAGS_t MT_LIST_CTRL_COLUMNS;
extern FLAGS_t MT_PROJECT;
extern FLAGS_t MT_COMMON_MENU;
extern FLAGS_t MT_EVENTS;
extern FLAGS_t MT_PREVIEW_CODE;
extern FLAGS_t MT_EDIT;
extern FLAGS_t MT_WIZARDPAGE;
extern FLAGS_t MT_MENUBAR;
extern FLAGS_t MT_MENU;
extern FLAGS_t MT_CONTROL_EVENTS;
extern FLAGS_t MT_TOOLBAR;
extern FLAGS_t MT_DV_LIST_CTRL_COL;
extern FLAGS_t MT_COLLPANE;
extern FLAGS_t MT_INFOBAR;
extern FLAGS_t MT_IMGLIST;
extern FLAGS_t MT_TIMER;
extern FLAGS_t MT_AUIMGR;
extern FLAGS_t MT_PASTE;
extern FLAGS_t MT_PG_PROPERTY;
extern FLAGS_t MT_PG_MANAGER;
extern FLAGS_t MT_RIBBON;
extern FLAGS_t MT_RIBBON_PAGE;
extern FLAGS_t MT_RIBBON_PANEL;
extern FLAGS_t MT_RIBBON_BUTTON_BAR;
extern FLAGS_t MT_RIBBON_TOOL_BAR;
extern FLAGS_t MT_RIBBON_GALLERY;
extern FLAGS_t MT_GRID;

class wxcWidget;
class PropertyBase;
class wxCrafterPlugin;

class wxMenu;
/**
 * @class Allocator
 * @author eran
 * @date 9/25/2012
 * @file allocator_mgr.h
 * @brief
 */
class Allocator
{
public:
    enum {
        INSERT_MAIN_SIZER,
        INSERT_SIBLING,
        INSERT_CHILD,
        INSERT_TOP_LEVEL,
        INSERT_PROMPT_CHILD_OR_SIBLING,
        INSERT_NONE
    };

protected:
    static Allocator* ms_instance;
    using ImageMap_t = std::map<int, int>;
    using InsertMap_t = std::map<int, std::map<int, int>>;

protected:
    wxImageList* m_imageList;
    ImageMap_t m_imageIds;
    InsertMap_t m_relations;
    wxCrafter::ResourceLoader m_bmpLoader;
    wxCrafterPlugin* m_plugin;

    FLAGS_t DoGetValidMenus(wxcWidget* item) const;

    void DoAddEditMenu(wxMenu& menu) const;
    void DoAddCommonMenu(wxMenu& menu) const;
    void DoAddProjectMenu(wxMenu& menu) const;
    void DoAddEventsMenu(wxMenu& menu) const;
    void DoAddControlEventsMenu(wxMenu& menu) const;
    void DoLink(int selected, int aboutToBeInsert, int relation);
    void DoLinkAll();

public:
    static Allocator* Instance();
    static void Release();

private:
    Allocator();
    virtual ~Allocator() = default;

public:
    void SetPlugin(wxCrafterPlugin* plugin) { m_plugin = plugin; }
    // int GetInsertionType(int controlId, int targetControlId) const
    int GetInsertionType(int controlId, int targetControlId, bool allowPrompt, wxcWidget* selectedWidget = NULL) const;
    bool CanPaste(wxcWidget* source, wxcWidget* target) const;

    void Register(wxcWidget* obj, const wxString& bmpname, int id = -1);
    wxImageList* GetImageList() { return m_imageList; }

    int GetImageId(int controlId) const;

    void PrepareMenu(wxMenu& menu, wxcWidget* item);

    wxMenu* CreateSizersMenu() const;
    wxMenu* CreateSizerTypeMenu() const;
    wxMenu* CreateInsertIntoSizerMenu() const;
    wxMenu* CreateControlsMenu() const;
    wxMenu* CreateTopLevelMenu() const;
    wxMenu* CreateContainersMenu() const;
};

#endif // VISUALIZERMGR_H
