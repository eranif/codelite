/***************************************************************
 * Name:      PolygonShape.h
 * Purpose:   Defines polygonial shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFPOLYGONSHAPE_H
#define _WXSFPOLYGONSHAPE_H

#include <wx/wxsf/RectShape.h>

// default values
/*! \brief Default value of wxSFPolygonShape::m_fConnextToVertex data member. */
#define sfdvPOLYGONSHAPE_VERTEXCONNECTIONS true

/*!
 * \brief Class extends the wxSFRectShape and encapsulates general polygon shape
 * defined by a set of its vertices. The class can be used as it is or as a base class
 * for shapes with more complex form and functionality.
 * \sa wxSFDiamondShape
 */
class WXDLLIMPEXP_SF wxSFPolygonShape : public wxSFRectShape
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFPolygonShape);

    /*! \brief Default constructor. */
	wxSFPolygonShape(void);
	/*!
     * \brief User constructor.
	 * \param n Number of the polygon vertices
	 * \param pts Array of the polygon vertices
	 * \param pos Relative position of the polygon shape
	 * \param manager Pointer of parent diagram manager
	 */
	wxSFPolygonShape(int n, const wxRealPoint pts[], const wxRealPoint& pos, wxSFDiagramManager* manager);
	/*!
     * \brief Copy constructor.
	 * \param obj Reference to a source object
	 */
	wxSFPolygonShape(const wxSFPolygonShape& obj);
	/*! \brief Destructor. */
	virtual ~wxSFPolygonShape(void);

    // public data accessors
    /*!
     * \brief Set connecting mode.
     * \param enable Set this parameter to TRUE if you want to connect
     * line shapes to the polygons's vertices, otherwise the lines will be connected
     * to the nearest point of the shape's border.
     */
    void SetConnectToVertex(bool enable){m_fConnectToVertex = enable;}
    /*!
     * \brief Get status of connecting mode.
     * \return TRUE if the line shapes will be connected to the polygon's vertices
     */
    bool IsConnectedToVertex(){return m_fConnectToVertex;}

	// public functions
	/*!
     * \brief Set the poly vertices which define its form.
	 * \param n Number of the vertices
	 * \param pts Array of the vertices
	 */
	void SetVertices(size_t n, const wxRealPoint pts[]);

	// public virtual functions
    /*!
     * \brief Resize the rectangle to bound all child shapes. The function can be overrided if neccessary. */
	virtual void FitToChildren();
	/*!
	 * \brief Get intersection point of the shape border and a line leading from
	 * 'start' point to 'end' point. The function can be overrided if neccessary.
	 * \param start Starting point of the virtual intersection line
     * \param end Ending point of the virtual intersection line
	 * \return Intersection point
	 */
	virtual wxRealPoint GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end);
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
	 * \param handle Reference to dragged handle
	 */
	virtual void OnHandle(wxSFShapeHandle &handle);

protected:
	// protected data members
	bool m_fConnectToVertex;
	wxXS::RealPointArray m_arrVertices;

	// protected functions
	/*!
	 * \brief Move all vertices so the polygon's relative bounding box position
	 * will be located in the origin.
	 */
	void NormalizeVertices();
	/*! \brief Scale polygon's vertices to fit into the rectangle bounding the polygon. */
	void FitVerticesToBoundingBox();
	/*! \brief Scale the bounding rectangle to fit all polygons vertices. */
	void FitBoundingBoxToVertices();
	/*!
	 * \brief Get polygon extents.
	 * \param minx Position of the left side of polygon's bounding box
	 * \param miny Position of the top side of polygon's bounding box
	 * \param maxx Position of the right side of polygon's bounding box
	 * \param maxy Position of the bottom side of polygon's bounding box
	 */
	void GetExtents(double *minx, double *miny, double *maxx, double *maxy);
	/*!
	 * \brief Get absolute positions of the polygon's vertices.
	 * \param pts Array of translated polygon's verices
	 */
	void GetTranslatedVerices(wxRealPoint pts[]);
	/*!
	 * \brief Get absolute positions of the polygon's vertices.
	 * \param pts Array of translated polygon's verices
	 */
	void GetTranslatedVerices(wxPoint pts[]);

    /*!
	 * \brief Draw the polygon shape.
     * \param dc Refernece to the device context where the shape will be drawn to
     */
	void DrawPolygonShape(wxDC& dc);

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

private:

	// private functions

	 /*! \brief Initialize serializable properties. */
	void MarkSerializableDataMembers();
};

#endif //_WXSFPOLYGONSHAPE_H
