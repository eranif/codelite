#include "fileextmanager.h"
#include <wx/filename.h>

std::map<wxString, FileExtManager::FileType> FileExtManager::m_map;

void FileExtManager::Init()
{
	static bool init_done(false);

	if ( !init_done ) {
		init_done = true;

		m_map[wxT("cc") ] = TypeSource;
		m_map[wxT("cpp")] = TypeSource;
		m_map[wxT("cxx")] = TypeSource;
		m_map[wxT("c++")] = TypeSource;
		m_map[wxT("c")  ] = TypeSource;

		m_map[wxT("h")   ] = TypeHeader;
		m_map[wxT("hpp") ] = TypeHeader;
		m_map[wxT("hxx") ] = TypeHeader;
		m_map[wxT("hh")  ] = TypeHeader;
		m_map[wxT("h++") ] = TypeHeader;
		m_map[wxT("inc") ] = TypeHeader;
		m_map[wxT("incl")] = TypeHeader;
		m_map[wxT("inl") ] = TypeHeader;

		m_map[wxT("rc")  ] = TypeResource;
		m_map[wxT("res") ] = TypeResource;
		m_map[wxT("xrc") ] = TypeResource;
		m_map[wxT("inl") ] = TypeResource;

		m_map[wxT("y")   ] = TypeYacc;
		m_map[wxT("l")   ] = TypeLex;
		m_map[wxT("ui")  ] = TypeQtForm;
		m_map[wxT("qrc") ] = TypeQtResource;
	}
}

FileExtManager::FileType FileExtManager::GetType(const wxString& filename)
{
	Init();

	wxFileName fn( filename );
	if ( fn.IsOk() == false ) {
		return TypeOther;
	}

	wxString e ( fn.GetExt() );
	e.MakeLower();
	e.Trim().Trim(false);

	std::map<wxString, FileType>::iterator iter = m_map.find(e);
	if ( iter == m_map.end() ) {
		return TypeOther;
	}
	return iter->second;
}
