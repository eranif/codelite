#ifndef DNDTABLESHAPE_H
#define DNDTABLESHAPE_H

#include <wx/wxsf/ShapeBase.h> // Base class: wxSFShapeBase
#include <wx/wxxmlserializer/XmlSerializer.h>
#include <wx/wx.h>
#include "table.h"

/*! \brief Transpor class for DnD functions  */
class dndTableShape : public wxSFShapeBase {

public:
	XS_DECLARE_CLONABLE_CLASS(dndTableShape);
	dndTableShape();
	dndTableShape(xsSerializable* pData);
	dndTableShape(Table* tab);
	virtual ~dndTableShape();
};

#endif // DNDTABLESHAPE_H
