/***************************************************************
 * Name:      ShapeCanvas.h
 * Purpose:   Defines shape canvas class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFSHAPECANVAS_H
#define _WXSFSHAPECANVAS_H

#include <wx/dataobj.h>
#include <wx/dnd.h>

#include <wx/wxsf/ShapeBase.h>
#include <wx/wxsf/DiagramManager.h>
#include <wx/wxsf/MultiSelRect.h>
#include <wx/wxsf/CanvasHistory.h>
#include <wx/wxsf/LineShape.h>
#include <wx/wxsf/EditTextShape.h>
#include <wx/wxsf/Printout.h>
#include <wx/wxsf/CommonFcn.h>

#ifdef __WXMAC__ 
#include <wx/mac/carbon/printdlg.h> 
#endif 

/*! \brief XPM (mono-)bitmap which can be used in shape's shadow brush */
extern const char* wxSFShadowBrush_xpm[];

/*! \brief Global page setup data */
extern wxPageSetupDialogData* g_pageSetupData;

/*! \brief Global print data, to remember settings during the session */
extern wxPrintData *g_printData;

#define sfDEFAULT_ME_OFFSET 5
#define sfSAVE_STATE true
#define sfDONT_SAVE_STATE false
#define sfFROM_PAINT true
#define sfNOT_FROM_PAINT false
#define sfTOPMOST_SHAPES true
#define sfALL_SHAPES false
#define sfPROMPT true
#define sfNO_PROMPT false

// default values
/*! \brief Default value of wxSFCanvasSettings::m_nBackgroundColor data member */
#define sfdvSHAPECANVAS_BACKGROUNDCOLOR wxColour(240, 240, 240)
/*! \brief Default value of wxSFCanvasSettings::m_nGridSize data member */
#define sfdvSHAPECANVAS_GRIDSIZE wxSize(10, 10)
/*! \brief Default value of wxSFCanvasSettings::m_nGridLineMult data member */
#define sfdvSHAPECANVAS_GRIDLINEMULT 1
/*! \brief Default value of wxSFCanvasSettings::m_nGridColor data member */
#define sfdvSHAPECANVAS_GRIDCOLOR wxColour(200, 200, 200)
/*! \brief Default value of wxSFCanvasSettings::m_nGridStyle data member */
#define sfdvSHAPECANVAS_GRIDSTYLE wxSOLID
/*! \brief Default value of wxSFCanvasSettings::m_CommnonHoverColor data member */
#define sfdvSHAPECANVAS_HOVERCOLOR wxColor(120, 120, 255)
/*! \brief Default value of wxSFCanvasSettings::m_nGradientFrom data member */
#define sfdvSHAPECANVAS_GRADIENT_FROM wxColour(240, 240, 240)
/*! \brief Default value of wxSFCanvasSettings::m_nGradientTo data member */
#define sfdvSHAPECANVAS_GRADIENT_TO wxColour(200, 200, 255)
/*! \brief Default value of wxSFCanvasSettings::m_nStyle data member */
#define sfdvSHAPECANVAS_STYLE wxSFShapeCanvas::sfsDEFAULT_CANVAS_STYLE
/*! \brief Default value of wxSFCanvasSettings::m_nShadowOffset data member */
#define sfdvSHAPECANVAS_SHADOWOFFSET wxRealPoint(4, 4)
/*! \brief Default shadow colour */
#define sfdvSHAPECANVAS_SHADOWCOLOR wxColour(150, 150, 150, 128)
/*! \brief Default value of wxSFCanvasSettings::m_ShadowFill data member */
#define sfdvSHAPECANVAS_SHADOWBRUSH wxBrush(sfdvSHAPECANVAS_SHADOWCOLOR, wxSOLID)
/*! \brief Default value of wxSFCanvasSettings::m_nPrintHAlign data member */
#define sfdvSHAPECANVAS_PRINT_HALIGN wxSFShapeCanvas::halignCENTER
/*! \brief Default value of wxSFCanvasSettings::m_nPrintVAlign data member */
#define sfdvSHAPECANVAS_PRINT_VALIGN wxSFShapeCanvas::valignMIDDLE
/*! \brief Default value of wxSFCanvasSettings::m_nPrintMode data member */
#define sfdvSHAPECANVAS_PRINT_MODE wxSFShapeCanvas::prnFIT_TO_MARGINS
/*! \brief Default value of wxSFCanvasSettings::m_nMinScale data member */
#define sfdvSHAPECANVAS_SCALE_MIN 0.1
/*! \brief Default value of wxSFCanvasSettings::m_nMaxScale data member */
#define sfdvSHAPECANVAS_SCALE_MAX 5


class wxSFCanvasDropTarget;

/*!
 * \brief Auxiliary class encapsulating shape drop target.
 */
class wxSFCanvasDropTarget : public wxDropTarget
{
	friend class wxSFShapeCanvas;

protected:
	wxSFCanvasDropTarget(wxDataObject *data, wxSFShapeCanvas *parent);
	virtual ~wxSFCanvasDropTarget();

	virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

	wxSFShapeCanvas *m_pParentCanvas;
};

/*!
 * \brief Auxiliary serializable class encapsulating the canvas properties.
 */
class wxSFCanvasSettings : public xsSerializable
{
public:
    DECLARE_DYNAMIC_CLASS(wxSFCanvasSettings);

    wxSFCanvasSettings();

    wxColour m_nBackgroundColor;
    wxColour m_nCommonHoverColor;

	wxColour m_nGradientFrom;
	wxColour m_nGradientTo;

