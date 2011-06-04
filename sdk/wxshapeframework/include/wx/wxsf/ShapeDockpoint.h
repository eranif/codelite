/***************************************************************
 * Name:      ShapeDockpoint.h
 * Purpose:   Defines shape dockpoint class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2010-12-14
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFSHAPEDOCKPOINT_H_
#define _WXSFSHAPEDOCKPOINT_H_

#include <wx/wxsf/ScaledDC.h>
#include <wx/wxxmlserializer/XmlSerializer.h>

class WXDLLIMPEXP_SF wxSFShapeBase;

// default values
/*! \brief Default value of wxSFConnectionPoint::m_nRelPosition data member */
#define sfdvCONNPOINT_RELPOS wxRealPoint(0, 0)

/*!
 * \brief Class encapsulating fixed connection point assignable to shapes. The assigned fixed connection
 * points are the only places where connected lines can start/end. 
 * \sa wxSFShapeBase::AddConnectionPoint()
 */
class wxSFConnectionPoint : public xsSerializable
{
public:

	friend class wxSFShapeBase;
	
	XS_DECLARE_CLONABLE_CLASS(wxSFConnectionPoint);
	
	/*! \brief Connection point type */
	enum CPTYPE
	{
		cpUNDEF,
		cpTOPLEFT,
		cpTOPMIDDLE,
		cpTOPRIGHT,
		cpCENTERLEFT,
		cpCENTERMIDDLE,
		cpCENTERRIGHT,
		cpBOTTOMLEFT,
		cpBOTTOMMIDDLE,
		cpBOTTOMRIGHT,
		cpCUSTOM
	};
	
	/*!
	 * \brief Basic constructor.
	 */
	wxSFConnectionPoint();
	/*!
	 * \brief Enhanced constructor.
	 * \param parent Pointer to parent shape
	 * \param type Connection point type
	 */
	wxSFConnectionPoint(wxSFShapeBase *parent, CPTYPE type);
	/*!
	 * \brief Enhanced constructor.
	 * \param parent Pointer to parent shape
	 * \param relpos Relative position in percentages
	 * \param id Connection point ID
	 */
	wxSFConnectionPoint(wxSFShapeBase *parent, const wxRealPoint& relpos, long id = -1);
	/*!
	 * \brief Copy constructor.
	 * \param obj Reference to source object
	 */
	wxSFConnectionPoint(const wxSFConnectionPoint &obj);
	/*!
	 * \brief Destructor.
	 */
	virtual ~wxSFConnectionPoint() {;}
	
	/*!
	 * \brief Get connection point type.
	 * \return Connection point type
	 */
	inline CPTYPE GetType() const { return m_nType; }
	/*!
	 * \brief Set parent shape.
	 * \param parent Pointer to parent shape
	 */
	inline void SetParentShape(wxSFShapeBase *parent) { wxASSERT(parent); m_pParentShape = parent; }
	/*!
	 * \brief Get parent shape.
	 * \return Pointer to parent shape
	 */
	inline wxSFShapeBase* GetParentShape() const {return m_pParentShape; }
	/*!
	 * \brief Set relative position of custom connection point.
	 * \param relpos Relative position in percetnages
	 */
	inline void SetRelativePosition(const wxRealPoint& relpos) { m_nRelPosition = relpos; }
	/*!
	 * \brief Get relative position of custom connection point.
	 * \return Relative position in percentages
	 */
	inline const wxRealPoint& GetRelativePosition() const { return m_nRelPosition; }
	/*!
	 * \brief Get absolute position of the connection point.
	 * \return Absolute position of the connection point
	 */
	wxRealPoint GetConnectionPoint() const;
	
	/*!
	 * \brief Find out whether given point is inside the connection point.
	 * \param pos Examined point
	 * \return TRUE if the point is inside the handle, otherwise FALSE
	 */
	virtual bool Contains(const wxPoint& pos) const;
	
	/*!
	 * \brief Draw connection point.
	 * \param dc Device context where the handle will be drawn
	 */
	void Draw(wxDC& dc);
	/*! \brief Refresh (repaint) the dock point */
	void Refresh();
	
protected:

	/*!
	 * \brief Draw the connection point in the normal way. The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawNormal(wxDC& dc);
	/*!
	 * \brief Draw the connection point in the hower mode (the mouse cursor is above the shape).
	 * The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawHover(wxDC& dc);

private:

	/*!
     * \brief Event handler called when the mouse pointer is moving above shape canvas.
     * \param pos Current mouse position
     */
	void _OnMouseMove(const wxPoint& pos);
	
	void MarkSerializableDataMembers();
	
	CPTYPE m_nType;
	wxSFShapeBase *m_pParentShape;
	bool m_fMouseOver;
	wxRealPoint m_nRelPosition;
};

#endif //_WXSFSHAPEDOCKPOINT_H_
