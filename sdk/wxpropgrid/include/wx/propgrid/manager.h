/////////////////////////////////////////////////////////////////////////////
// Name:        manager.h
// Purpose:     wxPropertyGridManager
// Author:      Jaakko Salli
// Modified by:
// Created:     Jan-14-2005
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_MANAGER_H_
#define _WX_PROPGRID_MANAGER_H_

#include <wx/propgrid/propgrid.h>

#if wxPG_INCLUDE_MANAGER || defined(DOXYGEN)

#include <wx/dcclient.h>
#include <wx/scrolwin.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/dialog.h> 

// -----------------------------------------------------------------------

#ifndef SWIG
extern WXDLLIMPEXP_PG const wxChar *wxPropertyGridManagerNameStr;
#endif

/** @class wxPropertyGridPage
    @ingroup classes
    @brief
    Holder of property grid page information. You can subclass this and
    give instance in wxPropertyGridManager::AddPage. It inherits from
    wxEvtHandler and can be used to process events specific to this
    page (id of events will still be same as manager's). If you don't
    want to use it to process all events of the page, you need to
    return false in the derived wxPropertyGridPage::IsHandlingAllEvents.

    Please note that wxPropertyGridPage lacks many non-const property
    manipulation functions found in wxPropertyGridManager. Please use
    parent manager (m_manager member variable) when needed.

    Please note that most member functions are inherited and as such not documented on
    this page. This means you will probably also want to read wxPropertyGridInterface
    class reference.

    <h4>Derived from</h4>

    wxPropertyGridInterface\n
    wxPropertyGridState\n
    wxEvtHandler\n
    wxObject\n

    <h4>Include files</h4>

    <wx/propgrid/manager.h>

    <h4>Event handling</h4>

    wxPropertyGridPage receives events emitted by its wxPropertyGridManager, but
    only those events that are specific to that page. If wxPropertyGridPage::IsHandlingAllEvents
    returns false, then unhandled events are sent to the manager's parent, as usual.
*/
class WXDLLIMPEXP_PG wxPropertyGridPage : public wxEvtHandler,
                                          public wxPropertyGridInterface,
                                          public wxPropertyGridState
{
    friend class wxPropertyGridManager;
#ifndef SWIG
	DECLARE_CLASS(wxPropertyGridPage)
#endif
public:

    wxPropertyGridPage();
    virtual ~wxPropertyGridPage();

    /** Deletes all properties on page.
    */
    virtual void Clear();

    /** Reduces column sizes to minimum possible that contents are still visibly (naturally
        some margin space will be applied as well).

        @retval
        Minimum size for the page to still display everything.

        @remarks
        This function only works properly if size of containing grid was already fairly large.

        Note that you can also get calculated column widths by calling GetColumnWidth()
        immediately after this function returns.
    */
    wxSize FitColumns();

    /** Returns page index in manager;
    */
    inline int GetIndex() const;

    /** Returns x-coordinate position of splitter on a page.
    */
    int GetSplitterPosition( int col = 0 ) const { return GetStatePtr()->DoGetSplitterPosition(col); }

    /** Returns "root property". It does not have name, etc. and it is not
        visible. It is only useful for accessing its children.
    */
    wxPGProperty* GetRoot() const { return GetStatePtr()->DoGetRoot(); }

    /** Return pointer to contained property grid state.
    */
    wxPropertyGridState* GetStatePtr()
    {
        return this;
    }

    /** Return pointer to contained property grid state.
    */
    const wxPropertyGridState* GetStatePtr() const
    {
        return this;
    }

    /** Returns id of the tool bar item that represents this page on wxPropertyGridManager's wxToolBar.
    */
    int GetToolId() const
    {
        return m_id;
    }

    /** Do any member initialization in this method.
        @remarks
        - Called every time the page is added into a manager.
        - You can add properties to the page here.
    */
    virtual void Init() {}

    /** Return false here to indicate unhandled events should be
        propagated to manager's parent, as normal.
    */
    virtual bool IsHandlingAllEvents() const { return true; }

    /** Called every time page is about to be shown.
        Useful, for instance, creating properties just-in-time.
    */
    virtual void OnShow();

    virtual void RefreshProperty( wxPGProperty* p );

    /** Sets splitter position on page.
        @remarks
        Splitter position cannot exceed grid size, and therefore setting it during
        form creation may fail as initial grid size is often smaller than desired
        splitter position, especially when sizers are being used.
    */
    void SetSplitterPosition( int splitterPos, int col = 0 );

protected:

    /** Propagate to other pages.
    */
    virtual void DoSetSplitterPosition( int pos,
                                        int splitterColumn = 0,
                                        bool allPages = false,
                                        bool fromAutoCenter = false );

    /** Page label (may be referred as name in some parts of documentation).
        Can be set in constructor, or passed in wxPropertyGridManager::AddPage(),
        but *not* in both.
    */
    wxString                m_label;

#ifndef SWIG

    //virtual bool ProcessEvent( wxEvent& event );

    wxPropertyGridManager*  m_manager;
    
    int                     m_id;  // toolbar index

private:
    bool                    m_isDefault; // is this base page object?

private:
    DECLARE_EVENT_TABLE()
#endif
};

