#ifndef PARSEDTOKEN_H
#define PARSEDTOKEN_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <vector>

class TagsManager;
class ParsedToken
{
	wxString      m_type;
	wxString      m_typeScope;
	wxString      m_oper;
	bool          m_isTemplate;
	wxArrayString m_templateInitialization;
	wxArrayString m_templateArgList;
	wxString      m_name;
	bool          m_subscriptOperator;
	wxString      m_currentScopeName;
	ParsedToken*  m_next;
	ParsedToken*  m_prev;

public:
	static void DeleteTokens( ParsedToken *head );

public:
	ParsedToken();
	~ParsedToken();

	bool IsThis() const;
	void SetCurrentScopeName(const wxString& currentScopeName) {
		this->m_currentScopeName = currentScopeName;
		this->m_currentScopeName.Trim().Trim(false);
	}
	const wxString& GetCurrentScopeName() const {
		return m_currentScopeName;
	}
	void SetName(const wxString& name) {
		this->m_name = name;
		this->m_name.Trim().Trim(false);;
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
	ParsedToken* GetNext() const {
		return m_next;
	}
	ParsedToken* GetPrev() const {
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
		this->m_oper.Trim().Trim(false);
	}
	void SetTypeName(const wxString& type) {
		this->m_type = type;
		this->m_type.Trim().Trim(false);
	}
	void SetTypeScope(const wxString& typeScope) {
		this->m_typeScope = typeScope;
		this->m_typeScope.Trim().Trim(false);

		if(this->m_typeScope.IsEmpty()) {
			this->m_typeScope = wxT("<global>");
		}
	}

	const wxString& GetOperator() const {
		return m_oper;
	}
	const wxString& GetTypeName() const {
		return m_type;
	}
	const wxString& GetTypeScope() const {
		return m_typeScope;
	}
	void SetTemplateArgList(const wxArrayString& templateArgList) {
		this->m_templateArgList = templateArgList;
	}
	void SetTemplateInitialization(const wxArrayString& templateInitialization) {
		this->m_templateInitialization = templateInitialization;
	}
	const wxArrayString& GetTemplateArgList() const {
		return m_templateArgList;
	}
	const wxArrayString& GetTemplateInitialization() const {
		return m_templateInitialization;
	}

	wxString GetContextScope() const;
	wxString GetPath() const;
	void     RemoveScopeFromType();

	/**
	 * @brief replace the current type which is template with an actual type
	 */
	bool ResovleTemplate(TagsManager *lookup);

	/**
	 * @brief fix the template type with actual type
	 * This method aims to fix the template argument NOT the token's type.
	 * so it will fix code like this:
	 *
	 * template <typename T> class A {
	 *     typedef iterator<T> Iter;
	 * };
	 *
	 * And assuming iterator class:
	 *
	 * template <typename _T1> class iterator {
	 *    _T1 m_current;
	 * };
	 *
	 * so this method replaces _T1 with T which can then be replaced with the actual type
	 *
	 * @param lookup
	 */
	void ResolveTemplateType(TagsManager *lookup);

	/**
	 * @brief replace template argument with the actual type (std::vector<wxString>: _Tp --> wxString)
	 * @param templateArg the template argument to replace
	 */
	wxString TemplateToType( const wxString &templateArg );
};

class TokenContainer {
public:
	ParsedToken *head;
	ParsedToken *current;
	bool         rew;
	int          retries;

public:
	TokenContainer() : head(NULL), current(NULL), retries(0){}

	bool Rewind() {
		if(retries > 3)
			return false;

		return rew;
	}

	void SetRewind(bool r) {
		if(r) {
			retries++;
		} else {
			rew = r;
		}

		if(r && retries <= 3) { // we allow maximum of 3 retries
			rew = r;
		}
	}
};

#endif // PARSEDTOKEN_H
