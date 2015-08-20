#include "Canvas.h"

#include "Shape.h"

Canvas::Canvas(GraphicalItem* parentItem, qreal x, qreal y)
    : ShapeRectangle(parentItem, x, y)
{

    //mItem->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    mItem->setData(Shape::Type::CANVAS, qVariantFromValue(static_cast<void *>(this)));
    setBorderColor(QColor(10, 10, 10));
    setBackgroundColor(QColor(255, 255, 255));
    boundingBox()->boundingBoxPointMoved(BoundingBoxPoint::BOTTOM_RIGHT, QPointF(300.0, 500.0));
}

Canvas::~Canvas()
{

}
