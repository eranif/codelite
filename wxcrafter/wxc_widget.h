#ifndef WRAPPERBASE_H
#define WRAPPERBASE_H

#include "aui_pane_info.h"
#include "bitmap_picker_property.h"
#include "bool_property.h"
#include "category_property.h"
#include "events_database.h"
#include "int_property.h"
#include "json_node.h"
#include "property_base.h"
#include "string_property.h"
#include "wx_ordered_map.h"
#include "wxc_settings.h"
#include "wxgui_defs.h"
#include <list>
#include <map>
#include <set>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>

typedef unsigned long long FLAGS_t;

static const wxString DEFAULT_AUI_DROPDOWN_FUNCTION = "ShowAuiToolMenu";
static const wxString DEFAULT_AUI_DROPDOWN_FUNCTION_AND_SIG =
    DEFAULT_AUI_DROPDOWN_FUNCTION + "(wxAuiToolBarEvent& event)";

enum CONTROL_TYPE {
    TYPE_SIZER = 0,             // 0
    TYPE_CONTROL,               // 1
    TYPE_LIST_CTRL,             // 2
    TYPE_LIST_CTRL_COL,         // 3
    TYPE_CONTAINER,             // 4
    TYPE_BOOK,                  // 5
    TYPE_FORM,                  // 6
    TYPE_FORM_FRAME,            // 7
    TYPE_WIZARD,                // 8
    TYPE_WIZARD_PAGE,           // 9
    TYPE_NOTEBOOK_PAGE,         // 10
    TYPE_SPLITTERWIN_PAGE,      // 11
    TYPE_SPLITTER_WINDOW,       // 12
    TYPE_SPACER,                // 13
    TYPE_MENUBAR,               // 14
    TYPE_MENU,                  // 15
    TYPE_SUBMENU,               // 16
    TYPE_MENUITEM,              // 17
    TYPE_TOOLBAR,               // 18
    TYPE_TOOLITEM,              // 19
    TYPE_STATUSABR,             // 20
    TYPE_AUITOOLBAR,            // 21
    TYPE_STD_BUTTON_SIZER,      // 22
    TYPE_STD_BUTTON,            // 23
    TYPE_DV_WITH_COLUMNS,       // 24
    TYPE_DV_LISTCTRLCOL,        // 25
    TYPE_COLOLAPSIBLEPANE,      // 26
    TYPE_COLOLAPSIBLEPANE_PANE, // 27
    TYPE_INFO_BAR,              // 28
    TYPE_INFO_BAR_BUTTON,       // 29
    TYPE_AUI_MGR,               // 30
    TYPE_BITMAP,                // 31
    TYPE_IMGLIST,               // 32
    TYPE_TIMER,                 // 33
    TYPE_PG_MGR,                // 34
    TYPE_PG_PROPERTY,           // 35
    TYPE_RIBBON_BAR,            // 36
    TYPE_RIBBON_PAGE,           // 37
    TYPE_RIBBON_PANEL,          // 38
    TYPE_RIBBON_BUTTONBAR,      // 39
    TYPE_RIBBON_BUTTON,         // 40
    TYPE_RIBBON_TOOLBAR,        // 41
    TYPE_RIBBON_TOOL,           // 42
    TYPE_RIBBON_GALLERY,        // 43
    TYPE_RIBBON_GALLERY_ITEM,   // 44
    TYPE_GRID,                  // 45
    TYPE_GRID_CHILD,            // 46
    TYPE_TREE_LIST_CTRL,        // 47
    TYPE_TREE_LIST_CTRL_COL,    // 48
    TYPE_TASKBARICON,           // 49
    TYPE_UNKNOWN
};

