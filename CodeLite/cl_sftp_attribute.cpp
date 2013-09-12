#include "cl_sftp_attribute.h"

SFTPAttribute::SFTPAttribute(sftp_attributes attr)
    : m_attributes(NULL)
{
    Assign(attr);
}

SFTPAttribute::~SFTPAttribute()
{
    DoClear();
}

void SFTPAttribute::Assign(sftp_attributes attr)
{
    DoClear();
    m_attributes = attr;
    DoConstruct();
}

void SFTPAttribute::DoClear()
{
    if ( m_attributes ) {
        sftp_attributes_free( m_attributes );
    }
    m_attributes = NULL;
    m_name.Clear();
    m_flags = 0;
    m_size = 0;
}

void SFTPAttribute::DoConstruct()
{
    if ( !m_attributes )
        return;
        
    m_name = m_attributes->name;
    m_size = m_attributes->size;
    m_flags = 0;
    
    switch ( m_attributes->type ) {
    case SSH_FILEXFER_TYPE_DIRECTORY:
        m_flags |= TYPE_FOLDER;
        break;
    default:
    case SSH_FILEXFER_TYPE_REGULAR:
        m_flags |= TYPE_REGULAR_FILE;
        break;
    case SSH_FILEXFER_TYPE_SPECIAL:
    case SSH_FILEXFER_TYPE_UNKNOWN:
        m_flags |= TYPE_UNKNOWN;
        break;
    case SSH_FILEXFER_TYPE_SYMLINK:
        m_flags |= TYPE_SYMBLINK;
        break;
    }
}
