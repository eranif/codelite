#ifndef INCLUDE_FINDER_H
#define INCLUDE_FINDER_H

#include <map>
#include <string>

class CommentParseResult {
private:
	std::map<size_t, std::string> m_comments;
	std::string                   m_filename;
	
public:
	void addComment(const std::string& comment, size_t line, bool cppComment)
	{
		// try to group this comment with the one above it
		std::string cmt = comment;
		if(cppComment && line) {
			size_t prevLine = line - 1;
			std::map<size_t, std::string>::iterator iter = m_comments.find(prevLine);
			if(iter != m_comments.end()) {
				cmt =  iter->second;
				cmt += "\n";
				cmt += comment;
				
				// remove the previous comment from the map
				m_comments.erase(iter);
			}
		}
		m_comments[line] = cmt;
	}
	
	std::string getCommentForLine(size_t line) const
	{
		std::map<size_t, std::string>::const_iterator iter = m_comments.find(line);
		if(iter == m_comments.end())
			return "";
		return iter->second;
	}
	
	void print()
	{
		std::map<size_t, std::string>::const_iterator iter = m_comments.begin();
		for(; iter != m_comments.end(); iter++) {
			printf("Line   : %u\n", (unsigned int)iter->first);
			printf("Comment:\n%s\n", iter->second.c_str());
		}
	}
	
	void setFilename(const std::string& filename)
	{
		m_filename = filename;
	}
	
	const std::string& getFilename() const
	{
		return m_filename;
	}
	
	void clear()
	{
		m_comments.clear();
		m_filename.clear();
	}
};

extern int ParseComments(const char* filePath, CommentParseResult &comments);

#endif
