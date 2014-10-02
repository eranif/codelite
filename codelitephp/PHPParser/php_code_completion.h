#ifndef PHPCODECOMPLETION_H
#define PHPCODECOMPLETION_H

#include "precompiled_header.h"
#include "ieditor.h"
#include <vector>
#include <smart_ptr.h>
#include <cl_command_event.h>
#include <cc_box_tip_window.h>
#include "PHPEntityBase.h"

struct PHPLocation
{
    wxString what;     // Token name
    wxString filename; // file name (absolute path)
    int linenumber;    // line number within filename
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

    bool CanCodeComplete(clCodeCompletionEvent& e) const;
    void DoShowCompletionBox(const PHPEntityBase::List_t& entries, const wxString& partname);

protected:
    TagEntryPtr DoPHPEntityToTagEntry(PHPEntityBase::Ptr_t entry);

private:
    PHPCodeCompletion();
    virtual ~PHPCodeCompletion();

    // Event handlers
    void OnCodeComplete(clCodeCompletionEvent& e);
    void OnCodeCompleteLangKeywords(clCodeCompletionEvent& e);
    void OnFunctionCallTip(clCodeCompletionEvent& e);
    void OnTypeinfoTip(clCodeCompletionEvent& e);
    void OnCodeCompletionBoxDismissed(clCodeCompletionEvent& e);
    void OnCodeCompletionGetTagComment(clCodeCompletionEvent& e);
    void OnFindSymbol(clCodeCompletionEvent& e);
    void OnDismissTooltip(wxCommandEvent& e);

public:
    void SetManager(IManager* manager) { this->m_manager = manager; }
    /**
     * @brief return a PHPLocationPtr for the declaration of the
     * expression in the given editor / position
     * @return a valid PHPLocation or NULL
     */
    PHPLocationPtr FindDefinition(IEditor* editor, int pos);
    
    /**
     * @brief return the PHPEntity under the caret
     */
    PHPEntityBase::Ptr_t GetPHPEntryUnderTheAtPos(IEditor* editor, int pos);
};

#endif // PHPCODECOMPLETION_H