// -----------------------------------------------------------------------

/** @class wxPropertyGridManager
    @ingroup classes
    @brief
    wxPropertyGridManager is an efficient multi-page version of wxPropertyGrid,
    which can optionally have toolbar for mode and page selection, and help text box.
    Use window flags to select components to include.

    <h4>Derived from</h4>

    wxPropertyGridInterface\n
    wxWindow\n
    wxEvtHandler\n
    wxObject\n

    <h4>Include files</h4>

    <wx/propgrid/manager.h>

    <h4>Window styles</h4>

    @link wndflags Additional Window Styles@endlink

    <h4>Event handling</h4>

    To process input from a propertygrid control, use these event handler macros to
    direct input to member functions that take a wxPropertyGridEvent argument.

    <table>
    <tr><td>EVT_PG_SELECTED (id, func)</td><td>Property is selected.</td></tr>
    <tr><td>EVT_PG_CHANGED (id, func)</td><td>Property value is modified.</td></tr>
    <tr><td>EVT_PG_CHANGING (id, func)</td><td>Property value is about to be changed. Use wxPropertyGridEvent::GetValue() to take a peek at the pending value.</td></tr>
    <tr><td>EVT_PG_HIGHLIGHTED (id, func)</td><td>Mouse moves over property. Event's property is NULL if hovered on area that is not a property.</td></tr>
    <tr><td>EVT_PG_PAGE_CHANGED (id, func)</td><td>User changed page in manager.</td></tr>
    <tr><td>EVT_PG_ITEM_COLLAPSED (id, func)</td><td>User collapses a property or category.</td></tr>
    <tr><td>EVT_PG_ITEM_EXPANDED (id, func)</td><td>User expands a property or category.</td></tr>
    <tr><td>EVT_BUTTON (id, func)</td><td>Button in a property editor was clicked. Only occurs if the property doesn't handle button clicks itself.</td></tr>
    <tr><td>EVT_TEXT (id, func)</td><td>wxTextCtrl based editor was updated (but property value was not yet modified)</td></tr>
    </table>

    @sa @link wxPropertyGridEvent wxPropertyGridEvent@endlink

*/
// BM_MANAGER
class WXDLLIMPEXP_PG wxPropertyGridManager : public wxPanel, public wxPropertyGridInterface
{
#ifndef SWIG
	DECLARE_CLASS(wxPropertyGridManager)
#endif
    friend class wxPropertyGridPage;
public:

#ifdef SWIG
    %pythonAppend wxPropertyGridManager {
        self._setOORInfo(self)
        self.DoDefaultTypeMappings()
        self.edited_objects = {}
        self.DoDefaultValueTypeMappings()
        if not hasattr(self.__class__,'_vt2setter'):
            self.__class__._vt2setter = {}
    }
    %pythonAppend wxPropertyGridManager() ""

    wxPropertyGridManager( wxWindow *parent, wxWindowID id = wxID_ANY,
               	           const wxPoint& pos = wxDefaultPosition,
               	           const wxSize& size = wxDefaultSize,
               	           long style = wxPGMAN_DEFAULT_STYLE,
               	           const wxChar* name = wxPyPropertyGridManagerNameStr );
    %RenameCtor(PrePropertyGridManager,  wxPropertyGridManager());

#else

