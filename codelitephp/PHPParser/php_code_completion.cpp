#include "php_code_completion.h"
#include "php_utils.h"
#include <wx/stc/stc.h>
#include <plugin.h>
#include <algorithm>
#include "php_strings.h"
#include "php_workspace.h"
#include <imanager.h>
#include <entry.h>
#include <event_notifier.h>
#include "cl_command_event.h"
#include "php_helpers.h"
#include <wx/log.h>
#include <macros.h>
#include "PHPEntityBase.h"
#include "PHPEntityVariable.h"
#include "PHPEntityFunction.h"
#include "PHPExpression.h"

///////////////////////////////////////////////////////////////////

struct PHPCCUserData {
    PHPEntityBase::Ptr_t entry;
    PHPCCUserData(PHPEntityBase::Ptr_t e)
        : entry(e)
    {
    }
};

/// Ascending sorting function
struct _SDescendingSort {
    bool operator()(const TagEntryPtr& rStart, const TagEntryPtr& rEnd)
    {
        return rStart->GetName().Cmp(rEnd->GetName()) > 0;
    }
};

struct _SAscendingSort {
    bool operator()(const TagEntryPtr& rStart, const TagEntryPtr& rEnd)
    {
        return rEnd->GetName().Cmp(rStart->GetName()) > 0;
    }
};
///////////////////////////////////////////////////////////////////

PHPCodeCompletion* PHPCodeCompletion::m_instance = NULL;

PHPCodeCompletion::PHPCodeCompletion()
    : m_manager(NULL)
    , m_typeInfoTooltip(NULL)
{
    EventNotifier::Get()->Bind(wxEVT_PHP_WORKSPACE_CLOSED, &PHPCodeCompletion::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_WORKSPACE_LOADED, &PHPCodeCompletion::OnWorkspaceOpened, this);
    EventNotifier::Get()->Connect(
        wxEVT_CC_CODE_COMPLETE, clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeComplete), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP,
                                  clCodeCompletionEventHandler(PHPCodeCompletion::OnFunctionCallTip),
                                  NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD,
                                  clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompleteLangKeywords),
                                  NULL,
                                  this);
    EventNotifier::Get()->Connect(
        wxEVT_CC_TYPEINFO_TIP, clCodeCompletionEventHandler(PHPCodeCompletion::OnTypeinfoTip), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED,
                                  clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompletionBoxDismissed),
                                  NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE_TAG_COMMENT,
                                  clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompletionGetTagComment),
                                  NULL,
                                  this);
    EventNotifier::Get()->Connect(
        wxEVT_CC_FIND_SYMBOL, clCodeCompletionEventHandler(PHPCodeCompletion::OnFindSymbol), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_EDITOR_TIP_DWELL_END, wxCommandEventHandler(PHPCodeCompletion::OnDismissTooltip), NULL, this);
}

PHPCodeCompletion::~PHPCodeCompletion()
{
    EventNotifier::Get()->Unbind(wxEVT_PHP_WORKSPACE_CLOSED, &PHPCodeCompletion::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_WORKSPACE_LOADED, &PHPCodeCompletion::OnWorkspaceOpened, this);

    EventNotifier::Get()->Disconnect(
        wxEVT_CC_CODE_COMPLETE, clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeComplete), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP,
                                     clCodeCompletionEventHandler(PHPCodeCompletion::OnFunctionCallTip),
                                     NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD,
                                     clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompleteLangKeywords),
                                     NULL,
                                     this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CC_TYPEINFO_TIP, clCodeCompletionEventHandler(PHPCodeCompletion::OnTypeinfoTip), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED,
                                     clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompletionBoxDismissed),
                                     NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE_TAG_COMMENT,
                                     clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompletionGetTagComment),
                                     NULL,
                                     this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CC_FIND_SYMBOL, clCodeCompletionEventHandler(PHPCodeCompletion::OnFindSymbol), NULL, this);

    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_EDITOR_TIP_DWELL_END, wxCommandEventHandler(PHPCodeCompletion::OnDismissTooltip), NULL, this);
}

PHPCodeCompletion* PHPCodeCompletion::Instance()
{
    if(!m_instance) {
        m_instance = new PHPCodeCompletion();
    }
    return m_instance;
}

