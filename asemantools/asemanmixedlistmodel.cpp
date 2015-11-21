#include "asemanmixedlistmodel.h"

#include <QTimer>
#include <QDebug>

class AsemanMixedListModelPrivate
{
public:
    QList<QAbstractListModel*> models;
    QVariantList cachedList;
    QTimer *initTimer;
    bool inited;
};

AsemanMixedListModel::AsemanMixedListModel(QObject *parent) :
    AsemanAbstractListModel(parent)
{
    p = new AsemanMixedListModelPrivate;
    p->inited = false;

    p->initTimer = new QTimer(this);
    p->initTimer->setInterval(200);
    p->initTimer->setSingleShot(true);

    connect(p->initTimer, SIGNAL(timeout()), SLOT(reinit_prv()));
}

int AsemanMixedListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant AsemanMixedListModel::data(const QModelIndex &index, int role) const
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, index);
        if(!newIndex.isValid())
            continue;

        if(role == RolesModelObject)
            return QVariant::fromValue<QObject*>(model);
        else
        if(role < Qt::UserRole)
            return model->data(newIndex, role);
        else
        if(model->roleNames().contains(role))
            return model->data(newIndex, role);
    }

    return QVariant();
}

bool AsemanMixedListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, index);
        if(!newIndex.isValid())
            continue;

        return model->setData(newIndex, value, role);
    }

    return false;
}

QHash<qint32, QByteArray> AsemanMixedListModel::roleNames() const
{
    QHash<qint32, QByteArray> roles;
    roles[RolesModelObject] = "modelObject";

    foreach(QAbstractListModel *model, p->models)
        roles.unite( model->roleNames() );

    return roles;
}

int AsemanMixedListModel::count() const
{
    int count = 0;
    foreach(QAbstractListModel *model, p->models)
        count += model->rowCount();
    return count;
}

void AsemanMixedListModel::setModels(const QVariantList &list)
{
    if(list == p->cachedList)
        return;

    p->cachedList = list;
    reinit();

    emit modelsChanged();
    emit countChanged();
}

QVariantList AsemanMixedListModel::models() const
{
    return p->cachedList;
}

Qt::ItemFlags AsemanMixedListModel::flags(const QModelIndex &index) const
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, index);
        if(!newIndex.isValid())
            continue;

        return model->flags(newIndex);
    }

    return Qt::NoItemFlags;
}

bool AsemanMixedListModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, parent);
        if(!newIndex.isValid())
            continue;

        return model->insertColumns(column, count, newIndex);
    }

    return false;
}

bool AsemanMixedListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, parent);
        if(!newIndex.isValid())
            continue;

        return model->insertRows(row, count, newIndex);
    }

    return false;
}

bool AsemanMixedListModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent, int destinationChild)
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, sourceParent);
        if(!newIndex.isValid())
            continue;

        return model->moveColumns(sourceParent, sourceColumn, count, destinationParent, destinationChild);
    }

    return false;
}

bool AsemanMixedListModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, sourceParent);
        if(!newIndex.isValid())
            continue;

        return model->moveRows(sourceParent, sourceRow, count, destinationParent, destinationChild);
    }

    return false;
}

bool AsemanMixedListModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, parent);
        if(!newIndex.isValid())
            continue;

        return model->removeColumns(column, count, newIndex);
    }

    return false;
}

bool AsemanMixedListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    foreach(QAbstractListModel *model, p->models)
    {
        QModelIndex newIndex = mapToModelIndex(model, parent);
        if(!newIndex.isValid())
            continue;

        return model->removeRows(row, count, newIndex);
    }

    return false;
}

void AsemanMixedListModel::columnsAboutToBeInserted_slt(const QModelIndex &parent, int first, int last)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        beginInsertColumns( mapFromModelIndex(model, parent),
                            mapFromModel(model, first),
                            mapFromModel(model, last));
}

void AsemanMixedListModel::columnsAboutToBeMoved_slt(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        beginMoveColumns( mapFromModelIndex(model, sourceParent),
                          mapFromModel(model, sourceStart),
                          mapFromModel(model, sourceEnd),
                          mapFromModelIndex(model, destinationParent),
                          mapFromModel(model, destinationColumn));
}

void AsemanMixedListModel::columnsAboutToBeRemoved_slt(const QModelIndex &parent, int first, int last)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        beginRemoveColumns( mapFromModelIndex(model, parent),
                            mapFromModel(model, first),
                            mapFromModel(model, last));
}

void AsemanMixedListModel::columnsInserted_slt(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        endInsertColumns();
}

