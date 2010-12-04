//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_dlg.h
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

#ifndef __tags_options_dlg__
#define __tags_options_dlg__

#include "tags_options_base_dlg.h"
#include "serialized_object.h"
#include "wx/filename.h"
#include "tags_options_data.h"

///////////////////////////////////////////////////////////////////////////

class TagsOptionsPageBase
{
public:
	TagsOptionsPageBase(){}
	virtual ~TagsOptionsPageBase(){}
	
	virtual void Save(TagsOptionsData &data) = 0;
	void SetFlag(TagsOptionsData &data, CodeCompletionOpts flag, bool set)
	{
		if (set) {
			data.SetFlags(data.GetFlags() | flag);
		} else {
			data.SetFlags(data.GetFlags() & ~(flag));
		}
	}
	
	void SetColouringFlag(TagsOptionsData &data, CodeCompletionColourOpts flag, bool set)
	{
		if (set) {
			data.SetCcColourFlags(data.GetCcColourFlags() | flag);
		} else {
			data.SetCcColourFlags(data.GetCcColourFlags() & ~(flag));
		}
	}
};

class CCIncludeFilesPage;
class CCAdvancePage;

///////////////////////////////////////////////////////////////////////////////
/// Class TagsOptionsDlg
///////////////////////////////////////////////////////////////////////////////
class TagsOptionsDlg : public TagsOptionsBaseDlg
{
	TagsOptionsData     m_data;
	size_t              m_colour_flags;
	CCIncludeFilesPage *m_includeFilesPage;
	CCAdvancePage *     m_advancedPage;
	
protected:
	void SetFlag(CodeCompletionOpts flag, bool set);
	void SetColouringFlag(CodeCompletionColourOpts flag, bool set);
	void CopyData        ();
	void OnButtonOK      (wxCommandEvent &event);
	
public:
	void Parse();

public:
	TagsOptionsDlg( wxWindow* parent, const TagsOptionsData& data);
	virtual ~TagsOptionsDlg();
	TagsOptionsData &GetData() {
		return m_data;
	}
};

#endif //__tags_options_dlg__
