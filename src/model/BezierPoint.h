#ifndef BEZIERPOINT_H
#define BEZIERPOINT_H

#include <QGraphicsEllipseItem>

#include "BezierElement.h"

namespace blueprint {

class ShapeBezier;

class BezierPoint : public BezierElement, public QGraphicsEllipseItem
{
public:
    BezierPoint(ShapeBezier* parent, const int index = 0);
    ~BezierPoint();

    QPointF pos() const override;
    void setPos(const QPointF& pos) override;
    void moveBy(const QPointF& delta) override;
    void setVisible(bool isVisible) override;

protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value);
};

}

#endif // BEZIERPOINT_H
