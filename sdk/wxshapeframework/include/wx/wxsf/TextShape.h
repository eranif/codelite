/***************************************************************
 * Name:      TextShape.h
 * Purpose:   Defines static text shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFTEXTSHAPE_H
#define _WXSFTEXTSHAPE_H

#include <wx/wxsf/RectShape.h>

// default values
/*! \brief Default value of wxSFTextShape::m_Font data member. */
#define sfdvTEXTSHAPE_FONT *wxSWISS_FONT
/*! \brief Default value of wxSFTextShape::m_TextColor data member. */
#define sfdvTEXTSHAPE_TEXTCOLOR *wxBLACK

/*! \brief Class encapsulates basic non-editable text shape which is suitable for
 * displaying of various text information in the canvas.
 * \sa wxSFEditTextShape
 */
class WXDLLIMPEXP_SF wxSFTextShape : public wxSFRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(wxSFTextShape);

    /*! \brief Default constructor. */
    wxSFTextShape(void);
    /*!
     * \brief User constructor.
     * \param pos Initial position
     * \param txt Text content
     * \param manager Pointer to parent diagram manager
     */
	wxSFTextShape(const wxRealPoint& pos, const wxString& txt, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Source objct
	 */
	wxSFTextShape(const wxSFTextShape& obj);
	/*! \brief Destructor */
    virtual ~wxSFTextShape();

    // public member data accessors
    /*!
     * \brief Set text font.
     * \param font Font
     */
    void SetFont(const wxFont& font);
    /*!
     * \brief Get text font.
     * \return Font
     */
    wxFont& GetFont(){return m_Font;}
    /*!
     * \brief Set text.
     * \param txt Text content
     */
    void SetText(const wxString& txt);
    /*!
     * \brief Get text.
     * \return Current text content
     */
    wxString GetText() const {return m_sText;}
    /*!
     * \brief Set text color.
     * \param col Text color
     */
    void SetTextColour(const wxColour& col){m_TextColor = col;}
    /*!
     * \brief Get text color.
     * \return Current text color
     */
    wxColour GetTextColour() const {return m_TextColor;}

    // public virtual functions
    /*!
	 * \brief Scale the shape size by in both directions. The function can be overrided if necessary
     * (new implementation should call default one ore scale shape's children manualy if neccesary).
     * \param x Horizontal scale factor
     * \param y Vertical scale factor
     * \param children TRUE if the shape's children shoould be scaled as well, otherwise the shape will be updated after scaling via Update() function.
     */
    virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);
	/*!
	 * \brief Event handler called during dragging of the shape handle.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation does nothing.
	 * \param handle Reference to dragged handle
	 */
    virtual void OnHandle(wxSFShapeHandle& handle);
	/*! \brief Upate shape (align all child shapes an resize it to fit them) */
	virtual void Update();

    // public functions
    wxSize GetTextExtent();
    void UpdateRectSize();

protected:

    // protected data members
    wxFont m_Font;
    wxColour m_TextColor;
    wxString m_sText;

    // protected virtual functions
	/*!
	 * \brief Draw the shape in the normal way. The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawNormal(wxDC& dc);
	/*!
	 * \brief Draw the shape in the hower mode (the mouse cursor is above the shape).
	 * The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawHover(wxDC& dc);
	/*!
	 * \brief Draw the shape in the highlighted mode (another shape is dragged over this
	 * shape and this shape will accept the dragged one if it will be dropped on it).
	 * The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawHighlighted(wxDC& dc);
	/*!
	 * \brief Draw shadow under the shape. The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shadow will be drawn to
	 */
	virtual void DrawShadow(wxDC& dc);

    /*!
     * \brief Serialize shape's properties to the given XML node. The serialization
     * routine is automatically called by the framework and should take care about serialization
     * of all specific (non-standard) shape's properties.
     * \param node Pointer to XML node where the shape's property nodes will be appended to
     * \sa xsSerializable::Serialize
     */
	virtual wxXmlNode* Serialize(wxXmlNode* node);
    /*!
     * \brief Deserialize shape's properties from the given XML node. The
     * routine is automatically called by the framework and should take care about deserialization
     * of all specific (non-standard) shape's properties.
     * \param node Pointer to a source XML node containig the shape's property nodes
     * \sa xsSerializable::Deserialize
     */
	virtual void Deserialize(wxXmlNode* node);

	/*!
     * \brief Event handler called during dragging of the left shape handle.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnLeftHandle(wxSFShapeHandle& handle);
	/*!
     * \brief Event handler called during dragging of the top shape handle.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnTopHandle(wxSFShapeHandle& handle);
	/*!
     * \brief Event handler called during dragging of the right shape handle.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnRightHandle(wxSFShapeHandle& handle);
	/*!
     * \brief Event handler called during dragging of the bottom shape handle.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */	
	virtual void OnBottomHandle(wxSFShapeHandle& handle);

	// protected functions
	/*!
	 * \brief Draw text shape.
	 * \param dc Device context where the text shape will be drawn to
	 */
	void DrawTextContent(wxDC& dc);

private:

	// private functions

	 /*! \brief Initialize serializable properties. */
	void MarkSerializableDataMembers();

	wxCoord m_nLineHeight;
};

#endif //_WXSFTEXTSHAPE_H
