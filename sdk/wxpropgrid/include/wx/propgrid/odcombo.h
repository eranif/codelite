///////////////////////////////////////////////////////////////////////////////
// Name:        odcombo.h
// Purpose:     wxPGOwnerDrawnComboBox and related classes interface
// Author:      Jaakko Salli
// Modified by:
// Created:     Jan-25-2005
// RCS-ID:      $Id:
// Copyright:   (c) 2005 Jaakko Salli
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_ODCOMBO_H_
#define _WX_PROPGRID_ODCOMBO_H_

#ifndef DOXYGEN

#include "wx/renderer.h"


// Item counts in GUI components were changed in 2.7.0
#if wxCHECK_VERSION(2,7,0)
    #define wxODCCount  unsigned int
    #define wxODCIndex  unsigned int
#else
    #define wxODCCount  int
    #define wxODCIndex  int
#endif


class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT wxButton;

#ifdef WXMAKINGLIB_PROPGRID
    #define WXDLLEXPORT_PGODC
#elif defined(WXMAKINGDLL_PROPGRID)
    #define WXDLLEXPORT_PGODC WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLEXPORT_PGODC WXIMPORT
#else // not making nor using DLL
    #define WXDLLEXPORT_PGODC
#endif

#define wxODComboBoxNameStr	wxT("wxOwnerDrawnComboBox")

class WXDLLEXPORT_PGODC wxPGComboPopup;
class WXDLLEXPORT_PGODC wxPGComboControlBase;
class WXDLLEXPORT_PGODC wxPGOwnerDrawnComboBox;


// ----------------------------------------------------------------------------


// New window styles for wxPGComboControlBase

enum
{
    // Double-clicking causes value to cycle (ie. call to popup's CycleValue method).
    wxPGCC_DCLICK_CYCLES              = 0x0100,

    // Use keyboard behaviour alternate to platform default:
    // Up an down keys will show popup instead of cycling value.
    wxPGCC_ALT_KEYS                   = 0x0200
};

#define wxPGCC_PROCESS_ENTER            0

// ----------------------------------------------------------------------------
// wxPGComboControlBase: a base class for generic control that looks like
// a wxComboBox but allows completely custom popup (in addition to other
// customizations).
// ----------------------------------------------------------------------------

// wxComboControl internal flags
enum
{
    // First those that can be passed to Customize.
    // It is Windows style for all flags to be clear.

    // Button is preferred outside the border (GTK style)
    wxPGCC_BUTTON_OUTSIDE_BORDER      = 0x0001,
    // Show popup on mouse up instead of mouse down (which is the Windows style)
    wxPGCC_POPUP_ON_MOUSE_UP          = 0x0002,
    // All text is not automatically selected on click
    wxPGCC_NO_TEXT_AUTO_SELECT        = 0x0004,
    // Drop-button stays depressed while the popup is open
    wxPGCC_BUTTON_STAYS_DOWN          = 0x0008,
    // Button covers the entire control
    wxPGCC_FULL_BUTTON                = 0x0010,

    // Internal use: signals creation is complete
    wxPGCC_IFLAG_CREATED              = 0x0100,
    // Internal use: really put button outside
    wxPGCC_IFLAG_BUTTON_OUTSIDE       = 0x0200,
    // Internal use: SetTextIndent has been called
    wxPGCC_IFLAG_INDENT_SET           = 0x0400,
    // Internal use: Set wxTAB_TRAVERSAL to parent when popup is dismissed
    wxPGCC_IFLAG_PARENT_TAB_TRAVERSAL = 0x0800,
    // Button has bitmap or has non-standard size
    wxPGCC_IFLAG_HAS_NONSTANDARD_BUTTON = 0x1000
};


// Flags used by PreprocessMouseEvent and HandleButtonMouseEvent
enum
{
    wxPGCC_MF_ON_BUTTON               =   0x0001, // cursor is on dropbutton area
};