    wxSize m_nGridSize;
	int m_nGridLineMult;
    wxColour m_nGridColor;
	int m_nGridStyle;

    wxRealPoint m_nShadowOffset;
    wxBrush m_ShadowFill;

    wxArrayString m_arrAcceptedShapes;

    double m_nScale;
	double m_nMinScale;
	double m_nMaxScale;

	long m_nStyle;

	int m_nPrintHAlign;
	int m_nPrintVAlign;
	int m_nPrintMode;
};

/*!
 * \brief Class encapsulating a Shape canvas. The shape canvas is window control
 * which extends the wxScrolledWindow and is responsible for displaying of shapes diagrams.
 * It also supports clipboard and drag&drop operations, undo/redo operations,
 * and graphics exporting functions.
 *
 * This class is a core framework class and provides many member functions suitable for adding,
 * removing, moving, resizing and drawing of shape objects. It can be used as it is or as a base class
 * if necessary. In that case, the default class functionality can be enhaced by overriding of
 * its virtual functions or by manual events handling. In both cases the user is responsible
 * for invoking of default event handlers/virtual functions otherwise the
 * built in functionality wont be available.
 * \sa wxSFDiagramManager
 */
class WXDLLIMPEXP_SF wxSFShapeCanvas : public wxScrolledWindow
{
public:

    friend class wxSFDiagramManager;
	friend class wxSFCanvasDropTarget;

