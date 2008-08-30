#ifndef __externaltoolsdata__
#define __externaltoolsdata__

#include <vector>
#include "serialized_object.h"

#define MAX_TOOLS 10

class ToolInfo : public SerializedObject
{
	wxString m_id;
	wxString m_path;
	wxString m_arguments;
	wxString m_wd;
	wxString m_name;
	wxString m_icon16;
	wxString m_icon24;
	bool m_captureOutput;

public:
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
	void SetName(const wxString &name) {
		this->m_name = name;
	}
	const wxString &GetName() const {
		return m_name;
	}
	void SetIcon16(const wxString& icon16) {
		this->m_icon16 = icon16;
	}
	void SetIcon24(const wxString& icon24) {
		this->m_icon24 = icon24;
	}
	const wxString& GetIcon16() const {
		return m_icon16;
	}
	const wxString& GetIcon24() const {
		return m_icon24;
	}

	void SetCaptureOutput(const bool& captureOutput) {
		this->m_captureOutput = captureOutput;
	}
	const bool& GetCaptureOutput() const {
		return m_captureOutput;
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
