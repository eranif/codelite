/***************************************************************
 * Name:      FlexGridShape.h
 * Purpose:   Defines flexible grid shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2008-09-27
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFFLEXGRIDSHAPE_H
#define _WXSFFLEXGRIDSHAPE_H

#include <wx/wxsf/GridShape.h>

// default values

WX_DEFINE_ARRAY(wxSFShapeBase*, ShapePtrArray);

/*!
 * \brief Class encapsulates a rectangular shape derived from wxSFGridShape class which acts as a flexible grid-based
 * container able to manage other assigned child shapes (it can control their position). The managed
 * shapes are aligned into defined grid with a behaviour similar to classic wxWidget's wxFlexGridSizer class.
 */
class WXDLLIMPEXP_SF wxSFFlexGridShape : public wxSFGridShape
{
    public:
        XS_DECLARE_CLONABLE_CLASS(wxSFFlexGridShape);

        /*! \brief Default constructor. */
        wxSFFlexGridShape();
        /*!
         * \brief User constructor.
         * \param pos Initial position
         * \param size Initial size
         * \param rows Number of grid rows
         * \param cols Number of grid columns
         * \param cellspace Additional space between managed shapes
         * \param manager Pointer to parent diagram manager
         */
        wxSFFlexGridShape(const wxRealPoint& pos, const wxRealPoint& size, int rows, int cols, int cellspace, wxSFDiagramManager* manager);
        /*!
        * \brief Copy constructor.
        * \param obj Reference to the source object
        */
        wxSFFlexGridShape(const wxSFFlexGridShape& obj);
        /*! \brief Destructor. */
        virtual ~wxSFFlexGridShape();

        // public virtual functions

        /*! \brief Do layout of assigned child shapes */
        virtual void DoChildrenLayout();

    private:

        // private data members
        wxXS::IntArray m_arrRowSizes;
        wxXS::IntArray m_arrColSizes;

        ShapePtrArray m_arrChildShapes;
};

#endif // _WXSFFLEXGRIDSHAPE_H
