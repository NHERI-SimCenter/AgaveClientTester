#ifndef REMOTEFILEITEM_H
#define REMOTEFILEITEM_H

#include <QStandardItem>

#include "../remoteFileOps/filenoderef.h"

class RemoteFileItem : public QStandardItem
{
public:
    RemoteFileItem();
    RemoteFileItem(FileNodeRef fileInfo);
    RemoteFileItem(RemoteFileItem * rowLeader, QString text);

    QList<RemoteFileItem*> getRowList();
    FileNodeRef getFile();

private:
    void appendToRowList(RemoteFileItem * toAdd);

    RemoteFileItem * myRowLeader = NULL;
    QList<RemoteFileItem*> rowList;
    FileNodeRef myFile;
};

#endif // REMOTEFILEITEM_H
