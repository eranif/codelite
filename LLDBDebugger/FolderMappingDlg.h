//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : FolderMappingDlg.h
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

#ifndef FOLDERMAPPINGDLG_H
#define FOLDERMAPPINGDLG_H
#include "UI.h"
#include "LLDBProtocol/LLDBPivot.h"

class FolderMappingDlg : public FolderMappingBaseDlg
{
public:
    FolderMappingDlg(wxWindow* parent);
    virtual ~FolderMappingDlg();
    LLDBPivot GetPivot() const;
    
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    
};
#endif // FOLDERMAPPINGDLG_H