// Namespace for wxComboControl feature flags
struct wxPGComboControlFeatures
{
    enum
    {
        MovableButton       = 0x0001, // Button can be on either side of control
        BitmapButton        = 0x0002, // Button may be replace with bitmap
        ButtonSpacing       = 0x0004, // Non-bitmap button can have spacing from
                                      // the edge of the control
        TextIndent          = 0x0008, // SetTextIndent can be used
        PaintControl        = 0x0010, // Combo control itself can be custom painted
        PaintWritable       = 0x0020, // A variable-width area in front of writable
                                      // combo control's textctrl can be custom
                                      // painted
        Borderless          = 0x0040, // wxNO_BORDER window style works

        // There is no feature flags for...
        // PushButtonBitmapBackground - if its in renderer native, then it should be
        //   not an issue to have it automatically under the bitmap.

        All                 = MovableButton|BitmapButton|
                              ButtonSpacing|TextIndent|
                              PaintControl|PaintWritable|
                              Borderless,
    };
};


class wxPGComboPopupWindow;


class WXDLLEXPORT_PGODC wxPGComboControlBase : public wxControl
{
    friend class wxPGComboPopup;
    friend class wxPGComboPopupWindow;
public:
    // ctors and such
    wxPGComboControlBase() : wxControl() { Init(); }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString& name);

    virtual ~wxPGComboControlBase();

    virtual bool Destroy();

    // show/hide popup window
    virtual void ShowPopup();
    virtual void HidePopup();

    // Override for totally custom combo action
    virtual void OnButtonClick();

    // return true if the popup is currently shown
    inline bool IsPopupShown() const { return m_isPopupShown; }

    // set interface class instance derived from wxPGComboPopup
    void SetPopup( wxPGComboPopup* iface );

    // get interface class instance derived from wxPGComboPopup
    wxPGComboPopup* GetPopup() const { return m_popupInterface; }

    // get the popup window containing the popup control
    wxWindow *GetPopupWindow() const { return m_winPopup; }

    // get the popup control/panel in window
    wxWindow *GetPopupControl() const { return m_popup; }

    // Get the text control which is part of the combobox.
    inline wxTextCtrl *GetTextCtrl() const { return m_text; }

    // get the dropdown button which is part of the combobox
    // note: its not necessarily a wxButton or wxBitmapButton
    inline wxWindow *GetButton() const { return m_btn; }

    // forward these methods to all subcontrols
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual bool SetFont(const wxFont& font);
#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif

    // wxTextCtrl methods - for readonly combo they should return
    // without errors.
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);
    void SetText(const wxString& value)
    {
        SetValue(value);
    }
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);
    virtual void Undo();

    // Returns the text field rectangle, including any image that is painted with it.
    inline const wxRect& GetTextRect() const
    {
        return m_tcArea;
    }

    //
    // Popup customization methods
    //

    // Sets minimum width of the popup. If wider than combo control, it will extend to the left.
    // Remarks:
    // * Value -1 indicates the default.
    // * Custom popup may choose to ignore this (wxPGOwnerDrawnComboBox does not).
    inline void SetPopupMinWidth( int width )
    {
        m_widthMinPopup = width;
    }

    // Sets preferred maximum height of the popup.
    // Remarks:
    // * Value -1 indicates the default.
    // * Custom popup may choose to ignore this (wxPGOwnerDrawnComboBox does not).
    inline void SetPopupMaxHeight( int height )
    {
        m_heightPopup = height;
    }

    // Extends popup size horizontally, relative to the edges of the combo control.
    // Remarks:
    // * Popup minimum width may override extLeft (ie. it has higher precedence).
    // * Values 0 indicate default.
    // * Custom popup may not take this fully into account (wxPGOwnerDrawnComboBox takes).
    inline void SetPopupExtents( int extLeft, int extRight )
    {
        m_extLeft = extLeft;
        m_extRight = extRight;
    }

    // Set width, in pixels, of custom paint area in writable combo.
    // In read-only, used to indicate area that is not covered by the
    // focus rectangle (which may or may not be drawn, depending on the
    // popup type).
    void SetCustomPaintWidth( int width );
    inline int GetCustomPaintWidth() const { return m_widthCustomPaint; }

    // Set position of dropdown button.
    //   width: 0 > for specific custom width, negative to adjust to smaller than default
    //   height: 0 > for specific custom height, negative to adjust to smaller than default
    //   side: wxLEFT or wxRIGHT, indicates on which side the button will be placed.
    //   spacingX: empty space on sides of the button. Default is 0.
    // Remarks:
    //   There is no spacingY - the button will be centered vertically.
    void SetButtonPosition( int width = 0, int height = 0, int side = wxRIGHT,
                            int spacingX = 0 /*, int spacingY = 0*/ );


    //
    // Sets dropbutton to be drawn with custom bitmaps.
    //
    //  bmpNormal: drawn when cursor is not on button
    //  blankButtonBg: Draw blank button background below the image.
    //                 NOTE! This is only properly supported on platforms with appropriate
    //                       method in wxRendererNative.
    //  bmpPressed: drawn when button is depressed
    //  bmpHover: drawn when cursor hovers on button. This is ignored on platforms
    //            that do not generally display hover differently.
    //  bmpDisabled: drawn when combobox is disabled.
    void SetButtonBitmaps( const wxBitmap& bmpNormal,
                           bool blankButtonBg = false,
                           const wxBitmap& bmpPressed = wxNullBitmap,
                           const wxBitmap& bmpHover = wxNullBitmap,
                           const wxBitmap& bmpDisabled = wxNullBitmap );

    //
    // This will set the space in pixels between left edge of the control and the
    // text, regardless whether control is read-only (ie. no wxTextCtrl) or not.
    // Platform-specific default can be set with value-1.
    // Remarks
    // * This method may do nothing on some native implementations.
    void SetTextIndent( int indent );

    // Returns actual indentation in pixels.
    inline wxCoord GetTextIndent() const
    {
        return m_absIndent;
    }

    //
    // Utilies needed by the popups or native implementations
    //

    // Draws focus background (on combo control) in a way typical on platform.
    // Unless you plan to paint your own focus indicator, you should always call this
    // in your wxPGComboPopup::PaintComboControl implementation.
    // In addition, it sets pen and text colour to what looks good and proper
    // against the background.
    // flags: wxRendererNative flags: wxCONTROL_ISSUBMENU: is drawing a list item instead of combo control
    //                                wxCONTROL_SELECTED: list item is selected
    //                                wxCONTROL_DISABLED: control/item is disabled
    virtual void DrawFocusBackground( wxDC& dc, const wxRect& rect, int flags );

    // Returns true if focused. Differs from FindFocus in that takes
    // child controls into account.
    bool IsFocused() const
    {
        const wxWindow* curFocus = FindFocus();
        if ( curFocus == this || (m_text && curFocus == m_text) )
            return true;

        return false;
    }

    // Returns true if focus indicator should be drawn.
    inline bool ShouldDrawFocus() const
    {
        const wxWindow* curFocus = FindFocus();
        return ( !m_isPopupShown &&
                 (curFocus == this || (m_btn && curFocus == m_btn)) &&
                 (m_windowStyle & wxCB_READONLY) );
    }

    // These methods return references to appropriate dropbutton bitmaps
    inline const wxBitmap& GetBitmapNormal() const { return m_bmpNormal; }
    inline const wxBitmap& GetBitmapPressed() const { return m_bmpPressed; }
    inline const wxBitmap& GetBitmapHover() const { return m_bmpHover; }
    inline const wxBitmap& GetBitmapDisabled() const { return m_bmpDisabled; }

    // Return internal flags
    inline wxUint32 GetInternalFlags() const { return m_iFlags; }

    // Return true if Create has finished
    inline bool IsCreated() const { return m_iFlags & wxPGCC_IFLAG_CREATED ? true : false; }

    // Popup may use these as callbacks to measure and draw list items.
    // (wxPGOwnerDrawnComboBox uses these, obviously)
    // item: -1 means item is the combo control itself
    // flags: wxPGCC_PAINTING_CONTROL is set if painting to combo control instead of list
    // return value: OnDrawItem must return true if it did anything
    virtual bool OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const;

    // Return item height, or -1 for text height (default)
    virtual wxCoord OnMeasureItem( size_t item ) const;

    // Return item width, or -1 for calculating from text extent (default)
    virtual wxCoord OnMeasureItemWidth( size_t item ) const;

    // Returns true if can and should send focus event to the main control from
    // textctrl input handler.
    inline bool ConsumingTextCtrlFocusEvent()
    {
        if ( m_skipTextCtrlFocusEvents == 0 )
            return true;

        m_skipTextCtrlFocusEvents--;
        return false;
    }

    // NOTE:
    // I basicly needed to add callback methods into wxComboControlBase - otherwise it
    // will not be easily possible to use wxPGVListBoxComboPopup from simultaneously existing
    // wxComboControl and wxPGGenericComboControl (since some native implementations
    // might not have all the features, I really would like to have this options).

