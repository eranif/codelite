/***************************************************************
 * Name:      AutoLayout.h
 * Purpose:   Declares class for autolayouting algoriths' provider
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2010-12-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFAUTOLAYOUT_H_
#define _WXSFAUTOLAYOUT_H_

#include <wx/wxsf/DiagramManager.h>

#include <wx/hashmap.h>

/*!
 * \brief Base class for all layouting algorithms. The class containts one abstract function modifying
 * shapes' layout and several helper functions.
 */
class WXDLLIMPEXP_SF wxSFLayoutAlgorithm : public wxObject
{
public:
	/*!
	 * \brief Destructor.
	 */
	virtual ~wxSFLayoutAlgorithm() {;}
	/*!
	 * \brief Function performing the layout change. All derived classes must implement it.
	 * \param shapes List of shapes which should be layouted
	 */
	virtual void DoLayout(ShapeList &shapes) = 0;
	
protected:
	/*!
	 * \brief Calculate bounding box surroundig given shapes.
	 * \param shapes List of shapes
	 * \return Bounding box
	 */
	wxRect GetBoundingBox(const ShapeList &shapes);
	/*!
	 * \brief Get overall extent of all given shapes calculated as a sum of their width and height.
	 * \param shapes List of shapes
	 * \return Overall shapes extent
	 */
	wxSize GetShapesExtent(const ShapeList &shapes);
	/*!
	 * \brief Get center point of given shapes.
	 * \param shapes List of shapes
	 * \return Center point
	 */
	wxRealPoint GetShapesCenter(const ShapeList &shapes);
	/*!
	 * \brief Get top-left point of bounding box surrounding given shapes.
	 * \param shapes List of shapes
	 * \return Top-left point of bounding box surrounding given shapes
	 */
	wxRealPoint GetTopLeft(const ShapeList &shapes);
};

WX_DECLARE_HASH_MAP( wxString, wxSFLayoutAlgorithm*, wxStringHash, wxStringEqual, LayoutAlgoritmMap );
	
/*!
 * \brief Class implements automatic diagram layout. The class allows to automatically layout shapes
 * included in diagram manager/shape canvas/list of shapes by using several pre-defined layouting 
 * algorithms. The class should be used as it is.
 */
class WXDLLIMPEXP_SF wxSFAutoLayout
{
public:
	/*!
	 * \brief Constructor.
	 */
	wxSFAutoLayout();
	/*!
	 * \brief Destructor.
	 */
	~wxSFAutoLayout();

	/*!
	 * \brief Layout shapes included in given list.
	 * \param shapes List of shapes
	 * \param algname Algorithm name
	 */
	void Layout(ShapeList &shapes, const wxString& algname);
	/*!
	 * \brief Layout shapes included in given diagram manager.
	 * \param manager Reference to diagram manager
	 * \param algname Algorithm name
	 */
	void Layout(wxSFDiagramManager &manager, const wxString& algname);
	/*!
	 * \brief Layout shapes included in given shape canvas.
	 * \param canvas Pointer to shape canvas
	 * \param algname
	 */
	void Layout(wxSFShapeCanvas *canvas, const wxString& algname);
	
	/*!
	 * \brief Register custom layouting algorithm (class derived from wxSFLayoutAlgorithm).
	 * \param algname Algorithm name
	 * \param alg Pointer to class encapsulating the layouting algorithm
	 * \return TRUE if the algorithm was successfuly registered, otherwise FALSE
	 */
	static bool RegisterLayoutAlgorithm(const wxString& algname, wxSFLayoutAlgorithm *alg);
	/*!
	 * \brief Clean up registered layouting algorithms. Must be called at application deinitialization
	 * if wxSFAutoLayout were used in the program.
	 */
	static void CleanUp();
	
	/*!
	 * \brief Get string list containing names of registered layouting algorithms.
	 * \return List of registered algorithm names.
	 */
	wxArrayString GetRegisteredAlgorithms();
	/*!
	 * \brief Get pointer to registered layouting algorithm.
	 * \param algname Algorithm name
	 * \return Pointer to class encapsulating selected layouting algorithm if registered, otherwise NULL
	 */
	wxSFLayoutAlgorithm *GetAlgorithm(const wxString &algname) { return m_mapAlgorithms[algname]; }
	
protected:
	/*!
	 * \brief Register all pre-defined layouting algorithms. The function is called from wxSFAutoLayout class
	 * constructor.
	 */
	void InitializeAllAlgorithms();
	/*!
	 * \brief Update given shape canvas.
	 * \param canvas
	 */
	void UpdateCanvas(wxSFShapeCanvas *canvas);
	
	static LayoutAlgoritmMap m_mapAlgorithms;
	
};

// pre-defined algorithms //////////////////////////////////////////////////////

/*!
 * \brief Class encapsulating algorithm which layouts all top-most shapes into circle registered under "Circle" name.
 * The algorithm doesn't optimize connection lines crossing.
 */
class wxSFLayoutCircle : public wxSFLayoutAlgorithm
{
public:
	/*!
	 * \brief Constructor.
	 */
	wxSFLayoutCircle() : m_DistanceRatio(1) {;}
	/*!
	 * \brief Destructor.
	 */
	virtual ~wxSFLayoutCircle() {;}
	
	/*!
	 * \brief Function performing the layout change.
	 * \param shapes List of shapes which should be layouted
	 */
	virtual void DoLayout(ShapeList &shapes);
	