enum SIZER_FLAG_VALUE {
    SZ_ALL = 0x00000001,
    SZ_LEFT = 0x00000002,
    SZ_RIGHT = 0x00000004,
    SZ_TOP = 0x00000008,
    SZ_BOTTOM = 0x00000010,
    SZ_EXPAND = 0x00000020,
    SZ_ALIGN_CENTER = 0x00000040,
    SZ_ALIGN_LEFT = 0x00000080,
    SZ_ALIGN_CENTER_HORIZONTAL = 0x00000100,
    SZ_ALIGN_RIGHT = 0x00000200,
    SZ_ALIGN_TOP = 0x00000400,
    SZ_ALIGN_CENTER_VERTICAL = 0x00000800,
    SZ_ALIGN_BOTTOM = 0x00001000,
    SZ_RESERVE_SPACE_EVEN_IF_HIDDEN = 0x00002000,
    // There's a value-clash between wxALIGN_LEFT and wxALIGN_TOP (both are wxALIGN_NOT == 0)
    // This made it impossible to select only one in Properties (or the toolbar). So provide a fake fake value.
    crafterALIGN_TOP = 0x10000000,
};

enum DuplicatingOptions {
    DO_renameNone = 0x00,
    DO_renameAllChildrenExceptUsernamed = 0x01,
    DO_renameAllChildren = 0x02,
    DO_copyEventsToo = 0x04
};

class AuiToolbarWrapperBase;

class wxcSizerItem : public wxSizerItem
{
public:
    wxcSizerItem() {}
    virtual ~wxcSizerItem() {}
    wxString GetBorderScaled() const;
};

class wxcWidget
{
    friend class AuiToolbarWrapperBase;

protected:
    static size_t s_objCounter;

public:
    enum XRC_TYPE { XRC_PREVIEW, XRC_DESIGNER, XRC_LIVE };

    enum COPY_REASON {
        CR_Copy,
        CR_Cut,
    };

public:
    typedef std::list<wxcWidget*> List_t;
    typedef wxOrderedMap<wxString, WxStyleInfo> MapStyles_t;
    typedef wxOrderedMap<wxString, PropertyBase*> MapProperties_t;
    typedef wxOrderedMap<wxString, ConnectDetails> MapEvents_t;
    typedef std::map<wxString, ConnectDetails> Map_t;
    typedef std::map<wxString, wxArrayString> AntiGroupMap_t;
    typedef std::map<int, int> SizerFlagsValueSet_t;

    static AntiGroupMap_t s_antiGroup;
    static SizerFlagsValueSet_t s_sizerFlagsValue;

protected:
    wxcWidget* m_parent;
    List_t m_children;
    wxcSizerItem m_sizerItem;
    wxString m_gbSpan;
    wxString m_gbPos;
    MapStyles_t m_styles;
    MapStyles_t m_sizerFlags;
    MapProperties_t m_properties;
    wxString m_namePattern;
    int m_type;
    wxMenu* m_eventsMenu;
    MapEvents_t m_connectedEvents;
    EventsDatabase m_controlEvents;
    AuiPaneInfo m_auiPaneInfo;

    static int m_copyCounter; /// An internal counter that holds a unique string that allows renaming copied items
    COPY_REASON m_copyReason; /// The reason for the copy
    wxString m_condname;

    static CustomControlTemplateMap_t ms_customControlsUsed;

protected:
    wxString BaseDoGenerateClassMember() const;

    void AddProperty(PropertyBase* prop);

    /**
     * @brief make the code generated by this widgets surrounded with
     * #if <condname>
     * ...
     * #endif
     */
    void SetIfBlockCond(const wxString& condname);
    /**
     * @brief wrap 'code' with the proper #if / #endif block
     * if needed. The code will get wrapped if !GetCondname().IsEmpty()
     * @param code
     */
    void WrapInIfBlockIfNeeded(wxString& code) const;

    const wxString& GetCondname() const { return m_condname; }

    /// aliases for adding property
    void AddCategory(const wxString& name) { AddProperty(new CategoryProperty(name)); }
    /**
     * @brief add string property
     */
    void AddText(const wxString& name, const wxString& description, const wxString& defaultValue = wxEmptyString)
    {
        AddProperty(new StringProperty(name, defaultValue, description));
    }

    /**
     * @brief add bool property
     */
    void AddBool(const wxString& name, const wxString& description, bool defaultValue)
    {
        AddProperty(new BoolProperty(name, defaultValue, description));
    }

