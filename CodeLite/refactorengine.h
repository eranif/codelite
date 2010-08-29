#ifndef REFACTORENGINE_H
#define REFACTORENGINE_H

#include <wx/event.h>
#include <wx/filename.h>
#include <vector>
#include <list>
#include "cppwordscanner.h"
#include "cpptoken.h"

typedef std::vector<wxFileName> wxFileList;
class wxProgressDialog;
//----------------------------------------------------------------------------------

struct RefactorSource {
	wxString name;
	wxString scope;
	bool     isClass;

	RefactorSource() : name(wxEmptyString), scope(wxEmptyString), isClass(false) {
	}

	void Reset() {
		name.clear();
		scope.clear();
		isClass = false;
	}
};

//-----------------------------------------------------------------------------------

class RefactoringEngine
{
	std::list<CppToken> m_candidates;
	std::list<CppToken> m_possibleCandidates;
	wxEvtHandler *      m_evtHandler;
public:
	static RefactoringEngine* Instance();
protected:
	wxProgressDialog *CreateProgressDialog(const wxString &title, int maxValue);

private:
	RefactoringEngine();
	~RefactoringEngine();
	bool DoResolveWord(  TextStatesPtr states,
						 const wxFileName& fn,
						 int pos,
						 int line,
						 const wxString &word,
						 RefactorSource *rs);

public:
	void SetCandidates(const std::list<CppToken>& candidates) {
		this->m_candidates = candidates;
	}
	void SetPossibleCandidates(const std::list<CppToken>& possibleCandidates) {
		this->m_possibleCandidates = possibleCandidates;
	}
	const std::list<CppToken>& GetCandidates() const {
		return m_candidates;
	}
	const std::list<CppToken>& GetPossibleCandidates() const {
		return m_possibleCandidates;
	}
	wxString GetExpression(int pos, TextStatesPtr states);

	void Clear();
	/**
	 * @brief rename global symbol. Global Symbol can be one of:
	 * <li>member
	 * <li>global variable
	 * <li>class
	 * <li>struct
	 * <li>union
	 * <li>namespace
	 * <li>function
	 * <li>prototype
	 * <li>typedef
	 * @param symname the name of the symbol
	 * @param fn current file
	 * @param line the symbol is loacted in this file
	 * @param pos at that position
	 * @param files perform the refactoring on these files
	 */
	void RenameGlobalSymbol(const wxString &symname, const wxFileName& fn, int line, int pos, const wxFileList& files);

	/**
	 * @brief NOT IMPLEMENTED YET
	 * @param symname symbol we want to rename
	 * @param fn the current file name (full path)
	 * @param line the line where our symbol appears
	 * @param pos the position of the symbol (this should be pointing to the *start* of the symbol)
	 */
	void RenameLocalSymbol (const wxString &symname, const wxFileName& fn, int line, int pos);
};

#endif // REFACTORENGINE_H
