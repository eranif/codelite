//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPLookupTable.h
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

#ifndef PHPLOOKUPTABLE_H
#define PHPLOOKUPTABLE_H

#include "PHPEntityBase.h"
#include "PHPSourceFile.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "smart_ptr.h"
#include "wx/wxsqlite3.h"
#include <set>
#include <unordered_set>
#include <vector>
#include <wx/longlong.h>
#include <wx/stopwatch.h>
#include <wx/string.h>
#include <wxStringHash.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPHP_PARSE_STARTED, clParseEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPHP_PARSE_ENDED, clParseEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPHP_PARSE_PROGRESS, clParseEvent);

enum ePhpScopeType {
    kPhpScopeTypeAny = -1,
    kPhpScopeTypeNamespace = 0,
    kPhpScopeTypeClass = 1,
};

/**
 * @class PHPLookupTable
 * @brief a database lookup class which allows a quick access
 * to all entities parsed in the workspace
 */
class WXDLLIMPEXP_CL PHPLookupTable
{
    wxSQLite3Database m_db;
    wxFileName m_filename;
    size_t m_sizeLimit;
    std::unordered_set<wxString> m_allClasses;

public:
    enum eLookupFlags {
        kLookupFlags_None = 0,
        kLookupFlags_ExactMatch = (1 << 1),
        kLookupFlags_Contains = (1 << 2),
        kLookupFlags_StartsWith = (1 << 3),
        kLookupFlags_Members = (1 << 4),         // Class members
        kLookupFlags_Constants = (1 << 5),       // 'const'
        kLookupFlags_Static = (1 << 6),          // include static members/functions (static::, class_type::)
        kLookupFlags_Self = (1 << 7),            // self::
        kLookupFlags_NameHintIsScope = (1 << 8), // the namehint provided is of a class name. When enabled, the search
                                                 // will try to take "\" into consideration
        kLookupFlags_Parent = (1 << 9),          // Exclude 'this' from the results and return only
                                                 // its parents parent::
        kLookupFlags_FunctionsAndConstsOnly = (1 << 10), // Fetch functions and consts ONLY
        kLookupFlags_IncludeAbstractMethods = (1 << 11), // Include abstract functions in the result set
    };

    enum eUpdateMode {
        kUpdateMode_Fast,
        kUpdateMode_Full,
    };
    typedef SmartPtr<PHPLookupTable> Ptr_t;
    static void DoSplitFullname(const wxString& fullname, wxString& ns, wxString& shortName);

private:
    void EnsureIntegrity(const wxFileName& filename);
    void DoAddNameFilter(wxString& sql, const wxString& nameHint, size_t flags);

    void CreateSchema();
    PHPEntityBase::Ptr_t DoFindMemberOf(wxLongLong parentDbId, const wxString& exactName,
                                        bool parentIsNamespace = false);

    void DoFixVarsDocComment(PHPEntityBase::List_t& matches, wxLongLong parentId);
    void DoGetInheritanceParentIDs(PHPEntityBase::Ptr_t cls, std::vector<wxLongLong>& parents,
                                   std::set<wxLongLong>& parentsVisited, bool excludeSelf);

    /**
     * @brief find namespace by fullname. If it does not exist, add it and return a pointer to it
     */
    PHPEntityBase::Ptr_t CreateNamespaceForDefine(PHPEntityBase::Ptr_t define);

    PHPEntityBase::Ptr_t DoFindScope(const wxString& fullname, ePhpScopeType scopeType = kPhpScopeTypeAny);
    PHPEntityBase::Ptr_t DoFindScope(wxLongLong id, ePhpScopeType scopeType = kPhpScopeTypeAny);
    /**
     * @brief sqlite uses '_' as a wildcard. escape it using '^'
     * as our escape char
     */
    wxString EscapeWildCards(const wxString& str);

    void DoAddLimit(wxString& sql);

    /**
     * @brief allocate new entity that matches the table name
     */
    PHPEntityBase::Ptr_t NewEntity(const wxString& tableName, ePhpScopeType scopeType = kPhpScopeTypeAny);

    /**
     * @brief load entities from table and name hint
     * if nameHint is empty, return empty list
     */
    void LoadFromTableByNameHint(PHPEntityBase::List_t& matches, const wxString& tableName, const wxString& nameHint,
                                 eLookupFlags flags);

