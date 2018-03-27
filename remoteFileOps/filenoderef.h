#ifndef FILENODEREF_H
#define FILENODEREF_H

#include "../AgaveClientInterface/filemetadata.h"

enum class NodeState;

class FileNodeRef : public FileMetaData
{
public:
    FileNodeRef();
    FileNodeRef& operator=(const FileNodeRef &toCopy);

    void setTimestamp(qint64 newTimestamp);
    qint64 getTimestamp() const;

    bool fileNodeExtant() const;
    NodeState getNodeState() const;
    bool isAncestorOf(const FileNodeRef &child) const;
    const FileNodeRef getChildWithName(QString childName) const;
    bool fileBufferLoaded() const;
    const QByteArray getFileBuffer() const;
    void setFileBuffer(const QByteArray * toSet) const;
    bool folderContentsLoaded() const;
    FileNodeRef getParent() const;
    QList<FileNodeRef> getChildList() const;
    bool isRootNode() const;

    void enactFolderRefresh(bool clearData = false) const;

private:
    qint64 timestamp = 0;
};

#endif // FILENODEREF_H