	/*!
	 * \brief Set ratio in which calculated distance between shapes will be reduced. Values less than
	 * 1 means that the distance will be smaller, values bigger than 1 means that the distance will be
	 * bigger.
	 * \param DistanceRatio Distance ratio
	 */
	void SetDistanceRatio(double DistanceRatio) {this->m_DistanceRatio = DistanceRatio;}
	/*!
	 * \brief Get distance ratio.
	 * \return Current distance ratio
	 */
	double GetDistanceRatio() const {return m_DistanceRatio;}
	
protected:
	double m_DistanceRatio;
};

/*!
 * \brief Class encapsulating algorithm which layouts all top-most shapes into vertical tree registered under "Vertical Tree" name.
 */
class wxSFLayoutVerticalTree : public wxSFLayoutAlgorithm
{
public:
	/*!
	 * \brief Constructor.
	 */
	wxSFLayoutVerticalTree() : m_HSpace(30), m_VSpace(30) {;}
	/*!
	 * \brief Destructor.
	 */
	virtual ~wxSFLayoutVerticalTree() {;}
	
	/*!
	 * \brief Function performing the layout change.
	 * \param shapes List of shapes which should be layouted
	 */
	virtual void DoLayout(ShapeList &shapes);

	/*!
	 * \brief Set horizontal space between shapes.
	 * \param HSpace Horizontal space in pixels.
	 */
	void SetHSpace(double HSpace) {this->m_HSpace = HSpace;}
	/*!
	 * \brief Set vertical space between shapes.
	 * \param VSpace Vertical space in pixels.
	 */
	void SetVSpace(double VSpace) {this->m_VSpace = VSpace;}
	/*!
	 * \brief Get horizontal space.
	 * \return Horizontal space in pixels.
	 */
	double GetHSpace() const {return m_HSpace;}
	/*!
	 * \brief Get vertical space.
	 * \return Vertical space in pixels.
	 */
	double GetVSpace() const {return m_VSpace;}
	
protected:
	double m_nMinX;
	int m_nCurrMaxWidth;
	
	double m_HSpace;
	double m_VSpace;

	/*!
	 * \brief Process single shape.
	 * \param node Pointer to processed shape.
	 * \param y Vertical position of the shape.
	 */
	void ProcessNode(wxSFShapeBase *node, double y);
};

/*!
 * \brief Class encapsulating algorithm which layouts all top-most shapes into horizontal tree registered under "Horizontal Tree" name.
 */
class wxSFLayoutHorizontalTree : public wxSFLayoutAlgorithm
{
public:
	/*!
	 * \brief Constructor.
	 */
	wxSFLayoutHorizontalTree() : m_HSpace(30), m_VSpace(30) {;}
	/*!
	 * \brief Destructor.
	 */
	virtual ~wxSFLayoutHorizontalTree() {;}
	
	/*!
	 * \brief Function performing the layout change.
	 * \param shapes List of shapes which should be layouted
	 */
	virtual void DoLayout(ShapeList &shapes);
	
	/*!
	 * \brief Set horizontal space between shapes.
	 * \param HSpace Horizontal space in pixels.
	 */
	void SetHSpace(double HSpace) {this->m_HSpace = HSpace;}
	/*!
	 * \brief Set vertical space between shapes.
	 * \param VSpace Vertical space in pixels.
	 */
	void SetVSpace(double VSpace) {this->m_VSpace = VSpace;}
	/*!
	 * \brief Get horizontal space.
	 * \return Horizontal space in pixels.
	 */
	double GetHSpace() const {return m_HSpace;}
	/*!
	 * \brief Get vertical space.
	 * \return Vertical space in pixels.
	 */
	double GetVSpace() const {return m_VSpace;}
	
protected:
	double m_nMinY;
	int m_nCurrMaxHeight;
	
	double m_HSpace;
	double m_VSpace;

	void ProcessNode(wxSFShapeBase *node, double x);
};

/*!
 * \brief Class encapsulating algorithm which layouts all top-most shapes into mesh registered under "Mesh" name.
 * The algorithm doesn't optimize connection lines crossing.
 */
class wxSFLayoutMesh : public wxSFLayoutAlgorithm
{
public:
	/*!
	 * \brief Constructor.
	 */
	wxSFLayoutMesh() : m_HSpace(30), m_VSpace(30) {;}
	/*!
	 * \brief Destructor.
	 */
	virtual ~wxSFLayoutMesh() {;}
	
	/*!
	 * \brief Function performing the layout change.
	 * \param shapes List of shapes which should be layouted
	 */
	virtual void DoLayout(ShapeList &shapes);
	
	/*!
	 * \brief Set horizontal space between shapes.
	 * \param HSpace Horizontal space in pixels.
	 */
	void SetHSpace(double HSpace) {this->m_HSpace = HSpace;}
	/*!
	 * \brief Set vertical space between shapes.
	 * \param VSpace Vertical space in pixels.
	 */
	void SetVSpace(double VSpace) {this->m_VSpace = VSpace;}
	/*!
	 * \brief Get horizontal space.
	 * \return Horizontal space in pixels.
	 */
	double GetHSpace() const {return m_HSpace;}
	/*!
	 * \brief Get vertical space.
	 * \return Vertical space in pixels.
	 */
	double GetVSpace() const {return m_VSpace;}
	
	
protected:
	double m_HSpace;
	double m_VSpace;
};

#endif //_WXSFAUTOLAYOUT_H_
