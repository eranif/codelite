/***************************************************************
 * Name:      DiagramManager.h
 * Purpose:   Defines shape manager class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-25
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFDIAGRAMMANAGER_H
#define _WXSFDIAGRAMMANAGER_H

#include <wx/wxsf/ShapeBase.h>
#include <wx/wxsf/CommonFcn.h>

#define serINCLUDE_PARENTS true
#define serWITHOUT_PARENTS false
#define sfINITIALIZE true
#define sfDONT_INITIALIZE false

class WXDLLIMPEXP_SF wxSFShapeCanvas;
class WXDLLIMPEXP_SF wxSFLineShape;

/*! \brief Auxiliary class encapsulation two variables suitable for shape IDs. It is
 * used for storing infomation about various relevant shape IDs */
class IDPair : public wxObject
{
public:
    /*! \brief Constructor */
	IDPair(long oldId, long newId){m_nOldID = oldId; m_nNewID = newId;}
	long m_nNewID;
	long m_nOldID;
};

WX_DECLARE_LIST(IDPair, IDList);

/*! \brief Class encapsulating framework's data layer. Its functions are responsible
 * for managing shape objects and their serialialization/deserialization. Presentation
 * layer is provided by wxSFShapeCanvas class which tightly cooperates with the shape
 * manager.
 *
 * An application using wxSF must have at least one shape manager object (one for
 * each diagram) and every shape manager can be assigned as a source to one shape
 * canvas (and vice versa).
 */
class WXDLLIMPEXP_SF wxSFDiagramManager : public wxXmlSerializer
{
public:
    XS_DECLARE_CLONABLE_CLASS(wxSFDiagramManager);

    /*! \brief Search mode flags for GetShapeAtPosition function */
	enum SEARCHMODE
	{
	    /*! \brief Search for selected shapes only */
		searchSELECTED,
		/*! \brief Search for unselected shapes only */
		searchUNSELECTED,
		/*! \brief Search for both selected and unselected shapes */
		searchBOTH
	};

    /*! \brief Constructor */
    wxSFDiagramManager();
    /*! \brief Copy constructor */
	wxSFDiagramManager(const wxSFDiagramManager &obj);
    /*! \brief Destructor */
    virtual ~wxSFDiagramManager();

    // public functions
	/*! \brief Get wxShapeFramework version number */
	const wxString& GetVersion() const { return m_sSFVersion; }

    /*!
     * \brief Create new direct connection between two shapes.
     *
     * This function creates new simple connection line (without arrows) between gived
     * shapes.
     * \param srcId ID of a source shape
     * \param trgId ID of target shape
     * \param saveState Set the parameter TRUE if you wish to save canvas state after the operation
	 * \param err Pointer to variable where operation result will be stored. Can be NULL.
     * \return Pointer to new connection object. The object is added to the shape canvas automaticaly.
     * \sa StartInteractiveConnection
     */
    wxSFShapeBase* CreateConnection(long srcId, long trgId, bool saveState = true, wxSF::ERRCODE *err = NULL);
    /*!
     * \brief Create new direct connection of given type between two shapes.
     *
     * This function creates new simple connection line (without arrows) between gived
     * shapes.
     * \param srcId ID of a source shape
     * \param trgId ID of target shape
	 * \param lineInfo Connection type (any class inherited from wxSFLineShape)
     * \param saveState Set the parameter TRUE if you wish to save canvas state after the operation
	 * \param err Pointer to variable where operation result will be stored. Can be NULL.
     * \return Pointer to new connection object. The object is added to the shape canvas automaticaly.
     * \sa StartInteractiveConnection
     */
    wxSFShapeBase* CreateConnection(long srcId, long trgId, wxClassInfo *lineInfo, bool saveState = true, wxSF::ERRCODE *err = NULL);
	/*!
     * \brief Create new direct connection of given type between two shapes.
     *
     * This function creates new simple connection line (without arrows) between gived
     * shapes.
     * \param srcId ID of a source shape
     * \param trgId ID of target shape
	 * \param line Pointer to line shape
     * \param saveState Set the parameter TRUE if you wish to save canvas state after the operation
	 * \param err Pointer to variable where operation result will be stored. Can be NULL.
     * \return Pointer to new connection object. The object is added to the shape canvas automaticaly.
     * \sa StartInteractiveConnection
     */
    wxSFShapeBase* CreateConnection(long srcId, long trgId, wxSFLineShape *line, bool saveState = true, wxSF::ERRCODE *err = NULL);
    /*!
     * \brief Create new shape and add it to the shape canvas.
     * \param shapeInfo Shape type
     * \param saveState Set the parameter TRUE if you wish to save canvas state after the operation
	 * \param err Pointer to variable where operation result will be stored. Can be NULL.
     * \return Pointer to new shape object. The object is added to the shape canvas automaticaly.
     */
	wxSFShapeBase* AddShape(wxClassInfo* shapeInfo, bool saveState = true, wxSF::ERRCODE *err = NULL);
	/*!
	 * \brief Create new shape and add it to the shape canvas.
	 * \param shapeInfo Shape type
	 * \param pos Shape position
	 * \param saveState Set the parameter TRUE if you wish to save canvas state after the operation
	 * \param err Pointer to variable where operation result will be stored. Can be NULL.
	 * \return Description
	 * \sa Seealso
	 */
	wxSFShapeBase* AddShape(wxClassInfo* shapeInfo, const wxPoint& pos, bool saveState = true, wxSF::ERRCODE *err = NULL);
    /*!
     * \brief Add an existing shape to the canvas.
     * \param shape Pointer to the shape
     * \param parent Pointer to the parent shape
     * \param pos Position
     * \param initialize TRUE if the shape should be reinitilialized, otherwise FALSE
     * \param saveState TRUE if the canvas state should be saved
	 * \param err Pointer to variable where operation result will be stored. Can be NULL.
     * \return Pointer to the shape
     */
	wxSFShapeBase* AddShape(wxSFShapeBase* shape, xsSerializable* parent,  const wxPoint& pos, bool initialize, bool saveState = true, wxSF::ERRCODE *err = NULL);

