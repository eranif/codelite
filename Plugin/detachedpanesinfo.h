//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : detachedpanesinfo.h              
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

#ifndef __detachedpanesinfo__
#define __detachedpanesinfo__

#include "serialized_object.h"
#include <wx/arrstr.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK DetachedPanesInfo : public SerializedObject
{
protected:
	wxArrayString m_panes;
public:
	DetachedPanesInfo(wxArrayString arr);
	DetachedPanesInfo(){}
	virtual ~DetachedPanesInfo();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);


    //Getters
	const wxArrayString& GetPanes() const {
		return m_panes;
	}

	//Setters
	void SetPanes(const wxArrayString& panes) {
		this->m_panes = panes;
	}

};

class WXDLLIMPEXP_SDK WorkspaceViewTabOrder
                        : public DetachedPanesInfo // Though they're not really connected, this allows a bit of code-reuse
{
public:
	WorkspaceViewTabOrder(wxArrayString arr) : DetachedPanesInfo(arr), m_SelectedTab(-1) {}
	WorkspaceViewTabOrder(){}
	virtual ~WorkspaceViewTabOrder() {}

	virtual void DeSerialize(Archive &arch) {
        arch.Read(wxT("TabOrder"), m_panes);
        arch.Read(wxT("SelectedTab"), m_SelectedTab);
    }
	virtual void Serialize(Archive &arch) {
        arch.Write(wxT("TabOrder"), m_panes);
        arch.Write(wxT("SelectedTab"), m_SelectedTab);
    }
    //Getter
	int GetLastSelectedTab() const {
		return m_SelectedTab;
	}
	//Setter
	void SetLastSelectedTab(int index) {
		m_SelectedTab = index;
	}

protected:
    int m_SelectedTab;
};
#endif // __detachedpanesinfo__
