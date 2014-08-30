#include "php_code_completion.h"
#include "php_utils.h"
#include <wx/stc/stc.h>
#include "php_parser_api.h"
#include <plugin.h>
#include <algorithm>
#include "php_strings.h"
#include "php_storage.h"
#include "php_workspace.h"
#include <imanager.h>
#include <entry.h>
#include <event_notifier.h>
#include "cl_command_event.h"
#include "php_helpers.h"
#include <wx/log.h>
#include <macros.h>

///////////////////////////////////////////////////////////////////

/// Ascending sorting function
struct _SDescendingSort {
    bool operator()(const TagEntryPtr &rStart, const TagEntryPtr &rEnd) {
        return rStart->GetName().Cmp(rEnd->GetName()) > 0;
    }
};

struct _SAscendingSort {
    bool operator()(const TagEntryPtr &rStart, const TagEntryPtr &rEnd) {
        return rEnd->GetName().Cmp(rStart->GetName()) > 0;
    }
};
///////////////////////////////////////////////////////////////////

PHPCodeCompletion* PHPCodeCompletion::m_instance = NULL;

PHPCodeCompletion::PHPCodeCompletion()
    : m_currentEditor(NULL)
    , m_manager(NULL)
    , m_typeInfoTooltip(NULL)
    , m_unitTestsMode(false)
{
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE,                   clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeComplete),                NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP,  clCodeCompletionEventHandler(PHPCodeCompletion::OnFunctionCallTip),             NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD ,     clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompleteLangKeywords),    NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CC_TYPEINFO_TIP,                    clCodeCompletionEventHandler(PHPCodeCompletion::OnTypeinfoTip),                 NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED,     clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompletionBoxDismissed),  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CC_CODE_COMPLETE_TAG_COMMENT,       clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompletionGetTagComment), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CC_FIND_SYMBOL,                     clCodeCompletionEventHandler(PHPCodeCompletion::OnFindSymbol),                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_EDITOR_TIP_DWELL_END,           wxCommandEventHandler(PHPCodeCompletion::OnDismissTooltip),              NULL, this);
}

PHPCodeCompletion::~PHPCodeCompletion()
{
    DoCleanup();
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE,                   clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeComplete),                NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP,  clCodeCompletionEventHandler(PHPCodeCompletion::OnFunctionCallTip),             NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD ,     clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompleteLangKeywords),    NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_TYPEINFO_TIP,                    clCodeCompletionEventHandler(PHPCodeCompletion::OnTypeinfoTip),                 NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED,     clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompletionBoxDismissed),  NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_CODE_COMPLETE_TAG_COMMENT,       clCodeCompletionEventHandler(PHPCodeCompletion::OnCodeCompletionGetTagComment), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CC_FIND_SYMBOL,                     clCodeCompletionEventHandler(PHPCodeCompletion::OnFindSymbol),                  NULL, this);
    
    EventNotifier::Get()->Disconnect(wxEVT_CMD_EDITOR_TIP_DWELL_END,           wxCommandEventHandler(PHPCodeCompletion::OnDismissTooltip),              NULL, this);
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

wxString PHPCodeCompletion::DoGetExpression(IEditor* editor, int pos)
{
    bool cont(true);
    int  depth(0);

    int  position( pos );
    int  at(position);
    bool prevGt(false);
    bool prevColon(false);
    while (cont && depth >= 0) {
        wxChar ch =editor->PreviousChar(position, at, true);
        position = at;
        //Eof?
        if (ch == 0) {
            at = 0;
            break;
        }

        //Comment?
        int style = editor->GetStyleAtPos(position);
        if( style == wxSTC_H_COMMENT       ||
            style == wxSTC_H_XCCOMMENT     ||
            style == wxSTC_H_SGML_COMMENT  ||
            style == wxSTC_HJ_COMMENT      ||
            style == wxSTC_HJ_COMMENTLINE  ||
            style == wxSTC_HJ_COMMENTDOC   ||
            style == wxSTC_HJA_COMMENT     ||
            style == wxSTC_HJA_COMMENTLINE ||
            style == wxSTC_HJA_COMMENTDOC  ||
            style == wxSTC_HB_COMMENTLINE  ||
            style == wxSTC_HBA_COMMENTLINE ||
            style == wxSTC_HP_COMMENTLINE  ||
            style == wxSTC_HPA_COMMENTLINE ||
            style == wxSTC_HPHP_COMMENT    ||
            style == wxSTC_HPHP_COMMENTLINE)
            continue;

        switch (ch) {
        case wxT(';'):
            // dont include this token
            at = editor->PositionAfterPos(at);
            cont = false;
            prevColon = false;
            break;
        case wxT('-'):
            if (prevGt) {
                prevGt = false;
                //if previous char was '>', we found an arrow so reduce the depth
                //which was increased
                depth--;
            } else {
                if (depth <= 0) {
                    //dont include this token
                    at = editor->PositionAfterPos(at);
                    cont = false;
                }
            }
            prevColon = false;
            break;
        case wxT(' '):
        case wxT('\n'):
        case wxT('\v'):
        case wxT('\t'):
        case wxT('\r'):
            prevGt = false;
            prevColon = false;
            if (depth <= 0) {
                cont = false;
                break;
            }
            break;
        case wxT('{'):
        case wxT('='):
            prevGt = false;
            prevColon = false;
            cont = false;
            break;
        case wxT('('):
        case wxT('['):
            depth--;
            prevGt = false;
            prevColon = false;
            if (depth < 0) {
                //dont include this token
                at =editor->PositionAfterPos(at);
                cont = false;
            }
            break;
        case wxT(','):
        case wxT('*'):
        case wxT('&'):
        case wxT('!'):
        case wxT('~'):
        case wxT('+'):
        case wxT('^'):
        case wxT('|'):
        case wxT('%'):
        case wxT('?'):
            prevGt = false;
            prevColon = false;
            if (depth <= 0) {

                //dont include this token
                at =editor->PositionAfterPos(at);
                cont = false;
            }
            break;
        case wxT('>'):
            prevGt = true;
            prevColon = false;
            depth++;
            break;
        case wxT('<'):
            prevGt = false;
            prevColon = false;
            depth--;
            if (depth < 0) {

                //dont include this token
                at = editor->PositionAfterPos(at);
                cont = false;
            }
            break;
        case wxT(')'):
        case wxT(']'):
            prevGt = false;
            prevColon = false;
            depth++;
            break;
        default:
            prevGt = false;
            prevColon = false;
            break;
        }
    }

    wxUnusedVar(prevColon);
    if (at < 0) at = 0;
    wxString expr = editor->GetTextRange(at, pos);
    expr.Trim().Trim(false);
    return expr;
}

