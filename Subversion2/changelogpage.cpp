//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : changelogpage.cpp
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

#include "changelogpage.h"
#include "subversion2.h"
#include "svn_local_properties.h"
#include <wx/tokenzr.h>

ChangeLogPage::ChangeLogPage(wxWindow* parent, Subversion2* plugin)
	: ChangeLogPageBase(parent)
	, m_plugin(plugin)
{
	m_macrosMap[wxT("$(BUGID)")]   = wxT("@@1@@");
	m_macrosMap[wxT("$(FRID)")]    = wxT("@@3@@");

	m_rmacrosMap[wxT("@@1@@")] = wxT("$(BUGID)");
	m_rmacrosMap[wxT("@@3@@")] = wxT("$(FRID)");
}

ChangeLogPage::~ChangeLogPage()
{
}

void ChangeLogPage::AppendText(const wxString& text)
{
	// search the lines for a pattern match
	SubversionLocalProperties props(m_url);
	wxString pattern = props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_MESSAGE);
	wxString urlPat  = props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_URL);
	wxString formattedText = DoFormatLinesToUrl(text, pattern, urlPat);

	pattern = props.ReadProperty(SubversionLocalProperties::FR_TRACKER_MESSAGE);
	urlPat  = props.ReadProperty(SubversionLocalProperties::FR_TRACKER_URL);
	formattedText = DoFormatLinesToUrl(formattedText, pattern, urlPat);

	m_textCtrl->AppendText(formattedText);
}

void ChangeLogPage::OnURL(wxTextUrlEvent& event)
{
	// we respond only for LeftDown event
	if(event.GetMouseEvent().LeftDown()) {
		int start = event.GetURLStart();
		int end   = event.GetURLEnd();

		wxString url = m_textCtrl->GetRange(start, end);
		wxLaunchDefaultBrowser(url);
	}
}

wxString ChangeLogPage::DoFormatLinesToUrl(const wxString& text, const wxString& pattern, const wxString& url)
{
	wxRegEx re;
	DoMakeRegexFromPattern(pattern, re);

	wxString tmpPat = pattern.c_str();
	tmpPat.Trim().Trim(false);

	if(re.IsValid() == false || tmpPat.IsEmpty()) {
		return text;
	}

	wxArrayString lines = wxStringTokenize(text, wxT("\n"), wxTOKEN_STRTOK);
	wxString      out;

	for(size_t i=0; i<lines.size(); i++) {
		wxString line = lines.Item(i).Trim().Trim(false);
		if(re.Matches(line)) {
			wxString bugFrId = re.GetMatch(line, 1);

			// Convert the bugFrId into URLs
			wxArrayString urls = DoMakeBugFrIdToUrl(bugFrId, url);
			if(urls.IsEmpty() == false) {
				for(size_t y=0; y<urls.size(); y++) {
					out << urls.Item(y) << wxT("\n");
				}
			} else {
				out << line << wxT("\n");
			}
		} else {
			out << line << wxT("\n");
		}
	}
	return out;
}

void ChangeLogPage::DoMakeRegexFromPattern(const wxString& pattern, wxRegEx& re)
{
	wxString tmpPat (pattern);
	tmpPat.Trim().Trim(false);

	if(tmpPat.IsEmpty())
		return;

	// replace all macros with unique strings
	for (const auto& [key, value] : m_macrosMap) {
		tmpPat.Replace(key, value);
	}

	// escape all regex key-chars
	tmpPat.Replace(wxT("."), wxT("\\."));
	tmpPat.Replace(wxT("*"), wxT("\\*"));
	tmpPat.Replace(wxT("+"), wxT("\\+"));
	tmpPat.Replace(wxT("?"), wxT("\\?"));
	tmpPat.Replace(wxT("["), wxT("\\["));
	tmpPat.Replace(wxT("]"), wxT("\\]"));
	tmpPat.Replace(wxT("("), wxT("\\("));
	tmpPat.Replace(wxT(")"), wxT("\\)"));
	tmpPat.Replace(wxT("}"), wxT("\\}"));
	tmpPat.Replace(wxT("{"), wxT("\\{"));
	tmpPat.Replace(wxT("$"), wxT("\\$"));
	tmpPat.Replace(wxT("^"), wxT("\\^"));

	// change each macro into regex. We assume that each BUGID/FRID are
	// alphanumeric
	for (const auto& [key, _] : m_rmacrosMap) {
		tmpPat.Replace(key, wxT("([a-zA-Z0-9]*)"));
	}

	re.Compile(tmpPat);
}

wxArrayString ChangeLogPage::DoMakeBugFrIdToUrl(const wxString& bugFrId, const wxString& urlPattern)
{
	wxArrayString outArr;
	wxArrayString ids = wxStringTokenize(bugFrId, wxT(","), wxTOKEN_STRTOK);
	for(size_t i=0; i<ids.size(); i++) {
		wxString id = ids.Item(i).Trim().Trim(false);

		wxString url = urlPattern;
		url.Replace(wxT("$(BUGID)"), id);
		url.Replace(wxT("$(FRID)"),  id);

		outArr.Add(url);
	}
	return outArr;
}