    /*!
     * \brief Default constructor
     */
    wxSFShapeCanvas();
    /*!
     * \brief Constructor
     * \param manager Pointer to shape manager
     * \param parent Parent window
     * \param id Window ID
     * \param pos Initial position
     * \param size Initial size
     * \param style Window style
     */
	wxSFShapeCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL);
	/*! \brief Destructor */
	~wxSFShapeCanvas(void);

    /*!
     * \brief Creates the window in two-step construction mode. SetDiagramManager() function must be also called to complete the canvas initialization.
     * \param parent Parent window
     * \param id Window ID
     * \param pos Initial position
     * \param size Initial size
     * \param style Window style
     * \param name Window name
     */
    virtual bool Create(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL, const wxString& name = wxT("scrolledWindow"));

    /*! \brief Working modes */
	enum MODE
	{
	    /*! \brief The shape canvas is in ready state (no operation is pending) */
		modeREADY = 0,
        /*! \brief Some shape handle is dragged */
		modeHANDLEMOVE,
		/*! \brief Handle of multiselection tool is dragged */
		modeMULTIHANDLEMOVE,
		/*! \brief Some shape/s is/are dragged */
		modeSHAPEMOVE,
		/*! \brief Multiple shape selection is in progess */
		modeMULTISELECTION,
		/*! \brief Interactive connection creation is in progress */
		modeCREATECONNECTION,
		/*! \brief Canvas is in the Drag&Drop mode */
		modeDND
	};

    /*! \brief Search mode flags for GetShapeAtPosition function */
	enum SEARCHMODE
	{
	    /*! \brief Search for selected shapes only*/
		searchSELECTED,
		/*! \brief Search for unselected shapes only */
		searchUNSELECTED,
		/*! \brief Search for both selected and unselected shapes */
		searchBOTH
	};

    /*! \brief Flags for AlignSelected function */
	enum VALIGN
	{
	    valignNONE,
	    valignTOP,
	    valignMIDDLE,
	    valignBOTTOM
	};

    /*! \brief Flags for AlignSelected function */
	enum HALIGN
	{
	    halignNONE,
	    halignLEFT,
	    halignCENTER,
	    halignRIGHT
	};

	/*! \brief Style flags */
	enum STYLE
	{
		/*! \brief Allow multiselection box. */
		sfsMULTI_SELECTION = 1,
		/*! \brief Allow shapes' size change done via the multiselection box. */
		sfsMULTI_SIZE_CHANGE = 2,
		/*! \brief Show grid. */
		sfsGRID_SHOW = 4,
		/*! \brief Use grid. */
		sfsGRID_USE = 8,
		/*! \brief Enable Drag & Drop operations. */
		sfsDND = 16,
		/*! \brief Enable Undo/Redo operations. */
		sfsUNDOREDO = 32,
		/*! \brief Enable the clipboard. */
		sfsCLIPBOARD = 64,
		/*! \brief Enable mouse hovering */
		sfsHOVERING = 128,
		/*! \brief Enable highligting of shapes able to accept dragged shape(s). */
		sfsHIGHLIGHTING = 256,
		/*! \brief Use gradient color for the canvas background. */
		sfsGRADIENT_BACKGROUND = 512,
		/*! \brief Print also canvas background. */
		sfsPRINT_BACKGROUND = 1024,
		/*! \brief Process mouse wheel by the canvas (canvas scale will be changed). */
		sfsPROCESS_MOUSEWHEEL = 2048,
		/*! \brief Default canvas style. */
		sfsDEFAULT_CANVAS_STYLE = sfsMULTI_SELECTION | sfsMULTI_SIZE_CHANGE | sfsDND | sfsUNDOREDO | sfsCLIPBOARD | sfsHOVERING | sfsHIGHLIGHTING
	};

    /*! \brief Flags for ShowShadow function. */
	enum SHADOWMODE
	{
	    /*! \brief Show/hide shadow under topmost shapes only. */
	    shadowTOPMOST,
	    /*! \brief Show/hide shadow under all shapes in the diagram. */
	    shadowALL
	};

	/*! \brief Printing modes used by SetPrintMode() function. */
	enum PRINTMODE
	{
	    /*! \brief This sets the user scale and origin of the DC so that the image fits
         * within the paper rectangle (but the edges could be cut off by printers
         * that can't print to the edges of the paper -- which is most of them. Use
         * this if your image already has its own margins. */
	    prnFIT_TO_PAPER,
        /*! \brief This sets the user scale and origin of the DC so that the image fits
         * within the page rectangle, which is the printable area on Mac and MSW
         * and is the entire page on other platforms. */
	    prnFIT_TO_PAGE,
        /*! \brief This sets the user scale and origin of the DC so that the image fits
         * within the page margins as specified by g_PageSetupData, which you can
         * change (on some platforms, at least) in the Page Setup dialog. Note that
         * on Mac, the native Page Setup dialog doesn't let you change the margins
         * of a wxPageSetupDialogData object, so you'll have to write your own dialog or
         * use the Mac-only wxMacPageMarginsDialog, as we do in this program. */
	    prnFIT_TO_MARGINS,
	    /*! \brief This sets the user scale and origin of the DC so that you could map the
	     * screen image to the entire paper at the same size as it appears on screen. */
	    prnMAP_TO_PAPER,
	    /*! \brief This sets the user scale and origin of the DC so that the image appears
         * on the paper at the same size that it appears on screen (i.e., 10-point
         * type on screen is 10-point on the printed page). */
	    prnMAP_TO_PAGE,
	    /*! \brief This sets the user scale and origin of the DC so that you could map the
	     * screen image to the page margins specified by the native Page Setup dialog at the same
	     * size as it appears on screen. */
	    prnMAP_TO_MARGINS,
	     /*! \brief This sets the user scale and origin of the DC so that you can to do you own
	      * scaling in order to draw objects at full native device resolution. */
	    prnMAP_TO_DEVICE
	};

	// public functions

    /*!
     * \brief Get diagram manager associated with this instance of shape canvas
     * \return Pointer to diagram manager
     * \sa wxSFDiagramManager
     */
    inline wxSFDiagramManager* GetDiagramManager() { return m_pManager; }
    /*!
     * \brief Set diagram manager for this shape canvas
     * \param manager Pointer to diagram manager instance
     * \sa wxSFDiagramManager
     */
    void SetDiagramManager(wxSFDiagramManager* manager);

    /*!
     * \brief Load serialized canvas content (diagrams) from given file.
     * \param file Full file name
     */
	void LoadCanvas(const wxString& file);
    /*!
     * \brief Save  canvas content (diagrams) to given file.
     * \param file Full file name
     */
	void SaveCanvas(const wxString& file);
    /*!
     * \brief Export canvas content to BMP file.
     * \param file Full file name
     */
	void SaveCanvasToBMP(const wxString& file);

    /*!
     * \brief Start interactive connection creation.
     *
     * This function switch the canvas to a mode in which a new shape connection
     * can be created interactively (by mouse operations). Every connection must
     * start and finish in some shape object or another connection. At the end of the
     * process the OnConnectionFinished event handler is invoked so the user can
     * set needed connection properties immediately.
     *
     * Function must be called from mouse event handler and the event must be passed
     * to the function.
     * \param shapeInfo Connection type
     * \param pos Position where to start
	 * \param err Pointer to variable where operation result will be stored. Can be NULL.
     * \sa CreateConnection
     */
    void StartInteractiveConnection(wxClassInfo* shapeInfo, const wxPoint& pos, wxSF::ERRCODE *err = NULL);
	 /*!
     * \brief Start interactive connection creation from existing line object.
     *
     * This function switch the canvas to a mode in which a new shape connection
     * can be created interactively (by mouse operations). Every connection must
     * start and finish in some shape object or another connection. At the end of the
     * process the OnConnectionFinished event handler is invoked so the user can
     * set needed connection properties immediately.
     *
     * Function must be called from mouse event handler and the event must be passed
     * to the function.
     * \param shape Pointer to existing line shape object which will be used as a connection.
     * \param pos Position where to start
	 * \param err Pointer to variable where operation result will be stored. Can be NULL.
     * \sa CreateConnection
     */
	void StartInteractiveConnection(wxSFLineShape* shape, const wxPoint& pos, wxSF::ERRCODE *err = NULL);
	
    /*! \brief Abort interactive connection creation process */
	void AbortInteractiveConnection();

    /*! \brief Select all shapes in the canvas */
	void SelectAll();
	/*! \brief Deselect all shapes */
	void DeselectAll();
	/*! \brief Hide handles of all shapes */
	void HideAllHandles();
	/*!
	 * \brief Repaint the shape canvas
	 * \param erase TRUE if the canvas should be erased before repainting
	 * \param rct Refreshed region (rectangle)
	 */
	void RefreshCanvas(bool erase, wxRect rct);
	/*!
	 * \brief Show shapes shadows (only current digram shapes are affected).
	 *
	 * The functions sets/unsets sfsSHOW_SHADOW flag for all shapes currently included in the diagram.
	 * \param show TRUE if the shadow shoud be shown, otherwise FALSE
	 * \param style Shadow style
	 * \sa SHADOWMODE
	 */
	void ShowShadows(bool show, SHADOWMODE style);

	/*!
	 * \brief Start Drag&Drop operation with shapes included in the given list.
	 * \param shapes List of shapes which should be dragged
	 * \param start A point where the dragging operation has started
	 * \return rct Drag result
	 */
	wxDragResult DoDragDrop(ShapeList &shapes, const wxPoint& start = wxPoint(-1, -1));
    /*! \brief Copy selected shapes to the clipboard */
	void Copy();
	/*! \brief Copy selected shapes to the clipboard and remove them from the canvas */
	void Cut();
	/*! \brief Paste shapes stored in the clipboard to the canvas */
	void Paste();
	/*! \brief Perform Undo operation (if available) */
	void Undo();
	/*! \brief Perform Redo operation (if available) */
	void Redo();
	/*! \brief Function returns TRUE if some shapes can be copied to the clipboard (it means they are selected) */
	bool CanCopy();
	/*! \brief Function returns TRUE if some shapes can be cutted to the clipboard (it means they are selected) */
	bool CanCut();
	/*! \brief Function returns TRUE if some shapes can be copied from the clipboard to the canvas
	 * (it means the clipboard contains stored shapes) */
	bool CanPaste();
	/*! \brief Function returns TRUE if Undo operation can be done */
	bool CanUndo();
	/*! \brief Function returns TRUE if Redo operation can be done */
	bool CanRedo();
	/*! \brief Function returns TRUE if AlignSelected function can be invoked (if more than
	 * one shape is selected) */
	bool CanAlignSelected();

    /*! \brief Save current canvas state (for Undo/Redo operations) */
	void SaveCanvasState();
	/*! \brief Clear all stored canvas states (no Undo/Redo operations will be available) */
	void ClearCanvasHistory();

	/*!
	 * \brief Print current canvas content.
	 * \param prompt If TRUE (sfPROMT) then the the native print dialog will be displayed before printing
	 */
	void Print(bool prompt = sfPROMPT);
	/*!
	 * \brief Print current canvas content using user-defined printout class.
	 * \param printout Pointer to user-defined printout object (inherited from wxSFPrintout class). Do not delete this object explicitly.
	 * \param prompt If TRUE (sfPROMT) then the native print dialog will be displayed before printing
	 * \sa wxSFPrintout
	 */
	void Print(wxSFPrintout *printout, bool prompt = sfPROMPT);
	/*! \brief Show print preview. */
	void PrintPreview();
	/*!
	 * \brief Show print preview using user-defined printout classes.
     * \param preview Pointer to user-defined printout object (inherited from wxSFPrintout class) used for print preview.
     * Do not delete this object explicitly.
     * \param printout Pointer to user-defined printout class (inherited from wxSFPrintout class) used for printing.
     * Do not delete this object explicitly. This parameter can be NULL (in this case a print button will not be available
     * in the print preview window).
     * \sa wxSFPrintout
	 */
	void PrintPreview(wxSFPrintout *preview, wxSFPrintout *printout = NULL);
	/*! \brief Show page setup dialog for printing. */
	void PageSetup();
	#ifdef __WXMAC__
	/*! \brief Show page margins setup dialog (available only for MAC). */
	void PageMargins();
	#endif

    /*!
     * \brief Convert device position to logical position.
     *
     * The function returns unscrolled unscaled canvas position.
     * \param pos Device position (for example mouse position)
     * \return Logical position
     */
	wxPoint DP2LP(const wxPoint& pos) const;
	wxRect DP2LP(const wxRect& rct) const;
    /*!
     * \brief Convert logical position to device position.
     *
     * The function returns scrolled scaled canvas position.
     * \param pos Logical position (for example shape position)
     * \return Device position
     */
	wxPoint LP2DP(const wxPoint& pos) const;
	wxRect LP2DP(const wxRect& rct) const;

	/*!
	 * \brief Get shape under current mouse cursor position (fast implementation - use everywhere
	 * it is possible instead of much slower GetShapeAtPosition()).
	 * \param mode Search mode
	 * \return Pointer to shape if found, otherwise NULL
	 * \sa SEARCHMODE, wxSFShapeCanvas::DP2LP, wxSFShapeCanvas::GetShapeAtPosition
	 */
	wxSFShapeBase* GetShapeUnderCursor(SEARCHMODE mode = searchBOTH);
	/*!
	 * \brief Get shape at given logical position
	 * \param pos Logical position
	 * \param zorder Z-order of searched shape (usefull if several shapes are located
	 * at the given position)
	 * \param mode Search mode
	 * \return Pointer to shape if found, otherwise NULL
	 * \sa SEARCHMODE, wxSFShapeCanvas::DP2LP,, wxSFShapeCanvas::GetShapeUnderCursor
	 */
	wxSFShapeBase* GetShapeAtPosition(const wxPoint& pos, int zorder = 1, SEARCHMODE mode = searchBOTH);
    /*!
     * \brief Get topmost handle at given position
     * \param pos Logical position
     * \return Pointer to shape handle object if found, otherwise NULL
     * \sa wxSFShapeCanvas::DP2LP
     */
	wxSFShapeHandle* GetTopmostHandleAtPosition(const wxPoint& pos);
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
	 * \brief Get list of selected shapes.
	 * \param selection Reference to shape list where pointers to all selected shapes will be stored
	 */
	void GetSelectedShapes(ShapeList& selection);
    /*!
	 * \brief Get box bounding all shapes in the canvas.
	 * \return Total bounding box
	 */
	wxRect GetTotalBoundingBox() const;
    /*!
     * \brief Get bounding box of all selected shapes.
     * \return Selection bounding box
     */
	wxRect GetSelectionBB();

	/*!
	 * \brief Align selected shapes in given directions.
	 *
	 * Shapes will be aligned according to most far shape in appropriate direction.
	 * \param halign Horizontal alignment
	 * \param valign Vertical alignment
	 */
	void AlignSelected(HALIGN halign, VALIGN valign);

    /*!
     * \brief Set canvas style.
	 *
	 * Default value is sfsMULTI_SELECTION | sfsMULTI_SIZE_CHANGE | sfsDND | sfsUNDOREDO | sfsCLIPBOARD | sfsHOVERING | sfsHIGHLIGHTING
     * \param style Combination of the canvas styles
     * \sa STYLE
     */
    inline void SetStyle(long style) { m_Settings.m_nStyle = style; }
    /*! \brief Get current canvas style. */
    inline long GetStyle() const { return m_Settings.m_nStyle; }
	/*! \brief Add new style flag. */
    inline void AddStyle(STYLE style) { m_Settings.m_nStyle |= style; }
	/*! \brief Remove given style flag. */
    inline void RemoveStyle(STYLE style) { m_Settings.m_nStyle &= ~style; }
	/*! \brief Check whether given style flag is used. */
    inline bool ContainsStyle(STYLE style) const { return (m_Settings.m_nStyle & style) != 0; }

	// public members accessors
	/*!
	 * \brief Set canvas background color.
	 * \param col Background color
	 */
	inline void SetCanvasColour(const wxColour& col) { m_Settings.m_nBackgroundColor = col; }
	/*!
	 * \brief Get canvas background color.
	 * \return Background color
	 */
	inline wxColour GetCanvasColour() const { return m_Settings.m_nBackgroundColor; }
	/*!
	 * \brief Set starting gradient color.
	 * \param col Color
	 */
	inline void SetGradientFrom(const wxColour& col) { m_Settings.m_nGradientFrom = col; }
	/*!
	 * \brief Get starting gradient color.
	 * \return Color
	 */
	inline wxColour GetGradientFrom() const { return m_Settings.m_nGradientFrom; }
	/*!
	 * \brief Set ending gradient color.
	 * \param col Color
	 */
	inline void SetGradientTo(const wxColour& col) { m_Settings.m_nGradientTo = col; }
	/*!
	 * \brief Get ending gradient color.
	 * \return Color
	 */
	inline wxColour GetGradientTo() const { return m_Settings.m_nGradientTo; }
	/*!
	 * \brief Get grid size.
	 * \return Grid size
	 */
	inline wxSize GetGrid() const { return m_Settings.m_nGridSize; }
	/*!
	 * \brief Set grid size.
	 * \param grid Grid size
	 */
	inline void SetGrid(wxSize grid) { m_Settings.m_nGridSize = grid; }
	/*!
	 * \brief Set grid line multiple.
	 * 
	 * Grid lines will be drawn in a distance calculated as grid size multiplicated by this value.
	 * Default value is 1.
	 * \param multiple Multiple value
	 */
	inline void SetGridLineMult(int multiple) { m_Settings.m_nGridLineMult = multiple; }
	/**
	 * \brief Get grid line multiple.
	 * \return Value by which a grid size will be multiplicated to determine grid lines distance
	 */
	inline int GetGrigLineMult() const { return m_Settings.m_nGridLineMult; }
	/*!
	 * \brief Set grid color.
	 * \param col Grid color
	 */
	inline void SetGridColour(const wxColour& col) { m_Settings.m_nGridColor = col; }
	/*!
	 * \brief Get grid color.
	 * \return Grid color
	 */
	inline wxColour GetGridColour() const { return m_Settings.m_nGridColor; }
	/*!
	 * \brief Set grid line style.
	 * \param style Line style
	 */
	inline void SetGridStyle(int style) { m_Settings.m_nGridStyle = style; }
	/*!
	 * \brief Get grid line style.
	 * \return Line style
	 */
	inline int GetGridStyle() const {return m_Settings.m_nGridStyle; }
	/*!
	 * \brief Set shadow offset.
	 * \param offset Shadow offset
	 */
	inline void SetShadowOffset(const wxRealPoint& offset) { m_Settings.m_nShadowOffset = offset; }
	/*!
	 * \brief Get shadow offset.
	 * \return Shadow offset
	 */
	inline wxRealPoint GetShadowOffset() const { return m_Settings.m_nShadowOffset; }
	/*!
	 * \brief Set shadow fill (used for shadows of non-text shapes only).
	 * \param brush Reference to brush object
	 */
	inline void SetShadowFill(const wxBrush& brush) { m_Settings.m_ShadowFill = brush; }
	/*!
	 * \brief Get shadow fill.
	 * \return Current shadow brush
	 */
	inline wxBrush GetShadowFill() const { return m_Settings.m_ShadowFill; }
	/*!
	 * \brief Set horizontal align of printed drawing.
	 * \param val Horizontal align
	 * \sa HALIGN
	 */
	inline void SetPrintHAlign(HALIGN val) { m_Settings.m_nPrintHAlign = (int)val; }
	/*!
	 * \brief Get horizontal align of printed drawing.
	 * \return Current horizontal align
	 * \sa HALIGN
	 */
	inline HALIGN GetPrintHAlign() const { return (HALIGN)m_Settings.m_nPrintHAlign; }
	/*!
	 * \brief Set vertical align of printed drawing.
	 * \param val Verical align
	 * \sa VALIGN
	 */
	inline void SetPrintVAlign(VALIGN val) { m_Settings.m_nPrintVAlign = (int)val; }
	/*!
	 * \brief Get vertical align of printed drawing.
	 * \return Current vertical align
	 * \sa VALIGN
	 */
	inline VALIGN GetPrintVAlign() const { return (VALIGN)m_Settings.m_nPrintVAlign; }
	/*!
	 * \brief Set printing mode for this canvas.
	 * \param mode Printing mode
	 * \sa PRINTMODE
	 */
	inline void SetPrintMode(PRINTMODE mode) { m_Settings.m_nPrintMode = (int)mode; }
	/*!
	 * \brief Get printing mode for this canvas.
	 * \return Current printing mode
	 * \sa PRINTMODE
	 */
	inline PRINTMODE GetPrintMode() const { return (PRINTMODE)m_Settings.m_nPrintMode; }
	/*!
	 * \brief Set canvas scale.
	 * \param scale Scale value
	 */
	void SetScale(double scale);
	/*!
	 * \brief Set minimal allowed scale (for mouse wheel scale change).
	 * \param scale Minimal scale
	 */
	void SetMinScale(double scale) { m_Settings.m_nMinScale = scale; }
	/*!
	 * \brief Get minimal allowed scale (for mouse wheel scale change).
	 * \return Minimal scale
	 */
	double GetMinScale() { return m_Settings.m_nMinScale; }
	/*!
	 * \brief Set maximal allowed scale (for mouse wheel scale change).
	 * \param scale Maximal scale
	 */
	void SetMaxScale(double scale) { m_Settings.m_nMaxScale = scale; }
	/*!
	 * \brief Set maximal allowed scale (for mouse wheel scale change).
	 * \return Maximal scale
	 */
	double GetMaxScale() { return m_Settings.m_nMaxScale; }
	/*!
	 * \brief Get the canvas scale.
	 * \return Canvas scale
	 */
	inline double GetScale() const { return m_Settings.m_nScale; }
	/*!
	 * \brief Set the canvas scale so a whole diagram is visible.
	 */
	void SetScaleToViewAll();
	/**
	 * \brief Scroll the shape canvas so the given shape will be located in its center.
	 * \param shape Pointer to focused shape
	 */
	void ScrollToShape(wxSFShapeBase* shape);
	/*!
	 * \brief Enable usage of wxGraphicsContext for drawing (if supported).
	 * \param enab If TRUE then the wxGraphicsContext will be used
	 */
    static void EnableGC(bool enab);
	/*!
	 * \brief Function returns information whether the wxGraphicsContext is enabled (if supported).
	 * \return TRUE if the wxGraphicsContext is enabled
	 */
    static bool IsGCEnabled() { return m_fEnableGC; }

	/*!
	 * \brief Get canvas workind mode.
	 * \return Working mode
	 * \sa MODE
	 */
	inline MODE GetMode() const { return m_nWorkingMode; }
	/*!
	 * \brief Set default hover color.
	 * \param col Hover color.
	 */
	void SetHoverColour(const wxColour& col);
	/*!
	 * \brief Get default hover color.
	 * \return Hover color
	 */
	inline wxColour GetHoverColour() const { return m_Settings.m_nCommonHoverColor; }
	/*!
	 * \brief Get canvas hostory manager.
	 * \return Reference to the canvas history manager
	 * \sa wxSFCanvasHistory
	 */
	inline wxSFCanvasHistory& GetHistoryManager() { return m_CanvasHistory; }

	/*!
	 * \brief Update given position so it will fit canvas grid (if enabled).
	 * \param pos Position which should be updated
	 * \return Updated position
	 */
	wxPoint FitPositionToGrid(const wxPoint& pos);
	/*! \brief Update size of multi selection rectangle */
	void UpdateMultieditSize();
	/*! \brief Update scroll window virtual size so it can display all shape canvas */
	void UpdateVirtualSize();
	/*! \brief Move all shapes so none of it will be located in negative position */
	void MoveShapesFromNegatives();
	/*! \brief Center diagram in accordance to the shape canvas extent. */
	void CenterShapes();
    /*!
     * \brief Validate selection (remove redundantly selected shapes etc...).
     * \param selection List of selected shapes that should be validated
     */
	void ValidateSelection(ShapeList& selection);

	/*!
	 * \brief Function responsible for drawing of the canvas's content to given DC.
	 * \param dc Reference to device context where the shapes will be drawn to
	 * \param fromPaint Set the argument to TRUE if the dc argument refers to the wxPaintDC instance
	 */
	void DrawContent(wxDC& dc, bool fromPaint);

    /*!
     * \brief Get reference to multiselection box
     * \return Reference to multiselection box object
     */
    inline wxSFMultiSelRect& GetMultiselectionBox() { return m_shpMultiEdit; }

	/*! \brief Close and delete all opened text editing controls actualy used by editable text shapes */
	void DeleteAllTextCtrls();

	// public virtual event handlers
    /*!
     * \brief Event handler called when the canvas is clicked by
	 * the left mouse button. The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Mouse event
     * \sa _OnLeftDown()
     */
	virtual void OnLeftDown(wxMouseEvent& event);
    /*!
     * \brief Event handler called when the canvas is double-clicked by
	 * the left mouse button. The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Mouse event
     * \sa _OnLeftDoubleClick()
     */
	virtual void OnLeftDoubleClick(wxMouseEvent& event);
    /*!
     * \brief Event handler called when the left mouse button is released.
     * The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Mouse event
     * \sa _OnLeftUp()
     */
	virtual void OnLeftUp(wxMouseEvent& event);
    /*!
     * \brief Event handler called when the canvas is clicked by
	 * the right mouse button. The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Mouse event
     * \sa _OnRightDown()
     */
	virtual void OnRightDown(wxMouseEvent& event);
    /*!
     * \brief Event handler called when the canvas is double-clicked by
	 * the right mouse button. The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Mouse event
     * \sa _OnRightDoubleClick()
     */
	virtual void OnRightDoubleClick(wxMouseEvent& event);
    /*!
     * \brief Event handler called when the right mouse button is released.
     * The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Mouse event
     * \sa _OnRightUp()
     */
	virtual void OnRightUp(wxMouseEvent& event);
    /*!
     * \brief Event handler called when the mouse pointer is moved.
     * The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Mouse event
     * \sa _OnMouseMove()
     */
	virtual void OnMouseMove(wxMouseEvent& event);
	/*!
     * \brief Event handler called when the mouse wheel position is changed.
     * The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Mouse event
     * \sa _OnMouseWheel()
     */
	virtual void OnMouseWheel(wxMouseEvent& event);
    /*!
     * \brief Event handler called when any key is pressed.
     * The function can be overrided if necessary.
     *
     * The function is called by the framework and provides basic functionality
     * needed for proper management of displayed shape. It is necessary to call
     * this function from overrided methods if the default canvas behaviour
     * should be preserved.
     * \param event Keyboard event
     * \sa _OnKeyDown()
     */
	virtual void OnKeyDown(wxKeyEvent& event);

    /*!
     * \brief Event handler called when any editable text shape is changed.
     * The function can be overrided if necessary.
	 * The function is called by the framework and its default implementation
     * generates wxEVT_SF_TEXT_CHANGE event.
     * \param shape Changed wxSFEditTextShape object
	 * \sa wxSFEditTextShape::EditLabel(), wxSFShapeTextEvent
     */
	virtual void OnTextChange(wxSFEditTextShape* shape);
	/*!
	 * \brief Event handler called after successfull conection creation. The function
	 * can be overrided if necessary. The default implementation
     * generates wxEVT_SF_LINE_DONE event.
	 * \param connection Pointer to new connection object
	 * \sa StartInteractiveConnection(), wxSFShapeEvent
	 */
	virtual void OnConnectionFinished(wxSFLineShape* connection);
	
	/*!
	 * \brief Event handler called after successfull conection creation in
	 * order to alow developper to perform some kind of checks
	 * before the connection is really added to the diagram. The function
	 * can be overrided if necessary. The default implementation
     * generates wxEVT_SF_LINE_DONE event.
	 * \param connection Pointer to new connection object
	 * \sa StartInteractiveConnection(), wxSFShapeEvent
	 * \return false if the generated event has been vetoed in this case,
	 * the connection creation is cancelled
	 */
	virtual bool OnPreConnectionFinished(wxSFLineShape* connection);

	/*!
	 * \brief Event handler called by the framework after any dragged shapes
	 * are dropped to the canvas. The default implementation
     * generates wxEVT_SF_ON_DROP event.
	 * \param x X-coordinate of a position the data was dropped to
	 * \param y Y-coordinate of a position the data was dropped to
	 * \param def Drag result
	 * \param dropped Reference to a list containing the dropped data
	 * \sa wxSFCanvasDropTarget, wxSFShapeDropEvent
	 */
	virtual void OnDrop(wxCoord x, wxCoord y, wxDragResult def, const ShapeList& dropped);

	/*!
	 * \brief Event handler called by the framework after pasting of shapes
	 * from the clipboard to the canvas. The default implementation
     * generates wxEVT_SF_ON_PASTE event.
	 * \param pasted Reference to a list containing the pasted data
	 * \sa wxSFShapeCanvas::Paste(), wxSFShapePasteEvent
	 */
	virtual void OnPaste(const ShapeList& pasted);
	
	/*!
	 * \brief Event handler called if canvas virtual size is going to be updated.
	 * The default implementation does nothing but the function can be overrided by
	 * a user to modify calculated virtual canvas size.
	 * \param virtrct Calculated canvas virtual size
	 */
	virtual void OnUpdateVirtualSize(wxRect &virtrct);


