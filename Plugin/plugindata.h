//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : plugindata.h
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
#ifndef __plugindata__
#define __plugindata__
#include "serialized_object.h"
#include "codelite_exports.h"
#include "plugin_version.h"

class WXDLLIMPEXP_SDK PluginInfo : public SerializedObject
{

	bool enabled;
	wxString name;
	wxString author;
	wxString description;
	wxString version;

public:
	PluginInfo();
	virtual ~PluginInfo();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);

	//Setters
	void SetAuthor(const wxString& author) {
		this->author = author;
	}
	void SetDescription(const wxString& description) {
		this->description = description;
	}
	void SetEnabled(const bool& enabled) {
		this->enabled = enabled;
	}
	void SetName(const wxString& name) {
		this->name = name;
	}
	void SetVersion(const wxString& version) {
		this->version = version;
	}

	//Getters
	const wxString& GetAuthor() const {
		return author;
	}
	const wxString& GetDescription() const {
		return description;
	}
	const bool& GetEnabled() const {
		return enabled;
	}
	const wxString& GetName() const {
		return name;
	}
	const wxString& GetVersion() const {
		return version;
	}
};

#endif // __plugindata__
