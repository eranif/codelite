#ifndef PARSEDTOKEN_H
#define PARSEDTOKEN_H

#include <wx/string.h>
#include <vector>

class ParsedToken
{
	wxString      m_type;
	wxString      m_typeScope;
	wxString      m_oper;
	bool          m_isTemplate;
	wxString      m_templateInitialization;
	wxString      m_templateArgList;
	wxString      m_name;
	bool          m_subscriptOperator;
	ParsedToken*  m_next;
	ParsedToken*  m_prev;

public:
	static void DeleteTokens( ParsedToken *head );

public:
	ParsedToken();
	~ParsedToken();

	void SetName(const wxString& name) {
		this->m_name = name;
	}
	const wxString& GetName() const {
		return m_name;
	}
	void SetNext(ParsedToken* next) {
		this->m_next = next;
	}
	void SetPrev(ParsedToken* prev) {
		this->m_prev = prev;
	}
	ParsedToken* GetNext() {
		return m_next;
	}
	ParsedToken* GetPrev() {
		return m_prev;
	}
	void SetSubscriptOperator(bool subscriptOperator) {
		this->m_subscriptOperator = subscriptOperator;
	}
	bool GetSubscriptOperator() const {
		return m_subscriptOperator;
	}

	void SetIsTemplate(bool isTemplate) {
		this->m_isTemplate = isTemplate;
	}
	bool GetIsTemplate() const {
		return m_isTemplate;
	}
	void SetOperator(const wxString& oper) {
		this->m_oper = oper;
	}
	void SetTemplateArgList(const wxString& templateArgList) {
		this->m_templateArgList = templateArgList;
	}
	void SetTemplateInitialization(const wxString& templateInitialization) {
		this->m_templateInitialization = templateInitialization;
	}
	void SetTypeName(const wxString& type) {
		this->m_type = type;
	}
	void SetTypeScope(const wxString& typeScope) {
		this->m_typeScope = typeScope;
	}
	const wxString& GetOperator() const {
		return m_oper;
	}
	const wxString& GetTemplateArgList() const {
		return m_templateArgList;
	}
	const wxString& GetTemplateInitialization() const {
		return m_templateInitialization;
	}
	const wxString& GetTypeName() const {
		return m_type;
	}
	const wxString& GetTypeScope() const {
		return m_typeScope;
	}
};

#endif // PARSEDTOKEN_H