    /**
     * @brief use typed: static::
     */
    bool CollectingStatics(size_t flags) const { return (flags & kLookupFlags_Static) || (flags & kLookupFlags_Self); }

    /**
     * @brief return children of parentId _WITHOUT_ taking inheritance into consideration
     */
    void DoFindChildren(PHPEntityBase::List_t& matches, wxLongLong parentId, size_t flags = kLookupFlags_None,
                        const wxString& nameHint = "");

    /**
     * @brief return the timestamp of the last parse for 'filename'
     */
    wxLongLong GetFileLastParsedTimestamp(const wxFileName& filename);

    /**
     * @brief update the file's last updated timestamp
     */
    void UpdateFileLastParsedTimestamp(const wxFileName& filename);

    /**
     * @brief check the database disk image to see if it corrupted
     */
    bool CheckDiskImage(wxSQLite3Database& db, const wxFileName& filename);

public:
    PHPLookupTable();
    virtual ~PHPLookupTable();

    /**
     * @brief rebuild the class cache
     */
    void RebuildClassCache();
    /**
     * @brief return the function closest to a given function and line number
     */
    PHPEntityBase::Ptr_t FindFunctionNearLine(const wxFileName& filename, int lineNumber);

    /**
     * @brief add class name to the class cache
     * @param classname
     */
    void UpdateClassCache(const wxString& classname);

    /**
     * @brief check if a class exists in the cache
     */
    bool ClassExists(const wxString& classname) const;

    void SetSizeLimit(size_t sizeLimit) { this->m_sizeLimit = sizeLimit; }
    /**
     * @brief return the entity at a given file/line
     */
    PHPEntityBase::Ptr_t FindFunctionByLineAndFile(const wxFileName& filename, int line);

    /**
     * @brief return list of functiosn from a given file
     */
    size_t FindFunctionsByFile(const wxFileName& filename, PHPEntityBase::List_t& functions);

    /**
     * @brief open the lookup table database
     */
    void Open(const wxString& workspacePath);

    /**
     * @brief open the symbols database
     */
    void Open(const wxFileName& dbfile);

    /**
     * @brief
     * @return
     */
    bool IsOpened() const;

    /**
     * @brief close the lookup table database
     */
    void Close();

    /**
     * @brief delete the symbols database file from the file system and recreate an empty one
     */
    void ResetDatabase();

    /**
     * @brief clear all cached data from the database
     */
    void ClearAll(bool autoCommit = true);

    /**
     * @brief find a scope symbol (class or namespace) by its fullname
     */
    PHPEntityBase::Ptr_t FindScope(const wxString& fullname);

    /**
     * @brief find a function by its fullname (scope+name)
     */
    PHPEntityBase::Ptr_t FindFunction(const wxString& fullname);

    /**
     * @brief find a class with a given name
     */
    PHPEntityBase::Ptr_t FindClass(const wxString& fullname);

    /**
     * @brief find a class with a given database ID
     */
    PHPEntityBase::Ptr_t FindClass(wxLongLong id);

    /**
     * @brief find a member of parentDbId with name that matches 'exactName'
     */
    PHPEntityBase::Ptr_t FindMemberOf(wxLongLong parentDbId, const wxString& exactName, size_t flags = 0);

    /**
     * @brief find children of a scope by its database ID.
     */
    PHPEntityBase::List_t FindChildren(wxLongLong parentId, size_t flags = kLookupFlags_None,
                                       const wxString& nameHint = "");

    /**
     * @brief find list of symbols with a given name (regardless of the type / scope)
     * a "free style" search
     */
    PHPEntityBase::List_t FindSymbol(const wxString& name);

    /**
     * @brief load the global functions and consts that matches nameHint
     * If nameHint is empty, return an empty list
     */
    PHPEntityBase::List_t FindGlobalFunctionAndConsts(size_t flags = PHPLookupTable::kLookupFlags_Contains,
                                                      const wxString& nameHint = "");

    /**
     * @brief load list of namespaces from the database that
     */
    PHPEntityBase::List_t FindNamespaces(const wxString& fullnameStartsWith, const wxString& shortNameContains = "");
    /**
     * @brief load list of entities from all the tables that matches 'nameHint'
     * if nameHint is empty, return an empty list
     */
    void LoadAllByFilter(PHPEntityBase::List_t& matches, const wxString& nameHint,
                         eLookupFlags flags = kLookupFlags_Contains);
    /**
     * @brief save source file into the database
     */
    void UpdateSourceFile(PHPSourceFile& source, bool autoCommit = true);