	/** Two step constructor. Call Create when this constructor is called to build up the
	    wxPropertyGridManager.
	*/
    wxPropertyGridManager();

    /** The default constructor. The styles to be used are styles valid for
        the wxWindow.
        @sa @link wndflags Additional Window Styles@endlink
    */
    wxPropertyGridManager( wxWindow *parent, wxWindowID id = wxID_ANY,
               	           const wxPoint& pos = wxDefaultPosition,
               	           const wxSize& size = wxDefaultSize,
               	           long style = wxPGMAN_DEFAULT_STYLE,
               	           const wxChar* name = wxPropertyGridManagerNameStr );

    /** Destructor */
    virtual ~wxPropertyGridManager();

#endif

    /** Creates new property page. Note that the first page is not created
        automatically.
        @param label
        A label for the page. This may be shown as a toolbar tooltip etc.
        @param bmp
        Bitmap image for toolbar. If wxNullBitmap is used, then a built-in
        default image is used.
        @param pageObj
        wxPropertyGridPage instance. Manager will take ownership of this object.
        NULL indicates that a default page instance should be created.
        @retval
        Returns index to the page created.
        @remarks
        If toolbar is used, it is highly recommended that the pages are
        added when the toolbar is not turned off using window style flag
        switching.
    */
    int AddPage( const wxString& label = wxEmptyString,
                 const wxBitmap& bmp = wxPG_NULL_BITMAP,
                 wxPropertyGridPage* pageObj = (wxPropertyGridPage*) NULL )
    {
        return InsertPage(-1,label,bmp,pageObj);
    }

    /** Returns true if all property grid data changes have been committed. Usually
        only returns false if value in active editor has been invalidated by a
        wxValidator.
    */
    bool CanClose()
    {
        return m_pPropGrid->CanClose();
    }

    void ClearModifiedStatus ( wxPGPropArg id );

    void ClearModifiedStatus ()
    {
        m_pPropGrid->ClearModifiedStatus();
    }

    /** Deletes all all properties and all pages.
    */
    virtual void Clear();

    /** Deletes all properties on given page.
    */
    void ClearPage( int page );

    /** Forces updating the value of property from the editor control.
        Returns true if DoPropertyChanged was actually called.
    */
    bool CommitChangesFromEditor( wxUint32 flags = 0 )
    {
        return m_pPropGrid->CommitChangesFromEditor(flags);
    }

    /** Two step creation. Whenever the control is created without any parameters,
        use Create to actually create it. Don't access the control's public methods
        before this is called.
        @sa @link wndflags Additional Window Styles@endlink
    */
    bool Create( wxWindow *parent, wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxPGMAN_DEFAULT_STYLE,
                 const wxChar* name = wxPropertyGridManagerNameStr );

    /** Enables or disables (shows/hides) categories according to parameter enable.
        WARNING: Not tested properly, use at your own risk.
    */
    bool EnableCategories( bool enable )
    {
        long fl = m_windowStyle | wxPG_HIDE_CATEGORIES;
        if ( enable ) fl = m_windowStyle & ~(wxPG_HIDE_CATEGORIES);
        SetWindowStyleFlag(fl);
        return true;
    }

    /** Selects page, scrolls and/or expands items to ensure that the
        given item is visible. Returns true if something was actually done.
    */
    bool EnsureVisible( wxPGPropArg id );

    /** Returns number of children of the root property of the selected page. */
    size_t GetChildrenCount()
    {
        return GetChildrenCount( m_pPropGrid->m_pState->m_properties );
    }

    /** Returns number of children of the root property of given page. */
    size_t GetChildrenCount( int pageIndex );

    /** Returns number of children for the property.

        NB: Cannot be in container methods class due to name hiding.
    */
    size_t GetChildrenCount( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(0)
        return p->GetChildCount();
    }

    /** Returns number of columns on given page. By the default,
        returns number of columns on current page. */
    int GetColumnCount( int page = -1 ) const;

