#include "parsedtoken.h"

ParsedToken::ParsedToken()
	: m_isTemplate(false)
	, m_subscriptOperator(false)
	, m_next(NULL)
	, m_prev(NULL)
{
}

ParsedToken::~ParsedToken()
{
}

void ParsedToken::DeleteTokens(ParsedToken* head)
{
	if( !head ) {
		return;
	}

	ParsedToken *n     (head);
	ParsedToken *tmpPtr(NULL);
	while( n ) {
		tmpPtr = n->GetNext();
		delete n;

		n = tmpPtr;
	}
}
