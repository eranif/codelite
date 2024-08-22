//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : memcheckerror.h
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
 * ErrorList implements of list of parsed errors. Basically Error have label and trace log - list of Locations.
 * LocationList represents stack trace.
 */

#ifndef _MEMCHECKERROR_H_
#define _MEMCHECKERROR_H_

#include <wx/wx.h>
#include <wx/tokenzr.h>

#include <list>

#include "memcheckdefs.h"

class MemCheckErrorLocation;
class MemCheckError;

typedef std::list<MemCheckErrorLocation> LocationList;
typedef std::list<MemCheckError> ErrorList;
typedef MemCheckError* MemCheckErrorPtr;


/**
 * @class MemCheckErrorReferrer
 * @brief wrapper pointer to MemCheckError
 * 
 * wxDVC needs reference to MemCheckError, the only way to achieve this is to set its wxClientData. But the problem is that whenever wxDVC is cleared it is made by wxCrafters wxDVCModel and this model frees all the client data. With this hack model clears only this wrapper class and leavs MemCheckErrorList intact.
 */
class MemCheckErrorReferrer: public wxClientData
{
    MemCheckError & m_error;
public:
    MemCheckErrorReferrer(MemCheckError & error) : wxClientData(), m_error(error) {};
    MemCheckError & Get() {
        return m_error;
    };
};

/**
 * @class MemCheckErrorLocationReferrer
 * @brief wrapper pointer to MemCheckErrorLocation
 * 
 * wxDVC needs reference to MemCheckErrorLocation, the only way to achieve this is to set its wxClientData. But the problem is that whenever wxDVC is cleared it is made by wxCrafters wxDVCModel and this model frees all the client data. With this hack model clears only this wrapper class and leavs MemCheckErrorList intact.
 */
class MemCheckErrorLocationReferrer: public wxClientData
{
    MemCheckErrorLocation & m_location;
public:
    MemCheckErrorLocationReferrer(MemCheckErrorLocation & location) : m_location(location) {};
    MemCheckErrorLocation & Get() {
        return m_location;
    };
};


/**
 * @class MemCheckErrorLocation
 * @brief Represents on record from error stacktrace.
 */
class MemCheckErrorLocation
{
public:
    bool operator==(const MemCheckErrorLocation & other) const;
    bool operator!=(const MemCheckErrorLocation & other) const;
    
    /**
     * @brief Returns all attributed concatenated to tab separated string.
     * @return string
     * 
     * this function is used in searching function
     */
    const wxString toString() const;
    
    /**
     * @brief Is used in tooltip.
     * @param workspacePath
     * @return string
     */
    const wxString toText(const wxString & workspacePath = wxEmptyString) const;
    
    /**
     * @brief If file is in workspace path, that path is trimmed
     * @param workspacePath
     * @return file name
     */
    const wxString getFile(const wxString & workspacePath = wxEmptyString) const;
    
    /**
     * @brief If object file is in workspace path, that path is trimmed
     * @param workspacePath
     * @return obj file name
     */
    const wxString getObj(const wxString & workspacePath = wxEmptyString) const;

    /**
     * @brief test if is NOT in workspace
     * @return true if location points to file not in workspace scope
     */
    const bool isOutOfWorkspace(const wxString & workspacePath) const;

    wxString func;
    wxString file;
    int line;
    wxString obj;
};


/**
 * @class MemCheckError
 * @brief Represents one error with label, stack trace (location list), and some additional record.
 *
 * Additional records have also stacktrace, so they are implemented same as errors. Auxiliary record implemented as list of error. Some tool have more than one auxiliary section. Type is used to distinguish between them.
 */
class MemCheckError
{
public:
    enum Type { TYPE_ERROR, TYPE_AUXILIARY };
    MemCheckError();
    
    
    /**
     * @brief Returns all attributed and attributes of all locations and all atributes from nested errors concatenated to tab separated string.
     * @return string
     *
     * TODO: It cloud be buffered to improve speed, but it would cost more memory.
     */
    const wxString toString() const;
    
    /**
     * @brief Is used in tooltip.
     * @param indent number of spaces
     * @return string
     */
    const wxString toText(unsigned int indent = 1) const;
    
    /**
     * @brief creates uniq name for suppresion
     * @return suppression rule as string
     *
     * FIXME This method must be moved to Valgrind procesor, it is Valgrind specfic.
     */
    const wxString getSuppression();
    
    
    /**
     * @brief Test if error has file on specified path.
     * @param path
     * @return True if at least one file is in path, otherwise false.
     */
    const bool hasPath(const wxString & path) const;

