#ifndef __commentconfigdata__
#define __commentconfigdata__

#include "serialized_object.h"

class CommentConfigData : public SerializedObject
{
	bool m_addStarOnCComment;
	bool m_continueCppComment;
	bool m_useSlash2Stars;
	bool m_useShtroodel;
	wxString m_classPattern;
	wxString m_functionPattern;
	
public:
	CommentConfigData();
	virtual ~CommentConfigData();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	
	
	//Setters
	void SetAddStarOnCComment(const bool& addStarOnCComment) {this->m_addStarOnCComment = addStarOnCComment;}
	void SetClassPattern(const wxString& classPattern) {this->m_classPattern = classPattern;}
	void SetContinueCppComment(const bool& continueCppComment) {this->m_continueCppComment = continueCppComment;}
	void SetFunctionPattern(const wxString& functionPattern) {this->m_functionPattern = functionPattern;}
	void SetUseShtroodel(const bool& useShtroodel) {this->m_useShtroodel = useShtroodel;}
	void SetUseSlash2Stars(const bool& useSlash2Stars) {this->m_useSlash2Stars = useSlash2Stars;}
	//Getters
	const bool& GetAddStarOnCComment() const {return m_addStarOnCComment;}
	const wxString& GetClassPattern() const {return m_classPattern;}
	const bool& GetContinueCppComment() const {return m_continueCppComment;}
	const wxString& GetFunctionPattern() const {return m_functionPattern;}
	const bool& GetUseShtroodel() const {return m_useShtroodel;}
	const bool& GetUseSlash2Stars() const {return m_useSlash2Stars;}
	
};
#endif // __commentconfigdata__
