#ifndef SHAPERECTANGLE_H
#define SHAPERECTANGLE_H

#include "ShapeBezier.h"


class ShapeRectangle : public ShapeBezier
{
public:
    ShapeRectangle(GraphicalItem* parentItem, qreal x, qreal y);

};

#endif // SHAPERECTANGLE_H