	/*!
	 * \brief Remove given shape from the shape canvas. The shape object will be deleted as well.
	 * \param shape Pointer to shape object should be deleted
	 * \param refresh Set the paramater to TRUE if you wish to repaint the canvas
	 */
	void RemoveShape(wxSFShapeBase* shape, bool refresh = true);
	/*!
	 * \brief Remove shapes from the shape canvas
	 * \param selection List of shapes which should be removed from the canvas
	 */
	void RemoveShapes(const ShapeList& selection);
	/*! \brief Remove all shapes from canvas */
	void Clear();
	
	/*! \brief Move all shapes so none of it will be located in negative position */
	void MoveShapesFromNegatives();
	
	/*! \brief Update all shapes in the diagram manager */
	void UpdateAll();

    /*!
     * \brief Serialize complete shape canvas to given file
     * \param file Output file
	 * \return TRUE on success, otherwise FALSE
     */
	virtual bool SerializeToXml(const wxString& file);
    /*!
     * \brief Deserialize complete shape canvas from given file
     * \param file Input file
	 * \return TRUE on success, otherwise FALSE
     */
	virtual bool DeserializeFromXml(const wxString& file);
    /*!
     * \brief Serialize complete shape canvas to given output stream
     * \param outstream Output stream
	 * \return TRUE on success, otherwise FALSE
     */
	virtual bool SerializeToXml(wxOutputStream& outstream);
    /*!
     * \brief Deserialize complete shape canvas from given input stream
     * \param instream Input stream
	 * \return TRUE on success, otherwise FALSE
     */
	virtual bool DeserializeFromXml(wxInputStream& instream);
	/*!
	 * \brief Deserialize shapes from XML and assign them to given parent.
	 *
     * The parent can be NULL (in that case topmost shapes will have no parent assigned).
	 * \param parent Parent shapes
	 * \param node Source XML node
	 */
	virtual void DeserializeObjects(xsSerializable* parent, wxXmlNode* node);

    /*!
     * \brief Add given shape type to an acceptance list. The acceptance list contains class
     * names of the shapes which can be inserted into this instance of shapes canvas.
     * Note: Keyword 'All' behaves like any class name.
     * \param type Class name of accepted shape object
     * \sa IsShapeAccepted
     */
	void AcceptShape(const wxString& type);
    /*!
     * \brief Tells whether the given shape type is accepted by this canvas instance (it means
     * whether this shape can be inserted into it).
     *
     * The function is typically used by the framework for determination whether class type supplied
     * by AddShape() function can be inserted into shape canvas.
     * \param type Class name of examined shape object
     * \return TRUE if the shape type is accepted, otherwise FALSE.
     */
	bool IsShapeAccepted(const wxString& type);
	/*!
	 * \brief Clear shape object acceptance list
	 * \sa AcceptShape
	 */
	inline void ClearAcceptedShapes() { m_arrAcceptedShapes.Clear(); }
	/*!
	 * \brief Get reference to shape acceptance list
	 */
	inline wxArrayString& GetAcceptedShapes() { return m_arrAcceptedShapes; }

