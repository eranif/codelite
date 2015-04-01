#include "JSLookUpTable.h"
#include <algorithm>
#include "JSLexerAPI.h"
#include <algorithm>
#include <wx/tokenzr.h>
#include "JSSourceFile.h"

JSLookUpTable::JSLookUpTable()
    : m_objSeed(0)
{
    m_scopes = &m_actualScopes;
    InitializeGlobalScope();
    m_scopes->push_back(m_globalScope);
}

JSLookUpTable::~JSLookUpTable() {}

void JSLookUpTable::AddObject(JSObject::Ptr_t obj)
{
    if(obj->IsFunction() || obj->IsClass()) {
        // if the object is a function, we add it under its name
        std::map<wxString, JSObject::Ptr_t>::iterator iter = m_classes.find(obj->GetName());
        if(iter != m_classes.end()) m_classes.erase(iter);
        m_classes.insert(std::make_pair(obj->GetName(), obj));
    } else {
        // This usually means that we are registering an annonymous object
        std::map<wxString, JSObject::Ptr_t>::iterator iter = m_classes.find(obj->GetType());
        if(iter != m_classes.end()) m_classes.erase(iter);
        m_classes.insert(std::make_pair(obj->GetType(), obj));
    }
}

JSObject::Ptr_t JSLookUpTable::CurrentScope() const { return m_scopes->at(m_scopes->size() - 1); }

wxString JSLookUpTable::CurrentPath() const { return CurrentScope()->GetType(); }

void JSLookUpTable::PopScope()
{
    // Never pop the global scope
    if(m_scopes->size() > 1) m_scopes->pop_back();
}

void JSLookUpTable::PushScope(JSObject::Ptr_t scope)
{
    if(scope && scope->IsFunction()) {
        m_scopes->push_back(scope);
    }
}

wxString JSLookUpTable::MakePath(const wxString& pathLastPart)
{
    wxString curpath = CurrentPath();
    if(curpath.IsEmpty()) {
        return pathLastPart;
    } else {
        curpath << "." << pathLastPart;
    }
    return curpath;
}

JSObject::Ptr_t JSLookUpTable::FindClass(const wxString& path) const
{
    wxArrayString types = ::wxStringTokenize(path, "|", wxTOKEN_STRTOK);
    if(types.size() == 1) {
        return DoFindSingleType(path);

    } else {
        // Multiple types, create a mega objects from all the various types and return it
        JSObject* result(NULL);
        for(size_t i = 0; i < types.size(); ++i) {
            JSObject::Ptr_t o = DoFindSingleType(types.Item(i));
            if(o) {
                if(!result) {
                    result = new JSObject();
                }
                // Merge the object into 'results'
                result->AddType(o->GetType(), false);
                JSObject::Map_t props = GetObjectProperties(o);
                result->GetProperties().insert(props.begin(), props.end());
                const std::set<wxString>& extends = o->GetExtends();
                for(std::set<wxString>::const_iterator iter = extends.begin(); iter != extends.end(); ++iter) {
                    result->Extends(*iter);
                }
            }
        }
        return result;
    }
}

JSObject::Ptr_t JSLookUpTable::DoFindSingleType(const wxString& type) const
{
    std::map<wxString, JSObject::Ptr_t>::const_iterator iter = m_classes.find(type);
    if(iter != m_classes.end()) return iter->second;

    // try the temp classes
    iter = m_tmpClasses.find(type);
    if(iter != m_tmpClasses.end()) return iter->second;
    return NULL;
}

void JSLookUpTable::SetTempScope(JSObject::Ptr_t obj)
{
    // only if the new scope is a function we use it
    if(obj && obj->IsFunction()) {
        m_tempScopes.clear();
        m_tempScopes.push_back(m_globalScope);
        m_tempScopes.push_back(obj);
        m_scopes = &m_tempScopes;
    } else {
        SwapScopes();
    }
}

void JSLookUpTable::SwapScopes() { m_scopes = &m_actualScopes; }

void JSLookUpTable::Print()
{
    std::for_each(
        m_classes.begin(), m_classes.end(), [&](const std::pair<wxString, JSObject::Ptr_t>& p) { p.second->Print(0); });
}

