//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : imemcheckprocessor.h
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
 *
 * @brief IMemCheckProcessor - parser interface.
 */

#ifndef _IMEMCHECKPROCESSOR_H_
#define _IMEMCHECKPROCESSOR_H_

#include "memcheckerror.h"

class MemCheckSettings;

/**
 * @brief Interface for any future error processor - parser.
 *
 * Main goal is to fetch error log from extern analyzer tool to internaly used structure - ErrorList.
 * Plugin creates right type of processor acording to settings.
 * At this time internal data storage for error is property of processor.
 */
class IMemCheckProcessor
{
public:
    /**
     * @brief ctor saves reference to settings and inicilizes other properties
     * @param settings reference to global plugin setting, each processor uses what part it needs
     */
    IMemCheckProcessor(MemCheckSettings* const settings)
        : m_settings(settings)
        , m_outputLogFileName(wxEmptyString)
        , m_errorList(){};

    virtual ~IMemCheckProcessor() {}

protected:
    MemCheckSettings* m_settings;
    wxString m_outputLogFileName;
    ErrorList m_errorList;

public:
    /**
     * @brief method "Process" parses external tool output and stores in errorList structure
     * @return reference to errorList
     */
    virtual ErrorList& GetErrors() { return m_errorList; };

    /**
     * @brief If tool uses temp file for error log, this function return its path.
     * @return log file name
     */
    virtual const wxString& GetOutputLogFileName() const { return m_outputLogFileName; }

    /**
     * @brief Reads settings and creates list of actual valid supp files.
     * @return wxArrayString of filenames
     */
    virtual wxArrayString GetSuppressionFiles() = 0;

    /**
     * @brief Combines processor settings and command to be checked and creates new memory tool execution command
     */
    virtual void GetExecutionCommand(const wxString& originalCommand, wxString& command, wxString& command_args) = 0;

    /**
     * @brief Processes data from external tool (log file) to ErrorList.
     */
    virtual bool Process(const wxString& outputLogFileName = wxEmptyString) = 0;
};

#endif //_IMEMCHECKPROCESSOR_H_