void PHPCodeCompletion::DoCodeComplete(IEditor* editor, int pos)
{
    DoInitialize(editor, pos);
    if(editor->GetCharAtPos( editor->PositionBeforePos(pos)) == wxT('(')) {
        DoCalltip(editor, pos);
        return;
    }

    // Second parse is needed to get the locals at the current position
    wxString text_range = editor->GetTextRange(0, pos);
    if ( !PHPParser.parseString(true, text_range, PHPGlobals::Type_Normal) ) {
        return;
    }

    // Extract the expression
    wxString expr = DoGetExpression(editor, pos);
    Token* token = PHPParser.makeTokenList( PHP_PREFIX_WITH_SPACE + expr, PHPParser.getLocals() );
    CHECK_PTR_RET( token );
    
    Token* last = token->getLast();
    CHECK_PTR_RET( last );
    
    TDeleter<Token> del( token );
    
    wxString partial_word;
    // if the token list ends with any operator, 
    // we perform 'code-complete' otherwise, we perform
    // word-completion
    if ( !last->endsWithOperator() ) {
        partial_word = last->getName();
        if ( !last->disonnectSelf() ) {
            // This means that last == head
            if( last->isStartsWithNSOperator() ) {
                DoSuggestGlobals(partial_word, "\\");
                
            } else {
                DoSuggestGlobals(partial_word, PHPParser.getCurrentNamespace());
            }
            return;
            
        } else {
            wxDELETE(last);
            
        }
        
    } 
    
    PHPEntry match;
    size_t flags;
    if ( ParseTokenList( token, true, match, flags ) ) {
        DoSuggestMembersOf(match, partial_word, flags);
    }
}

void PHPCodeCompletion::DoTypeinfoTip(IEditor* editor, int pos)
{
    wxPoint tippos = editor->GetSTC()->PointFromPosition(pos);
    tippos = editor->GetSTC()->ClientToScreen(tippos);
    tippos = EventNotifier::Get()->TopFrame()->ScreenToClient(tippos);
    
    PHPEntry match = GetPHPEntryUnderTheAtPos(editor, pos);
    if ( match.isOk() ) {
        if ( m_typeInfoTooltip ) {
            m_typeInfoTooltip->Destroy();
            m_typeInfoTooltip = NULL;
        }
        
        wxString type_info = match.getTypeInfoTooltip();
        if ( !type_info.IsEmpty() ) {
            m_typeInfoTooltip = new CCBoxTipWindow(EventNotifier::Get()->TopFrame(), type_info);
            m_typeInfoTooltip->PositionAt( wxGetMousePosition());
        }
    }
}

void PHPCodeCompletion::DoCalltip(IEditor* editor, int pos)
{
    DoInitialize(editor, pos);

    // Second parse is needed to get the locals at the current position
    if ( !PHPParser.parseString(true, editor->GetTextRange(0, pos), PHPGlobals::Type_Normal) ) {
        return;
    }

    // Extract the expression
    wxString expr = DoGetExpression(editor, pos - 1);
    if ( !PHPParser.parseString(true, PHP_PREFIX_WITH_SPACE + expr, PHPGlobals::Type_Expr) ) {
        return;
    }
    
    Token* token = PHPParser.makeTokenList( PHP_PREFIX_WITH_SPACE + expr, PHPParser.getLocals() );
    CHECK_PTR_RET( token );
     
    TDeleter<Token> del( token );
    PHPEntry match;
    size_t flags;
    if ( ParseTokenList( token, false, match, flags ) && match.isFunction() ) {
        // prepare the tip
        std::vector<TagEntryPtr> tips;
        TagEntryPtr tip = DoPHPEntryToTagEntry( match );
        tips.push_back(tip);
        GetCurrentEditor()->ShowCalltip(new clCallTip(tips));
    }
}


