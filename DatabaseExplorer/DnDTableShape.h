//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : DnDTableShape.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
