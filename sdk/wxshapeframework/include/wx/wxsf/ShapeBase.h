/***************************************************************
 * Name:      ShapeBase.h
 * Purpose:   Defines shape base class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFSHAPEBASE_H
#define _WXSFSHAPEBASE_H

#include <wx/wxprec.h>

#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/arrstr.h>
#include <wx/list.h>

#include <wx/wxsf/ShapeHandle.h>
#include <wx/wxsf/ShapeDockpoint.h>
#include <wx/wxsf/Defs.h>
#include <wx/wxxmlserializer/XmlSerializer.h>

#define sfRECURSIVE true
#define sfNORECURSIVE false
#define sfDIRECT true
#define sfINDIRECT false
#define sfWITHCHILDREN true
#define sfWITHOUTCHILDREN false
#define sfANY NULL

// default values
/*! \brief Default value of wxSFShapeBase::m_fVisible data member */
#define sfdvBASESHAPE_VISIBILITY true
/*! \brief Default value of wxSFShapeBase::m_fActive data member */
#define sfdvBASESHAPE_ACTIVITY true
/*! \brief Default value of wxSFShapeBase::m_nHoverColor data member */
#define sfdvBASESHAPE_HOVERCOLOUR wxColor(120, 120, 255)
/*! \brief Default value of wxSFShapeBase::m_nRelativePosition data member */
#define sfdvBASESHAPE_POSITION wxRealPoint(0, 0)
/*! \brief Default value of wxSFShapeBase::m_nVAlign data member */
#define sfdvBASESHAPE_VALIGN valignNONE
/*! \brief Default value of wxSFShapeBase::m_nHAlign data member */
#define sfdvBASESHAPE_HALIGN halignNONE
/*! \brief Default value of wxSFShapeBase::m_nVBorder data member */
#define sfdvBASESHAPE_VBORDER 0
/*! \brief Default value of wxSFShapeBase::m_nHBorder data member */
#define sfdvBASESHAPE_HBORDER 0
/*! \brief Default value of wxSFShapeBase::m_nStyle data member */
#define sfdvBASESHAPE_DEFAULT_STYLE sfsDEFAULT_SHAPE_STYLE
/*! \brief Default value of wxSFShapeBase::m_nCustomDockPoint data member */
#define sfdvBASESHAPE_DOCK_POINT -3

typedef SerializableList ConnectionPointList;

class WXDLLIMPEXP_SF wxSFShapeCanvas;
class WXDLLIMPEXP_SF wxSFDiagramManager;
class WXDLLIMPEXP_SF wxSFShapeBase;

/*! \brief Add child shape component created by the parent shape (not by the serializer) to this parent shape.
 * Serializable properties of added component will be serialized as the parent shape's properties.
 */ 
#define SF_ADD_COMPONENT(comp, name) \
	comp->EnableSerialization( false );	\
	this->AddProperty( new xsProperty( &comp, wxT("serializabledynamicnocreate"), name ) );	\
	this->AddChild( comp );	\

WX_DECLARE_LIST_WITH_DECL(wxSFShapeBase, ShapeList, class WXDLLIMPEXP_SF);

/*! \brief Base class for all shapes providing fundamental functionality and publishing set
 *  of virtual functions which must be defined by the user in derived shapes. This class
 *  shouldn't be used as it is.
 *
 *  Shape objects derived from this class use hierarchical approach. It means that every
 *  shape must have defined parent shape (can be NULL for topmost shapes). An absolute
 *  shape position is then calculated as a sumation of all relative positions of all parent
 *  shapes. Also the size of the parent shape can be limited be a boundind box of all
 *  children shapes.
 *
 *  This class also declares set of virtual functions used as event handlers for various
 *  events (moving, sizing, drawing, mouse events, serialization and deserialization requests, ...)
 *  mostly triggered by a parent shape canvas.
 */
class WXDLLIMPEXP_SF wxSFShapeBase : public xsSerializable
{
public:

    friend class wxSFShapeCanvas;
    friend class wxSFDiagramManager;
    friend class wxSFShapeHandle;

	XS_DECLARE_CLONABLE_CLASS(wxSFShapeBase);

    /*! \brief Bit flags for wxSFShapeBase::GetCompleteBoundingBox function */
    enum BBMODE
    {
		bbSELF = 1,
        bbCHILDREN = 2,
        bbCONNECTIONS = 4,
        bbSHADOW = 8,
        bbALL = 15
    };

    /*! \brief Search mode flags for GetAssignedConnections function */
	enum CONNECTMODE
	{
	    /*! \brief Search for connection starting in examined shape */
	    lineSTARTING,
	    /*! \brief Search for connection ending in examined shape */
	    lineENDING,
	    /*! \brief Search for both starting and ending connections */
	    lineBOTH
	};

    /*! \brief Flags for SetVAlign function */
	enum VALIGN
	{
	    valignNONE,
	    valignTOP,
	    valignMIDDLE,
	    valignBOTTOM,
	    valignEXPAND,
		valignLINE_START,
		valignLINE_END
	};

