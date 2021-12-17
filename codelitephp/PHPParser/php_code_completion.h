//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_code_completion.h
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

#ifndef PHPCODECOMPLETION_H
#define PHPCODECOMPLETION_H

#include "PHPEntityBase.h"
#include "PHPExpression.h"
#include "PHPLookupTable.h"
#include "ServiceProvider.h"
#include "cl_command_event.h"
#include "ieditor.h"
#include "php_event.h"
#include "precompiled_header.h"
#include "wxStringHash.h"
#include <cc_box_tip_window.h>
#include <cl_command_event.h>
#include <smart_ptr.h>
#include <vector>

struct PHPLocation {
    wxString what;     // Token name
    wxString filename; // file name (absolute path)
    int linenumber;    // line number within filename
    PHPEntityBase::Ptr_t entity;
    typedef SmartPtr<PHPLocation> Ptr_t;
};

class IManager;
class ResourceItem;
class PHPCodeCompletion : public ServiceProvider
{
public:
    enum {
        kCC_FUNCTIONS_ONLY = 0x00000001,
        kCC_SELF_FUNCTIONS = 0x00000002, // self::
        kCC_SELF_MEMBERS = 0x00000004,   // self::
        kCC_STATIC = 0x00000008,         // static::
        kCC_SELF = kCC_SELF_FUNCTIONS | kCC_SELF_MEMBERS,
    };

    static PHPCodeCompletion* Instance();
    static void Release();

protected:
    static PHPCodeCompletion* m_instance;
    IManager* m_manager;
    CCBoxTipWindow* m_typeInfoTooltip;
    PHPLookupTable m_lookupTable;
    std::unordered_map<wxString, PHPEntityBase::Ptr_t> m_currentNavBarFunctions;

    static bool CanCodeComplete(clCodeCompletionEvent& e, IEditor* editor);
    void DoShowCompletionBox(const PHPEntityBase::List_t& entries, PHPExpression::Ptr_t expr);
    IEditor* GetEditor(const wxString& filepath) const;

protected:
    /**
     * @brief convert PHP's entity to CodeLite's TagEntry class
     * This is needed mainly for display purposes (CodeLite's API requires
     * TagEntry for display)
     * This function never fails.
     */
    TagEntryPtr DoPHPEntityToTagEntry(PHPEntityBase::Ptr_t entry);
    PHPEntityBase::Ptr_t DoGetPHPEntryUnderTheAtPos(IEditor* editor, int pos, bool forFunctionCalltip);
    PHPEntityBase::List_t PhpKeywords(const wxString& prefix) const;

private:
    PHPCodeCompletion();
    virtual ~PHPCodeCompletion();

    // Event handlers
    void OnCodeComplete(clCodeCompletionEvent& e);
    void OnCodeCompleteLangKeywords(clCodeCompletionEvent& e);
    void OnFunctionCallTip(clCodeCompletionEvent& e);
    void OnTypeinfoTip(clCodeCompletionEvent& e);
    void OnCodeCompletionBoxDismissed(clCodeCompletionEvent& e);
    void OnFindSymbol(clCodeCompletionEvent& e);
    void OnQuickJump(clCodeCompletionEvent& e);
    void OnInsertDoxyBlock(clCodeCompletionEvent& e);
    void OnRetagWorkspace(wxCommandEvent& event);
    void OnParseEnded(clParseEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& e);

    // Workspace events
    void OnFileSaved(clCommandEvent& event);
    void DoSelectInEditor(IEditor* editor, const wxString& what, int from);

    void DoOpenEditorForEntry(PHPEntityBase::Ptr_t entry);

public:
    /**
     * @brief go to the definition of the word starting at pos
     * @param editor
     * @param pos
     */
    void GotoDefinition(IEditor* editor, int pos);

    void SetManager(IManager* manager) { this->m_manager = manager; }
    /**
     * @brief return a PHPLocation::Ptr_t for the declaration of the
     * expression in the given editor / position
     * @return a valid PHPLocation or NULL
     */
    PHPLocation::Ptr_t FindDefinition(IEditor* editor, int pos);

    /**
     * @brief return the PHPEntity under the caret
     */
    PHPEntityBase::Ptr_t GetPHPEntityAtPos(IEditor* editor, int pos);

    /**
     * @brief open the symbols database for the given workspace file.
     * Close any opened database
     */
    void Open(const wxFileName& workspaceFile);

    /**
     * @brief close the lookup database
     */
    void Close();

    /**
     * @brief called by the PHP symbols cache job.
     * This is to optimize the searching the database (loading the symbols into the
     * memory forces a kernel caching)
     */
    void OnSymbolsCached();

    /**
     * @brief same as the above function, but the caching went bad...
     */
    void OnSymbolsCacheError();
    /**
     * @brief expand 'require_once' line (or any require) by replacing __file__ etc with the proper
     * values and appending everything
     */
    wxString ExpandRequire(const wxFileName& curfile, const wxString& require);

    /**
     * @brief return the best location for inserting generated code inside a class name
     * @param filecontent
     * @return wxNOT_FOUND when could not determine the location
     */
    int GetLocationForSettersGetters(const wxString& filecontent, const wxString& classname);

    /**
     * @brief list members of a class defined in an editor at the current position
     * This only returns member variables (i.e. no constants, nor static members)
     */
    void GetMembers(IEditor* editor, PHPEntityBase::List_t& members, wxString& scope);
};

#endif // PHPCODECOMPLETION_H
