#ifndef LINKEDSTANDARDITEM_H
#define LINKEDSTANDARDITEM_H

#include <QStandardItem>

class LinkedStandardItem : public QStandardItem
{
public:
    LinkedStandardItem(QObject * linkedObject);
    LinkedStandardItem(QObject * linkedObject, QString text);
    ~LinkedStandardItem();
    QObject * getLinkedObject();
    template <class T> T * linked_obj_cast();

private:
    QObject * myLinkedObject;
};

#endif // LINKEDSTANDARDITEM_H
