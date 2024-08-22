//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ErdForeignKey.h
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

#ifndef ERDFOREIGNKEY_H
#define ERDFOREIGNKEY_H
#include "NArrow.h"
#include "OneArrow.h"
#include "constraint.h"

#include <wx/wx.h>
#include <wx/wxsf/CircleArrow.h>
#include <wx/wxsf/RoundOrthoShape.h> // Base class: wxSFOrthoLineShape
#include <wx/wxsf/SolidArrow.h>
#include <wx/wxsf/TextShape.h>
#include <wx/wxxmlserializer/XmlSerializer.h>
/*! \brief Class representing line in ERD diagram constructed on Foreign Key base. For more info see wxSF manual */
class ErdForeignKey : public wxSFRoundOrthoLineShape {
public:
	XS_DECLARE_CLONABLE_CLASS(ErdForeignKey);
	ErdForeignKey();
	ErdForeignKey(const ErdForeignKey& obj);
	ErdForeignKey(Constraint* pConstraint);

	virtual ~ErdForeignKey();
	
	virtual void CreateHandles();

protected:
	Constraint* m_pConstraint;
};

#endif // ERDFOREIGNKEY_H
