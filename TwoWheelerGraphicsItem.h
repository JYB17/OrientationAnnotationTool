#ifndef TWOWHEELERGRAPHICSITEM_H
#define TWOWHEELERGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMenu>

#include "InfoStructure.h"
#include "LabelManager.h"

//constexpr float RAD2DEG = 180.F / 3.141592F;
//constexpr float DEG2RAD = 1.F / RAD2DEG;

class TwoWheelerGraphicsItem : public QGraphicsItem {

    enum eResizeDirection {
        eLT,
        eT,
        eRT,
        eL,
        eR,
        eLB,
        eB,
        eRB,
        eNone
    };
    enum eClickMode {
        select,
        unselect
    };

public:
    explicit TwoWheelerGraphicsItem(LabelManager* label_manager, GtInfo& gt_info, int zValue);
//    ~TwoWheelerGraphicsItem();

private:
    QRectF m_rect;
//    QRectF m_rect_2;
    GtInfo& m_gtInfo;
    eResizeDirection m_itemController;
    LabelManager* m_labelmanager;// = nullptr;

    float x_center;
    float y_center;

//    float rect_w = 0.F;
//    float rect_h = 0.F;

//    float min_x;
//    float min_y;
//    float max_x;
//    float max_y;
//    bool has_one_wheel = false;

    void ResizeTop(QPointF mouse_pos);
    void ResizeLeft(QPointF mouse_pos);
    void ResizeRight(QPointF mouse_pos);
    void ResizeBottom(QPointF mouse_pos);

    void updateGtInfo();

    bool multiple_chosen;

    float calculateAngleInImage(float orig_angle){
//        return 0.F;
        return 360.F*DEG2RAD - (orig_angle*DEG2RAD + 90.F*DEG2RAD);
    }

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
//    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

};


#endif // TWOWHEELERGRAPHICSITEM_H
