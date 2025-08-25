#include "imageItem.h"
#include "path_utils.h"

#include <QPixmap>
#include <QPainter>
#include <QTransform>
#include <QGraphicsSceneMouseEvent>

ImageItem::ImageItem()
{
    QGraphicsItem::setAcceptHoverEvents(true);
    deleted = false;
    inputed = false;
    pixmap = nullptr;                  // مهم: مقداردهی
    rect = QRect(0, 0, 0, 0);
}

ImageItem::ImageItem(const QString& s)
{
    const QString p = imgPath(s);      // مسیر امن کنار exe
    pixmap = new QPixmap(p);
    imagePath = p;
    rect = pixmap->isNull() ? QRect(0,0,0,0) : pixmap->rect();
    QGraphicsItem::setAcceptHoverEvents(true);
    deleted = false;
    inputed = false;
}

ImageItem::ImageItem(const QString& s, const QRect &r)
{
    const QString p = imgPath(s);      // مسیر امن کنار exe
    pixmap = new QPixmap(p);
    imagePath = p;
    rect = r.isNull() ? (pixmap->isNull() ? QRect(0,0,0,0) : pixmap->rect()) : r;
    QGraphicsItem::setAcceptHoverEvents(true);
    deleted = false;
    inputed = false;
}

void ImageItem::setPixmap(const QString &s)
{
    const QString p = imgPath(s);      // همیشه مسیر را نرمال کن
    imagePath = p;
    if (!pixmap) {
        pixmap = new QPixmap(p);
    } else {
        *pixmap = QPixmap(p);          // واقعاً تصویر را عوض کن
    }
    // اگر می‌خواهی اندازهٔ قبلی حفظ شود، خط بعدی را حذف کن
    rect = pixmap->isNull() ? rect : pixmap->rect();
    update();                          // صحنه را ریفرش کن
}

void ImageItem::setRect(const QRect &r)
{
    rect = r;
    update();
}

void ImageItem::setName(const QString &s)
{
    name = s;
    update();
}

void ImageItem::setCharacteristic(const QString &c)
{
    characteristic = c;
    update();
}

QString ImageItem::getimagePath()
{
    return imagePath;
}

const QRect& ImageItem::getRect()
{
    return rect;
}

void ImageItem::rotate90(int rotateA)
{
    QTransform trans;
    trans.translate(rect.width()/2, rect.height()/2);
    trans.rotate(rotateA + 90);
    trans.translate(-rect.width()/2, -rect.height()/2);
    this->setTransform(trans);
}

void ImageItem::Delete()
{
    deleted = true;
    update();
}

void ImageItem::setInput(bool tf)
{
    inputed = tf;
    update();
}

bool ImageItem::getInput()
{
    return inputed;
}

bool ImageItem::getDelete()
{
    return deleted;
}

void ImageItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    if (!pixmap || pixmap->isNull()) return;   // جلوگیری از کرش

    QFont font = painter->font();
    font.setPixelSize(20);
    painter->setFont(font);
    painter->drawPixmap(rect, *pixmap);
    painter->drawText(rect, name);

    QRect crect = rect;
    font.setPixelSize(10);
    painter->setFont(font);
    crect.setX(rect.x() + rect.width() * 0.65);
    crect.setY(rect.y() + rect.height() * 0.1);
    painter->drawText(crect, characteristic);
}

QRectF ImageItem::boundingRect() const
{
    return rect;
}

void ImageItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    QGraphicsItem::mousePressEvent(e);
}

ImageItem::~ImageItem()
{
    delete pixmap;
}
