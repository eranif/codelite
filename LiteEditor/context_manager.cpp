#include "context_manager.h"
#include "context_cpp.h"
#include "context_text.h"
#include "context_base.h"
#include "generic_context.h"
#include "editor_config.h"
 
ContextManager::ContextManager()
{
	// register available context
	m_contextPool[wxT("C++")] = ContextBasePtr( new ContextCpp() );
	m_contextPool[wxT("Text")] = ContextBasePtr( new ContextText() );

	// load generic lexers
	EditorConfig::ConstIterator iter = EditorConfigST::Get()->LexerBegin();
	 for(; iter != EditorConfigST::Get()->LexerEnd(); iter++){
		LexerConfPtr lex = iter->second;
		//skip hardcoded lexers
		if(lex->GetName() != wxT("C++") && lex->GetName() != wxT("Text")){
			std::pair<wxString, ContextBasePtr> entry(lex->GetName(), new ContextGeneric(lex->GetName()));
			m_contextPool.insert(entry);
		}
	}
}

ContextManager::~ContextManager()
{
}

ContextBasePtr ContextManager::NewContext(wxWindow *parent, const wxString &lexerName)
{
	// this function is actually a big switch ....
	std::map<wxString, ContextBasePtr>::iterator iter = m_contextPool.find(lexerName);
	if( iter == m_contextPool.end()){
		return m_contextPool[wxT("Text")]->NewInstance((LEditor*)parent);
	}

	return iter->second->NewInstance((LEditor*)parent);
}
