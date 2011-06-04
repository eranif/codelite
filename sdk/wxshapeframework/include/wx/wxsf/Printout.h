/***************************************************************
 * Name:      Printout.h
 * Purpose:   Defines printout class for shape canvas
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2008-05-06
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFPRINTOUT_H
#define _WXSFPRINTOUT_H

#include <wx/print.h>
#include <wx/printdlg.h>

#include <wx/wxsf/Defs.h>

class WXDLLIMPEXP_SF wxSFShapeCanvas;

/*!
 * \brief Auxiliary printout class providing all necessary functions needed for canvas printing.
 * This class is used internally by the wxSFShapeCanvas class. It can be also used as a base class for other modified
 * printout classes.
 */
class WXDLLIMPEXP_SF wxSFPrintout : public wxPrintout
{
public:
    /*! \brief Default constructor */
    wxSFPrintout(const wxString& title, wxSFShapeCanvas *canvas);
    /*! \brief Default destructor */
    virtual ~wxSFPrintout();

    // public member data accessors
    /*!
     * \brief Set shape canvas which content should be printed.
     * \param canvas Pointer to shape canvas
     */
    void SetPrintedCanvas( wxSFShapeCanvas *canvas ){m_pCanvas = canvas;}

    // public virtual functions

    /*! \brief Called by printing framework. Functions TRUE if a page of given index already exists in printed document.
     * This function can be overrided if necessary. */
    virtual bool HasPage(int page);
    /*! \brief Called by printing framework. Initialize print job. This function can be overrided if necessary. */
    virtual bool OnBeginDocument(int startPage, int endPage);
    /*! \brief Called by printing framework. Deinitialize the print job. This function can be overrided if necessary. */
    virtual void OnEndDocument();
    /*! \brief Called by printing framework. It does the print job. This function can be overrided if necessary. */
    virtual bool OnPrintPage(int page);
    /*! \brief Called by printing framework. Supply information about printed pages. This function can be overrided if necessary. */
    virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

protected:
    // protected data members

    /*! \brief Pointer to parent shape canvas. */
    wxSFShapeCanvas *m_pCanvas;
};

#endif // _WXSFPRINTOUT_H
