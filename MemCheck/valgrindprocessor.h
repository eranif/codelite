//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : valgrindprocessor.h
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

/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#ifndef _VALGRINDPROCESSOR_H_
#define _VALGRINDPROCESSOR_H_

#include "imemcheckprocessor.h"
#include <wx/xml/xml.h>

/**
 * @class ValgrindMemcheckProcessor
 * @brief Implementation of valgrind's memcheck tool parser
 *
 * Settings for this parset is implemented in global settings. It could be moved here or to own file.
 */
class ValgrindMemcheckProcessor : public IMemCheckProcessor
{
public:
    /**
     * @brief interface implementation, does nothing more than inherited ctor
     * @param settings
     */
    ValgrindMemcheckProcessor(MemCheckSettings* const settings);

    /**
     * @brief interface implementation
     * @return list of supp files
     *
     * Some files are specified in setting. Workspace specific supp file need to be named just before analyse run,
     * according to current opened workspace.
     */
    virtual wxArrayString GetSuppressionFiles();

    /**
     * @brief interface implementation
     * @param originalCommand
     * @return
     *
     * Takes original command and prepend it with Valgrind command and its arguments.
     */
    virtual void GetExecutionCommand(const wxString& originalCommand, wxString& command, wxString& command_args);

    /**
     * @brief interface implementation
     * @param outputLogFileName
     * @return
     *
     * Loads Valgrind's xml log to wxXmlDocument, and goes trought nodes
     */
    virtual bool Process(const wxString& outputLogFileName = wxEmptyString);

protected:
    /**
     * @brief creates one MemCheckError object
     * @param doc whole log document
     * @param errorNode reference to current processed node in that doc file
     * @return MemCheckError object
     *
     * Auxiliary section is not in subnode. First part of the node describes particular error, second part describes
     * auxiliary info. For auxiliary is created sub MemCheckError object.
     */
    MemCheckError ProcessError(wxXmlDocument& doc, wxXmlNode* errorNode);

    /**
     * @brief creates one MemCheckErrorLocation object
     * @param doc whole log document
     * @param locationNode reference to current processed node in that doc file
     * @return MemCheckErrorLocation object
     */
    MemCheckErrorLocation ProcessLocation(wxXmlDocument& doc, wxXmlNode* locationNode);
};

#endif // _VALGRINDPROCESSOR_H_