protected:

	// protected data members
	MODE m_nWorkingMode;
	wxSFCanvasSettings m_Settings;
	static bool m_fEnableGC;

	// protected functions

private:

	// private data members

	wxSFMultiSelRect m_shpMultiEdit;
	static wxBitmap m_OutBMP;

	bool m_fCanSaveStateOnMouseUp;
	static int m_nRefCounter;

	/*! \brief Flag used for determination whether the D&D operation has started and ended in one canvas instance */
	bool m_fDnDStartedHere;
	/*! \brief Started position of current D&D operation */
	wxPoint m_nDnDStartedAt;
	/*! \brief Custom data format object (used for the clipboard and D&D operations */
	wxDataFormat m_formatShapes;

	wxPoint m_nPrevMousePos;

	/*! \brief Canvas history manager */
	wxSFCanvasHistory m_CanvasHistory;

	/*! \brief Pointer to parent data (shapes) manager */
    wxSFDiagramManager* m_pManager;

	/*! \brief Pointer to currently selected shape handle */
	wxSFShapeHandle* m_pSelectedHandle;
	/*! \brief Pointer to new line shape under constuction */
	wxSFLineShape* m_pNewLineShape;
	/*! \brief Pointer to topmost unselected shape under the mouse cursor */
	wxSFShapeBase *m_pUnselectedShapeUnderCursor;
	/*! \brief Pointer to topmost selected shape under the mouse cursor */
	wxSFShapeBase *m_pSelectedShapeUnderCursor;
	/*! \brief Pointer to topmost shape under the mouse cursor */
	wxSFShapeBase *m_pTopmostShapeUnderCursor;

	/*! \brief Current list of all shapes in the canvas updated during mouse movement */
	ShapeList m_lstCurrentShapes;

	// private functions

	/*! \brief Validate selection so the shapes in the given list can be processed by the clipboard functions */
	void ValidateSelectionForClipboard(ShapeList& selection);
	/*! \brief Append connections assigned to shapes in given list to this list as well */
	void AppendAssignedConnections(wxSFShapeBase *shape, ShapeList& selection, bool childrenonly);
	/*! \brief Initialize printing framework */
	void InitializePrinting();
	/*! \brief Deinitialize prnting framework */
	void DeinitializePrinting();
	/*! \brief Remove given shape for temporary containers */
	void RemoveFromTemporaries(wxSFShapeBase *shape);
	/*! \brief Clear all temporary containers */
	void ClearTemporaries();
	/*! \brief Assign give shape to parent at given location (if exists) */
	void ReparentShape(wxSFShapeBase *shape, const wxPoint& parentpos);

	// private event handlers
	/*!
	 * \brief Event handler called when the canvas should be repainted.
	 * \param event Paint event
	 */
	void _OnPaint(wxPaintEvent& event);
	/*!
	 * \brief Event handler called when the canvas should be erased.
	 * \param event Erase event
	 */
	void _OnEraseBackground(wxEraseEvent& event);
	/*!
	 * \brief Event handler called when the mouse pointer leaves the canvas window.
	 * \param event Mouse event
	 */
	void _OnLeaveWindow(wxMouseEvent& event);
	/*!
	 * \brief Event handler called when the mouse pointer enters the canvas window.
	 * \param event Mouse event
	 */
	void _OnEnterWindow(wxMouseEvent& event);
	/*!
	 * \brief Event handler called when the canvas size has changed.
	 * \param event Size event
	 */
	void _OnResize(wxSizeEvent& event);
	
	// original private event handlers
	/*!
	 * \brief Original private event handler called when the canvas is clicked by
	 * left mouse button. The handler calls user-overridable event handler function
	 * and skips the event for next possible processing.
	 * \param event Mouse event
	 * \sa wxSFShapeCanvas::OnLeftDown
	 */
	void _OnLeftDown(wxMouseEvent& event);
	/*!
	 * \brief Original private event handler called when the canvas is double-clicked by
	 * left mouse button. The handler calls user-overridable event handler function
	 * and skips the event for next possible processing.
	 * \param event Mouse event
	 * \sa wxSFShapeCanvas::OnLeftDoubleClick
	 */
	void _OnLeftDoubleClick(wxMouseEvent& event);
	/*!
	 * \brief Original private event handler called when the left mouse button
	 * is release above the canvas. The handler calls user-overridable event handler function
	 * and skips the event for next possible processing.
	 * \param event Mouse event
	 * \sa wxSFShapeCanvas::OnLeftUp
	 */
	void _OnLeftUp(wxMouseEvent& event);
	/*!
	 * \brief Original private event handler called when the canvas is clicked by
	 * right mouse button. The handler calls user-overridable event handler function
	 * and skips the event for next possible processing.
	 * \param event Mouse event
	 * \sa wxSFShapeCanvas::OnRightDown
	 */
	void _OnRightDown(wxMouseEvent& event);
	/*!
	 * \brief Original private event handler called when the canvas is double-clicked by
	 * right mouse button. The handler calls user-overridable event handler function
	 * and skips the event for next possible processing.
	 * \param event Mouse event
	 * \sa wxSFShapeCanvas::OnRightDoubleClick
	 */
	void _OnRightDoubleClick(wxMouseEvent& event);
	/*!
	 * \brief Original private event handler called when the right mouse button
	 * is release above the canvas. The handler calls user-overridable event handler function
	 * and skips the event for next possible processing.
	 * \param event Mouse event
	 * \sa wxSFShapeCanvas::OnRightUp
	 */
	void _OnRightUp(wxMouseEvent& event);
	/*!
	 * \brief Original private event handler called when the mouse pointer is moving above
	 * the canvas. The handler calls user-overridable event handler function
	 * and skips the event for next possible processing.
	 * \param event Mouse event
	 * \sa wxSFShapeCanvas::OnMouseMove
	 */
	void _OnMouseMove(wxMouseEvent& event);
	/*!
	 * \brief Original private event handler called when the mouse wheel pocition is changed.
	 * The handler calls user-overridable event handler function and skips the event
	 * for next possible processing.
	 * \param event Mouse event
	 * \sa wxSFShapeCanvas::OnMouseWheel
	 */
	void _OnMouseWheel(wxMouseEvent& event);
	/*!
	 * \brief Original private event handler called when any key is pressed.
	 * The handler calls user-overridable event handler function
	 * and skips the event for next possible processing.
	 * \param event Keyboard event
	 * \sa wxSFShapeCanvas::OnKeyDown
	 */
	void _OnKeyDown(wxKeyEvent& event);

	/*!
	 * \brief Function is called by associated wxSFCanvasDropTarget after any dragged shapes
	 * are dropped to the canvas.
	 * \param x X-coordinate of a position the data was dropped to
	 * \param y Y-coordinate of a position the data was dropped to
	 * \param def Drag result
	 * \param data Pointer to a data object encapsulating dropped data
	 * \sa wxSFCanvasDropTarget
	 */
	void _OnDrop(wxCoord x, wxCoord y, wxDragResult def, wxDataObject *data);
	

	DECLARE_EVENT_TABLE();
};

#endif //_WXSFSHAPECANVAS_H