    /** Returns height of the description text box. */
    int GetDescBoxHeight() const;

    /** Returns pointer to the contained wxPropertyGrid. This does not change
        after wxPropertyGridManager has been created, so you can safely obtain
        pointer once and use it for the entire lifetime of the instance.
    */
    wxPropertyGrid* GetGrid()
    {
        wxASSERT(m_pPropGrid);
        return m_pPropGrid;
    };

    const wxPropertyGrid* GetGrid() const
    {
        wxASSERT(m_pPropGrid);
        return (const wxPropertyGrid*)m_pPropGrid;
    };

    /** Returns iterator class instance.
        @remarks
        Calling this method in wxPropertyGridManager causes run-time assertion failure.
        Please only iterate through individual pages or use CreateVIterator().
    */
    wxPropertyGridIterator GetIterator( int flags = wxPG_ITERATE_DEFAULT, wxPGProperty* firstProp = NULL )
    {
        wxFAIL_MSG(wxT("Please only iterate through individual pages or use CreateVIterator()"));
        return wxPropertyGridInterface::GetIterator( flags, firstProp );
    }

    wxPropertyGridConstIterator GetIterator( int flags = wxPG_ITERATE_DEFAULT, wxPGProperty* firstProp = NULL ) const
    {
        wxFAIL_MSG(wxT("Please only iterate through individual pages or use CreateVIterator()"));
        return wxPropertyGridInterface::GetIterator( flags, firstProp );
    }

    /** Returns iterator class instance.
        @remarks
        Calling this method in wxPropertyGridManager causes run-time assertion failure.
        Please only iterate through individual pages or use CreateVIterator().
    */
    wxPropertyGridIterator GetIterator( int flags, int startPos )
    {
        wxFAIL_MSG(wxT("Please only iterate through individual pages or use CreateVIterator()"));
        return wxPropertyGridInterface::GetIterator( flags, startPos );
    }

    wxPropertyGridConstIterator GetIterator( int flags, int startPos ) const
    {
        wxFAIL_MSG(wxT("Please only iterate through individual pages or use CreateVIterator()"));
        return wxPropertyGridInterface::GetIterator( flags, startPos );
    }

    /** Similar to GetIterator, but instead returns wxPGVIterator instance,
        which can be useful for forward-iterating through arbitrary property
        containers.
    */
    virtual wxPGVIterator GetVIterator( int flags ) const;

#if wxPG_COMPATIBILITY_1_2_0

    /** Returns id of last child of given property.
        @deprecated
        Since version 1.3. Use GetPage()->GetIterator() instead.
        @remarks
        Returns even sub-properties.
    */
    wxPGProperty* GetLastChild( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullProperty)
        if ( !p->GetChildCount() ) return wxNullProperty;
        return p->Last();
    }

#endif // wxPG_COMPATIBILITY_1_2_0

    /** Returns currently selected page.
    */
    wxPropertyGridPage* GetCurrentPage() const
    {
        return GetPage(m_selPage);
    }

    /** Returns last page.
    */
    wxPropertyGridPage* GetLastPage() const
    {
        return GetPage((unsigned int)m_arrPages.size()-1);
    }

    /** Returns page object for given page index.
    */
    wxPropertyGridPage* GetPage( unsigned int ind ) const
    {
        return (wxPropertyGridPage*)m_arrPages.Item(ind);
    }

    /** Returns page object for given page name.
    */
    wxPropertyGridPage* GetPage( const wxString& name ) const
    {
        return GetPage(GetPageByName(name));
    }

    /** Returns index for a page name. If no match is found, wxNOT_FOUND is returned. */
    int GetPageByName( const wxString& name ) const;

    /** Returns index for a relevant propertygrid state. If no match is found,
        wxNOT_FOUND is returned.
    */
    int GetPageByState( const wxPropertyGridState* pstate ) const;

    /** Returns wxPropertyGridState of given page, current page's for -1.
    */
    virtual wxPropertyGridState* GetPageState( int page ) const;

    /** Returns number of managed pages. */
    size_t GetPageCount() const;

    /** Returns name of given page. */
    const wxString& GetPageName( int index ) const;

    /** Returns "root property" of the given page. It does not have name, etc.
        and it is not visible. It is only useful for accessing its children.
    */
    wxPGProperty* GetPageRoot( int index ) const;

    /** Returns id of property with given label (case-sensitive). If there is no
        property with such label, returned property id is invalid ( i.e. it will return
        false with IsOk method). If there are multiple properties with identical name,
        most recent added is returned.
    */
    wxPGProperty* GetPropertyByLabel( const wxString& name,
                                      wxPropertyGridState** ppState = (wxPropertyGridState**)NULL ) const;

    /** Returns cell background colour of a property. */
    wxColour GetPropertyColour( wxPGPropArg id ) const
    {
        return m_pPropGrid->GetPropertyColour(id);
    }

    /** Returns cell text colour of a property. */
    wxColour GetPropertyTextColour( wxPGPropArg id ) const
    {
        return m_pPropGrid->GetPropertyTextColour(id);
    }

    /** Returns index to currently selected page. */
    int GetSelectedPage() const { return m_selPage; }

    /** Shortcut for GetGrid()->GetSelection(). */
    wxPGProperty* GetSelectedProperty() const
    {
        return m_pPropGrid->GetSelection();
    }

    /** Synonyme for GetSelectedPage. */
    int GetSelection() const { return m_selPage; }

