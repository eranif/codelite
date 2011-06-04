/***************************************************************
 * Name:      GridShape.h
 * Purpose:   Defines grid shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2008-08-02
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFGRIDSHAPE_H
#define _WXSFGRIDSHAPE_H

#include <wx/wxsf/RectShape.h>

// default values
/*! \brief Default value of wxSFGridShape::m_nRows data member. */
#define sfdvGRIDSHAPE_ROWS 3
/*! \brief Default value of wxSFGridShape::m_nCols data member. */
#define sfdvGRIDSHAPE_COLS 3
/*! \brief Default value of wxSFGridShape::m_nCellSpace data member. */
#define sfdvGRIDSHAPE_CELLSPACE 5

/*!
 * \brief Class encapsulates a rectangular shape derived from wxSFRectShape class which acts as a grid-based
 * container able to manage other assigned child shapes (it can control their position). The managed
 * shapes are aligned into defined grid with a behaviour similar to classic wxWidget's wxGridSizer class.
 */
class WXDLLIMPEXP_SF wxSFGridShape : public wxSFRectShape
{
    public:
        XS_DECLARE_CLONABLE_CLASS(wxSFGridShape);
		
		friend class wxSFDiagramManager;

        /*! \brief Default constructor. */
        wxSFGridShape();
        /*!
         * \brief User constructor.
         * \param pos Initial position
         * \param size Initial size
         * \param rows Number of grid rows
         * \param cols Number of grid columns
         * \param cellspace Additional space between managed shapes
         * \param manager Pointer to parent diagram manager
         */
        wxSFGridShape(const wxRealPoint& pos, const wxRealPoint& size, int rows, int cols, int cellspace, wxSFDiagramManager* manager);
        /*!
        * \brief Copy constructor.
        * \param obj Reference to the source object
        */
        wxSFGridShape(const wxSFGridShape& obj);
        /*! \brief Destructor. */
        virtual ~wxSFGridShape();

        // public member data accessors

        /*!
        * \brief Set grid dimensions.
        * \param rows Number of rows
        * \param cols Number of columns
        */
        void SetDimensions(int rows, int cols);
        /*!
        * \brief Set space between grid cells (managed shapes).
        * \param cellspace Cellspace size
        */
        void SetCellSpace(int cellspace){m_nCellSpace = cellspace;}
        /*!
        * \brief Get grid dimensions.
        * \param rows Pointer to variable where number of rows will be stored
        * \param cols Pointer to variable where number of columns will be stored
        */
        void GetDimensions(int *rows, int *cols);
        /*!
        * \brief Get space between grid cells (managed shapes).
        * \return Cellspace size
        */
        int GetCellSpace(){return m_nCellSpace;}
		
       /*!
        * \brief Get managed shape specified by lexicographic cell index.
        * \param index Lexicographic index of requested shape
        * \return Pointer to shape object of given cell index if exists, otherwise NULL
        */
		wxSFShapeBase *GetManagedShape(size_t index);
       /*!
        * \brief Get managed shape specified by row and column indexes.
        * \param row Zero-base row index
        * \param col Zero-based column index
        * \return Pointer to shape object stored in specified grid cell if exists, otherwise NULL
        */
		wxSFShapeBase *GetManagedShape(int row, int col);

        // public functions

        /*!
         * \brief Clear information about managed shapes and set number of rows and columns to zero.
         *
         * Note that this function doesn't remove managed (child) shapes from the parent grid shape
         * (they are still its child shapes but aren't managed anymore).
         */
        void ClearGrid();
        /*!
        * \brief Append given shape to the grid at the last managed position.
        * \param shape Pointer to appended shape
        */
        bool AppendToGrid(wxSFShapeBase *shape);
        /*!
        * \brief Insert given shape to the grid at the given position.
        *
        * Note that the grid can grow in a vertical direction only, so if the user specify desired
        * horizontal position bigger than the current number of columns is then this function exits with
        * an error (false) return value. If specified vertical position exceeds the number or grid rows than
        * the grid is resized. If the given position (grid cell) is already occupied by some shape then the previous
        * one will be moved to the grid's last lexicographic position.
        * \param row Vertical position
        * \param col Horizontal position
        * \param shape Pointer to inserted shape
        * \return True on success, otherwise False
        */
        bool InsertToGrid(int row, int col, wxSFShapeBase *shape);
        /*!
        * \brief Insert given shape to the grid at the given position.
        *
        * Note that the given index is a lexicographic position of inserted shape. The given shape is inserted before
        * the existing item 'index', thus InsertToGrid(0, something) will insert an item in such way that it will become
        * the first grid element.
        * \param index Lexicographic position of inserted shape
        * \param shape Pointer to inserted shape
        * \return True on successe, otherwise False
        */
        bool InsertToGrid(int index, wxSFShapeBase *shape);
		/**
		 * \brief Remove shape with given ID from the grid.
		 * \param id ID of shape which should be removed
		 */
		void RemoveFromGrid(long id);

        // public virtual functions
        /*! \brief Upate shape (align all child shapes an resize it to fit them) */
        virtual void Update();

        /*! \brief Resize the shape to bound all child shapes. The function can be overrided if neccessary. */
        virtual void FitToChildren();

        /*! \brief Do layout of assigned child shapes */
        virtual void DoChildrenLayout();

        /*!
         * \brief Event handler called when any shape is dropped above this shape (and the dropped
         * shape is accepted as a child of this shape). The function can be overrided if necessary.
         *
         * The function is called by the framework (by the shape canvas).
         * \param pos Relative position of dropped shape
         * \param child Pointer to dropped shape
         */
        virtual void OnChildDropped(const wxRealPoint& pos, wxSFShapeBase *child);

    protected:
        // protected data members
        /*! \brief Number of grid rows. */
        int m_nRows;
        /*! \brief Number of grid columns. */
        int m_nCols;
        /*! \brief Space additional space between managed shapes. */
        int m_nCellSpace;
        /*! \brief Array containing the IDs of managed shapes. */
        wxXS::IntArray m_arrCells;

        // protected functions
        /*!
         * \brief Move and resize given shape so it will fit the given bounding rectangle.
         *
         * The shape is aligned inside the given bounding rectangle in accordance to the shape's
         * valign and halign flags.
         * \param shape Pointer to modified shape
         * \param rct Bounding rectangle
         * \sa wxSFShapeBase::SetVAlign, wxSFShapeBase::SetHAlign
         */
        void FitShapeToRect( wxSFShapeBase *shape, const wxRect& rct);

    private:
        // private functions
         /*! \brief Initialize serializable properties. */
        void MarkSerializableDataMembers();
};

#endif // _WXSFGRIDSHAPE_H
