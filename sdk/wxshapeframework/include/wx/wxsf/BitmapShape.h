/***************************************************************
 * Name:      BitmapShape.h
 * Purpose:   Defines bitmap shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFBITMAPSHAPE_H
#define _WXSFBITMAPSHAPE_H

#include <wx/wxsf/RectShape.h>

// default values
#define sfdvBITMAPSHAPE_SCALEIMAGE true
#define sfdvBITMAPSHAPE_XPMDATA wxT("")

/*!
 * \brief Class encapsulating the bitmap shape. The shape can display and control
 * files stored in formats supported by wxBitmap class loaded from a file or created
 * from XPM image. */
class WXDLLIMPEXP_SF wxSFBitmapShape : public wxSFRectShape
{
public:

	XS_DECLARE_CLONABLE_CLASS(wxSFBitmapShape);

    /*! \brief Default constructor. */
	wxSFBitmapShape(void);
	/*!
	 * \brief User contructor.
	 * \param pos Initial position
	 * \param bitmapPath BMP file name
	 * \param manager Pointer of parent manager
	 */
	wxSFBitmapShape(const wxRealPoint& pos, const wxString& bitmapPath, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Source shape
	 */
	wxSFBitmapShape(const wxSFBitmapShape& obj);
	/*! \brief Destructor */
	virtual ~wxSFBitmapShape(void);

	// public member data accessors
	/*!
	 * \brief Get full name of a source BMP file.
	 * \return String containig full file name
	 */
	wxString GetBitmapPath(){return m_sBitmapPath;}
	/*!
	 * \brief Enable/disable scaling mode of the bitmap.
	 * \param canscale Set TRUE if the bitmap shape could be scaled
	 */
	void EnableScale(bool canscale){m_fCanScale = canscale;}
	/*!
	 * \brief Get information about the possibility of the shape scaling.
	 * \return TRUE if the shape can be scaled, otherwise FALSE
	 */
	bool CanScale(){return m_fCanScale;}

	// public functions
    /*!
	 * \brief Load a bitmap from the file.
     * \param file File name (absolute or relative)
     * \param type Bitmap type (see the wxBitmap class reference)
     * \return TRUE on success, otherwise FALSE
     */
	bool CreateFromFile(const wxString& file, wxBitmapType type = wxBITMAP_TYPE_BMP );
    /*!
	 * \brief Load a bitmap from the XPM structure.
     * \param bits Buffer with the image bits
     * \return TRUE on success, otherwise FALSE
     */
	bool CreateFromXPM(const char* const* bits);

	// public virtual function
	/*!
	 * \brief Scale the bitmap shape in both directions. The function can be overrided if neccessary.
	 * \param x Scale ratio in the horizontal direction
	 * \param y Scale ratio in the vertical direction
     * \param children TRUE if the shape's children shoould be scaled as well, otherwise
     * the shape will be updated after scaling via Update() function.
     */
	virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);
	/*!
	 * \brief Event handler called when the user started to drag the shape handle. The function can be overrided if neccessary.
	 * \param handle Reference to the dragged shape handle
	 */
	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	/*!
	 * \brief Event handler called during dragging of the shape handle. The function can be overrided if neccessary.
	 * \param handle Reference to the dragged shape handle
	 */
	virtual void OnHandle(wxSFShapeHandle& handle);
	/*!
	 * \brief Event handler called when the user finished dragging of the shape handle. The function can be overrided if neccessary.
	 * \param handle Reference to the dragged shape handle
	 */
	virtual void OnEndHandle(wxSFShapeHandle& handle);

protected:

	// protected data members
	/*! \brief String containing the full bitmap file name. */
	wxString m_sBitmapPath;

	/*! \brief Currently processed (modified) bitmap. */
	wxBitmap m_Bitmap;
	/*! \brief Original archived bitmap. */
	wxBitmap m_OriginalBitmap;
	bool m_fCanScale;
	bool m_fRescaleInProgress;
	wxRealPoint m_nPrevPos;

	// protected functions
	/*!
	 * \brief Rescale the bitmap shape so it will fit the given extent. The
	 * shape position is not involved (the left-top bitmap corner is not moved).
	 * \param size New bitmap size
	 */
	void RescaleImage(const wxRealPoint& size);

	// protected virtual functions
	/*!
	 * \brief Draw the shape in the normal way. The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawNormal(wxDC& dc);
	/*!
	 * \brief Draw the shape in the hower mode (the mouse cursor is above the shape). The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawHover(wxDC& dc);
	/*!
	 * \brief Draw the shape in the highlighted mode (another shape is dragged over this shape and this shape will accept the dragged one if it will be dropped on it). The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawHighlighted(wxDC& dc);

	/*!
	 * \brief Serialize shape's properties to the given XML node.
	 * \param node Pointer to XML node where the shape's property nodes will be append to </param>
	 * \sa wxSFShapeBase::Serialize
	 */
	virtual wxXmlNode* Serialize(wxXmlNode* node);
	/*!
	 * \brief Deserialize shape's properties from the given XML node.
	 * \param node Source XML node containig the shape's property nodes
	 * \sa wxSFShapeBase::Deserialize
	 */
	virtual void Deserialize(wxXmlNode* node);

private:
	// private functions

	 /*! \brief Initialize serializable properties. */
	void MarkSerializableDataMembers();
};

#endif //_WXSFBITMAPSHAPE_H
