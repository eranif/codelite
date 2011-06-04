#include "DnDTableShape.h"
XS_IMPLEMENT_CLONABLE_CLASS(dndTableShape,wxSFShapeBase);

dndTableShape::dndTableShape(Table* tab):wxSFShapeBase()
{
	SetUserData(tab);
}

dndTableShape::~dndTableShape()
{
}


dndTableShape::dndTableShape()
{
}

dndTableShape::dndTableShape(xsSerializable* pData):wxSFShapeBase()
{
	SetUserData(pData);
}