void AsemanMixedListModel::columnsMoved_slt(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(destination)
    Q_UNUSED(column)
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        endMoveColumns();
}

void AsemanMixedListModel::columnsRemoved_slt(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        endRemoveColumns();
}

void AsemanMixedListModel::dataChanged_slt(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        emit dataChanged( mapFromModelIndex(model, topLeft),
                          mapFromModelIndex(model, bottomRight),
                          roles);
}

void AsemanMixedListModel::headerDataChanged_slt(Qt::Orientation orientation, int first, int last)
{
    Q_UNUSED(orientation)
    Q_UNUSED(first)
    Q_UNUSED(last)
}

void AsemanMixedListModel::layoutAboutToBeChanged_slt(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents)
    Q_UNUSED(hint)
}

void AsemanMixedListModel::layoutChanged_slt(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents)
    Q_UNUSED(hint)
}

void AsemanMixedListModel::modelAboutToBeReset_slt()
{
    beginResetModel();
}

void AsemanMixedListModel::modelReset_slt()
{
    endResetModel();
    emit countChanged();
}

void AsemanMixedListModel::rowsAboutToBeInserted_slt(const QModelIndex &parent, int start, int end)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        beginInsertRows( mapFromModelIndex(model, parent),
                         mapFromModel(model, start),
                         mapFromModel(model, end));
}

void AsemanMixedListModel::rowsAboutToBeMoved_slt(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        beginMoveRows( mapFromModelIndex(model, sourceParent),
                       mapFromModel(model, sourceStart),
                       mapFromModel(model, sourceEnd),
                       mapFromModelIndex(model, destinationParent),
                       mapFromModel(model, destinationRow));
}

void AsemanMixedListModel::rowsAboutToBeRemoved_slt(const QModelIndex &parent, int first, int last)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        beginRemoveRows( mapFromModelIndex(model, parent),
                         mapFromModel(model, first),
                         mapFromModel(model, last));
}

void AsemanMixedListModel::rowsInserted_slt(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        endInsertRows();

    emit countChanged();
}

void AsemanMixedListModel::rowsMoved_slt(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(destination)
    Q_UNUSED(row)
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        endMoveRows();

    emit countChanged();
}

void AsemanMixedListModel::rowsRemoved_slt(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(sender());
    if(model)
        endRemoveRows();

    emit countChanged();
}

void AsemanMixedListModel::modelDestroyed(QObject *obj)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel*>(obj);
    if(!model)
        return;

    p->models.removeAll(model);
    p->cachedList.removeAll(QVariant::fromValue<QObject*>(obj));
    emit modelsChanged();
}

void AsemanMixedListModel::reinit()
{
    p->inited = false;
    p->initTimer->stop();
    p->initTimer->start();
}

