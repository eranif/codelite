/***************************************************************
 * Name:      ShapeBaseXml.cpp
 * Purpose:   Implements basic shape's serialization capability
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

#include "wx/wxsf/ShapeBase.h"
#include "wx/wxsf/CommonFcn.h"

//----------------------------------------------------------------------------------//
// Serialization
//----------------------------------------------------------------------------------//

wxXmlNode* wxSFShapeBase::Serialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...

	node = xsSerializable::Serialize(node);

	return node;
}

void wxSFShapeBase::Deserialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...

	xsSerializable::Deserialize(node);

	if( m_pUserData )
	{
	    m_pUserData->SetParent(this);
	}
	
	// update fixed connection points
	for( ConnectionPointList::iterator it = m_lstConnectionPts.begin(); it != m_lstConnectionPts.end(); ++it )
	{
		wxSFConnectionPoint *pCp = (wxSFConnectionPoint*) *it;
		pCp->SetParentShape( this );
	}
}
