#include "linkedstandarditem.h"

LinkedStandardItem::LinkedStandardItem(QObject * linkedObject) : QStandardItem()
{
    myLinkedObject = linkedObject;
}

LinkedStandardItem::LinkedStandardItem(QObject * linkedObject, QString text) : QStandardItem(text)
{
    myLinkedObject = linkedObject;
}

QObject * LinkedStandardItem::getLinkedObject()
{
    return myLinkedObject;
}

template <class T> T * LinkedStandardItem::linked_obj_cast()
{
    return qobject_cast<T *>(myLinkedObject);
}
