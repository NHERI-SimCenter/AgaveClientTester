#include "filenoderef.h"

#include "../ae_globals.h"
#include "fileoperator.h"
#include "filetreenode.h"

FileNodeRef::FileNodeRef() : FileMetaData() {}

FileNodeRef& FileNodeRef::operator=(const FileNodeRef &toCopy)
{
    copyDataFrom(toCopy);
    timestamp = toCopy.timestamp;
    return *this;
}

void FileNodeRef::setTimestamp(qint64 newTimestamp)
{
    timestamp = newTimestamp;
}

qint64 FileNodeRef::getTimestamp() const
{
    return timestamp;
}

bool FileNodeRef::fileNodeExtant() const
{
    if (getFileType() == FileType::NIL) return false;

    return ae_globals::get_file_handle()->fileStillExtant(*this);
}

NodeState FileNodeRef::getNodeState() const
{
    return ae_globals::get_file_handle()->getFileNodeState(*this);
}

bool FileNodeRef::isAncestorOf(const FileNodeRef &child) const
{
    return ae_globals::get_file_handle()->isAncestorOf(*this, child);
}

const FileNodeRef FileNodeRef::getChildWithName(QString childName) const
{
    return ae_globals::get_file_handle()->getChildWithName(*this, childName);
}

bool FileNodeRef::fileBufferLoaded() const
{
    return (ae_globals::get_file_handle()->getFileNodeState(*this) == NodeState::FILE_BUFF_LOADED);
}

const QByteArray FileNodeRef::getFileBuffer() const
{
    return ae_globals::get_file_handle()->getFileBuffer(*this);
}

void FileNodeRef::setFileBuffer(const QByteArray * toSet) const
{
    ae_globals::get_file_handle()->setFileBuffer(*this, toSet);
}

bool FileNodeRef::folderContentsLoaded() const
{
    return (ae_globals::get_file_handle()->getFileNodeState(*this) == NodeState::FOLDER_CONTENTS_LOADED);
}

FileNodeRef FileNodeRef::getParent() const
{
    return (ae_globals::get_file_handle()->getParent(*this));
}

QList<FileNodeRef> FileNodeRef::getChildList() const
{
    return (ae_globals::get_file_handle()->getChildList(*this));
}

bool FileNodeRef::isRootNode() const
{
    return (ae_globals::get_file_handle()->nodeIsRoot(*this));
}

void FileNodeRef::enactFolderRefresh(bool clearData) const
{
    ae_globals::get_file_handle()->enactFolderRefresh(*this, clearData);
}
