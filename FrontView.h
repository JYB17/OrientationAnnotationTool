#ifndef FRONTVIEW_H
#define FRONTVIEW_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneWheelEvent>
#include <QImage>
#include <QPixmap>

class FrontView : public QObject {
    Q_OBJECT

public:
    explicit FrontView(QGraphicsScene* scene);
    QGraphicsPixmapItem* getFrontViewItem()
    {
        return sceneItem;
    }

private slots:
    void UpdateFrontImage(QImage* img);

private:
    QGraphicsPixmapItem* sceneItem;
    QPixmap qpixmap;

protected:
};

#endif // FRONTVIEW_H