    /*! \brief Flags for SetHAlign function */
	enum HALIGN
	{
	    halignNONE,
	    halignLEFT,
	    halignCENTER,
	    halignRIGHT,
	    halignEXPAND,
		halignLINE_START,
		halignLINE_END
	};

    /*! \brief Basic shape's styles used with SetStyle() function */
	enum STYLE
	{
	    /*! \brief Interactive parent change is allowed */
	    sfsPARENT_CHANGE = 1,
	    /*! \brief Interactive position change is allowed */
	    sfsPOSITION_CHANGE = 2,
	    /*! \brief Interactive size change is allowed */
	    sfsSIZE_CHANGE = 4,
	    /*! \brief Shape is highlighted at mouse hovering */
	    sfsHOVERING = 8,
	    /*! \brief Shape is highlighted at shape dragging */
	    sfsHIGHLIGHTING = 16,
	    /*! \brief Shape is always inside its parent */
	    sfsALWAYS_INSIDE = 32,
	    /*! \brief User data is destroyed at the shape deletion */
	    sfsDELETE_USER_DATA = 64,
	    /*! \brief The DEL key is processed by the shape (not by the shape canvas) */
	    sfsPROCESS_DEL = 128,
	    /*! \brief Show handles if the shape is selected */
	    sfsSHOW_HANDLES = 256,
	    /*! \brief Show shadow under the shape */
	    sfsSHOW_SHADOW = 512,
		/*! \brief Lock children relative position if the parent is resized */
		sfsLOCK_CHILDREN = 1024,
		/*! \brief Emit events (catchable in shape canvas) */
		sfsEMIT_EVENTS = 2048,
		/*! \brief Propagate mouse dragging event to parent shape */
		sfsPROPAGATE_DRAGGING = 4096,
		/*! \brief Propagate selection to parent shape (it means this shape cannot be selected because its focus is redirected to its parent shape) */
		sfsPROPAGATE_SELECTION = 8192,
		/*! \brief Default shape style */
		sfsDEFAULT_SHAPE_STYLE = sfsPARENT_CHANGE | sfsPOSITION_CHANGE | sfsSIZE_CHANGE | sfsHOVERING | sfsHIGHLIGHTING | sfsSHOW_HANDLES | sfsALWAYS_INSIDE | sfsDELETE_USER_DATA
	};

    /*! \brief Basic constructor. */
	wxSFShapeBase(void);
	/*!
	 * \brief Enhanced constructor.
	 * \param pos Initial relative position
	 * \param manager Pointer to parent diagram manager
     */
	wxSFShapeBase(const wxRealPoint& pos, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Reference to the source object
     */
	wxSFShapeBase(const wxSFShapeBase& obj);
	/*! \brief Destructor. */
	virtual ~wxSFShapeBase(void);

	// public functions

    /*! \brief Refresh (redraw) the shape */
	void Refresh();
	/*!
	 * \brief Draw shape. Default implementation tests basic shape visual states
	 * (normal/ready, mouse is over the shape, dragged shape can be accepted) and
	 * call appropriate virtual functions (DrawNormal, DrawHover, DrawHighlighted)
	 * for its visualisation. The function can be overrided if neccessary.
	 * \param dc Reference to a device context where the shape will be drawn to
	 * \param children TRUE if the shape's children should be drawn as well
	 */
	virtual void Draw(wxDC& dc, bool children = sfWITHCHILDREN);
    /*!
	 * \brief Test whether the given point is inside the shape. The function
     * can be overrided if neccessary.
     * \param pos Examined point
     * \return TRUE if the point is inside the shape area, otherwise FALSE
     */
	virtual bool Contains(const wxPoint& pos);
    /*!
	 * \brief Test whether the shape is completely inside given rectangle. The function
     * can be overrided if neccessary.
     * \param rct Examined rectangle
     * \return TRUE if the shape is completely inside given rectangle, otherwise FALSE
     */
	virtual bool IsInside(const wxRect& rct);
	/*!
	 * \brief Test whether the given rectangle intersects the shape.
	 * \param rct Examined rectangle
	 * \return TRUE if the examined rectangle intersects the shape, otherwise FALSE
	 */
	virtual bool Intersects(const wxRect& rct);
	/*!
	 * \brief Get the shape's absolute position in the canvas (calculated as a sumation
	 * of all relative positions in the shapes' hierarchy. The function can be overrided if neccessary.
	 * \return Shape's position
	 */
	virtual wxRealPoint GetAbsolutePosition();
	/*!
	 * \brief Get intersection point of the shape border and a line leading from
	 * 'start' point to 'end' point.  Default implementation does nothing. The function can be overrided if neccessary.
	 * \param start Starting point of the virtual intersection line
     * \param end Ending point of the virtual intersection line
	 * \return Intersection point
	 */
	virtual wxRealPoint GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end);
	/*!
	 * \brief Get shape's center. Default implementation does nothing. The function can be overrided if neccessary.
	 * \return Center point
	 */
	virtual wxRealPoint GetCenter();