    /**
     * @brief add integer property
     */
    void AddInteger(const wxString& name, const wxString& description, int defaultValue)
    {
        AddProperty(new IntProperty(name, defaultValue, description));
    }

    void DelProperty(const wxString& name);
    void DoClearFlags(MapStyles_t& mp);
    void DoAddSizerFlag(const wxString& name, WxStyleInfo info);
    virtual void ImportEventsFromXRC(const wxString& events);
    virtual void ImportEventFromwxSmith(const wxString& eventname, const wxString& handlername);
    virtual void ImportEventFromFB(const wxString& eventname, const wxString& handlername);

    // Properties accessors for
    ///////////////////////////////
    void DoSetPropertyStringValue(const wxString& propname, const wxString& value);

    bool DoCheckNameUniqueness(const wxString& name, const wxcWidget* widget) const;
    // Copy current wxcWidget children into 'widget'
    void DoCopyChildren(wxcWidget* widget, enum DuplicatingOptions nametypesToChange,
                        const std::set<wxString>& existingNames) const;
    virtual void DoDeepCopy(const wxcWidget& rhs, enum DuplicatingOptions nametypesToChange,
                            const std::set<wxString>& existingNames, const wxString& chosenName = "",
                            const wxString& chosenInheritedName = "", const wxString& chosenFilename = "");

public:
    void SetCopyReason(COPY_REASON cr) { m_copyReason = cr; }
    COPY_REASON GetCopyReason() const { return m_copyReason; }

    static void SetMsCustomControlsUsed(const CustomControlTemplateMap_t& customControlsUsed)
    {
        ms_customControlsUsed = customControlsUsed;
    }
    static CustomControlTemplateMap_t& GetMsCustomControlsUsed() { return ms_customControlsUsed; }

    /**
     * @brief return true if this control can be used (e.g. is it licensed?)
     */
    virtual bool IsLicensed() const;

    /**
     * @brief check whether 'name' is unique accross all children of this wxcWidget
     */
    virtual bool IsNameUnique(const wxString& name) const;
    wxString PropertyString(const wxString& propname, const wxString& defaultValue = "") const;
    int PropertyInt(const wxString& propname, int defval = -1) const;
    wxString PropertyFile(const wxString& propname) const;
    wxString PropertyBool(const wxString& propname) const;

    bool IsPropertyChecked(const wxString& propname) const { return PropertyBool(propname) == "true"; }

    /**
     * @brief return an ordered map of all events connected by this item and its children.
     * Unlike other WrapperBase::Map_t, the key in this map is the function name
     * and the value is the ConnectDetails
     */
    wxcWidget::Map_t GetConnectedEventsRecrusively() const;

    /**
     * @brief return property by its name
     * @return return the property or NULL if not found
     */
    PropertyBase* GetProperty(const wxString& name);

    /**
     * @brief return the real class name taking subclass property into consideration
     * @return
     */
    wxString GetRealClassName() const;

protected:
    void DoGetConnectedEventsRecrusively(wxcWidget::Map_t& events, const wxcWidget* wb) const;

    // Helpers
    wxString XRCPrefix(const wxString& class_name = wxEmptyString) const;
    wxString XRCSuffix() const;
    wxString XRCStyle(bool forPreview = false) const;
    wxString XRCSize(bool dontEmitDefault = true) const;
    wxString XRCValue() const;
    wxString XRCLabel() const;
    wxString XRCContentItems(bool ensureAtLeastOneEntry = false) const;
    wxString XRCUnknown() const;
    wxString XRCCommonAttributes() const;
    wxString XRCBitmap(const wxString& labelname = "bitmap") const;
    wxString XRCBitmap(const wxString& label, const wxString& bitmap) const;

    // Code Generator helpers
    wxString CPPStandardWxCtor(const wxString& defaultStyle) const;
    wxString CPPStandardWxCtorWithValue(const wxString& defaultStyle) const;
    wxString CPPStandardWxCtorWithLabel(const wxString& defaultStyle) const;
    wxString CPPCommonAttributes() const;