#if wxPG_COMPATIBILITY_1_2_0
    /** @deprecated
        Since version 1.3. Use GetPage() and wxPropertyGridPage facilities instead.
    */
    wxDEPRECATED( int GetTargetPage() const );
#endif

    /** Returns a pointer to the toolbar currently associated with the
        wxPropertyGridManager (if any). */
    wxToolBar* GetToolBar() const { return m_pToolbar; }

    /** Creates new property page. Note that the first page is not created
        automatically.
        @param index
        Add to this position. -1 will add as the last item.
        @param label
        A label for the page. This may be shown as a toolbar tooltip etc.
        @param bmp
        Bitmap image for toolbar. If wxNullBitmap is used, then a built-in
        default image is used.
        @param pageObj
        wxPropertyGridPage instance. Manager will take ownership of this object.
        If NULL, default page object is constructed.
        @retval
        Returns index to the page created.
    */
    virtual int InsertPage( int index, const wxString& label, const wxBitmap& bmp = wxNullBitmap,
                            wxPropertyGridPage* pageObj = (wxPropertyGridPage*) NULL );

    /** Returns true if any property on any page has been modified by the user. */
    bool IsAnyModified() const;

    /** Returns true if updating is frozen (ie. Freeze() called but not yet Thaw() ). */
    bool IsFrozen() const { return (m_pPropGrid->m_frozen>0)?true:false; }

    /** Returns true if any property on given page has been modified by the user. */
    bool IsPageModified( size_t index ) const;

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect* rect = (const wxRect*) NULL );

#if wxPG_COMPATIBILITY_1_2_0
    void RegisterAdvancedPropertyClasses() { }
#endif

    /** Removes a page.
        @retval
        Returns false if it was not possible to remove page in question.
    */
    virtual bool RemovePage( int page );

    /** Select and displays a given page.

        @param index
        Index of page being seleced. Can be -1 to select nothing.
    */
    void SelectPage( int index );

    /** Select and displays a given page (by label). */
    void SelectPage( const wxString& label )
    {
        int index = GetPageByName(label);
        wxCHECK_RET( index >= 0, wxT("No page with such name") );
        SelectPage( index );
    }

    /** Select and displays a given page. */
    void SelectPage( wxPropertyGridPage* ptr )
    {
        SelectPage( GetPageByState(ptr) );
    }

    /** Select a property. */
    bool SelectProperty( wxPGPropArg id, bool focus = false )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return p->GetParentState()->DoSelectProperty(p, focus);
    }

    /** Sets number of columns on given page (default is current page).
    */
    void SetColumnCount( int colCount, int page = -1 );

    /** Sets label and text in description box.
    */
    void SetDescription( const wxString& label, const wxString& content );

    /** Sets text colour of a category caption (but not it's children).
    */
    void SetCaptionTextColour( wxPGPropArg id, const wxColour& col )
    {
        m_pPropGrid->SetCaptionTextColour( id, col );
    }

    /** Sets y coordinate of the description box splitter. */
    void SetDescBoxHeight( int ht, bool refresh = true );

    /** Sets property attribute for all applicapple properties.
        Be sure to use this method after all properties have been
        added to the grid.
    */
    void SetPropertyAttributeAll( const wxString& name, wxVariant value );