    Type type;
    bool suppressed;
    wxString label;
    wxString suppression;
    LocationList locations;
    ErrorList nestedErrors;
};


/**
 * @brief flags to use with MemCheckIterTools
 */
enum {
    MC_IT_OMIT_NONWORKSPACE = 1 << 1,
    MC_IT_OMIT_DUPLICATIONS = 1 << 2,
    MC_IT_OMIT_SUPPRESSED = 1 << 3,
};


/**
 * @class MemCheckIterTools
 * @brief Iterator for iterating trought ErrorList and LocationList.
 *
 * There are three options in setting
 *     + omitNonWorkspace: Hide all other location than the ones with path from current workspace. In other words, shown are only locations from current workspace.
 *     + omitDuplications: Some errors cause that Valgrind prints almost identical error more than one time. So this reduces succeed errors which looks same.
 *     + omitSuppressed:   If error is suppressed (in supp notebook) it disappears from list. It is the same efect as creating supp and rerunning test.
 *
 * This class holds context of these setting for whole ErrorList and ErrorLocationList.
 * In python for example it would be question of ene predicate passed as argument to iterator function, in C I had to do it by hand :(
 * The reason id I need iterator that can hold some atritubes and pass them to subiterators.
 *
 * This functionality could be part of ErrorList
 */
class MemCheckIterTools
{
    struct IterTool {
        bool omitNonWorkspace;
        bool omitDuplications;
        bool omitSuppressed;
        wxString workspacePath;
        bool isEqual(MemCheckError & lhs, MemCheckError & rhs) const;
    } m_iterTool ;

public:
    class LocationListIterator : public std::iterator<std::input_iterator_tag, MemCheckErrorLocation>
    {
        LocationList::iterator p;
        LocationList::iterator m_end;
        IterTool m_iterTool;
    public:
        LocationListIterator(LocationList & l, const IterTool &iterTool);
        ~LocationListIterator();
        LocationList::iterator& operator++();
        LocationList::iterator operator++(int);
        bool operator==(const LocationList::iterator& rhs);
        bool operator!=(const LocationList::iterator& rhs);
        MemCheckErrorLocation & operator*();
    };


    class ErrorListIterator : public std::iterator<std::input_iterator_tag, MemCheckError>
    {
        ErrorList::iterator p;
        ErrorList::iterator m_end;
        IterTool m_iterTool;
    public:
        ErrorListIterator(ErrorList & l, const IterTool & iterTool);
        ~ErrorListIterator();
        ErrorList::iterator& operator++();
        ErrorList::iterator operator++(int);
        bool operator==(const ErrorList::iterator& rhs);
        bool operator!=(const ErrorList::iterator& rhs);
        MemCheckError & operator*();
    };

protected:
    /**
     * @brief ctor creates object which hold shared attributes and provides all functionality for iterration over ErrorLists and LocationLists
     * @param workspacePath
     * @param flags
     */
    MemCheckIterTools(const wxString & workspacePath, unsigned int flags);

    ErrorListIterator GetIterator(ErrorList & l);
    LocationListIterator GetIterator(LocationList & l);

public:
    /**
     * @brief Creates iterator with holds settings and does iteration.
     * @param l list to iterate over
     * @param workspacePath
     * @param flags MC_IT_OMIT_NONWORKSPACE | MC_IT_OMIT_DUPLICATIONS | MC_IT_OMIT_SUPPRESSED
     * @return iterator over ErrorList
     *
     * This method calls MemCheckIterTools constructor and then GetIterator method.
     */
    static ErrorListIterator Factory(ErrorList & l, const wxString & workspacePath, unsigned int flags);
    
    /**
     * @brief Creates iterator with holds settings and does iteration.
     * @param l list to iterate over
     * @param workspacePath
     * @param flags MC_IT_OMIT_NONWORKSPACE | MC_IT_OMIT_DUPLICATIONS | MC_IT_OMIT_SUPPRESSED
     * @return iterator over LocationList
     *
     * This method calls MemCheckIterTools constructor and then GetIterator method.
     */
    static LocationListIterator Factory(LocationList & l, const wxString & workspacePath, unsigned int flags);
};

#endif //_MEMCHECKERROR_H_