    /**
     * @brief
     * @return
     */
    wxString XRCSelection() const;

public:
    // Common properties accessors
    wxString SizeAsString() const;
    wxString Size() const;
    wxString ValueAsString() const;
    wxString WindowID() const;
    wxString Label() const;
    wxString CPPLabel() const;

    static size_t GetObjCounter() { return s_objCounter; }

    /**
     * @brief return true if this widget has a window style
     * @param styleBit the style bit
     */
    bool HasStyle(int styleBit) const;

    static int SizerFakeValue(int realValue);

    static void SetObjCounter(size_t counter) { s_objCounter = counter; }

protected:
    // Pure virtual
    virtual void GetIncludeFile(wxArrayString& headers) const = 0;
    void DoTraverseAndGenCode(wxArrayString& headers, wxString& ctorCode, wxString& members, wxString& eventFunctions,
                              wxString& eventConnectCode, wxStringMap_t& additionalFiles, wxString& dtorCode,
                              wxString& extraFunctionsImpl, wxString& extraFunctionsDecl) const;

    void DoGetCustomControlsName(const wxcWidget* widget, wxArrayString& controls) const;
    wxString DoGenerateEventStubs() const;
    wxString DoGenerateConnectCode() const;
    wxString DoGetScopeName() const;

    /**
     * @brief return a C++ code that represents this class
     */
    wxString DoGenerateCppCtorCode() const;
    wxString DoGenerateCppDtorCode() const;
    virtual wxString DoGenerateCppCtorCode_End() const;

    // NOP stub
    virtual void DoGenerateExtraFunctions(wxString& decl, wxString& impl) const
    {
        wxUnusedVar(decl);
        wxUnusedVar(impl);
    }

    /**
     * @brief generate getter for the control
     */
    virtual void DoGenerateGetters(wxString& decl) const;

    virtual wxString DoGenerateClassMember() const;

    /**
     * @brief a helper class that return the name property
     * or 'this' incase we are top-level window
     * This method is mainly used when connecting events.
     * It is advised to override it for wrappers of tool kind (like wxRibbonBar button, wxRibbonToolBar's tool etc)
     * and return the <code>GetParent()->GetName()</code> instead
     */
    virtual wxString GetCppName() const;

    /**
     * @brief when generating the connect code (for events)
     * decide whether to use:
     *
     * w->Connect(EVT_NAME, Handler, NULL, this);
     * <b>OR</b>
     * w->Connect(<b><i>ID</i></b>, EVT_NAME, Handler, NULL, this);
     */
    virtual bool UseIdInConnect() const { return false; }
    /**
     * @brief returns either <GetName()>BaseClass or,
     * if GetName() already ends in 'Base', <GetName()>Class
     */
    virtual wxString CreateBaseclassName() const;

    void RegisterEvent(const wxString& eventName, const wxString& className, const wxString& description,
                       const wxString& handlerName = "", const wxString& functionNameAndSig = "", bool noBody = false);
    /**
     * @brief acroynm for RegisterEvent(eventName, wxT("wxCommandEvent"), wxT("wxCommandEventHandler"))
     */
    void RegisterEventCommand(const wxString& eventName, const wxString& description)
    {
        RegisterEvent(eventName, wxT("wxCommandEvent"), description, wxT("wxCommandEventHandler"));
    }

    const wxcWidget* DoFindByName(const wxcWidget* parent, const wxString& name) const;

public:
    void GetCustomControlsName(wxArrayString& controls) const;
    wxString WrapInSizerXRC(const wxString& objXRC) const;
    wxString WrapInAuiPaneXRC(const wxString& objXRC) const;
    void AddEvent(const ConnectDetails& eventDetails);
    void RemoveEvent(const wxString& eventName);
    bool HasEvent(const wxString& eventName) const;
    ConnectDetails GetEvent(const wxString& eventName) const;
    ConnectDetails GetEventMetaData(const wxString& eventName) const;

    bool IsSizerFlagChecked(const wxString& style) const;
    void DoEnableStyle(wxcWidget::MapStyles_t& mp, const wxString& style, bool enable);