PHPEntry::Vector_t PHPCodeCompletion::DoSuggestGlobals(const wxString& partname, const wxString &ns)
{
    PHPEntry::Vector_t v;
    // incase we need members of the global namespace,
    // don't show the locals
    if ( ns != "\\" ) {
        PHPGlobals::EntryMap_t locals = PHPParser.getLocals();
        PHPGlobals::EntryMap_t::const_iterator iter = locals.begin();
        for(; iter != locals.end(); ++iter) {
            if ( iter->second.getName().StartsWith(partname) ) {
                v.push_back( iter->second );
            }
        }
    }
    // load namespaces and classes
    PHPEntry::Vector_t dbResutls = PHPStorage::Instance()->FetchNamespaceEntries(ns, partname, PHPStorage::FetchFlags_PartMatch);
    std::sort(dbResutls.begin(), dbResutls.end(), PHPEntry::LowerThan());
    
    // Always load the global functions
    PHPEntry::Vector_t dbResutlsFunctions = PHPStorage::Instance()->FindGloablFunctions(partname, PHPStorage::FetchFlags_PartMatch);
    std::sort(dbResutlsFunctions.begin(), dbResutlsFunctions.end(), PHPEntry::LowerThan());
    
    // Suggest the super globals
    PHPEntry::Vector_t dbSGResutlsFunctions = PHPStorage::Instance()->FindSuperGloabls(partname, PHPStorage::FetchFlags_PartMatch);
    std::sort(dbSGResutlsFunctions.begin(), dbSGResutlsFunctions.end(), PHPEntry::LowerThan());
    
    // Merge the results
    std::merge(dbResutls.begin(), dbResutls.end(), dbResutlsFunctions.begin(), dbResutlsFunctions.end(),     std::back_inserter(v));
    std::merge(dbResutls.begin(), dbResutls.end(), dbSGResutlsFunctions.begin(), dbSGResutlsFunctions.end(), std::back_inserter(v));
    PHPEntry::Vector_t::iterator it;
    
    if ( !v.empty() && !IsUnitTestsMode()) {
        // the UI will remove any duplicate entries
        DoShowCompletionBox(v, partname);
    }
    return v;
}

void PHPCodeCompletion::DoSuggestMembersOf(const PHPEntry& entry, const wxString& partname, size_t filter_flags)
{
    PHPEntry::Vector_t res;
    DoGetMembersOf(entry, partname, filter_flags, res);
    if ( !res.empty() ) {
        DoShowCompletionBox(res, partname, filter_flags);
    }
}

void PHPCodeCompletion::DoShowCompletionBox(const std::vector<PHPEntry>& entries, const wxString& partname, size_t filter_flags)
{
    std::vector<TagEntryPtr> tags;
    wxString lcName, lcPartname;
    lcPartname = partname;
    lcPartname.MakeLower();

    // search for the old item
    for (size_t i=0; i<entries.size(); i++) {

        wxString name = entries.at(i).getName();
        // remove the $ prefix from all non static members
        if(entries.at(i).getKind() == PHP_Kind_Member && !(entries.at(i).getAttributes() & PHP_Attr_Static) ) {
            if(name.StartsWith(wxT("$"))) {
                name.Remove(0, 1);
            }
        }

        lcName = name;
        lcName.MakeLower();
        if(!lcName.StartsWith(lcPartname))
            continue;

        TagEntryPtr t = DoPHPEntryToTagEntry(entries.at(i));
        t->SetUserData(new PHPEntry(entries.at(i)));
        m_shownEntries.insert((PHPEntry*)t->GetUserData());
        tags.push_back(t);
    }

    if(tags.empty())
        return;

    std::sort(tags.begin(), tags.end(), _SAscendingSort());
    GetCurrentEditor()->ShowCompletionBox(tags, partname, true, this);
}

void PHPCodeCompletion::DoCleanup()
{
    SetCurrentEditor(NULL);
    std::set<PHPEntry*>::iterator iterEntries = m_shownEntries.begin();
    for(; iterEntries != m_shownEntries.end(); iterEntries++) {
        delete (*iterEntries);
    }
    m_shownEntries.clear();
    m_currentFileClasses.clear();
    m_currentFileFunctions.clear();
    m_currentFileVariables.clear();
}

void PHPCodeCompletion::OnCodeCompletionBoxDismissed(clCodeCompletionEvent& e)
{
    e.Skip();
    DoCleanup();
}