#if wxPG_COMPATIBILITY_1_2_0
    /** Sets background colour of property and all its children. Colours of
        captions are not affected. Background brush cache is optimized for often
        set colours to be set last.

        @deprecated
        Since version 1.3. Use SetPropertyBackgroundColour instead.
    */
    wxDEPRECATED( void SetPropertyColour( wxPGPropArg id, const wxColour& col ) );
#endif

    /** Sets background colour of property and all its children. Colours of
        captions are not affected. Background brush cache is optimized for often
        set colours to be set last.
    */
    void SetPropertyBackgroundColour( wxPGPropArg id, const wxColour& col )
    {
        m_pPropGrid->SetPropertyBackgroundColour( id, col );
    }

    /** Sets text colour of property and all its children.
    */
    void SetPropertyTextColour( wxPGPropArg id, const wxColour& col )
    {
        m_pPropGrid->SetPropertyTextColour( id, col );
    }

    /** Sets background and text colour of property and all its children to the default. */
    void SetPropertyColourToDefault( wxPGPropArg id )
    {
        m_pPropGrid->SetPropertyColourToDefault(id);
    }

    /** Moves splitter as left as possible, while still allowing all
        labels to be shown in full.
        @param subProps
        If false, will still allow sub-properties (ie. properties which
        parent is not root or category) to be cropped.
        @param allPages
        If true, takes labels on all pages into account.
    */
    void SetSplitterLeft( bool subProps = false, bool allPages = true );

    /** Sets splitter position on individual page. */
    void SetPageSplitterPosition( int page, int pos, int column = 0 )
    {
        GetPage(page)->DoSetSplitterPosition( pos, column );
    }

    /** Sets splitter position for all pages.
        @remarks
        Splitter position cannot exceed grid size, and therefore setting it during
        form creation may fail as initial grid size is often smaller than desired
        splitter position, especially when sizers are being used.
    */
    void SetSplitterPosition( int pos, int column = 0 );

    /** Synonyme for SelectPage(name). */
    void SetStringSelection( const wxChar* name )
    {
        SelectPage( GetPageByName(name) );
    }

#if wxPG_COMPATIBILITY_1_2_0
    /** @deprecated
        Since version 1.3. Use GetPage() and wxPropertyGridPage facilities instead.
    */
    wxDEPRECATED( void SetTargetPage( int ) );
    wxDEPRECATED( void SetTargetPage( const wxChar* ) );
#endif

    /** Deselect current selection, if any (from current page).
        @retval
        false if editor could not be closed.
    */
    bool ClearSelection()
    {
        return m_pPropGrid->ClearSelection();
    }

#ifdef SWIG
    %pythoncode {
        def GetValuesFromPage(self,page,dict_=None,as_strings=False):
            "Same as GetValues, but returns values from specific page only. For argument descriptions, see GetValues."

            if dict_ is None:
                dict_ = {}
            elif hasattr(dict_,'__dict__'):
                dict_ = dict_.__dict__

            if not as_strings:
                getter = self.GetPropertyValue
            else:
                getter = self.GetPropertyValueAsString

            root = self.GetPageRoot(page)
            self._GetValues(root,self.GetFirstChild(root),dict_,getter)

            return dict_


        def GetValues(self,dict_=None,as_strings=False,inc_attributes=False):
            "Returns values in the grid."
            ""
            "dict_: if not given, then a new one is created. dict_ can be"
            "  object as well, in which case it's __dict__ is used."
            "as_strings: if True, then string representations of values"
            "  are fetched instead of native types. Useful for config and such."
            ""
            "Return value: dictionary with values. It is always a dictionary,"
            "so if dict_ was object with __dict__ attribute, then that attribute"
            "is returned."
            ""

            if dict_ is None:
                dict_ = {}
            elif hasattr(dict_,'__dict__'):
                dict_ = dict_.__dict__

            if not as_strings:
                getter = self.GetPropertyValue
            else:
                getter = self.GetPropertyValueAsString

            for page in range(0,self.GetPageCount()):
                root = self.GetPageRoot(page)
                self._GetValues(root,self.GetFirstChild(root),dict_,getter,inc_attributes)

            return dict_

        GetPropertyValues = GetValues

    }
