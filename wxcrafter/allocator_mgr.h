#ifndef VISUALIZERMGR_H
#define VISUALIZERMGR_H

#include "events_database.h"
#include "json_node.h"
#include "wx_ordered_map.h"
#include "wxc_widget.h"
#include "wxgui_bitmaploader.h"
#include <map>
#include <set>
#include <wx/imaglist.h>

// Widgets IDS
enum {

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                             //
    //                             **** ATTENTION ****                                             //
    //                                                                                             //
    // DO NOT INSERT NEW IDS IN THE MIDDLE. THE ORDER OF THE CONTROLS MATTERS                      //
    // ONE MUST NOT CHANGE IT OR IT WILL BREAK BACKWARD COMPATIBILITY                              //
    // IF YOU NEED TO ADD NEW CONTROL - ADD IT TO THE END OF THIS ENUM BUT BEFORE ID_LAST_CONTROL  //
    //                                                                                             //
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////

    ID_FORM_TYPE = 4100,
    ID_EVENT_EDITOR_COMMON = 4300,
    ID_EVENT_EDITOR = 4301,
    ID_GENERATE_CODE = 4399,
    ID_FIRST_CONTROL = 4400,
    ID_WXBUTTON = ID_FIRST_CONTROL,
    ID_WXBOXSIZER,             // 4401
    ID_WXFRAME,                // 4402
    ID_WXFLEXGRIDSIZER,        // 4403
    ID_WXBITMAPBUTTON,         // 4404
    ID_WXSTATICTEXT,           // 4405
    ID_WXTEXTCTRL,             // 4406
    ID_WXPANEL_TOPLEVEL,       // 4407
    ID_WXPANEL,                // 4408
    ID_WXSTATICBITMAP,         // 4409
    ID_WXCOMBOBOX,             // 4410
    ID_WXCHOICE,               // 4411
    ID_WXLISTBOX,              // 4412
    ID_WXLISTCTRL,             // 4413
    ID_WXLISTCTRL_COL,         // 4414
    ID_WXCHECKBOX,             // 4415
    ID_WXRADIOBOX,             // 4416
    ID_WXRADIOBUTTON,          // 4417
    ID_WXSTATICLINE,           // 4418
    ID_WXSLIDER,               // 4419
    ID_WXGAUGE,                // 4420
    ID_WXDIALOG,               // 4421
    ID_WXTREECTRL,             // 4422
    ID_WXHTMLWIN,              // 4423
    ID_WXRICHTEXT,             // 4424
    ID_WXCHECKLISTBOX,         // 4425
    ID_WXGRID,                 // 4426
    ID_WXTOGGLEBUTTON,         // 4427
    ID_WXSEARCHCTRL,           // 4428
    ID_WXCOLORPICKER,          // 4429
    ID_WXFONTPICKER,           // 4430
    ID_WXFILEPICKER,           // 4431
    ID_WXDIRPICKER,            // 4432
    ID_WXDATEPICKER,           // 4433
    ID_WXCALEDARCTRL,          // 4434
    ID_WXSCROLLBAR,            // 4435
    ID_WXSPINCTRL,             // 4436
    ID_WXSPINBUTTON,           // 4437
    ID_WXHYPERLINK,            // 4438
    ID_WXGENERICDIRCTRL,       // 4439
    ID_WXSCROLLEDWIN,          // 4440
    ID_WXPANEL_NOTEBOOK_PAGE,  // 4441
    ID_WXNOTEBOOK,             // 4442
    ID_WXTOOLBOOK,             // 4443
    ID_WXLISTBOOK,             // 4444
    ID_WXCHOICEBOOK,           // 4445
    ID_WXTREEBOOK,             // 4446
    ID_WXSPLITTERWINDOW,       // 4447
    ID_WXSPLITTERWINDOW_PAGE,  // 4448
    ID_WXSTATICBOXSIZER,       // 4449
    ID_WXWIZARD,               // 4450
    ID_WXWIZARDPAGE,           // 4451
    ID_WXGRIDSIZER,            // 4452
    ID_WXGRIDBAGSIZER,         // 4453
    ID_WXSPACER,               // 4454
    ID_WXTREEBOOK_SUB_PAGE,    // 4455 Fake control
    ID_WXAUINOTEBOOK,          // 4456
    ID_WXMENUBAR,              // 4457
    ID_WXMENU,                 // 4458
    ID_WXMENUITEM,             // 4459
    ID_WXSUBMENU,              // 4460
    ID_WXTOOLBAR,              // 4461
    ID_WXTOOLBARITEM,          // 4462
    ID_WXAUITOOLBAR,           // 4463
    ID_WXSTATUSBAR,            // 4464
    ID_WXCUSTOMCONTROL,        // 4465
    ID_WXSTC,                  // 4466
    ID_WXSTDDLGBUTTONSIZER,    // 4467
    ID_WXSTDBUTTON,            // 4468
    ID_WXDATAVIEWLISTCTRL,     // 4469
    ID_WXDATAVIEWTREECTRL,     // 4470
    ID_WXBANNERWINDOW,         // 4471
    ID_WXDATAVIEWCOL,          // 4472
    ID_WXDATAVIEWTREELISTCTRL, // 4473
    ID_WXCOMMANDLINKBUTTON,    // 4474
    ID_WXCOLLAPSIBLEPANE,      // 4475
    ID_WXCOLLAPSIBLEPANE_PANE, // 4476
    ID_WXINFOBAR,              // 4477
    ID_WXINFOBARBUTTON,        // 4478
    ID_WXWEBVIEW,              // 4479
    ID_WXAUIMANAGER,           // 4480
    ID_WXIMAGELIST,            // 4481
    ID_WXBITMAP,               // 4482
    ID_WXTIMER,                // 4483
    ID_WXPOPUPWINDOW,          // 4484
    ID_WXPROPERTYGRIDMANAGER,  // 4485
    ID_WXPGPROPERTY,           // 4486
    ID_WXPGPROPERTY_SUB,       // 4487 , fake one
    // RibbonBar UI
    ID_WXRIBBONBAR,                   // 4488
    ID_WXRIBBONPAGE,                  // 4489
    ID_WXRIBBONPANEL,                 // 4490
    ID_WXRIBBONBUTTONBAR,             // 4491
    ID_WXRIBBONBUTTON,                // 4492
    ID_WXRIBBONHYBRIDBUTTON,          // 4493
    ID_WXRIBBONDROPDOWNBUTTON,        // 4494
    ID_WXRIBBONTOGGLEBUTTON,          // 4495
    ID_WXRIBBONGALLERY,               // 4496
    ID_WXRIBBONGALLERYITME,           // 4497
    ID_WXRIBBONTOOLBAR,               // 4498
    ID_WXRIBBONTOOL,                  // 4499
    ID_WXRIBBONHYBRIDTOOL,            // 4500
    ID_WXRIBBONDROPDOWNTOOL,          // 4501
    ID_WXRIBBONTOGGLETOOL,            // 4502
    ID_WXRIBBONTOOLSEPARATOR,         // 4503
    ID_WXTOOLBARITEM_SEPARATOR,       // 4504
    ID_WXGLCANVAS,                    // 4505
    ID_WXGRIDCOL,                     // 4506
    ID_WXGRIDROW,                     // 4507
    ID_WXMEDIACTRL,                   // 4508
    ID_WXTREELISTCTRL,                // 4509
    ID_WXTREELISTCTRLCOL,             // 4510
    ID_WXSIMPLEBOOK,                  // 4511
    ID_WXTASKBARICON,                 // 4512
    ID_WXBITMAPTOGGLEBUTTON,          // 4513
    ID_WXAUITOOLBARLABEL,             // 4514
    ID_WXAUITOOLBARITEM_SPACE,        // 4515
    ID_WXAUITOOLBARITEM_STRETCHSPACE, // 4516
    ID_WXTOOLBARITEM_STRETCHSPACE,    // 4517
    ID_WXAUITOOLBARTOPLEVEL,          // 4518
    ID_WXANIMATIONCTRL,               // 4519
    ID_WXBITMAPCOMBOBOX,              // 4520
    ID_WXREARRANGELIST,               // 4521
    ID_WXSIMPLEHTMLLISTBOX,           // 4522
    ID_WXACTIVITYINDICATOR,           // 4523
    ID_WXTIMEPICKERCTRL,              // 4524
    // ADD_NEW_CONTROL
    ID_LAST_CONTROL
};

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
    typedef std::map<int, wxcWidget*> Map_t;
    typedef std::map<int, int> ImageMap_t;
    typedef std::map<int, std::map<int, int> > InsertMap_t;

