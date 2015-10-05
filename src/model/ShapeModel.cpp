#include "ShapeModel.h"

#include <QDebug>
#include <algorithm>

#include "Shape.h"

using namespace blueprint;

ShapeModel* ShapeModel::sInstance = nullptr;

ShapeModel::ShapeModel()
    : QAbstractItemModel(),
    mRootItem(nullptr)
{
    connect(this, &QAbstractItemModel::rowsInserted, this,
            [this](const QModelIndex& parent, int first, int /*last*/) {
        Shape* parentItem = itemFromIndex(parent);
        Shape* childItem = parentItem->child(first);
        emit shapeAdded(childItem);
    });

}

ShapeModel::~ShapeModel()
{
}

ShapeModel* ShapeModel::instance()
{
    if (!sInstance) {
        sInstance = new ShapeModel();
    }
    return sInstance;
}

void ShapeModel::addItem(Shape* item, Shape* parent)
{
    if (!parent) {
        parent = mRootItem;
    }
    // 0 because it should be displayed first in the treeview list
    // to respect z-index ordering
    int childRow = 0;
    QModelIndex parentIndex = parent->modelIndex() ?
                (QModelIndex)(*parent->modelIndex())
                : QModelIndex();
    beginInsertRows(parentIndex, childRow, childRow);
    item->setZValue(parent->zValue() + parent->childCount());
    parent->insertChild(childRow, item);
    QModelIndex childIndex = index(childRow, 0, parentIndex);
    item->setModelIndex(childIndex);
    endInsertRows();
}

void ShapeModel::removeItem(Shape* item)
{
    if (item == mRootItem) {
        // you wanted to delete root item? really??
        return;
    }
    Shape* parent = item->parentShape();
    if (!parent) {
        parent = mRootItem;
    }

    qDebug() << "Removing shape" << item->name();

    int childRow = parent->indexOf(item);
    const QModelIndex parentIndex = *parent->modelIndex();
    beginRemoveRows(parentIndex, childRow, childRow);
    if(parent->removeChild(item)) {
        item->setParentShape(nullptr);
    }
    endRemoveRows();
    emit shapeRemoved(item);
    if (item == mSelectedShape) {
        clearSelectedShape();
    }
}

void ShapeModel::selectShape(Shape* shape)
{
    if (shape == mSelectedShape) {
        return;
    }
    if (mSelectedShape) {
        mSelectedShape->setSelected(false);
    }
    shape->setSelected(true);
    mSelectedShape = shape;
    emit shapeSelected(shape);
}

void ShapeModel::clearSelectedShape()
{
    if (!mSelectedShape) {
        return;
    }
    mSelectedShape = nullptr;
    emit shapeSelected(mSelectedShape);
}

void ShapeModel::moveShape(Shape* shape, Shape* destinationParent, int destinationIndex)
{
    Shape* sourceParent = shape->parentShape();
    int sourceIndex = sourceParent->indexOf(shape);
    QModelIndex sourceParentIndex = *sourceParent->modelIndex();
    QModelIndex destinationParentIndex = *destinationParent->modelIndex();
    int modelRowIndex = destinationIndex;

    if (sourceParent == destinationParent) {
        if (sourceIndex < destinationIndex) {
            modelRowIndex = std::min(destinationIndex + 1, destinationParent->childCount());
        }
    }

    qDebug() << "Moving shape from index" << sourceIndex << "to index" << destinationIndex;

    beginMoveRows(sourceParentIndex, sourceIndex, sourceIndex, destinationParentIndex, modelRowIndex);
    sourceParent->removeChildAt(sourceIndex);
    destinationParent->insertChild(destinationIndex, shape);
    int childCount = destinationParent->childCount();
    for (int i = 0; i < childCount; ++i) {
        destinationParent->child(i)->setZValue(destinationParent->zValue() + (childCount - i));
    }
    endMoveRows();
}

QVariant ShapeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    Shape* item = itemFromIndex(index);
    return item->name();
}

QModelIndex ShapeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    Shape* parentItem = itemFromIndex(parent);
    Shape* childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex ShapeModel::parent(const QModelIndex& child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    Shape* childItem = itemFromIndex(child);
    Shape* parentItem = childItem->parentShape();

    if (parentItem == mRootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int ShapeModel::rowCount(const QModelIndex& parent) const
{
    Shape* parentItem;
    if (parent.column() > 0) {
        return 0;
    }

    parentItem = itemFromIndex(parent);
    return parentItem->childCount();
}

int ShapeModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

bool ShapeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole) {
        return false;
    }

    Shape* item = itemFromIndex(index);
    item->setName(value.toString());
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags ShapeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return Qt::ItemIsEditable
           | Qt::ItemIsDragEnabled
           | Qt::ItemIsDropEnabled
           | Qt::ItemIsSelectable
            | QAbstractItemModel::flags(index);
}

Qt::DropActions ShapeModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void ShapeModel::setRootItem(Shape* rootItem)
{
    mRootItem = rootItem;
    mRootItem->setModelIndex(index(0, 0));
}

Shape* ShapeModel::itemFromIndex(const QModelIndex& index) const
{
    if (index.isValid()) {
        return static_cast<Shape*>(index.internalPointer());
    } else {
        return mRootItem;
    }
}

Shape* ShapeModel::itemFromParentIndex(const QModelIndex& parentIndex, int row) const
{
    Shape* parentItem = itemFromIndex(parentIndex);
    return parentItem->child(row);
}