	/*!
	 * \brief Function called by the framework responsible for creation of shape handles
     * at the creation time. Default implementation does nothing. The function can be overrided if neccesary.
     */
	virtual void CreateHandles();
	/*!
	 * \brief Show/hide shape handles. Hidden handles are inactive.
	 * \param show TRUE for showing, FALSE for hidding
	 */
	void ShowHandles(bool show);

    /*!
     * \brief Set shape's style.
	 *
	 * Default value is sfsPARENT_CHANGE | sfsPOSITION_CHANGE | sfsSIZE_CHANGE | sfsHOVERING | sfsHIGHLIGHTING | sfsSHOW_HANDLES | sfsALWAYS_INSIDE | sfsDELETE_USER_DATA
     * \param style Combination of the shape's styles
     * \sa STYLE
     */
    inline void SetStyle(long style) { m_nStyle = style; }
    /*! \brief Get current shape style. */
    inline long GetStyle() const { return m_nStyle; }
    inline void AddStyle(STYLE style) { m_nStyle |= style; }
    inline void RemoveStyle(STYLE style) { m_nStyle &= ~style; }
    inline bool ContainsStyle(STYLE style) const { return (m_nStyle & style) != 0; }

    /*!
	 * \brief Get child shapes associated with this (parent) shape.
     * \param type Type of searched child shapes (NULL for any type)
     * \param children List of child shapes
     * \param recursive Set this flag TRUE if also children of children of ... should be found (also sfRECURSIVE a sfNORECURSIVE constants can be used).
	 * \param mode Search mode (has sense only for recursive search) 
	 * \sa xsSerializable::SEARCHMODE 
     */
	void GetChildShapes(wxClassInfo *type, ShapeList& children, bool recursive = false, xsSerializable::SEARCHMODE mode = xsSerializable::searchBFS);
	/*!
	 * \brief Get neighbour shapes connected to this shape.
	 * \param neighbours List of neighbour shapes
	 * \param shapeInfo Line object type
	 * \param condir Connection direction
	 * \param direct Set this flag to TRUE if only closest shapes should be found,
	 * otherwise also shapes connected by forked lines will be found (also
	 * constants sfDIRECT and sfINDIRECT can be used)
	 * \sa CONNECTMODE
	 */
	void GetNeighbours(ShapeList& neighbours, wxClassInfo* shapeInfo, CONNECTMODE condir, bool direct = true);
	/*!
	 * \brief Get list of connections assigned to this shape.
	 * 
	 * Note: For proper functionality the shape must be managed by a diagram manager.
	 * \param shapeInfo Line object type
	 * \param mode Search mode
	 * \param lines Reference to shape list where pointers to all found connections will be stored
	 * \sa wxSFShapeBase::CONNECTMODE
	 */
	void GetAssignedConnections(wxClassInfo* shapeInfo, wxSFShapeBase::CONNECTMODE mode, ShapeList& lines);

    /*!
	 * \brief Get shapes's bounding box. The function can be overrided if neccessary.
     * \return Bounding rectangle
     */
	virtual wxRect GetBoundingBox();
	/*!
	 * \brief Get shape's bounding box which includes also associated child shapes and connections.
	 * \param rct Returned bounding rectangle
	 * \param mask Bit mask of object types which should be included into calculation
	 * \sa BBMODE
	 */
	void GetCompleteBoundingBox(wxRect& rct, int mask = bbALL);

