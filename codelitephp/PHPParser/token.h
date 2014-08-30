#ifndef TOKEN_H
#define TOKEN_H

#include <wx/string.h>

class Token
{
    wxString m_name;
    wxString m_scope;
    long     m_dbId;
    Token*   m_next;
    Token*   m_prev;
    int      m_operator;
    bool     m_startsWithNSOperator;
    wxString m_expression;
    bool     m_isParent;
    size_t   m_flags;
public:
    enum {
        kSelf   = 0x00000001,
        kStatic = 0x00000002,
        kThis   = 0x00000004,
    };

public:
    Token();
    virtual ~Token();

    Token* getLast();

    bool endsWithOperator() const {
        return m_operator != 0;
    }

    void setFlags(size_t flags) {
        this->m_flags = flags;
    }
    size_t getFlags() const {
        return m_flags;
    }
    void setExpression(const wxString& expression) {
        this->m_expression = expression;
    }
    const wxString& getExpression() const {
        return m_expression;
    }
    void setStartsWithNSOperator(bool startsWithNSOperator) {
        this->m_startsWithNSOperator = startsWithNSOperator;
    }
    bool isStartsWithNSOperator() const {
        return m_startsWithNSOperator;
    }
    bool isHead() const {
        return m_prev == NULL;
    }
    bool isLast() const {
        return m_next == NULL;
    }

    void setNext(Token* next) {
        this->m_next = next;
    }
    void setOperator(int oper) {
        this->m_operator = oper;
    }
    void setPrev(Token* prev) {
        this->m_prev = prev;
    }
    Token* getNext() {
        return m_next;
    }
    int getOperator() const {
        return m_operator;
    }
    Token* getPrev() {
        return m_prev;
    }
    void setName(const wxString& name) {
        this->m_name = name;
    }
    void setScope(const wxString& scope) {
        this->m_scope = scope;
    }
    const wxString& getName() const {
        return m_name;
    }
    const wxString& getScope() const {
        return m_scope;
    }
    bool isResolved() const {
        return m_dbId != wxNOT_FOUND;
    }
    void setDbId(long dbId) {
        this->m_dbId = dbId;
    }
    long getDbId() const {
        return m_dbId;
    }
    bool more() const {
        return m_next != NULL;
    }
    Token* append() {
        Token* token = new Token();
        m_next = token;
        token->m_prev = this;
        return token;
    }

    bool disonnectSelf();
    bool isValid() const {
        return !m_name.IsEmpty() || (isHead() && isStartsWithNSOperator());
    }

    /**
     * @brief shrink the list by combining all namespaces into a single namespace
     */
    void shrink();

    bool isNamespace() const ;

    void setIsParent(bool isParent) {
        this->m_isParent = isParent;
    }
    /**
     * @brief is parent keyword?
     * @return
     */
    bool isParent() const {
        return m_isParent;
    }
};
#endif // TOKEN_H