protected:

    //
    // Override these for customization purposes
    //

    // called from wxSizeEvent handler
    virtual void OnResize() = 0;

    // Return native text identation (for pure text, not textctrl)
    virtual wxCoord GetNativeTextIndent() const;

    // Called in syscolourchanged handler and base create
    virtual void OnThemeChange();

    // Creates wxTextCtrl.
    //   extraStyle: Extra style parameters
    void CreateTextCtrl( int extraStyle, const wxValidator& validator );

    // Installs standard input handler to combo
    void InstallInputHandlers();

    // Flags for DrawButton.
    enum
    {
        Button_PaintBackground             = 0x0001, // Paints control background below the button
        Button_BitmapOnly                  = 0x0002  // Only paints the bitmap
    };

    // Draws dropbutton. Using wxRenderer or bitmaps, as appropriate.
    // Flags are defined above.
    void DrawButton( wxDC& dc, const wxRect& rect, int flags = Button_PaintBackground );

    // Call if cursor is on button area or mouse is captured for the button.
    //bool HandleButtonMouseEvent( wxMouseEvent& event, bool isInside );
    bool HandleButtonMouseEvent( wxMouseEvent& event, int flags );

    // Conversion to double-clicks and some basic filtering
    // returns true if event was consumed or filtered (event type is also set to 0 in this case)
    //bool PreprocessMouseEvent( wxMouseEvent& event, bool isOnButtonArea );
    bool PreprocessMouseEvent( wxMouseEvent& event, int flags );

    //
    // This will handle left_down and left_dclick events outside button in a Windows-like manner.
    // If you need alternate behaviour, it is recommended you manipulate and filter events to it
    // instead of building your own handling routine (for reference, on wxEVT_LEFT_DOWN it will
    // toggle popup and on wxEVT_LEFT_DCLICK it will do the same or run the popup's dclick method,
    // if defined - you should pass events of other types of it for common processing).
    void HandleNormalMouseEvent( wxMouseEvent& event );

    // Creates popup window, calls interface->Create(), etc
    void CreatePopup();

    // common code to be called on popup hide/dismiss
    void OnPopupDismiss();

    // Dispatches command event from the control
    //void SendEvent( int evtType = wxEVT_COMMAND_COMBOBOX_SELECTED, int selection, );

    // override the base class virtuals involved in geometry calculations
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual wxSize DoGetBestSize() const;

    // Recalculates button and textctrl areas. Called when size or button setup change.
    //   btnWidth: default/calculated width of the dropbutton. 0 means unchanged,
    //             just recalculate.
    void CalculateAreas( int btnWidth = 0 );

    // Standard textctrl positioning routine. Just give it platform-dependant
    // textctrl coordinate adjustment.
    virtual void PositionTextCtrl( int textCtrlXAdjust, int textCtrlYAdjust );

    // event handlers
    void OnSizeEvent(wxSizeEvent& event);
    void OnFocusEvent(wxFocusEvent& event);
    void OnKeyEvent(wxKeyEvent& event);
    void OnTextCtrlEvent(wxCommandEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // Set customization flags (directs how wxComboControlBase helpers behave)
    void Customize( wxUint32 flags ) { m_iFlags |= flags; }

    // Dispatches size event and refreshes
    void RecalcAndRefresh();


    // This is used when m_text is hidden (readonly).
    wxString                m_valueString;

    // the text control and button we show all the time
    wxTextCtrl*             m_text;
    wxWindow*               m_btn;

    // wxPopupWindow or similar containing the window managed by the interface.
    wxWindow*               m_winPopup;

    // the popup control/panel
    wxWindow*               m_popup;

    // popup interface
    wxPGComboPopup*         m_popupInterface;

    // this is input etc. handler the text control
    wxEvtHandler*           m_textEvtHandler;

    // this is for the top level window
    wxEvtHandler*           m_toplevEvtHandler;

    // this is for the control in popup
    wxEvtHandler*           m_popupExtraHandler;

    // needed for "instant" double-click handling
    //wxLongLong              m_timeLastMouseUp;

    // used to prevent immediate re-popupping incase closed popup
    // by clicking on the combo control (needed because of inconsistent
    // transient implementation across platforms).
    wxLongLong              m_timeCanAcceptClick;

    // how much popup should expand to the left/right of the control
    wxCoord                 m_extLeft;
    wxCoord                 m_extRight;

    // minimum popup width
    wxCoord                 m_widthMinPopup;

    // preferred popup height
    wxCoord                 m_heightPopup;

    // how much of writable combo is custom-paint by callback?
    // also used to indicate area that is not covered by "blue"
    // selection indicator.
    wxCoord                 m_widthCustomPaint;

    // absolute text indentation, in pixels
    wxCoord                 m_absIndent;

    // Width of the "fake" border
    wxCoord                 m_widthCustomBorder;

    // The button and textctrl click/paint areas
    wxRect                  m_tcArea;
    wxRect                  m_btnArea;

    // current button state (uses renderer flags)
    int                     m_btnState;

    // button position
    int                     m_btnWid;
    int                     m_btnHei;
    int                     m_btnSide;
    int                     m_btnSpacingX;

    // last default button width
    int                     m_btnWidDefault;

    // custom dropbutton bitmaps
    wxBitmap                m_bmpNormal;
    wxBitmap                m_bmpPressed;
    wxBitmap                m_bmpHover;
    wxBitmap                m_bmpDisabled;

    // area used by the button
    wxSize                  m_btnSize;

    // platform-dependant customization and other flags
    wxUint32                m_iFlags;

    // draw blank button background under bitmap?
    bool                    m_blankButtonBg;

    // is the popup window currenty shown?
    bool                    m_isPopupShown;

    // TODO: Remove after real popup works ok.
    unsigned char           m_fakePopupUsage;

    wxByte                  m_skipTextCtrlFocusEvents;

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_ABSTRACT_CLASS(wxPGComboControlBase)
};