void PHPCodeCompletion::OnCodeCompletionGetTagComment(clCodeCompletionEvent& e)
{
    if ( PHPWorkspace::Get()->IsOpen() ) {
        
        TagEntryPtr tag = e.GetTagEntry();
        void * data = tag->GetUserData();
        
        if( data ) {
            std::set<PHPEntry*>::iterator iter = m_shownEntries.find((PHPEntry*)data);
            if(iter != m_shownEntries.end()) {

                wxString comment, docComment;
                docComment = (*iter)->getDocComment();
                if(docComment.IsEmpty() == false) {
                    comment << docComment;
                    comment.Trim().Trim(false);        // The Doc comment
                    comment << wxT("\n<hr>\n");  // HLine
                }

                wxFileName fn((*iter)->getFileName());
                fn.MakeRelativeTo(PHPWorkspace::Get()->GetFilename().GetPath());
                comment << fn.GetFullName() << wxT(" : ") << (*iter)->getLineNumber();
                e.SetTooltip(comment);
            }
        }
        
    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::DoInitialize(IEditor* editor, int pos)
{
    DoCleanup();

    SetCurrentEditor(editor);

    // Parse the entire file to get a complete list of all functions / classes
    if ( !PHPParser.parseString(true, editor->GetTextRange(0, editor->GetLength()), PHPGlobals::Type_Normal, true) ) {
        return;
    }

    // Store the results
    const std::vector<PHPEntry*>& classes = PHPParser.getClasses();
    std::map<wxString, PHPEntry> classesMap;
    for(size_t i=0; i<classes.size(); i++) {
        classesMap[classes.at(i)->getName()] = *classes.at(i);
    }
    SetCurrentFileClasses(classesMap);
    SetCurrentFileFunctions(PHPParser.getGlobalFunctions());
    SetCurrentFileVariables(PHPParser.getGlobalVariables());
    SetCurrentFileGlobals(PHPParser.getGlobals());
}

TagEntryPtr PHPCodeCompletion::DoPHPEntryToTagEntry(const PHPEntry& entry)
{
    TagEntryPtr t(new TagEntry());
    wxString name = entry.getName();

    if( entry.getKind() == PHP_Kind_Member && name.StartsWith(wxT("$")) && !(entry.getAttributes() & PHP_Attr_Static) ) {
        name.Remove(0, 1);
    }

    t->SetName(name);
    t->SetReturnValue(entry.getType());
    t->SetParent(entry.getParent());
    t->SetPattern(t->GetName());

    // Access
    if(entry.getAttributes() & PHP_Access_Private)
        t->SetAccess(wxT("private"));

    else if(entry.getAttributes() & PHP_Access_Protected)
        t->SetAccess(wxT("protected"));

    else
        t->SetAccess(wxT("public"));

    switch(entry.getKind()) {
    case PHP_Kind_Constant:
        t->SetKind(wxT("macro"));
        break;
    case PHP_Kind_Namespace:
        t->SetKind(wxT("namespace"));
        break;
    case PHP_Kind_Class:
        t->SetKind("class");
        break;
    case PHP_Kind_Member:
    case PHP_Kind_Variable:
        t->SetKind(wxT("variable"));
        break;
    case PHP_Kind_Function: {
        t->SetKind(wxT("function"));

        // prepare the function signature
        wxString signature;
        signature << wxT("(");
        for(size_t i=0; i<entry.getArguments().size(); i++) {
            PHPFuncArg arg = entry.getArguments().at(i);
            if(arg.getAttributes() & PHP_Attr_Reference)
                signature << wxT("&");
            signature << arg.getName() << wxT(", ");
        }
        if(entry.getArguments().empty() == false) {
            signature.RemoveLast(2);
        }
        signature << wxT(")");
        t->SetSignature(signature);
        t->SetPattern( t->GetName() + signature);
    }
    break;
    default:
        t->SetKind(wxT("variable"));
        break;
    }
    t->SetFlags(TagEntry::Tag_No_Signature_Format);
    return t;
}

void PHPCodeCompletion::DoCopyEntryProperties(const PHPEntry& src, PHPEntry& target)
{
    CHECK_PHP_WORKSPACE();
    target.setType(src.getType());
    target.setName(src.getName());
    target.setNameSpace(src.getNameSpace());
    target.setKind(src.getKind());
    target.setDocComment(src.getDocComment());
    target.setParent(src.getParent()); 
    target.setFileName(src.getFileName());
    target.setLineNumber(src.getLineNumber());
    if(target.getKind() == PHP_Kind_Function) {
        // copy the function arguments as well
        target.setArguments(src.getArguments());
    }
}

void PHPCodeCompletion::OnCodeComplete(clCodeCompletionEvent& e)
{
    if ( PHPWorkspace::Get()->IsOpen()) {
        if ( !CanCodeComplete(e) )
            return;

        IEditor *editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // we handle only .php files
            if(IsPHPFile(editor)) {
                // get the position
                DoCodeComplete(editor, e.GetPosition());
                return;
            }
        }
    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::OnFunctionCallTip(clCodeCompletionEvent& e)
{
    if ( PHPWorkspace::Get()->IsOpen()) {
        if ( !CanCodeComplete(e) )
            return;

        IEditor *editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // we handle only .php files
            if(IsPHPFile(editor)) {
                // get the position
                DoCalltip(editor, e.GetPosition());
                return;
            }
        }

    } else {
        e.Skip();
        
    }
}

void PHPCodeCompletion::OnFindSymbol(clCodeCompletionEvent& e)
{
    if ( PHPWorkspace::Get()->IsOpen() ) {
        if ( !CanCodeComplete(e) )
            return;

        IEditor *editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            std::vector<ResourceItem> items;
            items.reserve(50); // try to preallocate memory

            if(PHPWorkspace::Get()->IsOpen()) {
                PHPStorage::Instance()->FindSymbol(e.GetString(), items);

            } else if(IsPHPFile(editor)) {
                // Try it by parsing the local file
                if ( !PHPParser.parseFile(true, editor->GetFileName().GetFullPath(), PHPGlobals::Type_Normal, true) ) {
                    return;
                }
                DoFindSymbolFromLocalParse(items, e.GetString());

            } else {
                e.Skip();
                return;
            }

            // Display matches to the user
            // FIXME: we need to open a dialog here
            
            // if(items.size() > 1) {
            //     // Do something with the matches
            //     OpenResourceDlg dlg(FRAME, items, m_manager);
            //     if(dlg.ShowModal() == wxID_OK) {
            //         DoOpenResource(dlg.GetSelectedItem());
            //     }
            // } else if(items.size() == 1) {
            //     DoOpenResource( &items.at(0) );
            // }
        }

    } else {
        e.Skip();
    }
}

void PHPCodeCompletion::DoOpenResource(const ResourceItem* itemData)
{
    CHECK_PHP_WORKSPACE();
    if(itemData) {
        if(m_manager->OpenFile(itemData->filename.GetFullPath())) {
            if(itemData->type != PHP_Kind_File) {
                IEditor *editor = m_manager->GetActiveEditor();
                if(editor) {
                    m_manager->FindAndSelect(itemData->name, itemData->name, editor->PosFromLine(itemData->line - 1));
                }
            }
        }
    }
}

void PHPCodeCompletion::DoFindSymbolFromLocalParse(std::vector<ResourceItem>& items, const wxString &name)
{
//    // First look at the classes
//    std::vector<PHPEntry*> &classes = PHPParser.getClasses();
//    for(size_t i=0; i<classes.size(); i++) {
//        if(classes.at(i)->getName().CmpNoCase(name) == 0) {
//            ResourceItem resourceItem;
//            QDBMResourceItem item(*classes.at(i));
//            item.ToResourceItem(resourceItem);
//            items.push_back(resourceItem);
//        }
//
//        const std::vector<PHPEntry> &entries = classes.at(i)->getEntries();
//        for(size_t j=0; j<entries.size(); j++) {
//            if(entries.at(j).getName().CmpNoCase(name) == 0) {
//                ResourceItem resourceEntryItem;
//                QDBMResourceItem entryItem(entries.at(j));
//                entryItem.ToResourceItem(resourceEntryItem);
//                items.push_back(resourceEntryItem);
//            }
//        }
//    }
//
//    // Go over the functions table
//    std::map<wxString, PHPEntry> globals = PHPParser.getGlobals();
//    std::map<wxString, PHPEntry>::iterator iter = globals.find(name);
//    if(iter != globals.end()) {
//        ResourceItem resourceItem;
//        QDBMResourceItem item(iter->second);
//        item.ToResourceItem(resourceItem);
//        items.push_back(resourceItem);
//    }
}

//bool PHPCodeCompletion::DoExpandFunctionReturnValue(PHPEntry* entry, std::map<wxString, PHPEntry>& locals, bool &stop)
//{
//    stop = true;
//    wxString  currentNameSpace = PHPParser.getCurrentNamespace();
//
//    // Check the local file's function table
//    bool success = false;
//    std::map<wxString, PHPEntry>::const_iterator iter = GetCurrentFileGlobals().find(entry->getName());
//    if(iter != GetCurrentFileGlobals().end()) {
//        DoCopyEntryProperties(iter->second, *entry);
//        success = true;
//    }
//
//    // If no match in the local file's table and a workspace is opened, try
//    // so search for the function in the database
//    if(!success && PHPWorkspace::Instance()->IsOpen()) {
//        if(PHPStorage::Instance()->FindFunction(entry->getParent(), currentNameSpace, entry->getName(), PHPStorage::FetchFlags_ExactMatch, *entry))
//            success = true;
//    }
//
//    // Incase we managed to find a match, try to further expand the return value
//    // incase we are returning a local variable from the functions' body:
//    // <code>
//    // function foo() {
//    //    $a = new Bar;
//    //    return $a;
//    // }
//    // foo()->
//    // </code>
//    // the return value of the function is $a, but what we really want is 'Bar'
//    if(success) {
//        wxString entryType = entry->getType();
//        if(entryType.StartsWith(wxT("$"))) {
//            // the return type is not fully expanded
//            // Search the locals table and try to replace it with
//            // the actual type
//            std::map<wxString, PHPEntry>::const_iterator iter = locals.find(entryType);
//            if(iter != locals.end()) {
//                DoCopyEntryProperties(iter->second, *entry);
//                stop = (entryType == entry->getType()); // Avoid recursion by making sure that the new type is different from the new expanded type
//                return true;
//            }
//
//        } else if(PHPWorkspace::Instance()->IsOpen() && !entry->getTypeScope().IsEmpty()) {
//            // Either a type (class) or a function
//            // we might end up here in case of such code:
//            // <code>
//            // function foo() {
//            //     $a = new Bar;
//            //     return $a;
//            // }
//            // function Baz() {
//            //     return foo();
//            // }
//            // Baz()->
//            // </code>
//            if(PHPStorage::Instance()->FindFunction(entry->getTypeScope(), currentNameSpace, entryType, PHPStorage::FetchFlags_ExactMatch, *entry)) {
//                stop = (entryType == entry->getType()); // Avoid recursion by making sure that the new type is different from the new expanded type
//                return true;
//            }
//
//            // No match for function, try to locate a class
//            PHPEntry cls;
//            if(PHPStorage::Instance()->FindClass(entryType, currentNameSpace, PHPStorage::FetchFlags_ExactMatch, cls)) {
//                stop = true;
//                return true;
//            }
//        } else {
//            stop = true;
//        }
//    }
//    stop = true;
//    return false;
//}

void PHPCodeCompletion::OnTypeinfoTip(clCodeCompletionEvent& e)
{
    if ( PHPWorkspace::Get()->IsOpen() ) {
        if ( !CanCodeComplete(e) )
            return;

        IEditor *editor = dynamic_cast<IEditor*>(e.GetEditor());
        if(editor) {
            // we handle only .php files
            if(IsPHPFile(editor)) {
                // get the position
                DoTypeinfoTip(editor, e.GetPosition());
                return;
            }
        }
    
    } else {
        e.Skip();
        
    }
}

void PHPCodeCompletion::OnDismissTooltip(wxCommandEvent& e)
{
    IEditor *editor = dynamic_cast<IEditor*>(e.GetEventObject());
    if(editor) {
        // we handle only .php files
        if(IsPHPFile(editor)) {
            // get the position
            if ( m_typeInfoTooltip ) {
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
        return DoFindDefinition(editor, pos);
    }
    return NULL;
}

PHPLocationPtr PHPCodeCompletion::DoFindDefinition(IEditor* editor, int pos)
{
    CHECK_PHP_WORKSPACE_RET_NULL();
    // adjust the position to match a complete word
    pos = editor->GetSTC()->WordEndPosition(pos, true);

    // Keep the displayed name
    DoInitialize(editor, pos);

    // Second parse is needed to get the locals at the current position
    if ( !PHPParser.parseString(true, editor->GetTextRange(0, pos), PHPGlobals::Type_Normal) ) {
        return NULL;
    }

    // Extract the expression
    wxString expr = DoGetExpression(editor, pos);
    if ( !PHPParser.parseString(true, PHP_PREFIX_WITH_SPACE + expr, PHPGlobals::Type_Expr) ) {
        return NULL;
    }
    
    Token* token = PHPParser.makeTokenList( PHP_PREFIX_WITH_SPACE + expr, PHPParser.getLocals() );
    if ( ! token ) { return NULL; }
    
    TDeleter<Token> del( token );
    PHPEntry match;
    size_t flags;
    if ( ParseTokenList( token, false, match, flags ) ) {
        PHPLocationPtr loc(new PHPLocation);
        loc->filename   = match.getFileName();
        loc->linenumber = match.getLineNumber() - 1;
        loc->what       = match.getName();
        return loc;
    }
    return NULL;
}

void PHPCodeCompletion::OnCodeCompleteLangKeywords(clCodeCompletionEvent& e)
{
    e.Skip();
}

bool PHPCodeCompletion::ParseTokenList(Token* token, bool deepResolvingLastToken, PHPEntry& match, size_t& filter_flags)
{
    filter_flags = 0; // Display all
    
    if ( !m_unitTestsMode ) {
        CHECK_PHP_WORKSPACE_RET_FALSE();
    }
    
    CHECK_PTR_RET_FALSE(token);
    
    PHPEntry currentScope;
    wxString currentScoptPath;
    
    // Build a full path string to the current scope (i.e. the location where the caret is)
    // For example, if the caret is represented by | then the following code is using the fullpath of: \Bla\Foo
    // namespace Bla;
    //  class Foo {
    //      function bar() {
    //          | 
    //      }
    //  }
    //
    currentScoptPath << "\\" << PHPParser.getCurrentNamespace();
    if ( PHPParser.getCurrentClass() && !PHPParser.getCurrentClass()->isGlobalScopeClass() ) {
        currentScoptPath << "\\" << PHPParser.getCurrentClass()->getName();
    }
    PHPStorage::Instance()->FindByFullPath(currentScoptPath, currentScope);
    
    Token* cur = token;
    while ( cur ) {
        
        // First we try the local table
        if ( cur->isHead() && cur->getName().StartsWith("$") ) {
            PHPGlobals::EntryMap_t locals = PHPParser.getLocals();
            PHPGlobals::EntryMap_t::iterator iter = locals.find( cur->getName() );
            if ( iter != locals.end() ) {
                wxString new_type = iter->second.getType();
                new_type.Trim().Trim(false);
                cur->setName( new_type );
                cur->setStartsWithNSOperator( new_type.StartsWith("\\") );
            }
        } else if ( cur->isHead() && cur->getFlags() & Token::kSelf ) {
            // restrict to self:: options
            filter_flags |= kCC_SELF;
            
        } else if ( cur->isHead() && cur->getFlags() & Token::kStatic ) {
            // restrict to static:: options
            filter_flags |= kCC_STATIC;
            
        } else if ( !cur->isHead() ) {
            // remove the self restriction
            filter_flags &= ~kCC_SELF;
            filter_flags &= ~kCC_STATIC;
        }

        // Fix the 'use Full\Path as NewName;' statements
        if ( PHPParser.getUseTable().count(cur->getName()) ) {
            // the token name is an alias, use the real name
            wxString new_name = PHPParser.getUseTable().find(cur->getName())->second;
            cur->setName( new_name );
            cur->setStartsWithNSOperator( new_name.StartsWith("\\") );
        }

        if ( cur->isHead() && cur->isStartsWithNSOperator() ) {
            
            if ( !PHPStorage::Instance()->FindEntryByNameAndNS(cur->getName(), "", match) ) {
                return false;
            }
            cur->setDbId(match.getDbId());
            
        } else if ( cur->isHead() && cur->getName() == "parent" && cur->getOperator() == T_PAAMAYIM_NEKUDOTAYIM ) {
            // parent::
            
            // to make sure we don't get into an infinite loop don't call continue unless the new name is not "parent"
            if ( currentScope.getInherits() == "parent" ) {
                return false;
                
            } else {
                cur->setName( currentScope.getInherits() );
                cur->setStartsWithNSOperator( currentScope.getInherits().StartsWith("\\") );
                cur->setIsParent(true);
                continue;
            }
         
        } else if ( cur->isHead() ) {
            // relative path (cur->getName() does not start with NS operator)
            
            // First try the matching for the current scope (NS\\Class)
            // if this fails, try the namespace only (
            
            if ( currentScope.isNamespace() ) {
                // the current scope is namespace (not inside class)
                if ( !PHPStorage::Instance()->FindByFullPath(currentScope.getContainerPath() + "\\" + cur->getName(), match) )
                    return false;
                    
            } else if ( !PHPStorage::Instance()->FindEntryByNameAndParentId(currentScope.getDbId(), cur->getName(), match) ) {
                if ( !PHPStorage::Instance()->FindEntryByNameAndParentId(currentScope.getParentDbId(), cur->getName(), match) ) {
                    // Last try: search for the global namespace for function only
                    if ( !PHPStorage::Instance()->FindEntryByNameAndParentId(-1, cur->getName(), PHP_Kind_Function, match) ) {
                        return false;
                    }
                }
            }
            cur->setDbId(match.getDbId());

        } else {
            if ( !PHPStorage::Instance()->FindEntryByNameAndParentId(cur->getPrev()->getDbId(), cur->getName(), match) ) {
               return false;
            }
            cur->setDbId( match.getDbId() );
        }
        
        // If the match is not a scope (namespace or class) - resolve it further
        wxString type = match.getType();
        if ( !match.isScope() && !type.IsEmpty() ) {
            // When using this parsing code to resolve 'goto definition' 
            // there is no need to perform a "deep" resolve of the last token
            if ( !(cur->isLast() && !deepResolvingLastToken) ) {
                bool isAbs = type.StartsWith("\\");
                if ( !isAbs ) {
                    type.Prepend("\\").Prepend(PHPParser.getCurrentNamespace());
                }
                
                if ( !PHPStorage::Instance()->FindEntryByNameAndNS( type, "", match ) ) {
                    return false;
                }
                cur->setDbId( match.getDbId() );
            }
        }
        
        // apply display restrictions depend on the current token being processed
        if ( cur->isParent() ) {
            filter_flags |= kCC_FUNCTIONS_ONLY;
            
        } else {
            filter_flags &= ~kCC_FUNCTIONS_ONLY;
        }
        cur = cur->getNext();
    }
    return true;
}

PHPEntry PHPCodeCompletion::GetPHPEntryUnderTheAtPos(IEditor* editor, int pos)
{
    if ( !PHPWorkspace::Get()->IsOpen() )
        return PHPEntry();
        
    pos = editor->GetSTC()->WordEndPosition(pos, true);
    DoInitialize(editor, pos);

    // Second parse is needed to get the locals at the current position
    if ( !PHPParser.parseString(true, editor->GetTextRange(0, pos), PHPGlobals::Type_Normal) )
        return PHPEntry();

    // Extract the expression
    wxString expr = DoGetExpression(editor, pos);
    if ( !PHPParser.parseString(true, PHP_PREFIX_WITH_SPACE + expr, PHPGlobals::Type_Expr) )
        return PHPEntry();
    
    Token* token = PHPParser.makeTokenList( PHP_PREFIX_WITH_SPACE + expr, PHPParser.getLocals() );
    if ( !token ) {
        return PHPEntry();
    }
    
    TDeleter<Token> del( token );
    PHPEntry match;
    size_t flags;
    if ( ParseTokenList( token, false, match, flags ) ) {
        return match;
    }
    return PHPEntry();
}

bool PHPCodeCompletion::CanCodeComplete(clCodeCompletionEvent& e) const
{
    int pos = e.GetPosition();
    if ( pos ) pos -= 1;
    IEditor *editor = dynamic_cast<IEditor *>(e.GetEditor());
    if ( !editor )
        return false;
    
    // we can get style 0 if we added chars and they were not styled just yet
    // sd we use the first style near our position (backward)
    int lineNumber = editor->LineFromPos(pos);
    int lineStartPos = editor->PosFromLine( lineNumber );
    
    if ( lineStartPos > pos )
        return false;
        
    int styleAt (0);
    int retryLeft ( pos - lineStartPos + 2 );
    while ( (styleAt == 0) && retryLeft && pos > 0 ) {
        styleAt = editor->GetStyleAtPos(pos);
        if ( styleAt == 0 ) {
            --pos;
        }
        --retryLeft;
    }

    return (editor && 
            !e.IsInsideCommentOrString() && 
            IsPHPSection( styleAt ) && 
            !IsPHPCommentOrString( styleAt ));
}

bool PHPCodeCompletion::TestSuggestMembers(const wxString& fileContent, const wxString& expression, PHPEntry::Vector_t& matches)
{
    // perform a parse to get list of locals
    PHPParser.parseString(true, fileContent, PHPGlobals::Type_Normal);
    Token* token = PHPParser.makeTokenList( PHP_PREFIX_WITH_SPACE + expression, PHPParser.getLocals() );
    
    CHECK_PTR_RET_FALSE( token );
    
    Token* last = token->getLast();
    CHECK_PTR_RET_FALSE( last );
    last->disonnectSelf();
    
    TDeleter<Token> del( token );
    
    wxString partial_word;
    // if the token list ends with any operator, 
    // we perform 'code-complete' otherwise, we perform
    // word-completion
    if ( !last->endsWithOperator() ) {
        partial_word = last->getName();
        if ( !last->disonnectSelf() ) {
            // This means that last == head
            if( last->isStartsWithNSOperator() ) {
                matches = DoSuggestGlobals(partial_word, "\\");
                return true;
                
            } else {
                matches = DoSuggestGlobals(partial_word, PHPParser.getCurrentNamespace());
                return true;
            }
            return false;
            
        } else {
            wxDELETE(last);
        }
    } 
    
    PHPEntry match;
    size_t flags;
    if ( ParseTokenList( token, true, match, flags ) ) {
        DoGetMembersOf(match, "", flags, matches);
        return true;
    }
    return false;
}

void PHPCodeCompletion::DoGetMembersOf(const PHPEntry& entry, const wxString& partname, size_t filter_flags, PHPEntry::Vector_t& res)
{
    size_t fetchFlags = PHPStorage::FetchFlags_PartMatch|PHPStorage::FetchFlags_FullClass;
    if ( filter_flags & kCC_FUNCTIONS_ONLY ) {
        fetchFlags |= PHPStorage::FetchFlags_FunctionsOnly;
    }
    
    if ( filter_flags & kCC_SELF ) {
        fetchFlags |= PHPStorage::FetchFlags_Self;
    }
    if ( filter_flags & kCC_STATIC ) {
        fetchFlags |= PHPStorage::FetchFlags_Static;
    }
    
    if ( entry.isNamespace() ) {
        res = PHPStorage::Instance()->FetchNamespaceEntries(entry.getContainerPath(), partname, fetchFlags);
        
    } else {
        
        // "class" make sure we display completion entries from inheritance as well (or traits)
        PHPEntry::LongVector_t parentsIds;
        if ( filter_flags & kCC_SELF ) {
            // no inheritance, on "this"
            parentsIds.push_back( entry.getDbId() );
            res = PHPStorage::Instance()->FindEntriesByParentId( entry.getDbId(), fetchFlags );
            
        } else {
            PHPStorage::Instance()->GetParents(entry, parentsIds);
            PHPEntry::Vector_t tmpRes;
            for(size_t i=0; i<parentsIds.size(); ++i) {
                if ( !partname.IsEmpty() ) {
                    tmpRes = PHPStorage::Instance()->FindEntriesByParentIdAndName(parentsIds.at(i), partname, fetchFlags); 
                } else {
                    size_t tmpFlags = fetchFlags;
                    tmpFlags &= ~PHPStorage::FetchFlags_PartMatch; // exclude partial match
                    tmpRes = PHPStorage::Instance()->FindEntriesByParentId(parentsIds.at(i), tmpFlags); 
                }
                res.insert(res.end(), tmpRes.begin(), tmpRes.end());
            }
        }
        
        // Remove duplicate entries from the result
        std::sort(res.begin(), res.end(), PHPEntry::LowerThanByDbId());
        PHPEntry::Vector_t::iterator iterEnd = std::unique(res.begin(), res.end(), PHPEntry::Predicate());
        res.resize( std::distance(res.begin(), iterEnd) );
    }
}

PHPEntry PHPCodeCompletion::GetClassNearPosition(IEditor* editor, int pos)
{
    if ( !PHPWorkspace::Get()->IsOpen() )
        return PHPEntry();

    pos = editor->GetSTC()->WordEndPosition(pos, true);
    if ( !PHPParser.parseString(true, editor->GetTextRange(0, pos), PHPGlobals::Type_Normal, true) ) {
        return PHPEntry();
    }

    if ( PHPParser.getCurrentClass() ) {
        const PHPEntry *entry = PHPParser.getCurrentClass();
        if ( entry->isClass() ) {
            PHPEntry cls;
            if ( PHPStorage::Instance()->FindClass(entry->getName(), entry->getNameSpace(), PHPStorage::FetchFlags_ExactMatch, cls) ) {
                return cls;
            }
        }
    }
    return PHPEntry();
}
