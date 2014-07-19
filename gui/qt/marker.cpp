#include "marker.h"

marker::marker(qreal x1, qreal y1, int markerType, const QPen &pen, const QBrush &brush)
{
  prepareGeometryChange();
  if (markerType == marker::haltMarker)
  {
    wid = 4;
    hei = 8;
  }
  else if (markerType == marker::wedge)
  {
    wid = 10;
    hei = 12;
  }
  else if (markerType == marker::labelWedge)
  {
    QFontMetrics fm(QFont("Tahoma", 8, QFont::Bold));
    int pixelsWide = fm.width("W");
    int pixelHeight = fm.height();
    wid = pixelsWide;
    hei = 8 + pixelHeight + 2; //arrow height + font height + space between
  }
  else if (markerType == marker::invertedWedge)
  {
    wid = 10;
    hei = 12;
  }
  else if (markerType == marker::smallInvertedWedge)
  {
    wid = 6;
    hei = 8;
  }
  this->markerType = markerType;

  this->pen = pen;
  this->brush = brush;
  reverseHalt = false;
  setPos(x1,y1);
}

marker::marker(qreal x1, qreal y1, qreal w1, qreal h1, const QPen &pen, const QBrush &brush)
{
  prepareGeometryChange();
  wid = w1;
  hei = h1;
  this->pen = pen;
  this->brush = brush;
  reverseHalt = false;
  setPos(x1,y1);
  //setAcceptHoverEvents(true);
}

marker::~marker()
{
}
QRectF marker::boundingRect() const
{

  QRectF rec(-wid/2,-hei/2,wid,hei);
  return rec;
}
void marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  QRectF p(boundingRect());
  QPolygonF polygon;

  painter->setPen(pen);
  painter->setBrush(brush);
  painter->setFont(QFont("Tahoma", 8, QFont::Bold));
  if (markerType == marker::haltMarker)
  {
    if (!reverseHalt)
    {
      QPointF mid = QPointF(p.right(),0);

      polygon << p.topLeft();
      polygon << p.bottomLeft();
      polygon << mid;
      painter->drawPolygon(polygon);
    }
    else
    {
      QPolygonF polygon;
      QPointF mid = QPointF(p.left(),0);

      polygon << p.topRight();
      polygon << p.bottomRight();
      polygon << mid;
      painter->drawPolygon(polygon);
    }
  }
  else if (markerType == marker::wedge)
  {
    QPointF mid = QPointF(0,p.bottom());

    polygon << p.topLeft();
    polygon << p.topRight();
    polygon << mid;
    painter->drawPolygon(polygon);
  }
  else if (markerType == marker::labelWedge)
  {
    QPointF mid = QPointF(0,p.bottom()-1);
    QFontMetrics fm(painter->font());
    int wi = fm.width("L");

    polygon << QPointF(p.left()+1, p.bottom() - 8);
    polygon << QPointF(p.right()-1, p.bottom() - 8);
    polygon << mid;
    painter->drawPolygon(polygon);
    QString chara = "L";
    painter->drawText(0 - wi/2, 2 ,chara);

  }
  else if (markerType == marker::invertedWedge)
  {
    QPointF mid = QPointF(0,p.top());

    polygon << p.bottomLeft();
    polygon << p.bottomRight();

    polygon << mid;
    painter->drawPolygon(polygon);
  }
  else if (markerType == marker::smallInvertedWedge)
  {
  /*  QPainterPath mark;
    QPen pen;

    mark.moveTo(x,y);
    mark.lineTo(x-4,y+6);
    mark.lineTo(x+4,y+6);
    mark.lineTo(x,y);

    pen.setBrush(QColor(Qt::magenta));
    pen.setWidth(1);

    graphScene->addPath(mark, pen);*/
  }
}

QPainterPath marker::shape() const
{
  QPainterPath path;
   path.addEllipse(boundingRect());
   return path;
}

void marker::setMarkerType(int markerType)
{
  this->markerType = markerType;
}

void marker::setReverseHaltMarker(bool yes)
{
  this->reverseHalt = yes;
}

void marker::hdopToColor(float hdopm, QColor &color)
{
  if (hdopm > 10)
  {
    color = Qt::red;
  }
  else if (hdopm < 0.005)
  {
    color = Qt::green;
  }
  else if (hdopm < 2)
  {
    color.setRgb(hdopm * 255/5,255,0);
  }
  else
  {
    color.setRgb(255, 255 - (hdopm - 5) * 255/5,0);
  }
}