void PHPCodeCompletion::Release()
{
    if(m_instance) {
        delete m_instance;
    }
    m_instance = NULL;
}

void PHPCodeCompletion::DoShowCompletionBox(const PHPEntityBase::List_t& entries, const wxString& partname)
{
    std::vector<TagEntryPtr> tags;
    wxString lcName, lcPartname;
    lcPartname = partname;
    lcPartname.MakeLower();

    // search for the old item
    PHPEntityBase::List_t::const_iterator iter = entries.begin();
    for(; iter != entries.end(); ++iter) {
        PHPEntityBase::Ptr_t entry = *iter;
        wxString name = entry->GetName();
        // remove the $ prefix from all non static members
        if(entry->Is(kEntityTypeVariable) && !entry->Cast<PHPEntityVariable>()->IsStatic()) {
            if(name.StartsWith(wxT("$"))) {
                name.Remove(0, 1);
            }
        }

        lcName = name;
        lcName.MakeLower();
        if(!lcName.StartsWith(lcPartname)) continue;

        TagEntryPtr t = DoPHPEntityToTagEntry(entry);
        t->SetUserData(new PHPCCUserData(entry));
        tags.push_back(t);
    }

    if(tags.empty()) return;

    std::sort(tags.begin(), tags.end(), _SAscendingSort());
    m_manager->GetActiveEditor()->ShowCompletionBox(tags, partname, true, this);
}

void PHPCodeCompletion::OnCodeCompletionBoxDismissed(clCodeCompletionEvent& e) { e.Skip(); }

void PHPCodeCompletion::OnCodeCompletionGetTagComment(clCodeCompletionEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {

        TagEntryPtr tag = e.GetTagEntry();
        void* data = tag->GetUserData();

        if(data) {
            PHPCCUserData* userData = reinterpret_cast<PHPCCUserData*>(data);

            wxString comment, docComment;
            docComment = userData->entry->GetDocComment();
            if(docComment.IsEmpty() == false) {
                comment << docComment;
                comment.Trim().Trim(false); // The Doc comment
                comment << wxT("\n<hr>\n"); // HLine
            }

            wxFileName fn(userData->entry->GetFilename());
            fn.MakeRelativeTo(PHPWorkspace::Get()->GetFilename().GetPath());
            comment << fn.GetFullName() << wxT(" : ") << userData->entry->GetLine();
            e.SetTooltip(comment);
        }

    } else {
        e.Skip();
    }
}

TagEntryPtr PHPCodeCompletion::DoPHPEntityToTagEntry(PHPEntityBase::Ptr_t entry)
{
    TagEntryPtr t(new TagEntry());
    wxString name = entry->GetName();

    if(entry->Is(kEntityTypeVariable) && entry->Cast<PHPEntityVariable>()->IsMember() && name.StartsWith(wxT("$")) &&
       !entry->Cast<PHPEntityVariable>()->IsStatic()) {
        name.Remove(0, 1);
    }

    t->SetName(name);
    t->SetParent(entry->Parent() ? entry->Parent()->GetName() : "");
    t->SetPattern(t->GetName());
    t->SetComment(entry->GetDocComment());

    // Access
    if(entry->Is(kEntityTypeVariable)) {
        PHPEntityVariable* var = entry->Cast<PHPEntityVariable>();

        // visibility
        if(var->IsPrivate())
            t->SetAccess(wxT("private"));
        else if(var->IsProtected())
            t->SetAccess(wxT("protected"));
        else
            t->SetAccess(wxT("public"));

        // type (affects icon)
        if(var->IsConst()) {
            t->SetKind("macro");
        } else {
            t->SetKind("variable");
        }
        t->SetReturnValue("");

    } else if(entry->Is(kEntityTypeFunction)) {
        PHPEntityFunction* func = entry->Cast<PHPEntityFunction>();
        if(func->HasFlag(PHPEntityFunction::kPrivate)) {
            t->SetAccess(wxT("private"));
        } else if(func->HasFlag(PHPEntityFunction::kProtected)) {
            t->SetAccess("protected");
        } else {
            t->SetAccess(wxT("public"));
        }
        t->SetSignature(func->GetSignature());
        t->SetPattern(func->GetName() + func->GetSignature());
        t->SetKind("function");

    } else if(entry->Is(kEntityTypeClass)) {
        t->SetAccess(wxT("public"));
        t->SetKind("class");

    } else if(entry->Is(kEntityTypeNamespace)) {
        t->SetAccess("public");
        t->SetKind("namespace");
    }
    t->SetFlags(TagEntry::Tag_No_Signature_Format);
    return t;
}