JSObject::Map_t JSLookUpTable::GetVisibleVariables()
{
    // we start from the inner scope so incase we have a collision
    // the inner scope variable "wins"
    JSObject::Map_t variables;

    for(int i = (int)m_scopes->size() - 1; i >= 0; --i) {
        JSObject::Ptr_t scope = m_scopes->at(i);
        const JSObject::Map_t& scopeVariables = scope->GetVariables();
        variables.insert(scopeVariables.begin(), scopeVariables.end());
    }
    return variables;
}

wxString JSLookUpTable::GenerateNewType()
{
    wxString type;
    type << "__object" << ++m_objSeed;
    return type;
}

void JSLookUpTable::Clear()
{
    // Clear this one
    m_classes.clear();
    m_tmpClasses.clear();
    m_actualScopes.clear();
    m_tempScopes.clear();
    m_scopes = NULL;
}

void JSLookUpTable::PrepareLookup()
{
    m_actualScopes.clear();
    m_tempScopes.clear();
    m_scopes = &m_actualScopes;
    m_globalScope.Reset(NULL); // Delete the current global scope
    InitializeGlobalScope();
    PopulateWithGlobals(); // populate the global scope with the classes
    m_scopes->push_back(m_globalScope);
}

JSObject::Ptr_t JSLookUpTable::NewObject() const
{
    JSObject::Ptr_t obj(new JSObject());
    return obj;
}

JSObject::Ptr_t JSLookUpTable::NewFunction() const
{
    JSObject::Ptr_t obj(new JSObject());
    obj->SetFunction();
    return obj;
}

void JSLookUpTable::PopulateWithGlobals()
{
    // Register the global objects as variables of the global scope
    {
        JSObject::Ptr_t objTemplate = FindClass("Document");
        if(objTemplate) {
            JSObject::Ptr_t objInstance = objTemplate->NewInstance("document");
            m_globalScope->AddVariable(objInstance);
        }
    }
    {
        JSObject::Ptr_t objTemplate = FindClass("Window");
        if(objTemplate) {
            JSObject::Ptr_t objInstance = objTemplate->NewInstance("window");
            m_globalScope->AddVariable(objInstance);
        }
    }

    // And finally, add all the classes we found during the initial parsing as properties
    // of the global scope
    m_globalScope->GetProperties().insert(m_classes.begin(), m_classes.end());
}

void JSLookUpTable::InitializeGlobalScope()
{
    m_globalScope.Reset(NULL);
    m_globalScope = NewFunction();
    m_globalScope->SetGlobalScope();
    SetThis(m_globalScope);
}

void JSLookUpTable::CopyClassTable(JSLookUpTable::Ptr_t other, bool move)
{
    m_tmpClasses.clear();
    m_classes.clear();
    m_classes.insert(other->m_classes.begin(), other->m_classes.end());
    m_tmpClasses.insert(other->m_tmpClasses.begin(), other->m_tmpClasses.end());
    if(move) {
        other->m_classes.clear();
        other->m_tmpClasses.clear();
    }
}

void JSLookUpTable::ClearTempClassTable() { m_tempScopes.clear(); }

JSObject::Ptr_t JSLookUpTable::NewTempObject()
{
    JSObject::Ptr_t obj = NewObject();
    obj->AddType(GenerateNewType(), true);
    m_tmpClasses.insert(std::make_pair(obj->GetType(), obj));
    return obj;
}

JSObject::Map_t JSLookUpTable::GetObjectProperties(JSObject::Ptr_t o) const
{
    JSObject::Map_t properties;
    properties.insert(o->GetProperties().begin(), o->GetProperties().end());

    const std::set<wxString>& extends = o->GetExtends();
    std::for_each(extends.begin(), extends.end(), [&](const wxString& className) {
        JSObject::Ptr_t cls = FindClass(className);
        if(cls) {
            properties.insert(cls->GetProperties().begin(), cls->GetProperties().end());
        }
    });
    return properties;
}

JSThisLocker::JSThisLocker(JSLookUpTable::Ptr_t lookup, int exitDepth, JSSourceFile* source, JSObject::Ptr_t This)
    : m_lookup(lookup)
    , m_sourceFile(source)
    , m_exitDepth(exitDepth)
{
    if(!This->HasFlag(JSObject::kJS_AnonFunction)) {
        m_oldThis = m_lookup->GetThis();
        m_lookup->SetThis(This);
    } else {
        m_oldThis.Reset(NULL);
    }
}

JSThisLocker::~JSThisLocker()
{
    if(m_oldThis && m_sourceFile && (m_exitDepth == m_sourceFile->GetDepth())) {
        m_lookup->SetThis(m_oldThis);
    }
}