#endif

protected:

    //
    // Subclassing helpers
    //

    /** Creates property grid for the manager. Override to use subclassed
        wxPropertyGrid.
    */
    virtual wxPropertyGrid* CreatePropertyGrid() const;

    virtual void RefreshProperty( wxPGProperty* p );

public:

#ifndef DOXYGEN

    //
    // Overridden functions - no documentation required.
    //

    virtual wxSize DoGetBestSize() const;
    void SetId( wxWindowID winid );

    virtual void Freeze();
    virtual void Thaw();
    virtual void SetExtraStyle ( long exStyle );
    virtual bool SetFont ( const wxFont& font );
    virtual void SetWindowStyleFlag ( long style );

protected:

public:

#ifndef SWIG

    //
    // Event handlers
    //
    void OnMouseMove( wxMouseEvent &event );
    void OnMouseClick( wxMouseEvent &event );
    void OnMouseUp( wxMouseEvent &event );
    void OnMouseEntry( wxMouseEvent &event );

    void OnPaint( wxPaintEvent &event );

    void OnToolbarClick( wxCommandEvent &event );
    void OnResize( wxSizeEvent& event );
    void OnPropertyGridSelect( wxPropertyGridEvent& event );

protected:

    wxPropertyGrid* m_pPropGrid;

    wxArrayPtrVoid  m_arrPages;

#if wxUSE_TOOLBAR
    wxToolBar*      m_pToolbar;
#endif
    wxStaticText*   m_pTxtHelpCaption;
    wxStaticText*   m_pTxtHelpContent;

    wxPropertyGridPage*     m_emptyPage;

    long            m_iFlags;

    // Selected page index.
    int             m_selPage;

    int             m_width;

    int             m_height;

    int             m_extraHeight;

    int             m_splitterY;

    int             m_splitterHeight;

    int             m_nextTbInd;

    int             m_dragOffset;

    wxCursor        m_cursorSizeNS;

    int             m_nextDescBoxSize;

    wxWindowID      m_baseId;

    unsigned char   m_dragStatus;

    unsigned char   m_onSplitter;



    virtual wxPGProperty* DoGetPropertyByName( wxPGPropNameStr name ) const;

    /** Select and displays a given page. */
    virtual bool DoSelectPage( int index );

    // Sets some members to defaults.
	void Init1();

    // Initializes some members.
	void Init2( int style );

/*#ifdef __WXMSW__
    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle) const;
#endif*/

    /** Recalculates new positions for components, according to the
        given size.
    */
    void RecalculatePositions( int width, int height );

    /** (Re)creates/destroys controls, according to the window style bits. */
    void RecreateControls();

    void UpdateDescriptionBox( int new_splittery, int new_width, int new_height );

    void RepaintSplitter( wxDC& dc, int new_splittery, int new_width, int new_height, bool desc_too );

    void SetDescribedProperty( wxPGProperty* p );

    // Reimplement these to handle "descboxheight" state item
    virtual bool SetEditableStateItem( const wxString& name, wxVariant value );
    virtual wxVariant GetEditableStateItem( const wxString& name ) const;

    virtual bool ProcessEvent( wxEvent& event );

private:
    DECLARE_EVENT_TABLE()
#endif // #ifndef SWIG
#endif // #ifndef DOXYGEN
};

// -----------------------------------------------------------------------

inline int wxPropertyGridPage::GetIndex() const
{
    if ( !m_manager )
        return wxNOT_FOUND;
    return m_manager->GetPageByState(this);
}

// -----------------------------------------------------------------------

#endif // wxPG_INCLUDE_MANAGER

#endif // _WX_PROPGRID_MANAGER_H_
