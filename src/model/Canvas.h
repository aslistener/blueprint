#ifndef CANVAS_H
#define CANVAS_H

#include <QList>

#include "model/ShapeRectangle.h"
#include "model/TreeItem.h"

namespace blueprint {

class Canvas : public ShapeRectangle
{
public:
    Canvas(TreeItem* parentItem, qreal x, qreal y);
    ~Canvas();

};
}

#endif // CANVAS_H
