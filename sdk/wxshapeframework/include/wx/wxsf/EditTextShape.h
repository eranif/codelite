/***************************************************************
 * Name:      EditTextShape.h
 * Purpose:   Defines editable text shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFEDITTEXTSHAPE_H
#define _WXSFEDITTEXTSHAPE_H

#include <wx/wxsf/TextShape.h>

#define sfCANCEL_TEXT_CHANGES false
#define sfAPPLY_TEXT_CHANGES true

/*! \brief Default value of wxSFEditTextShape::m_fForceMultiline data member */
#define sfdvEDITTEXTSHAPE_FORCEMULTILINE false
/*! \brief Default value of wxSFEditTextShape::m_nEditType data member */
#define sfdvEDITTEXTSHAPE_EDITTYPE wxSFEditTextShape::editINPLACE

class WXDLLIMPEXP_SF wxSFEditTextShape;

/*!
 * \brief Auxiliary class providing neccessary functionality needed for in-place
 * modification of a content of the text shape. </summary>
 * \sa wxSFEditTextShape
 */
class wxSFContentCtrl : public wxTextCtrl
{
public:
    /*!
	 * \brief Constructor.
     * \param parent Pointer to the parent window
     * \param id ID of the text control window
     * \param parentShape Pointer to the parent editable text shape
     * \param content Initial content of the text control
     * \param pos Initial position
     * \param size Initial size
     * \param style Window style
     */
	wxSFContentCtrl(wxWindow* parent, wxWindowID id, wxSFEditTextShape* parentShape, const wxString& content, wxPoint pos, wxSize size, int style);

    /*!
	 * \brief Finish the editing process/ 
	 * \param apply If TRUE then changes made in eddited text will be applied on text shape, otherwise it will be canceled 
	 */
	void Quit( bool apply = sfAPPLY_TEXT_CHANGES );

protected:

	wxWindow* m_pParent;
	wxSFEditTextShape* m_pParentShape;
	wxString m_sPrevContent;

    /*!
	 * \brief Event handler called if the text control lost the focus.
     * \param event Reference to the event class instance
     */
	void OnKillFocus(wxFocusEvent& event);
	/*!
	 * \brief Event handler called if the key was pressed in the text control.
	 * \param event Reference to the event class instance
	 */
	void OnKeyDown(wxKeyEvent& event);

	DECLARE_EVENT_TABLE();
};

/*!
 * \brief Auxiliary class providing neccessary functionality needed for dialog-based
 * modification of a content of the text shape. </summary>
 * \sa wxSFEditTextShape
 */
class wxSFDetachedContentCtrl : public wxDialog 
{
public:
    /*!
	 * \brief Constructor.
     * \param parent Pointer to the parent window
     * \param id ID of the text control window
     * \param title Dialog's title
     * \param pos Initial position
     * \param size Initial size
     * \param style Window style
     */
	wxSFDetachedContentCtrl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Edit content"),
							 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
							 long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	
	/*! \brief Destructor. */
	~wxSFDetachedContentCtrl();
	
	// public member data accessors
	/**
	 * \brief Set content of dialog's text edit control.
	 * \param txt Text content
	 */
	void SetContent(const wxString& txt){ m_pText->SetValue( txt ); }
	/**
	 * \brief Get content of dialog's text edit control.
	 * \return Edited text
	 */
	wxString GetContent() const { return m_pText->GetValue(); }
		
protected:
	// protected data members
	wxTextCtrl* m_pText;
};

/*!
 * \brief Class encapsulating the editable text shape. It extends the basic text shape.
 * \sa wxSFTextShape
 */
class WXDLLIMPEXP_SF wxSFEditTextShape : public wxSFTextShape
{
public:
	friend class wxSFContentCtrl;

	XS_DECLARE_CLONABLE_CLASS(wxSFEditTextShape);
	
	enum EDITTYPE
	{
		editINPLACE = 0,
		editDIALOG,
		editDISABLED
	};

    /*! \brief Default constructor. */
	wxSFEditTextShape(void);
    /*!
	 * \brief User constructor.
	 * \param pos Initial position
	 * \param txt Text content
	 * \param manager Pointer to the parent canvas
	 */
	wxSFEditTextShape(const wxRealPoint& pos, const wxString& txt, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFEditTextShape(const wxSFEditTextShape& obj);
	/*! \brief Destructor. */
	virtual ~wxSFEditTextShape(void);

	// public member data accessors
	/**
	 * \brief Set way how the text shape's content can be edited.
	 * \param type Edit control type
	 * \sa EDITTYPE
	 */
	void SetEditType( EDITTYPE type) { m_nEditType = type; }
	/**
	 * \brief Get current type of text shape's edit control.
	 * \return Type of edit control
	 * \sa EDITTYPE
	 */
	const EDITTYPE& GetEditType() const { return m_nEditType; }
	/*!
	 * \brief Get pointer to assigned text control allowing user to change the
	 * shape's content directly in the canvas.
	 * \return Pointer to instance of wxSFContentCtrl class
	 */
	wxSFContentCtrl* GetTextCtrl() {return m_pTextCtrl;}
	

	// public functions
	/*! \brief Switch the shape to a label editation mode. */
	void EditLabel();
	/*! \brief Force the edit text control to be multiline
	 *  \param multiline If TRUE then the associated text control will be allways multiline
	 */
	void ForceMultiline(bool multiline){m_fForceMultiline = multiline;}

	// public virtual functions
	/*!
	 * \brief Event handler called when the shape was double-clicked.
	 * The function can be overrided if neccessary.
	 * \param pos Mouse position.
	 */
	virtual void OnLeftDoubleClick(const wxPoint& pos);
	/*!
	 * \brief Event handler called when any key is pressed (in the shape canvas).
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * \param key The key code
	 * \return The function must return TRUE if the default event routine should be called
	 * as well, otherwise FALSE
	 * \sa wxSFShapeBase::OnKey
	 */
	virtual bool OnKey(int key);

protected:
	wxSFContentCtrl* m_pTextCtrl;
	
	long m_nCurrentState;
	bool m_fForceMultiline;
	EDITTYPE m_nEditType;
};

#endif //_WXSFEDITTEXTSHAPE_H
