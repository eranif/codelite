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
#include "php_parser_thread.h"
#include "macros.h"
#include "PHPEntityClass.h"

///////////////////////////////////////////////////////////////////

struct PHPCCUserData
{
    PHPEntityBase::Ptr_t entry;
    PHPCCUserData(PHPEntityBase::Ptr_t e)
        : entry(e)
    {
    }
};

/// Ascending sorting function
struct _SDescendingSort
{
    bool operator()(const TagEntryPtr& rStart, const TagEntryPtr& rEnd)
    {
        return rStart->GetName().Cmp(rEnd->GetName()) > 0;
    }
};

struct _SAscendingSort
{
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
    EventNotifier::Get()->Connect(
        wxEVT_CMD_RETAG_WORKSPACE, wxCommandEventHandler(PHPCodeCompletion::OnRetagWorkspace), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_RETAG_WORKSPACE_FULL, wxCommandEventHandler(PHPCodeCompletion::OnRetagWorkspace), NULL, this);

    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, clCommandEventHandler(PHPCodeCompletion::OnFileSaved), this);
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
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_RETAG_WORKSPACE, wxCommandEventHandler(PHPCodeCompletion::OnRetagWorkspace), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_RETAG_WORKSPACE_FULL, wxCommandEventHandler(PHPCodeCompletion::OnRetagWorkspace), NULL, this);

    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, clCommandEventHandler(PHPCodeCompletion::OnFileSaved), this);

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

