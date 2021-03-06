#include "TestSerialization.h"

#include <QFile>

#include "io/Parcel.h"
#include "io/XmlInputOutput.h"

#include "TestUtils.h"
#include "model/Canvas.h"
#include "model/Page.h"
#include "model/Shape.h"
#include "model/ShapeBezier.h"
#include "model/BezierElement.h"
#include "model/ShapeFactory.h"

using namespace blueprint;

TestSerialization::TestSerialization(QObject *parent)
    : QObject(parent),
      mBlueprint()
{

}

TestSerialization::~TestSerialization()
{
}

void TestSerialization::initTestCase()
{
    TestUtils::toggleLogOutput(false);

    mBlueprint.setName("Test Project");

    Page* page = ShapeFactory::createPage();
    page->setName("Page 1");

    Canvas* canvas = ShapeFactory::createCanvas(page);
    canvas->setName("Canvas 1");

    Shape* rectangle = ShapeFactory::createRectangle(canvas);
    canvas->appendChild(rectangle);
    page->appendChild(canvas);

    canvas = ShapeFactory::createCanvas(page);
    canvas->setName("Canvas 2");
    page->appendChild(canvas);

    mBlueprint.addPage(page);

    page = ShapeFactory::createPage();
    page->setName("Page 2");
    mBlueprint.addPage(page);
}

void TestSerialization::testSerialization()
{
    Parcel* parcel = mBlueprint.toParcel();

//    QCOMPARE(parcel->value("name").toString(), shape->name());
//    QCOMPARE(parcel->value("type").toInt(), static_cast<int>(shape->shapeType()));

    QFile output("test.xml");
    output.open(QIODevice::ReadWrite);
    XmlInputOutput xml;
    xml.write(output, *parcel);

    output.seek(0);
    QString result(output.readAll());

    TestUtils::toggleLogOutput(true);
//    qDebug() << "Result" << result;
    TestUtils::toggleLogOutput(false);

    output.close();
    delete parcel;
}

void TestSerialization::testDeserialization()
{
    QFile input("test.xml");
    input.open(QIODevice::ReadOnly);
    XmlInputOutput xml;
    Parcel* parcel = xml.read(input);
    input.close();

    Blueprint blueprint;
    blueprint.fromParcel(*parcel);
    QCOMPARE(blueprint.name(), mBlueprint.name());
    QCOMPARE(blueprint.pageCount(), mBlueprint.pageCount());

    Page* parsedPage = blueprint.page(0);
    Page* originalPage = mBlueprint.page(0);
    compareShapesBezier(*parsedPage, *originalPage);

    delete parcel;
}

void TestSerialization::compareShapes(const Shape& lhs, const Shape& rhs)
{
    QCOMPARE(lhs.name(), rhs.name());
    QCOMPARE(lhs.childCount(), rhs.childCount());
    QCOMPARE(lhs.shapeType(), rhs.shapeType());
    QCOMPARE(lhs.foregroundColor(), rhs.foregroundColor());
    QCOMPARE(lhs.borderWidth(), rhs.borderWidth());
}

void TestSerialization::compareShapesBezier(const ShapeBezier& lhs, const ShapeBezier& rhs)
{
   compareShapes(lhs, rhs);
   QCOMPARE(lhs.elementCount(), rhs.elementCount());
   for (int i = 0; i < lhs.elementCount(); ++i) {
        const BezierElement* leftElement = lhs.element(i);
        const BezierElement* rightElement = rhs.element(i);
        QCOMPARE(leftElement->index(), rightElement->index());
        QCOMPARE(leftElement->elementType(), rightElement->elementType());
        QCOMPARE(leftElement->pos(), rightElement->pos());
   }
    for (int i = 0; i < lhs.childCount(); ++i) {
        ShapeBezier* s1 = static_cast<ShapeBezier*>(lhs.child(i));
        ShapeBezier* s2 = static_cast<ShapeBezier*>(rhs.child(i));
        compareShapesBezier(*s1, *s2);
    }
}

