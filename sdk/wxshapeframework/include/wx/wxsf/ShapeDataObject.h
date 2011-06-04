/***************************************************************
 * Name:      ShapeDataObject.h
 * Purpose:   Defines shape data object class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFSHAPEDATAOBJECT_H
#define _WXSFSHAPEDATAOBJECT_H

#include <wx/wxsf/DiagramManager.h>

/*!
 * \brief Class encapsulating data object used during clipboard operations with shapes.
 */
class wxSFShapeDataObject : public wxDataObjectSimple
{
public:
    /*!
     * \brief Default constructor
     * \param format Data format
     */
	wxSFShapeDataObject(const wxDataFormat& format);
    /*!
     * \brief User constructor
     * \param format Data format
     * \param selection List of shapes which should be stored in the data object
     * \param manager Pointer to diagram manager which manages stored shapes
     */
	wxSFShapeDataObject(const wxDataFormat& format, const ShapeList& selection, wxSFDiagramManager* manager);
	/*! \brief Destructor */
	virtual ~wxSFShapeDataObject(void);

    /*! \brief Function returns sizes of the data object */
	virtual size_t GetDataSize() const;
	/*!
	 * \brief Function should export data from data object to given buffer.
	 * \param buf External output data buffer
	 * \return TRUE on success, otherwise FALSE
	 */
	virtual bool GetDataHere(void* buf) const;
	/*!
	 * \brief Function should inport data from data object from given buffer.
	 * \param len Data lenght
	 * \param buf External input data buffer
	 * \return TRUE on success, otherwise FALSE
	 */
	virtual bool SetData(size_t len, const void* buf);

	wxTextDataObject m_Data;

protected:

    /*!
     * \brief Serialize shapes to data object.
     * \param selection List of shapes which should be serialized
     * \param manager Parent diagram manager
     * \return String containing serialized information
     */
	wxString SerializeSelectedShapes(const ShapeList& selection, wxSFDiagramManager* manager);

};

#endif //_WXSFSHAPEDATAOBJECT_H