class WXDLLEXPORT_PGODC wxPGGenericComboControl : public wxPGComboControlBase
{
public:
    // ctors and such
    wxPGGenericComboControl() : wxPGComboControlBase() { Init(); }

    wxPGGenericComboControl(wxWindow *parent,
                          wxWindowID id = wxID_ANY,
                          const wxString& value = wxEmptyString,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxValidator& validator = wxDefaultValidator,
                          const wxString& name = wxODComboBoxNameStr)
        : wxPGComboControlBase()
    {
        Init();

        (void)Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxODComboBoxNameStr);

    virtual ~wxPGGenericComboControl();

    static int GetFeatures() { return wxPGComboControlFeatures::All; }

protected:

    virtual void OnResize();

    // event handlers
    //void OnSizeEvent( wxSizeEvent& event );
    void OnPaintEvent( wxPaintEvent& event );
    void OnMouseEvent( wxMouseEvent& event );

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxPGGenericComboControl)
};



#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)

class WXDLLEXPORT_PGODC wxPGComboControl : public wxPGComboControlBase
{
public:
    // ctors and such
    wxPGComboControl() : wxPGComboControlBase() { Init(); }

    wxPGComboControl(wxWindow *parent,
                          wxWindowID id = wxID_ANY,
                          const wxString& value = wxEmptyString,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxValidator& validator = wxDefaultValidator,
                          const wxString& name = wxODComboBoxNameStr)
        : wxPGComboControlBase()
    {
        Init();

        (void)Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxODComboBoxNameStr);

