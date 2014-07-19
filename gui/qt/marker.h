#ifndef POINTITEM_H
#define POINTITEM_H

#include <QGraphicsItem>
#include <QtGui>


class marker : public QGraphicsItem
{
    //Q_OBJECT
public:
  enum { Type = UserType + 1 };
  enum markerTypes
  {
    haltMarker = 0,
    wedge,
    labelWedge,
    invertedWedge,
    smallInvertedWedge
  };

  int type() const { return Type; }
  marker(qreal x1, qreal y1, int markerType, const QPen &pen = QPen(Qt::white), const QBrush &brush = QBrush());
  marker(qreal x, qreal y, qreal w, qreal h,const QPen &pen = QPen(Qt::white), const QBrush &brush = QBrush());
  ~marker();
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  QPainterPath shape() const;
  void setMarkerType(int markerType);
  void setReverseHaltMarker(bool yes);

protected:

private:
  qreal wid;
  qreal hei;
  QPen pen;
  QBrush brush;
  int markerType;
  bool reverseHalt;

  void hdopToColor(float hdopm, QColor &color);

};

#endif // POINTITEM_H
