//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : iconfigtool.h
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

#ifndef ICONFIGTOOL_H
#define ICONFIGTOOL_H

#include "serialized_object.h"
#include "wx/string.h"

//------------------------------------------------------------------
// The configuration tool interface
//------------------------------------------------------------------
/**
 * @class IConfigTool
 * @author Eran
 * @date 05/07/08
 * @file iconfigtool.h
 * @brief a configuration tool that allow plugin easy access to CodeLite's configuration file. Plugin should only
 * read/write its own values
 * @sa SerializedObject
 */
class IConfigTool
{
public:
    IConfigTool() {}
    virtual ~IConfigTool() {}

    /**
     * @brief read an object from the configuration file and place it into obj
     * @param name the name of the object that is stored in the configuration file
     * @param obj a pointer previsouly allocated SerializedObject
     * @return true if the object exist and was read successfully from the configuration file
     */
    virtual bool ReadObject(const wxString& name, SerializedObject* obj) = 0;

    /**
     * @brief write an object to the configuration file
     * @param name the name that identifies the object
     * @param obj object to store.
     * @return true on success, false otherwise
     */
    virtual bool WriteObject(const wxString& name, SerializedObject* obj) = 0;
};

#endif // ICONFIGTOOL_H