    virtual ~wxPGComboControl();

    //virtual void DrawFocusBackground( wxDC& dc, const wxRect& rect, int flags );

    static int GetFeatures() { return wxPGComboControlFeatures::All; }

protected:

    // customization
    virtual void OnResize();
    virtual wxCoord GetNativeTextIndent() const;
    virtual void OnThemeChange();

    // event handlers
    //void OnSizeEvent( wxSizeEvent& event );
    void OnPaintEvent( wxPaintEvent& event );
    void OnMouseEvent( wxMouseEvent& event );

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxPGComboControl)
};

#else

class WXDLLEXPORT_PGODC wxPGComboControl : public wxPGGenericComboControl
{
public:
    // ctors and such
    wxPGComboControl() : wxPGGenericComboControl() {}

    wxPGComboControl(wxWindow *parent,
                          wxWindowID id = wxID_ANY,
                          const wxString& value = wxEmptyString,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxValidator& validator = wxDefaultValidator,
                          const wxString& name = wxODComboBoxNameStr)
        : wxPGGenericComboControl()
    {
        (void)Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxODComboBoxNameStr)
    {
        return wxPGGenericComboControl::Create(parent,id,value,pos,size,style,validator,name);
    }

    virtual ~wxPGComboControl() {}

protected:

private:
    DECLARE_DYNAMIC_CLASS(wxPGComboControl)
};


