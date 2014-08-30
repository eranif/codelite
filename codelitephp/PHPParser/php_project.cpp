#include "php_project.h"
#include <macros.h>
#include "php_workspace.h"
#include <wx/tokenzr.h>
#include <dirtraverser.h>
#include <wx/busyinfo.h>
#include <wx/utils.h>
#include <event_notifier.h>
#include <cl_command_event.h>
#include <plugin.h>
#include <macros.h>
#include <wx/msgdlg.h>

void PHPProject::FromJSON(const JSONElement& element)
{
    m_folders.clear();
    m_name = element.namedObject("m_name").toString();
    m_isActive = element.namedObject("m_isActive").toBool();
    m_settings.FromJSON( element.namedObject("settings") );

    if ( element.hasNamedObject("folders") ) {
        JSONElement folders = element.namedObject("folders");
        int size = folders.arraySize();
        for( int i=0; i<size; ++i ) {
            PHPFolder::Ptr_t folder( new PHPFolder() );
            folder->FromJSON(folders.arrayItem(i));
            m_folders.insert( std::make_pair(folder->GetName(), folder) );
        }
    }
}

void PHPProject::ToJSON(JSONElement& pro) const
{
    pro.addProperty( "m_name", m_name );
    pro.addProperty( "m_isActive", m_isActive );
    pro.append( m_settings.ToJSON() );

    JSONElement foldersArr = JSONElement::createArray("folders");
    pro.append(foldersArr);
    PHPFolder::Map_t::const_iterator iter = m_folders.begin();
    for( ; iter != m_folders.end(); ++iter ) {
        foldersArr.arrayAppend( iter->second->ToJSON() );
    }
}

void PHPProject::GetFiles(wxArrayString& files) const
{
    PHPFolder::Map_t::const_iterator iter = m_folders.begin();
    for(; iter != m_folders.end(); ++iter ) {
        iter->second->GetFiles( files, GetFilename().GetPath() );
    }
}

PHPFolder::Ptr_t PHPProject::AddFolder(const wxString& name)
{
    wxArrayString parts = ::wxStringTokenize(name, "/", wxTOKEN_STRTOK);
    if ( parts.IsEmpty() ) {
        return PHPFolder::Ptr_t(NULL);
    }
    
    if ( parts.Item(0) != "." ) {
        parts.Insert(".", 0);
    }
    
    if ( parts.GetCount() == 1 ) {
        
        // top level folder
        if ( m_folders.count(name) ) {
            return m_folders.find(name)->second;
        } else {
            PHPFolder::Ptr_t folder( new PHPFolder(name) );
            folder->SetParent( NULL );
            folder->CreaetOnFileSystem( GetFilename().GetPath() );
            
            m_folders.insert( std::make_pair(name, folder) );
            return folder;
        }
        
    } else {
        // Build the path
        PHPFolder::Ptr_t folder(NULL);
        PHPFolder::Map_t::iterator iter = m_folders.find( parts.Item(0) );
        if ( iter == m_folders.end() ) {
            folder = AddFolder(parts.Item(0));
            folder->CreaetOnFileSystem( GetFilename().GetPath() );
            
        } else {
            folder = iter->second;
        }
        
        // Start from 1
        for(size_t i=1; i<parts.GetCount(); ++i) {
            folder = folder->AddFolder(parts.Item(i));
            folder->CreaetOnFileSystem( GetFilename().GetPath() );
        }
        return folder;
    }
}

void PHPProject::Create(const wxFileName& filename)
{
    m_filename = filename;
    m_name = filename.GetName();
    Save();
}

void PHPProject::Load(const wxFileName& filename)
{
    m_filename = filename;
    JSONRoot root(m_filename);
    FromJSON( root.toElement() );
}

void PHPProject::Save()
{
    JSONRoot root(cJSON_Object);
    JSONElement pro = root.toElement();
    ToJSON( pro );
    root.save( m_filename );
}