    /*!
	 * \brief Scale the shape size by in both directions. The function can be overrided if necessary
     * (new implementation should call default one ore scale shape's children manualy if neccesary).
     * \param x Horizontal scale factor
     * \param y Vertical scale factor
     * \param children TRUE if the shape's children shoould be scaled as well, otherwise the shape will be updated after scaling via Update() function.
     */
	virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);
    /*!
	 * \brief Scale the shape size by in both directions.
     * \param scale Scaling factor
     * \param children TRUE if the shape's children shoould be scaled as well, otherwise the shape will be updated after scaling via Update() function.
     */
	void Scale(const wxRealPoint& scale, bool children = sfWITHCHILDREN);
    /*!
     * \brief Scale shape's children
     * \param x Horizontal scale factor
     * \param y Vertical scale factor
     * \sa Scale
     */
	void ScaleChildren(double x, double y);
	/*!
	 * \brief Move the shape to the given absolute position. The function can be overrided if necessary.
	 * \param x X coordinate
	 * \param y Y coordinate
	 */
	virtual void MoveTo(double x, double y);
	/*!
	 * \brief Move the shape to the given absolute position.
	 * \param pos New absolute position
	 */
	void MoveTo(const wxRealPoint& pos);
	/*!
	 * \brief Move the shape by the given offset. The function can be overrided if neccessary.
	 * \param x X offset
	 * \param y Y offset
	 */
	virtual void MoveBy(double x, double y);
	/*!
	 * \brief Move the shape by the given offset.
	 * \param delta Offset
	 */
	void MoveBy(const wxRealPoint& delta);

	/*! \brief Update the shape's position in order to its alignment */
	void DoAlignment();

	/*! \brief Upate shape (align all child shapes an resize it to fit them) */
	virtual void Update();
	/*! \brief Resize the shape to bound all child shapes. The function can be overrided if neccessary. */
	virtual void FitToChildren();

	// public member data accessors
	/*! \brief Function returns TRUE if the shape is selected, otherwise returns FALSE */
	bool IsSelected() const { return m_fSelected; }
	/*!
	 * \brief Set the shape as a selected/deselected one
	 * \param state Selection state (TRUE is selected, FALSE is deselected)
	 */
	void Select(bool state) { m_fSelected = state; ShowHandles(state && (m_nStyle & sfsSHOW_HANDLES)); }

    /*!
	 * \brief Set shape's relative position. Absolute shape's position is then calculated
     * as a sumation of the relative positions of this shape and all parent shapes in the shape's
     * hierarchy.
     * \param pos New relative position
     * \sa MoveTo
     */
	inline void SetRelativePosition(const wxRealPoint& pos) { m_nRelativePosition = pos; }
    /*!
	 * \brief Set shape's relative position. Absolute shape's position is then calculated
     * as a sumation of the relative positions of this shape and all parent shapes in the shape's
     * hierarchy.
     * \param x Horizontal coordinate of new relative position
     * \param y Vertical coordinate of new relative position
     * \sa MoveTo
     */
	inline void SetRelativePosition(double x, double y) { m_nRelativePosition.x = x; m_nRelativePosition.y = y; }
    /*!
	 * \brief Get shape's relative position.
     * \return Current relative position
     * \sa GetAbsolutePosition
     */
	inline wxRealPoint GetRelativePosition() const { return m_nRelativePosition; }
	/*!
	 * \brief Set vertical alignment of this shape inside its parent
	 * \param val Alignment type
	 * \sa VALIGN
	 */
	inline void SetVAlign(VALIGN val) { m_nVAlign = val; }
	/*!
	 * \brief Get vertical alignment of this shape inside its parent
	 * \return Alignment type
	 * \sa VALIGN
	 */
	inline VALIGN GetVAlign() const { return m_nVAlign; }
	/*!
	 * \brief Set horizontal alignment of this shape inside its parent
	 * \param val Horizontal type
	 * \sa HALIGN
	 */
	inline void SetHAlign(HALIGN val) { m_nHAlign = val; }
	/*!
	 * \brief Get horizontal alignment of this shape inside its parent
	 * \return Alignment type
	 * \sa VALIGN
	 */
	inline HALIGN GetHAlign() const { return m_nHAlign; }
	/*!
	 * \brief Set vertical border between this shape and its parent (is vertical
	 * alignment is set).
	 * \param border Vertical border
	 * \sa SetVAlign
	 */
	inline void SetVBorder(double border) { m_nVBorder = border; }
	/*!
	 * \brief Get vertical border between this shape and its parent (is vertical
	 * alignment is set).
	 * \return Vertical border
	 * \sa SetVAlign
	 */
	inline double GetVBorder() const { return m_nVBorder; }
	/*!
	 * \brief Set horizontal border between this shape and its parent (is horizontal
	 * alignment is set).
	 * \param border Horizontal border
	 * \sa SetVAlign
	 */
	inline void SetHBorder(double border) { m_nHBorder = border; }
	/*!
	 * \brief Get horizontal border between this shape and its parent (is horizontal
	 * alignment is set).
	 * \return Vertical border
	 * \sa SetHAlign
	 */
	inline double GetHBorder() const { return m_nHBorder; }
	/**
	 * \brief Set custom dock point used if the shape is child shape of a line shape.
	 * \param dp Custom dock point
	 */
	inline void SetCustomDockPoint(int dp) { m_nCustomDockPoint = dp; }
	/**
	 * \brief Get custom dock point used if the shape is child shape of a line shape.
	 * \return Custom dock point
	 */
	inline int GetCustomDockPoint() { return m_nCustomDockPoint; }

    /*! \brief Get pointer to a parent shape */
	wxSFShapeBase* GetParentShape();
    /*! \brief Get pointer to the topmost parent shape */
	wxSFShapeBase* GetGrandParentShape();
	/**
	 * \brief Determine whether this shape is ancestor of given child shape.
	 * \param child Pointer to child shape.
	 * \return TRUE if this shape is parent of given child shape, otherwise FALSE
	 */
	bool IsAncestor(wxSFShapeBase *child);
	/**
	 * \brief Determine whether this shape is descendant of given parent shape.
	 * \param parent Pointer to parent shape
	 * \return TRUE if this shape is a child of given parent shape, otherwise FALSE
	 */
	bool IsDescendant(wxSFShapeBase *parent);

    /*!
     * \brief Associate user data with the shape.
     *
     * If the data object is properly set then its marked properties will be serialized
     * together with the parent shape.
     * \param data Pointer to user data
     */
    void SetUserData(xsSerializable* data);
     /*!
     * \brief Get associated user data.
     *
     * \return Pointer to user data
     */
    inline xsSerializable* GetUserData() { return m_pUserData; }

	/*!
	 * \brief Get shape's parent diagram manager.
	 * \return Pointer to diagram manager
	 * \sa wxSFDiagramManager
	 */
	inline wxSFDiagramManager* GetShapeManager(){ return (wxSFDiagramManager*)m_pParentManager; }
	/*!
	 * \brief Get shape's parent canvas
	 * \return Pointer to shape canvas if assigned via diagram manager, otherwise NULL
	 * \sa wxSFDiagramManager
	 */
	wxSFShapeCanvas* GetParentCanvas();
	/*!
	 * \brief Get the shape's visibility status
	 * \return TRUE if the shape is visible, otherwise FALSE
	 */
	inline bool IsVisible() const { return m_fVisible; }
	/*!
	 * \brief Show/hide shape
	 * \param show Set the parameter to TRUE if the shape should be visible, otherwise use FALSE
	 */
	inline void Show(bool show) { m_fVisible = show; }
	/*!
	 * \brief Set shape's hover color
	 * \param col Hover color
	 */
	inline void SetHoverColour(const wxColour& col) { m_nHoverColor = col; }
	/*!
	 * \brief Get shape's hover color
	 * \return Current hover color
	 */
	inline wxColour GetHoverColour() const { return m_nHoverColor; }
	/*!
	 * \brief Function returns value of a shape's activation flag.
	 *
	 * Non-active shapes are visible, but don't receive (process) any events.
	 * \return TRUE if the shape is active, othervise FALSE
	 */
	inline bool IsActive() const { return m_fActive; }
	/*!
	 * \brief Shape's activation/deactivation
	 *
	 * Deactivated shapes are visible, but don't receive (process) any events.
	 * \param active TRUE for activation, FALSE for deactivation
	 * \return Description
	 * \sa Show
	 */
	inline void Activate(bool active) { m_fActive = active; }

    /*!
     * \brief Tells whether the given shape type is accepted by this shape (it means
     * whether this shape can be its parent).
     *
     * The function is typically used by the framework for determination whether a dropped
     * shape can be assigned to an underlying shape as its child.
     * \param type Class name of examined shape object
     * \return TRUE if the shape type is accepted, otherwise FALSE.
     */
    bool IsChildAccepted(const wxString& type);
    /*!
     * \brief Function returns TRUE if all currently dragged shapes can be accepted
     * as children of this shape.
     * \sa IsShapeAccepted
     */
    bool AcceptCurrentlyDraggedShapes();
    /*!
     * \brief Add given shape type to an acceptance list. The acceptance list contains class
     * names of the shapes which can be accepted as children of this shape.
     * Note: Keyword 'All' behaves like any class name.
     * \param type Class name of accepted shape object
     * \sa IsChildAccepted
     */
	inline void AcceptChild(const wxString& type) { m_arrAcceptedChildren.Add(type); }
	/*!
	 * \brief Get shape types acceptance list.
	 * \return String array with class names of accepted shape types.
	 * \sa IsChildAccepted
	 */
	inline wxArrayString& GetAcceptedChildren() { return m_arrAcceptedChildren; }
    /*!
     * \brief Tells whether the given connection type is accepted by this shape (it means
     * whether this shape can be connected to another one by a connection of given type).
     *
     * The function is typically used by the framework during interactive connection creation.
     * \param type Class name of examined connection object
     * \return TRUE if the connection type is accepted, otherwise FALSE.
     */
	bool IsConnectionAccepted(const wxString& type);
    /*!
     * \brief Add given connection type to an acceptance list. The acceptance list contains class
     * names of the connection which can be accepted by this shape.
     * Note: Keyword 'All' behaves like any class name.
     * \param type Class name of accepted connection object
     * \sa IsConnectionAccepted
     */
	inline void AcceptConnection(const wxString& type) { m_arrAcceptedConnections.Add(type); }
	/*!
	 * \brief Get connection types acceptance list.
	 * \return String array with class names of accepted connection types.
	 * \sa IsConnectionAccepted
	 */
	inline wxArrayString& GetAcceptedConnections() { return m_arrAcceptedConnections; }
    /*!
     * \brief Tells whether the given shape type is accepted by this shape as its source neighbour(it means
     * whether this shape can be connected from another one of given type).
     *
     * The function is typically used by the framework during interactive connection creation.
     * \param type Class name of examined connection object
     * \return TRUE if the shape type is accepted, otherwise FALSE.
     */
	bool IsSrcNeighbourAccepted(const wxString& type);
    /*!
     * \brief Add given shape type to an source neighbours' acceptance list. The acceptance list contains class
     * names of the shape types which can be accepted by this shape as its source neighbour.
     * Note: Keyword 'All' behaves like any class name.
     * \param type Class name of accepted connection object
     * \sa IsSrcNeighbourAccepted
     */
	inline void AcceptSrcNeighbour(const wxString& type) { m_arrAcceptedSrcNeighbours.Add(type); }
	/*!
	 * \brief Get source neighbour types acceptance list.
	 * \return String array with class names of accepted source neighbours types.
	 * \sa IsSrcNeighbourAccepted
	 */
	inline wxArrayString& GetAcceptedSrcNeighbours() { return m_arrAcceptedSrcNeighbours; }
    /*!
     * \brief Tells whether the given shape type is accepted by this shape as its target neighbour(it means
     * whether this shape can be connected to another one of given type).
     *
     * The function is typically used by the framework during interactive connection creation.
     * \param type Class name of examined connection object
     * \return TRUE if the shape type is accepted, otherwise FALSE.
     */
	bool IsTrgNeighbourAccepted(const wxString& type);
    /*!
     * \brief Add given shape type to an target neighbours' acceptance list. The acceptance list contains class
     * names of the shape types which can be accepted by this shape as its target neighbour.
     * Note: Keyword 'All' behaves like any class name.
     * \param type Class name of accepted connection object
     * \sa IsTrgNeighbourAccepted
     */
	inline void AcceptTrgNeighbour(const wxString& type) { m_arrAcceptedTrgNeighbours.Add(type); }
	/*!
	 * \brief Get target neighbour types acceptance list.
	 * \return String array with class names of accepted target neighbours types.
	 * \sa IsTrgNeighbourAccepted
	 */
	inline wxArrayString& GetAcceptedTrgNeighbours() { return m_arrAcceptedTrgNeighbours; }
	/*!
	 * \brief Clear shape object acceptance list
	 * \sa AcceptChild
	 */
	inline void ClearAcceptedChilds() { m_arrAcceptedChildren.Clear(); }
	/*!
	 * \brief Clear connection object acceptance list
	 * \sa AcceptConnection
	 */
	inline void ClearAcceptedConnections() { m_arrAcceptedConnections.Clear(); }
	/*!
	 * \brief Clear source neighbour objects acceptance list
	 * \sa AcceptSrcNeighbour
	 */
	inline void ClearAcceptedSrcNeighbours() { m_arrAcceptedSrcNeighbours.Clear(); }
	/*!
	 * \brief Clear target neighbour objects acceptance list
	 * \sa AcceptTrgNeighbour
	 */
	inline void ClearAcceptedTrgNeighbours() { m_arrAcceptedTrgNeighbours.Clear(); }

    /*!
     * \brief Get list of currently assigned shape handles.
     * \return Reference to the handle list
     * \sa CHandleList
     */
	inline HandleList& GetHandles() { return m_lstHandles; }
	/*!
	 * \brief Get shape handle.
	 * \param type Handle type
	 * \param id Handle ID (usefull only for line control points)
	 * \return Pointer to the shape handle object
	 * \sa wxSFShapeHandle
	 */
	wxSFShapeHandle* GetHandle(wxSFShapeHandle::HANDLETYPE type, long id = -1);
	/*!
	 * \brief Add new handle to the shape.
	 *
	 * The function creates new instance of shape handle (if it doesn't exist yet)
	 * and inserts it into handle list.
	 * \param type Handle type
	 * \param id Handle ID (usefull only for line control points)
	 * \sa wxSFShapeHandle
	 */
	void AddHandle(wxSFShapeHandle::HANDLETYPE type, long id = -1);
	/*!
	 * \brief Remove given shape handle (if exists).
	 * \param type Handle type
	 * \param id Handle ID (usefull only for line control points)
	 * \sa wxSFShapeHandle
	 */
	void RemoveHandle(wxSFShapeHandle::HANDLETYPE type, long id = -1);
	
	/*!
	 * \brief Get reference to connection points list.
	 * \return Constant reference to connection points list
	 */
	inline ConnectionPointList& GetConnectionPoints() { return m_lstConnectionPts; }
	/*!
	 * \brief Get connection point of given type assigned to the shape.
	 * \param type Connection point type
	 * \param id Optional connection point ID
	 * \return Pointer to connection point if exists, otherwise NULL
	 * \sa wxSFConnectionPoint::CPTYPE
	 */
	wxSFConnectionPoint* GetConnectionPoint(wxSFConnectionPoint::CPTYPE type, long id = -1);
	/*!
	 * \brief Get connection point closest to the diven position.
	 * \param pos Position
	 * \return Pointer to closest connection point if exists, otherwise NULL
	 */
	wxSFConnectionPoint* GetNearestConnectionPoint(const wxRealPoint& pos);
	/*!
	 * \brief Assign connection point of given type to the shape.
	 * \param type Connection point type
	 * \param persistent TRUE if the connection point should be serialized
	 * \sa wxSFConnectionPoint::CPTYPE
	 */
	void AddConnectionPoint(wxSFConnectionPoint::CPTYPE type, bool persistent = true);
	/*!
	 * \brief Assigned given connection point to the shape.
	 * \param cp Pointer to connection point (shape will take the ownership)
	 * \param persistent TRUE if the connection point should be serialized
	 */
	void AddConnectionPoint(wxSFConnectionPoint *cp, bool persistent = true);
	/*!
	 * \brief Assign custom connection point to the shape.
	 * \param relpos Relative position in percentages
	 * \param id Optional connection point ID
	 * \param persistent TRUE if the connection point should be serialized
	 */
	void AddConnectionPoint(const wxRealPoint& relpos, long id = -1, bool persistent = true);
	/*!
	 * \brief Remove connection point of given type from the shape (if pressent).
	 * \param type Connection point type
	 * \sa wxSFConnectionPoint::CPTYPE
	 */
	void RemoveConnectionPoint(wxSFConnectionPoint::CPTYPE type);

	// public event handlers
	/*!
	 * \brief Event handler called when the shape is clicked by
	 * the left mouse button. The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_LEFT_DOWN event.
	 * \param pos Current mouse position
	 * \sa wxSFShapeCanvas
	 */
	virtual void OnLeftClick(const wxPoint& pos);
	/*!
	 * \brief Event handler called when the shape is clicked by
	 * the right mouse button. The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_RIGHT_DOWN event.
	 * \param pos Current mouse position
	 * \sa wxSFShapeCanvas
	 */
	virtual void OnRightClick(const wxPoint& pos);
	/*!
	 * \brief Event handler called when the shape is double-clicked by
	 * the left mouse button. The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_LEFT_DCLICK event.
	 * \param pos Current mouse position
	 * \sa wxSFShapeCanvas
	 */
	virtual void OnLeftDoubleClick(const wxPoint& pos);
	/*!
	 * \brief Event handler called when the shape is double-clicked by
	 * the right mouse button. The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_RIGHT_DCLICK event.
	 * \param pos Current mouse position
	 * \sa wxSFShapeCanvas
	 */
	virtual void OnRightDoubleClick(const wxPoint& pos);

	/*!
	 * \brief Event handler called at the begining of the shape dragging process.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_DRAG_BEGIN event.
	 * \sa wxSFShapeCanvas
	 */
	virtual void OnBeginDrag(const wxPoint& pos);
	/*!
	 * \brief Event handler called during the shape dragging process.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_DRAG event.
	 * \param pos Current mouse position
	 * \sa wxSFShapeCanvas
	 */
	virtual void OnDragging(const wxPoint& pos);
	/*!
	 * \brief Event handler called at the end of the shape dragging process.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_DRAG_END event.
	 * \param pos Current mouse position
	 * \sa wxSFShapeCanvas
	 */
	virtual void OnEndDrag(const wxPoint& pos);

	/*!
	 * \brief Event handler called when the user started to drag the shape handle.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_HANDLE_BEGIN event.
	 * \param handle Reference to dragged handle
	 */
	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	/*!
	 * \brief Event handler called during dragging of the shape handle.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_HANDLE event.
	 * \param handle Reference to dragged handle
	 */
	virtual void OnHandle(wxSFShapeHandle& handle);
	/*!
	 * \brief Event handler called when the user finished dragging of the shape handle.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_HANDLE_END event.
	 * \param handle Reference to dragged handle
	 */
	virtual void OnEndHandle(wxSFShapeHandle& handle);
	/*!
	 * \brief Event handler called when a mouse pointer enters the shape area.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_MOUSE_ENTER event.
	 * \param pos Current mouse position
	 */
	virtual void OnMouseEnter(const wxPoint& pos);
	/*!
	 * \brief Event handler called when a mouse pointer moves above the shape area.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_MOUSE_OVER event.
	 * \param pos Current mouse position
	 */
	virtual void OnMouseOver(const wxPoint& pos);
	/*!
	 * \brief Event handler called when a mouse pointer leaves the shape area.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_MOUSE_LEAVE event.
	 * \param pos Current mouse position
	 */
	virtual void OnMouseLeave(const wxPoint& pos);
	/*!
	 * \brief Event handler called when any key is pressed (in the shape canvas).
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_KEYDOWN event.
	 * \param key The key code
	 * \return The function must return TRUE if the default event routine should be called
	 * as well, otherwise FALSE
	 * \sa wxSFShapeBase::_OnKey
	 */
	virtual bool OnKey(int key);
	/*!
	 * \brief Event handler called when any shape is dropped above this shape (and the dropped
	 * shape is accepted as a child of this shape). The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation emmits wxEVT_SF_SHAPE_CHILD_DROP event.
	 * \param pos Relative position of dropped shape
	 * \param child Pointer to dropped shape
	 */
	virtual void OnChildDropped(const wxRealPoint& pos, wxSFShapeBase *child);