#endif



// ----------------------------------------------------------------------------
// wxComboPopupInterface is the interface class that lies between
// the wxPGOwnerDrawnComboBox and its popup window.
// ----------------------------------------------------------------------------

// wxComboPopup internal flags
enum
{
    wxPGCP_IFLAG_CREATED      = 0x0001, // Set by wxComboControlBase after Create is called
};

class WXDLLEXPORT_PGODC wxPGComboPopup
{
    friend class wxPGComboControlBase;
public:
    wxPGComboPopup(wxPGComboControl *combo)
    {
        m_combo = combo;
        m_iFlags = 0;
    }

    virtual ~wxPGComboPopup();

    // Create the popup child control.
    // Return true for success.
    virtual bool Create(wxWindow* parent) = 0;

    // We must have an associated control which is subclassed by the combobox.
    virtual wxWindow *GetControl() = 0;

    // Called immediately after the popup is shown
    virtual void OnPopup();

    // Called when popup is dismissed
    virtual void OnDismiss();

    // Called just prior to displaying popup.
    // Default implementation does nothing.
    virtual void SetStringValue( const wxString& value );

    // Gets displayed string representation of the value.
    virtual wxString GetStringValue() const = 0;

    // This is called to custom paint in the combo control itself (ie. not the popup).
    // Default implementation draws value as string.
    virtual void PaintComboControl( wxDC& dc, const wxRect& rect );

    // Receives key events from the parent wxComboControl.
    // Events not handled should be skipped, as usual.
    virtual void OnComboKeyEvent( wxKeyEvent& event );

    // Implement if you need to support special action when user
    // double-clicks on the parent wxComboControl.
    virtual void OnComboDoubleClick();

    // Return final size of popup. Called on every popup, just prior to OnShow.
    // minWidth = preferred minimum width for window
    // prefHeight = preferred height. Only applies if > 0,
    // maxHeight = max height for window, as limited by screen size
    //   and should only be rounded down, if necessary.
    virtual wxSize GetAdjustedSize( int minWidth, int prefHeight, int maxHeight );

