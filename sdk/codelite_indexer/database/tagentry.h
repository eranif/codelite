#ifndef __tagentry__
#define __tagentry__

#include <string>
#include <map>

class TagEntry
{
	std::string                        m_line;
	std::string                        m_path;        ///< Tag full path
	std::string                        m_file;        ///< File this tag is found
	int                                m_lineNumber;  ///< Line number
	std::string                        m_pattern;     ///< A pattern that can be used to locate the tag in the file
	std::string                        m_kind;        ///< Member, function, class, typedef etc.
	std::string                        m_parent;      ///< Direct parent
	std::string                        m_name;        ///< Tag name (short name, excluding any scope names)
	std::map<std::string, std::string> m_extFields;   ///< Additional extension fields
	std::string                        m_scope;

public:
	void toString();

protected:
	void updatePath(std::string & path);
	void initialize();

	/**
	 * Construct a TagEntry from values.
	 * \param fileName File name
	 * \param name Tag name
	 * \param lineNumber Tag line number
	 * \param pattern Pattern
	 * \param kind Tag kind (class, struct, etc)
	 * \param extFields Map of extenstion fields (key:value)
	 * \param project Project name
	 */
	void create(const std::string &fileName,
	            const std::string &name,
	            int lineNumber,
	            const std::string &pattern,
	            const std::string &kind,
	            std::map<std::string, std::string>& extFields);
public:
	TagEntry(const char *line);
	~TagEntry();

	std::string getExtField(const std::string& extField) const {
		std::map<std::string, std::string>::const_iterator iter = m_extFields.find(extField);
		if (iter == m_extFields.end())
			return "";
		return iter->second;
	}

	void setFile(const std::string& file) {
		this->m_file = file;
	}
	void setKind(const std::string& kind) {
		this->m_kind = kind;
	}
	void setLineNumber(const int& lineNumber) {
		this->m_lineNumber = lineNumber;
	}
	void setName(const std::string& name) {
		this->m_name = name;
	}
	void setPath(const std::string& path) {
		this->m_path = path;
	}
	void setPattern(const std::string& pattern) {
		this->m_pattern = pattern;
	}
	void setScope(const std::string& scope) {
		this->m_scope = scope;
	}
	const std::map<std::string, std::string>& getExtFields() const {
		return m_extFields;
	}
	const std::string& getFile() const {
		return m_file;
	}
	const std::string& getKind() const {
		return m_kind;
	}
	const int& getLineNumber() const {
		return m_lineNumber;
	}
	const std::string& getName() const {
		return m_name;
	}
	const std::string& getPath() const {
		return m_path;
	}
	const std::string& getPattern() const {
		return m_pattern;
	}
	const std::string& getScope() const {
		return m_scope;
	}
	void setParent(const std::string& parent) {
		this->m_parent = parent;
	}
	const std::string& getParent() const {
		return m_parent;
	}
};
#endif // __tagentry__
