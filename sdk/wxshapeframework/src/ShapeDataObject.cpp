/***************************************************************
 * Name:      ShapeDataObject.cpp
 * Purpose:   Implements shape data object class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include <wx/mstream.h>

#include "wx/wxsf/ShapeDataObject.h"

wxSFShapeDataObject::wxSFShapeDataObject(const wxDataFormat& format)
: wxDataObjectSimple(format)
{
	m_Data.SetText(wxT("<?xml version=\"1.0\" encoding=\"utf-8\"?><chart />"));
}

wxSFShapeDataObject::wxSFShapeDataObject(const wxDataFormat& format, const ShapeList& selection, wxSFDiagramManager* manager)
: wxDataObjectSimple(format)
{
	m_Data.SetText(SerializeSelectedShapes(selection, manager));
}

wxSFShapeDataObject::~wxSFShapeDataObject(void)
{
}

wxString wxSFShapeDataObject::SerializeSelectedShapes(const ShapeList& selection, wxSFDiagramManager* manager)
{
	// create root node
	wxSFShapeBase *pShape;
	wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("chart"));

	// serialize copied shapes to XML node
	ShapeList::compatibility_iterator node = selection.GetFirst();
	while(node)
	{
		pShape = node->GetData();
		if(pShape)
		{
			// serialize parent's children
			manager->SerializeObjects(pShape, root, serINCLUDE_PARENTS);
		}
		node = node->GetNext();
	}

	// create XML document in the memory stream
	wxMemoryOutputStream outstream;

	wxXmlDocument xmlDoc;
	xmlDoc.SetRoot(root);
	xmlDoc.Save(outstream);

	char *buffer = new char [outstream.GetSize()];

	if(buffer)
	{
		memset(buffer, 0, outstream.GetSize());

		outstream.CopyTo(buffer, outstream.GetSize()-1);
		wxString output(buffer, wxConvUTF8);
		delete [] buffer;

		return output;
	}
	else
		return wxT("<?xml version=\"1.0\" encoding=\"utf-8\"?><chart />");
}

size_t wxSFShapeDataObject::GetDataSize() const
{
	return m_Data.GetDataSize();
}

bool wxSFShapeDataObject::GetDataHere(void* buf) const
{
	return m_Data.GetDataHere(buf);
}

bool wxSFShapeDataObject::SetData(size_t len, const void* buf)
{
	return m_Data.SetData(len, buf);
}
