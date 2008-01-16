#ifndef _WX_FNB_DROP_TARGET_H 
#define _WX_FNB_DROP_TARGET_H

#include <wx/wx.h>
#include <wx/dnd.h>

/**
\brief Contains the information about dragged page (page index and container).
*/
class wxFNBDragInfo
{
	wxWindow * m_Container;
	int m_PageIndex;	
public:		
	/**
	Constructor
	\param container - pointer to wxPageContainer object which contains dragged page
	\param pageindex - index of dragged page
	*/
	wxFNBDragInfo(wxWindow * container, int pageindex) : m_Container(container), m_PageIndex(pageindex){}	

	/**
	 * \brief default constructor
	 */
	wxFNBDragInfo() : m_Container(0), m_PageIndex(0){} 

	/**
	Returns wxPageContainer object which contains dragged page
	*/
	wxWindow * GetContainer() {return m_Container;}

	/**
	Returns the index of dragged page
	*/
	int GetPageIndex() {return m_PageIndex;}
};

class wxFNBDragInfoDataObject : public wxDataObjectSimple
{
public:
    wxFNBDragInfoDataObject(const wxDataFormat& format):wxDataObjectSimple(format)
    {}

    wxFNBDragInfo DragInfo;

    wxFNBDragInfo *GetData()
    {
        return &DragInfo;
    }

    // get the size of our data
    virtual size_t GetDataSize() const
    { return sizeof(wxFNBDragInfo); }

    // copy our data to the buffer
    virtual bool GetDataHere(void *buf) const
    {
        memcpy(buf, &DragInfo, sizeof(wxFNBDragInfo));
        return true;
    }

    // copy data from buffer to our data
    virtual bool SetData(size_t WXUNUSED(len), const void *buf)
    {
        // don't check the len. Under Win98 the value of 'len' == 0
        memcpy(&DragInfo, buf, sizeof(wxFNBDragInfo));
        return true;
    }
};

/**
\brief Used for processing drag-n-drop opeartions
*/
template <class T>
class wxFNBDropTarget : public wxDropTarget
{
private:
	typedef wxDragResult (T::*pt2Func)(wxCoord, wxCoord, int, wxWindow *);
	T* m_pParent;
	pt2Func m_pt2CallbackFunc;
	wxFNBDragInfoDataObject * m_DataObject;
public:
	/**
	\brief Constructor
	\param pParent - Object that will handle drag-n-drop operation
	\param pt2CallbackFunc - Pointer to callback method which should be called after dragging the notebook page
	*/
    wxFNBDropTarget(T* pParent, pt2Func pt2CallbackFunc)
		: m_pParent(pParent)
		, m_pt2CallbackFunc(pt2CallbackFunc)
		, m_DataObject(NULL)
	{
		m_DataObject = new wxFNBDragInfoDataObject(wxDataFormat(wxT("wxFNB")));
		SetDataObject(m_DataObject);
	}
	/**
	\brief Virtual Destructor
	*/
	virtual ~wxFNBDropTarget(void) {}
	/**
	\brief Used for processing drop operation
	\param x - X-coordinate
	\param y - Y-coordinate
	\param def - Result of drag-n-drop operation
	*/
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult WXUNUSED(def))
	{		
		GetData();
		wxFNBDragInfo * draginfo = (wxFNBDragInfo *)m_DataObject->GetData();
		if(!draginfo) 
		{
			return wxDragNone;
		}
		return (m_pParent->*m_pt2CallbackFunc)(x, y, draginfo->GetPageIndex(), (T *)draginfo->GetContainer()); 	
	}
};

/**
 * \ingroup wxFlatNotebook
 * This class represents a source for a drag and drop operation
 * We override wxDropSource class to provide user with a feedback
 *
 * \version 1.0
 * first version
 *
 * \date 10-11-2006
 *
 * \author Eran
 */
class wxFNBDropSource : public wxDropSource 
{
	wxWindow* m_win;
public:
	/**
	 * Parameterized constructor
	 * \param win 
	 * \param iconCopy 
	 * \param iconMove 
	 * \param iconNone 
	 */
	wxFNBDropSource(wxWindow* win = NULL)
	: wxDropSource(win)
		, m_win( win )
	{
	}

	/**
	 * Destructor
	 */
	virtual ~wxFNBDropSource()
	{
	}

	/**
	 * give some custom UI feedback during the drag and drop operation in this function. It is called on each mouse move, so your implementation must not be too slow
	 * \param effect The effect to implement. One of wxDragCopy, wxDragMove, wxDragLink and wxDragNone
	 * \return 
	 */
	virtual bool GiveFeedback(wxDragResult effect);
};
#endif