protected:

	// protected data members
	/*! \brief Selection flag */
	bool m_fSelected;
	/*! \brief Visibility flag */
	bool m_fVisible;
	/*! \brief Activation flag */
	bool m_fActive;
    /*! \brief Shape's style mask */
	long m_nStyle;

	wxColour m_nHoverColor;
	wxRealPoint m_nRelativePosition;
	/*! \brief String list with class names of accepted child shapes */
	wxArrayString m_arrAcceptedChildren;
	/*! \brief String list with class names of accepted connections */
	wxArrayString m_arrAcceptedConnections;
	/*! \brief String list with class names of accepted source neighbour shapes */
	wxArrayString m_arrAcceptedSrcNeighbours;
	/*! \brief String list with class names of accepted target neighbour shapes */
	wxArrayString m_arrAcceptedTrgNeighbours;
    /*! \brief Value of vertical border used by AlignV function */
	double m_nVBorder;
    /*! \brief Value of horizontal border used by AlignH function */
    double m_nHBorder;
    /*! \brief Vertical alignment of child shapes */
    VALIGN m_nVAlign;
    /*! \brief Horizontal alignment of child shapes */
    HALIGN m_nHAlign;
	/*! \brief Custom line dock point */
	int m_nCustomDockPoint;

	/*! \brief Handle list */
	HandleList m_lstHandles;
	ConnectionPointList m_lstConnectionPts;

	/*! \brief Container for serializable user data associated with the shape */
	xsSerializable *m_pUserData;

	// protected functions
	/*!
	 * \brief Draw the shape in the normal way. The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawNormal(wxDC& dc);
	/*!
	 * \brief Draw the shape in the selected way. The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawSelected(wxDC& dc);
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
     *
     * Note, that the shape serialization is used not only for saving canvas's content to a file
     * but also during Undo/Redo and the clipboard operations so it is very important to implement this virtual
     * function otherwise all mentioned operations wont be available for this shape.
     *
     * \param node Pointer to XML node where the shape's property nodes will be appended to
     * \sa xsSerializable::Serialize
     */
	virtual wxXmlNode* Serialize(wxXmlNode* node);
    /*!
     * \brief Deserialize shape's properties from the given XML node. The
     * routine is automatically called by the framework and should take care about deserialization
     * of all specific (non-standard) shape's properties.
     *
     * Note, that the shape serialization is used not only for saving canvas's content to a file
     * but also during Undo/Redo and the clipboard operations so it is very important to implement this virtual
     * function otherwise all mentioned operations wont be available for this shape.
     *
     * \param node Pointer to a source XML node containig the shape's property nodes
     * \sa xsSerializable::Deserialize
     */
    virtual void Deserialize(wxXmlNode* node);

    /*!
     * \brief Repaint the shape
     * \param rct Canvas portion that should be updated
     */
	void Refresh(const wxRect& rct);
	
	/**
	 * \brief Get absolute position of the shape parent.
	 * \return Absolute position of the shape parent if exists, otherwise 0,0
	 */
	wxRealPoint GetParentAbsolutePosition();

