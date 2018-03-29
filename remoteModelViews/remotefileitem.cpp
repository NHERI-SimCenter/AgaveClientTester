#include "remotefileitem.h"

RemoteFileItem::RemoteFileItem()
{
    myFile = FileNodeRef::nil();
}

RemoteFileItem::RemoteFileItem(FileNodeRef fileInfo) : QStandardItem()
{
    myFile = fileInfo;
}

RemoteFileItem::RemoteFileItem(RemoteFileItem * rowLeader, QString text) : QStandardItem(text)
{
    myFile = FileNodeRef::nil();
    if (rowLeader == NULL)
    {
        qDebug("Warning: Invalid Remote File Item Construction");
        return;
    }
    myRowLeader = rowLeader;
    myRowLeader->appendToRowList(this);
}

QList<RemoteFileItem*> RemoteFileItem::getRowList()
{
    if (myRowLeader == NULL) return rowList;
    return myRowLeader->getRowList();
}

FileNodeRef RemoteFileItem::getFile()
{
    if (myRowLeader == NULL) return myFile;
    return myRowLeader->getFile();
}

void RemoteFileItem::appendToRowList(RemoteFileItem * toAdd)
{
    rowList.append(toAdd);
}