    virtual void FixPaths(const wxString& cwd);
    virtual void Serialize(JSONElement& json) const;
    virtual void UnSerialize(const JSONElement& json);

    /**
     * @brief Extract properties from this XRC object node.
     * Derived classes may extract their own, more specific, ones too
     */
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);

    /**
     * @brief Extract properties from this wxSmith object node.
     * Derived classes may extract their own, more specific, ones too
     */
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);

    /**
     * @brief Extract properties from this wxFB object node.
     * Derived classes may extract their own, more specific, ones too
     */
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);

    // Cleanup
    /**
     * @brief remove and delete all the widget children
     */
    void DeleteAllChildren();
    /**
     * @brief change the parent of this widget
     */
    void Reparent(wxcWidget* parent);
    // Children manipulation
    void MoveUp();
    void MoveDown();
    bool CanMoveUp() const;
    bool CanMoveDown() const;
    bool CanMoveRight() const
    { // i.e. can move into an adjacent sibling that's a sizer
        return (GetAdjacentSiblingSizer() != NULL);
    }
    /**
     * @brief copy all oldWidget's children into newWidget and place newWidget in the
     * same location as oldWidget occupied. In addition, this function also frees the memory
     * allocated by 'newWidget' (Eran, should that read 'oldWidget'?)
     */
    static void ReplaceWidget(wxcWidget* oldWidget, wxcWidget* newWidget);

    /**
     * @brief move oldWidget into newWidget and place newWidget in the
     * same location as oldWidget occupied.
     */
    static void InsertWidgetInto(wxcWidget* oldWidget, wxcWidget* newWidget);

    /**
     * @brief copy sizer and wxAUI info from source widget to target widget
     */
    static void CopySizerAndAuiInfo(const wxcWidget* source, wxcWidget* target);

    /**
     * @brief update the registered events if needed
     * By default this function does nothing. See comment inside GetControlEvents() below for more details
     */
    virtual void UpdateRegisteredEventsIfNeeded() {}

    /**
     * @brief Is this a 'real' control, or something else e.g. a sizer or a toolbar separator, that doesn't do events
     * Relevant derived classes can return false instead.
     */
    virtual bool IsEventHandler() const { return true; }