private:

	// private data members
	bool m_fMouseOver;
	bool m_fFirstMove;
	bool m_fHighlighParent;

	wxRealPoint m_nMouseOffset;

    // private functions

	/*! \brief Initialize serializable properties. */
	void MarkSerializableDataMembers();
	
	/*!
	 * \brief Auxiliary function called by GetNeighbours function.
	 * \param neighbours List of neighbour shapes
	 * \param shapeInfo Line object type
	 * \param condir Connection direction
	 * \param direct Set this flag to TRUE if only closest shapes should be found,
	 * otherwise also shapes connected by forked lines will be found (also
	 * constants sfDIRECT and sfINDIRECT can be used)
	 * \sa GetNeighbours
	 */
	void _GetNeighbours(ShapeList& neighbours, wxClassInfo *shapeInfo, CONNECTMODE condir, bool direct);
	/*!
	 * \brief Auxiliary function called by GetCompleteBoundingBox function.
	 * \param rct Returned bounding rectangle
	 * \param mask Bit mask of object types which should be included into calculation
	 * \sa BBMODE
	 */
	void _GetCompleteBoundingBox(wxRect& rct, int mask = bbALL);

	// private event handlers

    /*!
     * \brief Original protected event handler called when the mouse pointer is moving around the shape canvas.
     *
	 * The function is called by the framework (by the shape canvas). After processing the event
	 * relevant overridable event handlers are called.
     * \param pos Current mouse position
     * \sa wxShapeBase::OnMouseEnter, wxShapeBase::OnMouseOver, wxShapeBase::OnMouseLeave
     */
	void _OnMouseMove(const wxPoint& pos);
    /*!
     * \brief Original protected event handler called at the begininig of dragging process.
     *
	 * The function is called by the framework (by the shape canvas). After processing the event
	 * an overridable event handler is called.
     * \param pos Current mouse position
     * \sa wxShapeBase::OnBeginDrag
     */
	void _OnBeginDrag(const wxPoint& pos);
    /*!
     * \brief Original protected event handler called during a dragging process.
     *
	 * The function is called by the framework (by the shape canvas). After processing the event
	 * an overridable event handler is called.
     * \param pos Current mouse position
     * \sa wxShapeBase::OnDragging
     */
	void _OnDragging(const wxPoint& pos);
    /*!
     * \brief Original protected event handler called at the end of dragging process.
     *
	 * The function is called by the framework (by the shape canvas). After processing the event
	 * an overridable event handler is called.
     * \param pos Current mouse position
     * \sa wxShapeBase::OnEndDrag
     */
	void _OnEndDrag(const wxPoint& pos);

	/*!
	 * \brief Original protected event handler called when any key is pressed (in the shape canvas).
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation performs operations necessary for proper shape's
	 * moving and repainting.
	 * \param key The key code
	 * \sa wxSFShapeBase::OnKey
	 */
	void _OnKey(int key);

	/*!
	 * \brief Original protected event handler called during dragging of the shape handle.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation manages the child shapes' alignment (if set).
	 * \param handle Reference to dragged handle
	 */
	void _OnHandle(wxSFShapeHandle& handle);
};

extern ShapeList m_lstProcessed;

#endif //_WXSFSHAPEBASE_H
