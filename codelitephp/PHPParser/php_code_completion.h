#ifndef PHPCODECOMPLETION_H
#define PHPCODECOMPLETION_H

#include "precompiled_header.h"
#include "ieditor.h"
#include "php_parser_api.h"
#include <vector>
#include <smart_ptr.h>
#include <set>
#include <cl_command_event.h>
#include <cc_box_tip_window.h>

struct PHPLocation {
    wxString what;       // Token name
    wxString filename;   // file name (absolute path)
    int      linenumber; // line number within filename
};
typedef SmartPtr<PHPLocation> PHPLocationPtr;

class IManager;
class ResourceItem;
class PHPCodeCompletion : public wxEvtHandler
{
public:
    enum {
        kCC_FUNCTIONS_ONLY = 0x00000001,
        kCC_SELF_FUNCTIONS = 0x00000002, // self::
        kCC_SELF_MEMBERS   = 0x00000004, // self::
        kCC_STATIC         = 0x00000008, // static::
        kCC_SELF           = kCC_SELF_FUNCTIONS | kCC_SELF_MEMBERS,
    };
    
    static PHPCodeCompletion* Instance();
    static void Release();

    void SetUnitTestsMode(bool unitTestsMode) {
        this->m_unitTestsMode = unitTestsMode;
    }
protected:
    static PHPCodeCompletion* m_instance;
    IEditor *                 m_currentEditor;
    std::set<PHPEntry*>       m_shownEntries;
    IManager*                 m_manager;
    CCBoxTipWindow*           m_typeInfoTooltip;

    // Parsing results
    std::map<wxString, PHPEntry> m_currentFileClasses;
    std::map<wxString, PHPEntry> m_currentFileFunctions;
    std::map<wxString, PHPEntry> m_currentFileVariables;
    std::map<wxString, PHPEntry> m_currentFileGlobals; // for convinience, both variables + functions
    bool                         m_unitTestsMode;

    bool CanCodeComplete( clCodeCompletionEvent& e ) const;

protected:
    void SetCurrentFileClasses(const std::map<wxString, PHPEntry>& currentFileClasses) {
        this->m_currentFileClasses = currentFileClasses;
    }
    void SetCurrentFileFunctions(const std::map<wxString, PHPEntry>& currentFileFunctions) {
        this->m_currentFileFunctions = currentFileFunctions;
    }
    void SetCurrentFileVariables(const std::map<wxString, PHPEntry>& currentFileVariables) {
        this->m_currentFileVariables = currentFileVariables;
    }
    const std::map<wxString, PHPEntry>& GetCurrentFileClasses() const {
        return m_currentFileClasses;
    }
    const std::map<wxString, PHPEntry>& GetCurrentFileFunctions() const {
        return m_currentFileFunctions;
    }
    const std::map<wxString, PHPEntry>& GetCurrentFileVariables() const {
        return m_currentFileVariables;
    }

    void SetCurrentFileGlobals(const std::map<wxString, PHPEntry>& currentFileGlobals) {
        this->m_currentFileGlobals = currentFileGlobals;
    }
    const std::map<wxString, PHPEntry>& GetCurrentFileGlobals() const {
        return m_currentFileGlobals;
    }

protected:
    wxString           DoGetExpression(IEditor *editor, int pos);
    PHPEntry::Vector_t DoSuggestGlobals(const wxString& partname, const wxString& ns);

    void           DoGetMembersOf       (const PHPEntry& entry, const wxString& partname, size_t filter_flags, PHPEntry::Vector_t& res);
    void           DoSuggestMembersOf   (const PHPEntry &entry, const wxString &partname, size_t filter_flags);
    void           DoShowCompletionBox  (const std::vector<PHPEntry>& entries, const wxString &partname, size_t filter_flags = 0);
    void           DoCleanup            ();
    void           DoInitialize         (IEditor *editor, int pos);
    TagEntryPtr    DoPHPEntryToTagEntry (const PHPEntry& entry);
    void           DoCopyEntryProperties(const PHPEntry &src, PHPEntry &target);
    void           DoCodeComplete(IEditor* editor, int pos);
    void           DoCalltip(IEditor *editor, int pos);
    void           DoTypeinfoTip(IEditor *editor, int pos);
    PHPLocationPtr DoFindDefinition(IEditor *editor, int pos);
    void           DoOpenResource(const ResourceItem *itemData);
    void           DoFindSymbolFromLocalParse(std::vector<ResourceItem> &items, const wxString &name);
    //bool           DoExpandFunctionReturnValue(PHPEntry* entry, std::map<wxString, PHPEntry>& locals, bool &stop);

    void SetCurrentEditor(IEditor* currentEditor) {
        this->m_currentEditor = currentEditor;
    }
    IEditor* GetCurrentEditor() {
        return m_currentEditor;
    }

private:
    PHPCodeCompletion();
    virtual ~PHPCodeCompletion();

    void OnCodeComplete               (clCodeCompletionEvent &e);
    void OnCodeCompleteLangKeywords   (clCodeCompletionEvent &e);
    void OnFunctionCallTip            (clCodeCompletionEvent &e);
    void OnTypeinfoTip                (clCodeCompletionEvent &e);
    void OnCodeCompletionBoxDismissed (clCodeCompletionEvent &e);
    void OnCodeCompletionGetTagComment(clCodeCompletionEvent &e);
    void OnFindSymbol                 (clCodeCompletionEvent &e);
    void OnDismissTooltip(wxCommandEvent &e);


    bool ParseTokenList(Token* token, bool deepResolvingLastToken, PHPEntry& match, size_t &filter_flags);

public:
    PHPEntry GetPHPEntryUnderTheAtPos(IEditor* editor, int pos);
    
    /**
     * @brief return the scope near a given position. scope can be class or namespace
     */
    PHPEntry GetClassNearPosition(IEditor* editor, int pos);
    
    void SetManager(IManager* manager) {
        this->m_manager = manager;
    }

    bool IsUnitTestsMode() const {
        return m_unitTestsMode;
    }
    /**
     * @brief return a PHPLocationPtr for the declaration of the
     * expression in the given editor / position
     * @return a valid PHPLocation or NULL
     */
    PHPLocationPtr FindDefinition(IEditor *editor, int pos);

    /**
     * @brief suggest list of members for a give expression
     * This function is intended for UnitTestings ONLY
     * @param fileContent the file content
     * @param expression the expression we want to parse
     */
    bool TestSuggestMembers(const wxString &fileContent, const wxString &expression, PHPEntry::Vector_t& matches);
};

#endif // PHPCODECOMPLETION_H