void PHPCodeCompletion::OnCodeComplete(clCodeCompletionEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        if(!CanCodeComplete(e)) return;

        IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // we handle only .php files
            if(IsPHPFile(editor)) {

                // Perform the code completion here
                PHPExpression expr(editor->GetTextRange(0, e.GetPosition()));
                PHPEntityBase::Ptr_t entity = expr.Resolve(m_lookupTable, editor->GetFileName().GetFullPath());
                if(entity) {
                    PHPEntityBase::List_t matches =
                        m_lookupTable.FindChildren(entity->GetDbId(),
                                                   PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(),
                                                   expr.GetFilter());
                    if(!matches.empty()) {
                        // Show the code completion box
                        DoShowCompletionBox(matches, expr.GetFilter());
                    }
                }
            }
        }
    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::OnFunctionCallTip(clCodeCompletionEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        if(!CanCodeComplete(e)) return;

        IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // we handle only .php files
            if(IsPHPFile(editor)) {
                // get the position

                return;
            }
        }

    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::OnFindSymbol(clCodeCompletionEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        if(!CanCodeComplete(e)) return;

        IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // FIXME : implement find-symbol
        }

    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::OnTypeinfoTip(clCodeCompletionEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        if(!CanCodeComplete(e)) return;

        IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // we handle only .php files
            if(IsPHPFile(editor)) {
                // FIXME: implement this using the new method
                return;
            }
        }

    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::OnDismissTooltip(wxCommandEvent& e)
{
    IEditor* editor = dynamic_cast<IEditor*>(e.GetEventObject());
    if(editor) {
        // we handle only .php files
        if(IsPHPFile(editor)) {
            // get the position
            if(m_typeInfoTooltip) {
                m_typeInfoTooltip->Destroy();
                m_typeInfoTooltip = NULL;
            }
            return;
        }
    }
    e.Skip();
}

PHPLocationPtr PHPCodeCompletion::FindDefinition(IEditor* editor, int pos)
{
    CHECK_PHP_WORKSPACE_RET_NULL();

    if(IsPHPFile(editor)) {
        // FIXME :: Implement this
        return NULL;
    }
    return NULL;
}

void PHPCodeCompletion::OnCodeCompleteLangKeywords(clCodeCompletionEvent& e) { e.Skip(); }

PHPEntityBase::Ptr_t PHPCodeCompletion::GetPHPEntryUnderTheAtPos(IEditor* editor, int pos)
{
    if(!PHPWorkspace::Get()->IsOpen()) return PHPEntityBase::Ptr_t(NULL);
    pos = editor->GetSTC()->WordEndPosition(pos, true);
    return PHPEntityBase::Ptr_t(NULL);
}

bool PHPCodeCompletion::CanCodeComplete(clCodeCompletionEvent& e) const
{
    int pos = e.GetPosition();
    if(pos) pos -= 1;
    IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
    if(!editor) return false;

    // we can get style 0 if we added chars and they were not styled just yet
    // sd we use the first style near our position (backward)
    int lineNumber = editor->LineFromPos(pos);
    int lineStartPos = editor->PosFromLine(lineNumber);

    if(lineStartPos > pos) return false;

    int styleAt(0);
    int retryLeft(pos - lineStartPos + 2);
    while((styleAt == 0) && retryLeft && pos > 0) {
        styleAt = editor->GetStyleAtPos(pos);
        if(styleAt == 0) {
            --pos;
        }
        --retryLeft;
    }

    return (editor && !e.IsInsideCommentOrString() && IsPHPSection(styleAt) && !IsPHPCommentOrString(styleAt));
}

void PHPCodeCompletion::OnWorkspaceOpened(PHPEvent& event)
{
    event.Skip();
    if(m_lookupTable.IsOpened()) {
        m_lookupTable.Close();
    }

    wxFileName fnWorkspace(event.GetFileName());
    m_lookupTable.Open(fnWorkspace.GetPath());
}

void PHPCodeCompletion::OnWorkspaceClosed(PHPEvent& event)
{
    event.Skip();
    m_lookupTable.Close();
}
