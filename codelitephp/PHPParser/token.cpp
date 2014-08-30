#include "token.h"
#include "php_expr_parser.cpp.h"

Token::Token()
    : m_dbId(wxNOT_FOUND)
    , m_next(NULL)
    , m_prev(NULL)
    , m_operator(0)
    , m_startsWithNSOperator(false)
    , m_isParent(false)
    , m_flags(0)
{
}

Token::~Token()
{
    wxDELETE(m_next);
}

bool Token::disonnectSelf()
{
    if ( isHead() ) {
        return false;
    }

    // Remove it (we definitly have m_prev, otherwise, isHead() is true)
    this->m_prev->m_next = this->m_next;

    if ( this->m_next ) {
        this->m_next->m_prev = this->m_prev;
    }
    return true;
}

Token* Token::getLast()
{
    Token* cur = this;
    while ( cur ) {
        if ( cur->m_next ) {
            cur = cur->m_next;
        } else {
            break;
        }
    }
    return cur;
}

void Token::shrink()
{
    if ( !isHead() )
        return;
    
    Token* current = this;
    while ( current && current->m_next && current->isNamespace() ) {
        Token* removedToken = current->m_next;
        if ( removedToken->isLast() && removedToken->getName().IsEmpty() )
            break;
            
        // Join this token with the next one
        current->m_name << "\\" << removedToken->getName();
        current->setOperator( removedToken->getOperator() );
        
        // Free the next token
        if ( removedToken->m_next ) {
            removedToken->m_next->m_prev = current;
        }
        
        current->m_next = removedToken->m_next;
        
        removedToken->m_next = NULL;
        removedToken->m_prev = NULL;
        wxDELETE( removedToken );
    }
}

bool Token::isNamespace() const
{
    return getOperator() == T_NS_SEPARATOR;
}