public:
    wxcWidget(int type);
    virtual ~wxcWidget();

    void SetGbPos(const wxString& gbPos) { this->m_gbPos = gbPos; }
    void SetGbSpan(const wxString& gbSpan) { this->m_gbSpan = gbSpan; }
    const wxString& GetGbPos() const { return m_gbPos; }
    const wxString& GetGbSpan() const { return m_gbSpan; }
    EventsDatabase& GetControlEvents()
    {
        /// Allow the widget to register / remove events based on information which was
        /// not available while the widget was constructed.
        /// A good example is ToolBarItem: it serves 2 widgets wx{Aui}ToolBar
        /// However, there is no way to determine the parent of the toolbar item at the creation time
        /// this functions attempts to fix this by allowing the widget to adjust its content after a parent has been
        /// asigned to it
        UpdateRegisteredEventsIfNeeded();
        return m_controlEvents;
    }

    void SetType(int type) { this->m_type = type; }
    int GetType() const { return m_type; }
    /**
     * @brief return the top level window for this control
     */
    wxcWidget* GetTopLevel() const;

    /**
     * @brief find the first child with a given name (recursive)
     */
    const wxcWidget* FindChildByName(const wxString& name) const;

    const wxcWidget* FindFirstDirectChildOfType(int type) const;
    /**
     * @brief create an XRC code that represents this class
     */
    virtual void ToXRC(wxString& text, XRC_TYPE type) const = 0;

    /**
     * @brief return the CPP code suitable for this class
     */
    virtual wxString CppCtorCode() const = 0;

    /**
     * @brief an optional d-tor code
     */
    virtual wxString CppDtorCode() const { return ""; }

    /**
     * @brief return the underlyging WX class represented by
     * this class
     */
    virtual wxString GetWxClassName() const = 0;

    /**
     * @brief return a new instance of this class
     */
    virtual wxcWidget* Clone() const = 0;

    /**
     * @brief create a copy of this class - with all its properties / sizers / styles and wxAUI properties copied
     */
    virtual wxcWidget* Copy(enum DuplicatingOptions nametypesToChange, const std::set<wxString>& existingNames,
                            const wxString& chosenName = "", const wxString& chosenInheritedName = "",
                            const wxString& chosenFilename = "") const;

    /**
     * @brief if the controls requires an additional files, generate their content / names
     * and place them inside the 'additionalFiles' variable
     */
    virtual void GenerateAdditionalFiles(wxStringMap_t& additionalFiles) const;

    wxcSizerItem& SizerItem() { return m_sizerItem; }

    // UI Helper methods
    const wxcWidget::MapProperties_t& GetProperties() const { return m_properties; }

    void SetSizerFlags(const MapStyles_t& sizerFlags)
    {
        MapStyles_t::const_iterator iter = sizerFlags.begin();
        for(; iter != sizerFlags.end(); ++iter) {
            if(m_sizerFlags.Contains(iter->second.style_name)) {
                m_sizerFlags.Item(iter->second.style_name).is_set = iter->second.is_set;
            }
        }
    }

    void SetStyles(size_t value);

    void ClearStyles() { DoClearFlags(m_styles); }

    void EnableStyle(const wxString& style, bool enable) { DoEnableStyle(m_styles, style, enable); }

    void EnableSizerFlag(const wxString& flag, bool enable);

    void SetStyles(const MapStyles_t& styles)
    {
        MapStyles_t::const_iterator iter = styles.begin();
        for(; iter != styles.end(); ++iter) {
            if(m_styles.Contains(iter->second.style_name)) {
                m_styles.Item(iter->second.style_name).is_set = iter->second.is_set;
            }
        }
    }

    const MapStyles_t& GetStyles() const { return m_styles; }

    const MapStyles_t& GetSizerFlags() const { return m_sizerFlags; }

    /*!
     * \brief Clears all sizeritem and similar things
     */
    void ClearSizerAll()
    {
        ClearSizerFlags();
        SizerItem().SetBorder(0);
        SizerItem().SetProportion(0);
        SetGbSpan("");
        SetGbPos("");
    }

    void ClearSizerFlags() { DoClearFlags(m_sizerFlags); }
    wxString GenerateName() const
    {
        wxString n;
        n << m_namePattern << ++s_objCounter;
        return n;
    }

    void SetId(const wxString& id) { DoSetPropertyStringValue(PROP_WINDOW_ID, id); }

    wxString GetId() const;

    /**
     * @brief return the Windowed parent i.e. an actual window that can be the parent when constructing
     * this object
     * @return parent name or the "NULL" string
     */
    wxString GetWindowParent() const;
    /**
     * @brief can this wrapper be a window parent?
     */
    virtual bool IsValidParent() const { return true; }
    /**
     * @brief return true if this item is inside a sizer
     */
    bool IsSizerItem() const;

    /**
     * @brief return true if this element is an AUI pane
     *
     */
    bool IsAuiPane() const;

    /**
     * @brief return true if the widget is a toolbar item (but not a control)
     */
    virtual bool IsToolBarTool() const { return false; }
    /**
     * @brief by default, we are not managed by wxAUIManager
     */
    virtual bool IsAuiManaged() const;

    bool IsGridBagSizerItem() const;

    /**
     * @brief return true of this child is the last child of its parent
     */
    bool IsLastChild() const;

    /**
     * @brief return true if this item as a toolbar item
     * @return
     */
    bool IsParentToolbar() const;

    /**
     * @brief return true if the direct parent of this control is
     * of type wxAuiToolBar
     */
    bool IsParentAuiToolbar() const;

    /**
     * @brief is this wrapper represents a top window class?
     */
    virtual bool IsTopWindow() const { return false; }

    /**
     * @brief is this wxcWidget represents a wxWindow derived object?
     */
    virtual bool IsWxWindow() const { return true; }
    /**
     * @brief return true if this window has a child of type toolbar
     */
    bool HasToolBar() const;
    /**
     * @brief return true if this window has a child of type menubar
     */
    bool HasMenuBar() const;
    /**
     * @brief return true if this window has a child of type status bar
     */
    bool HasStatusBar() const;

    virtual bool IsSizer() const { return false; }

    /**
     * @brief return true if this window is a container and MUST have sizer
     * an example for this is 'wxPanel', wxScrolledWindow'
     */
    virtual bool IsContainer() const { return false; }

    /**
     * @brief return true of this class represents a real window (i.e. not sizer) AND
     * has at least one direct child of type sizer
     * @return
     */
    virtual bool HasMainSizer() const;

    virtual void ChildrenXRC(wxString& text, XRC_TYPE type) const;

    void RemoveChild(wxcWidget* child);
    void AddChild(wxcWidget* child)
    {
        child->SetParent(this);
        m_children.push_back(child);
    }

    void InsertBefore(wxcWidget* item, wxcWidget* insertBefore);
    void InsertAfter(wxcWidget* item, wxcWidget* insertAfter);
    wxcWidget* GetAdjacentSibling(bool previous) const; // i.e. the previous (or next) control with the same parent
    wxcWidget* GetAdjacentSiblingSizer(bool* isAbove = NULL) const; // Return an adjacent sibling that's a sizer, or
                                                                    // NULL. If non-NULL, direction will hold whether
                                                                    // above or below

    void SetChildren(const List_t& children) { this->m_children = children; }

    void SetPropertyString(const wxString& property, const wxString& value)
    {
        DoSetPropertyStringValue(property, value);
    }

    void SetName(const wxString& name) { DoSetPropertyStringValue(PROP_NAME, name); }

    void SetTitle(const wxString& title) { DoSetPropertyStringValue(PROP_TITLE, title); }

    void SetFilename(const wxString& filename) { DoSetPropertyStringValue(PROP_FILE, filename); }

    void SetVirtualFolder(const wxString& vd) { DoSetPropertyStringValue(PROP_VIRTUAL_FOLDER, vd); }

    virtual void SetParent(wxcWidget* parent);

    const List_t& GetChildren() const { return m_children; }
    List_t& GetChildren() { return m_children; }

    virtual wxString GetName() const { return PropertyString(PROP_NAME); }

    /**
     * @brief return the "real" control name. By default the widget name is the name entered by the user
     * in the "Name" field. In order for the code to compile properly, this name should be unique (in the top level
     * window)
     * However, some wxcWidgets have a "Name" property, but it should not be a unique (a good example for this, is a
     * column name, which is not a real control)
     * Such wxcWidgets may override this function and provide an empty string (or anything else)
     */
    virtual wxString GetRealName() const { return PropertyString(PROP_NAME); }

    wxString GetVirtualFolder() const { return PropertyString(PROP_VIRTUAL_FOLDER); }
    wxcWidget* GetParent() const { return m_parent; }
    wxSize GetSize() const;
    void RemoveFromParent();
    wxString SizerFlags(const wxString& defaultFlags) const;
    size_t SizerFlagsAsInteger() const;
    wxString StyleFlags(const wxString& deafultStyle = wxT("")) const;
    size_t StyleFlagsAsInteger() const;
    virtual wxMenu* GetEventsMenu() { return m_eventsMenu; }

    static CONTROL_TYPE GetWidgetType(int type);
    CONTROL_TYPE GetWidgetType() const;

    /**
     * @brief return true of 'this' is a child (direct or indirect)
     * of 'p'
     */
    bool IsDirectOrIndirectChildOf(wxcWidget* p) const;
    void SetAuiPaneInfo(const AuiPaneInfo& auiPaneInfo) { this->m_auiPaneInfo = auiPaneInfo; }
    const AuiPaneInfo& GetAuiPaneInfo() const { return m_auiPaneInfo; }

    AuiPaneInfo& GetAuiPaneInfo() { return m_auiPaneInfo; }

    /**
     * @brief Recursively fills the set with every control's names in this (initially) top-level window
     */
    void StoreNames(std::set<wxString>& store);
};

#endif // WRAPPERBASE_H