    // Return true if you want delay call to Create until the popup is shown
    // for the first time. It is more efficient, but note that it is often
    // more convenient to have the control created immediately.
    // Default returns false.
    virtual bool LazyCreate();

    //
    // Utilies
    //

    // Hides the popup
    void Dismiss();

    // Returns true if Create has been called.
    inline bool IsCreated() const
    {
        return (m_iFlags & wxPGCP_IFLAG_CREATED) ? true : false;
    }

protected:
    wxPGComboControl*     m_combo;
    wxUint32                    m_iFlags;
};




// New window styles for wxPGOwnerDrawnComboBox

enum
{
    // Causes double-clicking to cycle the item instead of showing
    // the popup. Shift-pressed causes cycling direction to reverse.
    //wxODCB_DOUBLE_CLICK_CYCLES      = wxPGCC_SPECIAL_DOUBLE_CLICK,

    // if used, control itself is not custom paint using callback
    // even if this is not used, writable combo is never custom paint
    // until SetCustomPaintWidth is called
    wxODCB_STD_CONTROL_PAINT        = 0x0400
};


//
// Callback flags
//

enum
{
    // when set, we are painting the selected item in control,
    // not in the popup
    wxPGCC_PAINTING_CONTROL       = 0x0001,
    wxPGCC_PAINTING_SELECTED      = 0x0002
};

//
// Callback arguments:
//   pCb: combo box in question
//   item: index of item drawn or measured
//   dc: device context to draw on. NULL reference when measuring.
//   rect: draw call: rectangle in device context to limit the drawing on. Use rect.x and rect.y
//                    as the origin.
//         measure call: initially width and height are -1. You need to set rect.height to whatever
//                    is the height of the given item.
//   flags: see above
/*
typedef void (wxEvtHandler::* wxComboPaintCallback)( wxPGComboControl* pCb,
                                                     int item,
                                                     wxDC& dc,
                                                     wxRect& rect,
                                                     int flags );
*/



#include "wx/vlbox.h"

// ----------------------------------------------------------------------------
// wxPGVListBoxComboPopup is a wxVListBox customized to act as a popup control.
//
// Notes:
//   wxOwnerDrawnComboBox uses this as its popup. However, it always derives
//   from native wxComboControl. If you need to use this popup with
//   wxPGGenericComboControl, then remember that vast majority of item manipulation
//   functionality is implemented in the wxPGVListBoxComboPopup class itself.
//
// ----------------------------------------------------------------------------


class wxPGVListBoxComboPopup : public wxVListBox, public wxPGComboPopup
{
    friend class wxPGOwnerDrawnComboBox;
public:

    // ctor and dtor
    wxPGVListBoxComboPopup(wxPGComboControl* combo/*, wxComboPaintCallback callback*/);
    virtual ~wxPGVListBoxComboPopup();

    // required virtuals
    virtual bool Create(wxWindow* parent);
    virtual wxWindow *GetControl() { return this; }
    virtual void SetStringValue( const wxString& value );
    virtual wxString GetStringValue() const;

    // more customization
    virtual void OnPopup();
    virtual wxSize GetAdjustedSize( int minWidth, int prefHeight, int maxHeight );
    virtual void PaintComboControl( wxDC& dc, const wxRect& rect );
    virtual void OnComboKeyEvent( wxKeyEvent& event );
    virtual void OnComboDoubleClick();
    //virtual bool CycleValue( bool forward );
    //virtual bool OnComboDoubleClick();
    virtual bool LazyCreate();

    // Item management
    void SetSelection( int item );
    void Insert( const wxString& item, int pos );
    int Append(const wxString& item);
    void Clear();
    void Delete( wxODCIndex item );
    void SetItemClientData(wxODCIndex n, void* clientData, wxClientDataType clientDataItemsType);
    void *GetItemClientData(wxODCIndex n) const;
    void SetString( int item, const wxString& str );
    wxString GetString( int item ) const;
    wxODCCount GetCount() const;
    int FindString(const wxString& s) const;
    int GetSelection() const;

    void Populate( int n, const wxString choices[] );
    void ClearClientDatas();