protected:
    Map_t m_objs;
    wxImageList* m_imageList;
    ImageMap_t m_imageIds;
    InsertMap_t m_relations;
    wxCrafter::ResourceLoader m_bmpLoader;
    wxCrafterPlugin* m_plugin;

    static EventsDatabase m_commonEvents;

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
    static void Initialize();

private:
    Allocator();
    virtual ~Allocator();

public:
    wxcWidget* Create(int id);
    void SetPlugin(wxCrafterPlugin* plugin) { m_plugin = plugin; }
    // int GetInsertionType(int controlId, int targetControlId) const
    int GetInsertionType(int controlId, int targetControlId, bool allowPrompt, wxcWidget* selectedWidget = NULL) const;
    bool CanPaste(wxcWidget* source, wxcWidget* target) const;

    void Register(wxcWidget* obj, const wxString& bmpname, int id = -1);
    wxImageList* GetImageList() { return m_imageList; }

    static EventsDatabase& GetCommonEvents() { return m_commonEvents; }

    static int StringToId(const wxString& classname); // Translates e.g. "wxTextCtrl" to ID_WXTEXTCTRL

    int GetImageId(int controlId) const;
    wxcWidget* CreateWrapperFromJSON(const JSONElement& json);

    void PrepareMenu(wxMenu& menu, wxcWidget* item);

    wxMenu* CreateSizersMenu() const;
    wxMenu* CreateSizerTypeMenu() const;
    wxMenu* CreateInsertIntoSizerMenu() const;
    wxMenu* CreateControlsMenu() const;
    wxMenu* CreateTopLevelMenu() const;
    wxMenu* CreateContainersMenu() const;
};

#endif // VISUALIZERMGR_H