PHPFolder::Ptr_t PHPProject::Folder(const wxString& name) const
{
    PHPFolder::Ptr_t pFolder(NULL);
    wxArrayString parts = ::wxStringTokenize(name, "/", wxTOKEN_STRTOK);
    for(size_t i=0; i<parts.GetCount(); ++i) {
        if ( pFolder ) {
            pFolder = pFolder->Folder(parts.Item(i));
            
        } else if ( m_folders.count(parts.Item(i)) ) {
            pFolder = m_folders.find(parts.Item(i))->second;
        }
        
        if ( !pFolder ) {
            break;
        }
    }
    return pFolder;
}

void PHPProject::DeleteFolder(const wxString& name)
{
    if ( m_folders.count(name) ) {
        // a top folder remove all its children (folders + files) and notify
        m_folders.find(name)->second->RemoveFilesRecursively(m_filename.GetPath());
        m_folders.erase(name);
        
    } else {
        PHPFolder::Ptr_t parentFolder = GetParentFolder(name);
        CHECK_PTR_RET(parentFolder);
        parentFolder->DeleteChildFolder(name.AfterLast('/'), m_filename.GetPath());
    }
}

PHPFolder::Ptr_t PHPProject::GetParentFolder(const wxString& child_folder) const
{
    PHPFolder::Ptr_t pFolder(NULL);
    wxArrayString parts = ::wxStringTokenize(child_folder, "/", wxTOKEN_STRTOK);
    for(size_t i=0; i<parts.GetCount()-1; ++i) {
        if ( pFolder ) {
            pFolder = pFolder->Folder(parts.Item(i));
            
        } else {
            pFolder = m_folders.find(parts.Item(i))->second;
        }
        
        if ( !pFolder ) {
            break;
        }
    }
    return pFolder;
}

void PHPProject::ImportDirectory(const wxString& path, const wxString &filespec, bool recursive)
{
    DirTraverser traverser( filespec );
    wxDir dir(path);
    {
        wxBusyInfo info(_("Scanning for files to import..."));
        wxYieldIfNeeded();
        dir.Traverse(traverser, wxEmptyString, recursive ? wxDIR_DEFAULT : wxDIR_FILES);
    }
    
    wxArrayString files = traverser.GetFiles();
    wxArrayString filesAdded;
    {
        wxBusyInfo info(_("Adding files to project..."));
        wxYieldIfNeeded();
        wxString projectPath = m_filename.GetPath();
        PHPFolder::Map_t cache;
        for(size_t i=0; i<files.GetCount(); ++i) {

            
            // Normalize the virtual folder path
            wxString folder = PHPFolder::GetFolderPathFromFileFullPath(files.Item(i), projectPath);
            if ( folder.IsEmpty() ) {
                ::wxMessageBox(_("You can only import files located under the project directory tree"), "CodeLite", wxOK|wxICON_ERROR|wxCENTER);
                return;
            }
            
            // Cache the folders for better performance...
            PHPFolder::Ptr_t pFolder(NULL);
            if ( cache.count(folder) ) {
                pFolder = cache.find(folder)->second;
            }
            
            if ( !pFolder ) {
                pFolder = AddFolder( folder );
                cache.insert( std::make_pair(folder, pFolder) );
            }
            
            if ( pFolder->AddFile( files.Item(i) ) ) {
                // Keep the files added as full path
                filesAdded.Add( files.Item(i) );
            }
        }
    }
    Save();

    // Notify the plugins
    clCommandEvent evtFilesAdded(wxEVT_PROJ_FILE_ADDED);
    evtFilesAdded.SetInt( kEventImportingFolder );
    evtFilesAdded.SetStrings( filesAdded );
    EventNotifier::Get()->AddPendingEvent( evtFilesAdded );
}

PHPFolder::Ptr_t PHPProject::FolderByFileFullPath(const wxString& filename) const
{
    // Convert filename to folder path
    wxString folderPath = PHPFolder::GetFolderPathFromFileFullPath(filename, m_filename.GetPath());
    return Folder(folderPath);
}

bool PHPProject::RenameFile(const wxFileName& filename, const wxString& newFullName)
{
    // Locate the folder
    PHPFolder::Ptr_t pFolder = FolderByFileFullPath(filename.GetFullPath());
    CHECK_PTR_RET_FALSE(pFolder);
    
    return pFolder->RenameFile(filename.GetFullPath(), newFullName);
}