    // helpers
    inline int GetItemAtPosition( const wxPoint& pos ) { return HitTest(pos); }
    inline wxCoord GetTotalHeight() const { return EstimateTotalHeight(); }
    inline wxCoord GetLineHeight(int n) const { return OnMeasureItem(n); }

protected:

	virtual wxCoord OnGetRowHeight(size_t n) const { return OnMeasureItem(n); }

    // Called by OnComboDoubleClick and OnComboKeyEvent
    bool HandleKey( int keycode, bool saturate );

    // sends combobox select event from the parent combo control
    void SendComboBoxEvent( int selection );
    void DismissWithEvent();

    // Re-calculates width for given item
    void CheckWidth( int pos );

    // wxVListBox implementation
    virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
    virtual wxCoord OnMeasureItem(size_t n) const;
    void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const;

    // filter mouse move events happening outside the list box
    // move selection with cursor
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnLeftClick(wxMouseEvent& event);

#if wxCHECK_VERSION(2,8,0)
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
#endif

    wxArrayString           m_strings;
    wxArrayPtrVoid          m_clientDatas;
    wxArrayInt              m_widths; // cached line widths

    wxString                m_stringValue;

    wxFont                  m_font;

    int                     m_value; // selection
    int                     m_itemHover; // on which item the cursor is
    int                     m_widestWidth; // width of widest item thus far
    int                     m_avgCharWidth;
    int                     m_baseImageWidth; // how much per item drawn in addition to text
    int                     m_itemHeight; // default item height (calculate from font size
                                          // and used in the absence of callback)
    wxClientDataType        m_clientDataItemsType;

private:

    // has the mouse been released on this control?
    bool m_clicked;

    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// wxPGOwnerDrawnComboBox: a generic wxComboBox that allows custom paint items.
//
// ----------------------------------------------------------------------------

class WXDLLEXPORT_PGODC wxPGOwnerDrawnComboBox : public wxPGComboControl, public wxItemContainer
{
    friend class wxPGComboPopupWindow;
    friend class wxPGComboControlBase;
public:

    // ctors and such
    wxPGOwnerDrawnComboBox() : wxPGComboControl() { Init(); }

    wxPGOwnerDrawnComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               int n,
               const wxString choices[],
               //wxComboPaintCallback callback,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxODComboBoxNameStr)
        : wxPGComboControl()
    {
        Init();

        (void)Create(parent, id, value, pos, size, n, choices,
                     /*callback,*/ style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxODComboBoxNameStr);

    wxPGOwnerDrawnComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxODComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = (const wxString *) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxODComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxODComboBoxNameStr);

    virtual ~wxPGOwnerDrawnComboBox();

    // wxControlWithItems methods
    virtual void Clear();
    virtual void Delete(wxODCIndex n);
    virtual wxODCCount GetCount() const;
    virtual wxString GetString(wxODCIndex n) const;
    virtual void SetString(wxODCIndex n, const wxString& s);
    virtual int FindString(const wxString& s) const;
    virtual void Select(int n);
    virtual int GetSelection() const;
    void SetSelection(int n) { Select(n); }

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

protected:

    // clears all allocated client datas
    void ClearClientDatas();

    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, wxODCIndex pos);
    virtual void DoSetItemClientData(wxODCIndex n, void* clientData);
    virtual void* DoGetItemClientData(wxODCIndex n) const;
    virtual void DoSetItemClientObject(wxODCIndex n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(wxODCIndex n) const;

#if wxCHECK_VERSION(2,9,0)
    virtual int DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type);
    virtual void DoClear() { Clear(); }
    virtual void DoDeleteOneItem(unsigned int pos) { Delete(pos); }
#endif

    // overload m_popupInterface member so we can access specific popup interface easier
    wxPGVListBoxComboPopup*     m_popupInterface;

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxPGOwnerDrawnComboBox)
};

#endif  // !DOXYGEN

#endif // _WX_PROPGRID_ODCOMBO_H_