void AsemanMixedListModel::reinit_prv()
{
    foreach(QAbstractListModel *model, p->models)
    {
        disconnect(model, SIGNAL(destroyed(QObject*)),
                   this, SLOT(modelDestroyed(QObject*)));
        disconnect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                   this, SLOT(dataChanged_slt(QModelIndex,QModelIndex,QVector<int>)));
        disconnect(model, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
                   this, SLOT(columnsAboutToBeInserted_slt(QModelIndex,int,int)));
        disconnect(model, SIGNAL(columnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
                   this, SLOT(columnsAboutToBeMoved_slt(QModelIndex,int,int,QModelIndex,int)));
        disconnect(model, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
                   this, SLOT(columnsAboutToBeRemoved_slt(QModelIndex,int,int)));
        disconnect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),
                   this, SLOT(columnsInserted_slt(QModelIndex,int,int)));
        disconnect(model, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)),
                   this, SLOT(columnsMoved_slt(QModelIndex,int,int,QModelIndex,int)));
        disconnect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
                   this, SLOT(columnsRemoved_slt(QModelIndex,int,int)));
        disconnect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                   this, SLOT(dataChanged_slt(QModelIndex,QModelIndex,QVector<int>)));
        disconnect(model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
                   this, SLOT(headerDataChanged_slt(Qt::Orientation,int,int)));
        disconnect(model, SIGNAL(layoutAboutToBeChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),
                   this, SLOT(layoutAboutToBeChanged_slt(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)));
        disconnect(model, SIGNAL(layoutChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),
                   this, SLOT(layoutChanged_slt(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)));
        disconnect(model, SIGNAL(modelAboutToBeReset()),
                   this, SLOT(modelAboutToBeReset_slt()));
        disconnect(model, SIGNAL(modelReset()),
                   this, SLOT(modelReset_slt()));
        disconnect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
                   this, SLOT(rowsAboutToBeInserted_slt(QModelIndex,int,int)));
        disconnect(model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
                   this, SLOT(rowsAboutToBeMoved_slt(QModelIndex,int,int,QModelIndex,int)));
        disconnect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                   this, SLOT(rowsAboutToBeRemoved_slt(QModelIndex,int,int)));
        disconnect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this, SLOT(rowsInserted_slt(QModelIndex,int,int)));
        disconnect(model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                   this, SLOT(rowsMoved_slt(QModelIndex,int,int,QModelIndex,int)));
        disconnect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                   this, SLOT(rowsRemoved_slt(QModelIndex,int,int)));
    }

    beginResetModel();
    p->models.clear();
    foreach(const QVariant &var, p->cachedList)
    {
        QAbstractListModel *model = qobject_cast<QAbstractListModel*>(var.value<QObject*>());
        if(!model)
            continue;

        p->models << model;
        connect(model, SIGNAL(destroyed(QObject*)),
                this, SLOT(modelDestroyed(QObject*)));
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                this, SLOT(dataChanged_slt(QModelIndex,QModelIndex,QVector<int>)));
        connect(model, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
                this, SLOT(columnsAboutToBeInserted_slt(QModelIndex,int,int)));
        connect(model, SIGNAL(columnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(columnsAboutToBeMoved_slt(QModelIndex,int,int,QModelIndex,int)));
        connect(model, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
                this, SLOT(columnsAboutToBeRemoved_slt(QModelIndex,int,int)));
        connect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),
                this, SLOT(columnsInserted_slt(QModelIndex,int,int)));
        connect(model, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(columnsMoved_slt(QModelIndex,int,int,QModelIndex,int)));
        connect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
                this, SLOT(columnsRemoved_slt(QModelIndex,int,int)));
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                this, SLOT(dataChanged_slt(QModelIndex,QModelIndex,QVector<int>)));
        connect(model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
                this, SLOT(headerDataChanged_slt(Qt::Orientation,int,int)));
        connect(model, SIGNAL(layoutAboutToBeChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),
                this, SLOT(layoutAboutToBeChanged_slt(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)));
        connect(model, SIGNAL(layoutChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),
                this, SLOT(layoutChanged_slt(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)));
        connect(model, SIGNAL(modelAboutToBeReset()),
                this, SLOT(modelAboutToBeReset_slt()));
        connect(model, SIGNAL(modelReset()),
                this, SLOT(modelReset_slt()));
        connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
                this, SLOT(rowsAboutToBeInserted_slt(QModelIndex,int,int)));
        connect(model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(rowsAboutToBeMoved_slt(QModelIndex,int,int,QModelIndex,int)));
        connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                this, SLOT(rowsAboutToBeRemoved_slt(QModelIndex,int,int)));
        connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(rowsInserted_slt(QModelIndex,int,int)));
        connect(model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(rowsMoved_slt(QModelIndex,int,int,QModelIndex,int)));
        connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(rowsRemoved_slt(QModelIndex,int,int)));
    }
    endResetModel();
    p->inited = true;
}

QModelIndex AsemanMixedListModel::mapFromModelIndex(QAbstractListModel *model, const QModelIndex &index) const
{
    if(index.parent().isValid())
        return index;

    const int mappedRow = mapFromModel(model, index.row());
    return model->index( mappedRow, index.column(), index.parent());
}

int AsemanMixedListModel::mapFromModel(QAbstractListModel *model, int row) const
{
    return row+modelPad(model);
}

QModelIndex AsemanMixedListModel::mapToModelIndex(QAbstractListModel *model, const QModelIndex &index) const
{
    if(index.parent().isValid())
        return index;

    const int mappedRow = mapToModel(model, index.row());
    if(mappedRow < 0)
        return QModelIndex();
    else
        return model->index(mappedRow, index.column(), index.parent());
}

int AsemanMixedListModel::mapToModel(QAbstractListModel *model, int row) const
{
    int newRow = row-modelPad(model);
    if(0 <= newRow && newRow < model->rowCount())
        return newRow;
    else
        return -1;
}

int AsemanMixedListModel::modelPad(QAbstractListModel *model) const
{
    int rowPad = 0;
    foreach(QAbstractListModel *mdl, p->models)
    {
        if(mdl == model)
            break;

        rowPad += mdl->rowCount();
    }

    return rowPad;
}

AsemanMixedListModel::~AsemanMixedListModel()
{
    delete p;
}