    /**
     * @brief update list of source files
     */
    template <typename GoindDownFunc>
    void RecreateSymbolsDatabase(const wxArrayString& files, eUpdateMode updateMode, GoindDownFunc pFuncGoingDown,
                                 bool parseFuncBodies = true);

    /**
     * @brief delete all entries belonged to filename.
     * @param filename the file name
     * @param autoCommit when true, issue a begin/commit transcation commands
     */
    void DeleteFileEntries(const wxFileName& filename, bool autoCommit = true);

    /**
     * @brief load function arguments from the database
     * @param parentId the function database ID
     */
    PHPEntityBase::List_t LoadFunctionArguments(wxLongLong parentId);

    /**
     * @brief return reference to the underlying database
     */
    wxSQLite3Database& Database() { return m_db; }
};

template <typename GoindDownFunc>
void PHPLookupTable::RecreateSymbolsDatabase(const wxArrayString& files, eUpdateMode updateMode,
                                             GoindDownFunc pFuncGoingDown, bool parseFuncBodies)
{
    try {

        {
            clParseEvent event(wxPHP_PARSE_STARTED);
            event.SetTotalFiles(files.GetCount());
            event.SetCurfileIndex(0);
            EventNotifier::Get()->AddPendingEvent(event);
        }

        wxStopWatch sw;
        sw.Start();

        m_allClasses.clear(); // clear the cache
        m_db.Begin();
        for(size_t i = 0; i < files.GetCount(); ++i) {
            if(pFuncGoingDown()) { break; }
            {
                clParseEvent event(wxPHP_PARSE_PROGRESS);
                event.SetTotalFiles(files.GetCount());
                event.SetCurfileIndex(i);
                event.SetFileName(files.Item(i));
                EventNotifier::Get()->AddPendingEvent(event);
            }

            wxFileName fnFile(files.Item(i));
            bool reParseNeeded(true);

            if(updateMode == kUpdateMode_Fast) {
                // Check to see if we need to re-parse this file
                // and store it to the database

                if(!fnFile.Exists()) {
                    reParseNeeded = false;
                } else {
                    time_t lastModifiedOnDisk = fnFile.GetModificationTime().GetTicks();
                    wxLongLong lastModifiedInDB = GetFileLastParsedTimestamp(fnFile);
                    if(lastModifiedOnDisk <= lastModifiedInDB.ToLong()) { reParseNeeded = false; }
                }
            }

            // Ensure that the file exists
            if(!fnFile.Exists()) { reParseNeeded = false; }

            // Parse only valid PHP files
            if(FileExtManager::GetType(fnFile.GetFullName()) != FileExtManager::TypePhp) { reParseNeeded = false; }

            if(reParseNeeded) {
                // For performance reaons, load the file into memory and then parse it
                wxFileName fnSourceFile(files.Item(i));
                wxString content;
                if(!FileUtils::ReadFileContent(fnSourceFile, content, wxConvISO8859_1)) {
                    clWARNING() << "PHP: Failed to read file:" << fnSourceFile << "for parsing" << clEndl;
                    continue;
                }
                PHPSourceFile sourceFile(content, this);
                sourceFile.SetFilename(fnSourceFile);
                sourceFile.SetParseFunctionBody(parseFuncBodies);
                sourceFile.Parse();
                UpdateSourceFile(sourceFile, false);
            }
        }
        m_db.Commit();
        long elapsedMs = sw.Time();
        clDEBUG1() << _("PHP: parsed ") << files.GetCount() << " in " << elapsedMs << " milliseconds" << clEndl;

        {
            clParseEvent event(wxPHP_PARSE_ENDED);
            event.SetTotalFiles(files.GetCount());
            event.SetCurfileIndex(files.GetCount());
            EventNotifier::Get()->AddPendingEvent(event);
        }

    } catch(wxSQLite3Exception& e) {
        try {
            m_db.Rollback();

        } catch(...) {
        }

        {
            // always make sure that the end event is sent
            clParseEvent event(wxPHP_PARSE_ENDED);
            event.SetTotalFiles(files.GetCount());
            event.SetCurfileIndex(files.GetCount());
            EventNotifier::Get()->AddPendingEvent(event);
        }

        clWARNING() << "PHPLookupTable::UpdateSourceFiles:" << e.GetMessage() << clEndl;
    }
}

#endif // PHPLOOKUPTABLE_H