void PHPCodeCompletion::DoShowCompletionBox(const PHPEntityBase::List_t& entries, PHPExpression::Ptr_t expr)
{
    std::vector<TagEntryPtr> tags;
    wxStringSet_t uniqueEntries;

    // search for the old item
    PHPEntityBase::List_t::const_iterator iter = entries.begin();
    for(; iter != entries.end(); ++iter) {
        PHPEntityBase::Ptr_t entry = *iter;
        if(uniqueEntries.count(entry->GetFullName()) == 0) {
            uniqueEntries.insert(entry->GetFullName());
        } else {
            // don't add duplicate entries
            continue;
        }

        PHPEntityBase::Ptr_t ns = expr->GetSourceFile()->Namespace(); // the namespace at the source file
        TagEntryPtr t = DoPHPEntityToTagEntry(entry);
        t->SetUserData(new PHPCCUserData(entry));
        tags.push_back(t);
    }

    if(tags.empty())
        return;

    std::sort(tags.begin(), tags.end(), _SAscendingSort());
    m_manager->GetActiveEditor()->ShowCompletionBox(tags, expr->GetFilter(), true, this);
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
                docComment.Trim().Trim(false);          // The Doc comment
                comment << docComment << wxT("\n<hr>"); // HLine
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
    wxString name = entry->GetShortName();

    if(entry->Is(kEntityTypeVariable) && entry->Cast<PHPEntityVariable>()->IsMember() && name.StartsWith(wxT("$")) &&
       !entry->Cast<PHPEntityVariable>()->IsStatic()) {
        name.Remove(0, 1);
    } else if((entry->Is(kEntityTypeClass) || entry->Is(kEntityTypeNamespace)) && name.StartsWith("\\")) {
        name.Remove(0, 1);
    }

    t->SetName(name);
    t->SetParent(entry->Parent() ? entry->Parent()->GetFullName() : "");
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
        if(var->IsConst() || var->IsDefine()) {
            t->SetKind("macro");
        } else {
            t->SetKind("variable");
        }
        t->SetReturnValue("");

    } else if(entry->Is(kEntityTypeFunction)) {
        PHPEntityFunction* func = entry->Cast<PHPEntityFunction>();
        if(func->HasFlag(kFunc_Private)) {
            t->SetAccess(wxT("private"));
        } else if(func->HasFlag(kFunc_Protected)) {
            t->SetAccess("protected");
        } else {
            t->SetAccess(wxT("public"));
        }
        t->SetSignature(func->GetSignature());
        t->SetPattern(func->GetShortName() + func->GetSignature());
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
        if(!CanCodeComplete(e))
            return;

        IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // we handle only .php files
            if(IsPHPFile(editor)) {

                // Check if the code completion was triggered due to user
                // typing '(', in this case, call OnFunctionCallTip()
                wxChar charAtPos = editor->GetCharAtPos(editor->GetCurrentPosition() - 1);
                if(charAtPos == '(') {
                    OnFunctionCallTip(e);

                } else {
                    // Perform the code completion here
                    PHPExpression::Ptr_t expr(new PHPExpression(editor->GetTextRange(0, e.GetPosition())));
                    PHPEntityBase::Ptr_t entity = expr->Resolve(m_lookupTable, editor->GetFileName().GetFullPath());
                    if(entity) {
                        // Suggets members for the resolved entity
                        PHPEntityBase::List_t matches;
                        expr->Suggest(entity, m_lookupTable, matches);
                        
                        // Remove duplicates from the list
                        if(!matches.empty()) {
                            // Show the code completion box
                            DoShowCompletionBox(matches, expr);
                        }
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
        if(!CanCodeComplete(e))
            return;

        IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // we handle only .php files
            if(IsPHPFile(editor)) {
                // get the position
                PHPEntityBase::Ptr_t resolved = DoGetPHPEntryUnderTheAtPos(editor, editor->GetCurrentPosition(), true);
                if(resolved) {
                    // In PHP there is no overloading, so there can be only one signature for a function
                    // so we simply place our match into TagEntryPtrVector_t structure and pass it to the editor
                    TagEntryPtrVector_t tags;
                    tags.push_back(DoPHPEntityToTagEntry(resolved));
                    clCallTipPtr callTip(new clCallTip(tags));
                    editor->ShowCalltip(callTip);
                }
            }
        }

    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::OnFindSymbol(clCodeCompletionEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        if(!CanCodeComplete(e))
            return;

        IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            PHPEntityBase::Ptr_t resolved = GetPHPEntryUnderTheAtPos(editor, editor->GetCurrentPosition());
            if(resolved) {
                m_manager->OpenFile(resolved->GetFilename().GetFullPath(), "", resolved->GetLine());
            }
        }

    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::OnTypeinfoTip(clCodeCompletionEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        if(!CanCodeComplete(e))
            return;

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

PHPLocation::Ptr_t PHPCodeCompletion::FindDefinition(IEditor* editor, int pos)
{
    CHECK_PHP_WORKSPACE_RET_NULL();
    PHPLocation::Ptr_t loc; // Null
    if(IsPHPFile(editor)) {
        PHPEntityBase::Ptr_t resolved = GetPHPEntryUnderTheAtPos(editor, editor->GetCurrentPosition());
        if(resolved) {
            loc = new PHPLocation;
            loc->filename = resolved->GetFilename().GetFullPath();
            loc->linenumber = resolved->GetLine();
            loc->what = resolved->GetShortName();
        }
    }
    return loc;
}

void PHPCodeCompletion::OnCodeCompleteLangKeywords(clCodeCompletionEvent& e) { e.Skip(); }

PHPEntityBase::Ptr_t PHPCodeCompletion::GetPHPEntryUnderTheAtPos(IEditor* editor, int pos)
{
    return DoGetPHPEntryUnderTheAtPos(editor, pos, false);
}

/* static */
bool PHPCodeCompletion::CanCodeComplete(clCodeCompletionEvent& e)
{
    int pos = e.GetPosition();
    if(pos)
        pos -= 1;
    IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
    if(!editor)
        return false;

    // we can get style 0 if we added chars and they were not styled just yet
    // sd we use the first style near our position (backward)
    int lineNumber = editor->LineFromPos(pos);
    int lineStartPos = editor->PosFromLine(lineNumber);

    if(lineStartPos > pos)
        return false;

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

void PHPCodeCompletion::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    // check if the saved file is a PHP file
    // In case it is, then re-parse the file and store the results
    if(::IsPHPFile(event.GetFileName())) {
        PHPParserThreadRequest* req = new PHPParserThreadRequest(PHPParserThreadRequest::kParseSingleFile);
        req->file = event.GetFileName();
        req->workspaceFile = PHPWorkspace::Get()->GetFilename().GetFullPath();
        PHPParserThread::Instance()->Add(req);
    }
}

void PHPCodeCompletion::OnRetagWorkspace(wxCommandEvent& event)
{
    event.Skip();
    if(PHPWorkspace::Get()->IsOpen()) {
        event.Skip(false);
        // Reparse the workspace
        PHPWorkspace::Get()->ParseWorkspace(event.GetEventType() == wxEVT_CMD_RETAG_WORKSPACE_FULL);
    }
}

PHPEntityBase::Ptr_t PHPCodeCompletion::DoGetPHPEntryUnderTheAtPos(IEditor* editor, int pos, bool forFunctionCalltip)
{
    if(!PHPWorkspace::Get()->IsOpen())
        return PHPEntityBase::Ptr_t(NULL);
    pos = editor->GetSTC()->WordEndPosition(pos, true);

    // Get the expression under the caret
    wxString unsavedBuffer = editor->GetTextRange(0, pos);
    wxString filter;
    PHPEntityBase::Ptr_t resolved;

    // Parse the source file
    PHPSourceFile source(unsavedBuffer);
    source.SetFilename(editor->GetFileName());
    source.SetParseFunctionBody(false);
    source.Parse();

    PHPEntityBase::Ptr_t currentScope = source.CurrentScope();
    if(currentScope && currentScope->Is(kEntityTypeClass)) {
        // we are trying to resolve a 'word' under the caret within the class
        // body but _not_ within a function body (i.e. it can only be
        // a definition of some kind)
        // try to construct an expression that will work
        int wordStart = editor->GetSTC()->WordStartPosition(pos, true);
        wxString theWord = editor->GetTextRange(wordStart, pos);
        wxString theWordNoDollar = theWord;
        if(theWord.StartsWith("$")) {
            theWordNoDollar = theWord.Mid(1);
        }
        PHPExpression expr2(unsavedBuffer, "<?php $this->" + theWordNoDollar, forFunctionCalltip);
        resolved = expr2.Resolve(m_lookupTable, editor->GetFileName().GetFullPath());
        filter = expr2.GetFilter();
        if(!resolved) {
            // Maybe its a static member/function/const, try using static keyword
            PHPExpression expr3(unsavedBuffer, "<?php static::" + theWord, forFunctionCalltip);
            resolved = expr2.Resolve(m_lookupTable, editor->GetFileName().GetFullPath());
            filter = expr2.GetFilter();
        }
    }

    if(!resolved) {
        PHPExpression expr(unsavedBuffer, "", forFunctionCalltip);
        resolved = expr.Resolve(m_lookupTable, editor->GetFileName().GetFullPath());
        filter = expr.GetFilter();
    }

    if(resolved && !filter.IsEmpty()) {
        resolved = m_lookupTable.FindMemberOf(resolved->GetDbId(), filter);

        if(resolved && resolved->Is(kEntityTypeFunction)) {
            // for a function, we need to load its children (function arguments)
            resolved->SetChildren(m_lookupTable.LoadFunctionArguments(resolved->GetDbId()));
        }
    }
    return resolved;
}

void PHPCodeCompletion::Open(const wxFileName& workspaceFile)
{
    Close();
    m_lookupTable.Open(workspaceFile.GetPath());
}

void PHPCodeCompletion::Close()
{
    if(m_lookupTable.IsOpened()) {
        m_lookupTable.Close();
    }
}
