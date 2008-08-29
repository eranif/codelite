#ifndef __externaltoolsdata__
#define __externaltoolsdata__

#include <vector>
#include "serialized_object.h"

class ToolInfo : public SerializedObject
{
	wxString m_id;
	wxString m_path;
	wxString m_arguments;
	wxString m_wd;

public:
	ToolInfo(const wxString &id, const wxString &path, const wxString &args, const wxString &wd);
	ToolInfo();
	~ToolInfo();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);

	void SetArguments(const wxString& arguments) {
		this->m_arguments = arguments;
	}
	void SetId(const wxString& id) {
		this->m_id = id;
	}
	void SetPath(const wxString& path) {
		this->m_path = path;
	}
	void SetWd(const wxString& wd) {
		this->m_wd = wd;
	}
	const wxString& GetArguments() const {
		return m_arguments;
	}
	const wxString& GetId() const {
		return m_id;
	}
	const wxString& GetPath() const {
		return m_path;
	}
	const wxString& GetWd() const {
		return m_wd;
	}
};

class ExternalToolsData : public SerializedObject
{
	std::vector<ToolInfo> m_tools;
public:
	ExternalToolsData();
	virtual ~ExternalToolsData();

	const std::vector<ToolInfo>& GetTools() const;
	void SetTools(const std::vector<ToolInfo>& tools);
	
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
};

#endif // __externaltoolsdata__