    /*!
     * \brief Find shape with given ID.
     * \param id Shape's ID
     * \return Pointer to shape if exists, otherwise NULL
     */
	wxSFShapeBase* FindShape(long id);
	/*!
	 * \brief Get list of connections assigned to given parent shape.
	 * \param parent Pointer to parent shape
	 * \param shapeInfo Line object type
	 * \param mode Search mode
	 * \param lines Reference to shape list where pointers to all found connections will be stored
	 * \sa wxSFShapeBase::CONNECTMODE
	 */
	void GetAssignedConnections(wxSFShapeBase* parent, wxClassInfo* shapeInfo, wxSFShapeBase::CONNECTMODE mode, ShapeList& lines);
	/*!
	 * \brief Get list of shapes of given type.
	 * \param shapeInfo Shape object type
	 * \param shapes Reference to shape list where pointers to all found shapes will be stored
	 * \param mode Search algorithm
	 * \sa xsSerializable::SEARCHMODE 
	 */
	void GetShapes(wxClassInfo* shapeInfo, ShapeList& shapes, xsSerializable::SEARCHMODE mode = xsSerializable::searchBFS);
	/*!
	 * \brief Get shape at given logical position
	 * \param pos Logical position
	 * \param zorder Z-order of searched shape (usefull if several shapes are located at the given position)
	 * \param mode Search mode
	 * \return Pointer to shape if found, otherwise NULL
	 * \sa SEARCHMODE, wxSFShapeCanvas::DP2LP,, wxSFShapeCanvas::GetShapeUnderCursor
	 */
	wxSFShapeBase* GetShapeAtPosition(const wxPoint& pos, int zorder = 1, SEARCHMODE mode = searchBOTH);
	/*!
	 * \brief Get list of all shapes located at given position
	 * \param pos Logical position
	 * \param shapes Reference to shape list where pointers to all found shapes will be stored
	 * \sa wxSFShapeCanvas::DP2LP
	 */
	void GetShapesAtPosition(const wxPoint& pos, ShapeList& shapes);
	/*!
	 * \brief Get list of shapes located inside given rectangle
	 * \param rct Examined rectangle
	 * \param shapes Reference to shape list where pointers to all found shapes will be stored
	 */
	void GetShapesInside(const wxRect& rct, ShapeList& shapes);

	/*!
	 * \brief Determines whether the diagram manager contains some shapes.
	 * \return TRUE if there are no shapes in the manager, otherwise FALSE
	 */
	inline bool IsEmpty() const { return ! GetRootItem()->HasChildren(); }
    /*!
     * \brief Function finds out whether given shape has some children.
     * \param parent Pointer to potential parent shape
     * \return TRUE if the parent shape has children, otherwise FALSE
     */
	bool HasChildren(wxSFShapeBase* parent);
	/*!
	 * \brief Get neighbour shapes connected to given parent shape.
	 * \param parent Pointer to parent shape (can be NULL for all topmost shapes)
	 * \param neighbours List of neighbour shapes
	 * \param shapeInfo Line object type
	 * \param condir Connection direction
	 * \param direct Set this flag to TRUE if only closest shapes should be found,
	 * otherwise also shapes connected by forked lines will be found (also
	 * constants sfDIRECT and sfINDIRECT can be used)
	 * \sa wxSFShapeBase::CONNECTMODE
	 */
	void GetNeighbours(wxSFShapeBase* parent, ShapeList& neighbours, wxClassInfo* shapeInfo, wxSFShapeBase::CONNECTMODE condir, bool direct = true);


    // public member data accessors
    /*!
     * \brief Set associated shape canvas
     * \param canvas Pointer to shape canvas
     */
    inline void SetShapeCanvas(wxSFShapeCanvas* canvas) { m_pShapeCanvas = canvas; }
    /*!
     * \brief Get associated shape canvas
     * \return Pointer to shape canvas
     */
    inline wxSFShapeCanvas* GetShapeCanvas() { return m_pShapeCanvas; }

protected:

    // protected data members
    /*! \brief List of accepted shape types */
    wxArrayString m_arrAcceptedShapes;

    wxSFShapeCanvas* m_pShapeCanvas;

private:

    /*! \brief Auxiliary list */
    IDList m_lstIDPairs;
    /*! \brief Auxiliary list */
	ShapeList m_lstLinesForUpdate;
	/*! \brief Auxiliary list */
	ShapeList m_lstGridsForUpdate;

	/*! \brief wxSF version number */
	wxString m_sSFVersion;

	/*! \brief Update connection shapes after importing/dropping of new shapes */
	void UpdateConnections();
	/*! \brief Update grid shapes after importing/dropping of new shapes */
	void UpdateGrids();

	/*!
	 * \brief Deserialize shapes from XML and assign them to given parent.
	 * \param parent Parent shapes
	 * \param node Source XML node
	 */
	void _DeserializeObjects(xsSerializable* parent, wxXmlNode* node);

};

#endif //_WXSFDIAGRAMMANAGER_H
