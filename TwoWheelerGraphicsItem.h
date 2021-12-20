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

//constexpr float_t RAD2DEG = 180.F / 3.141592F;
//constexpr float_t DEG2RAD = 1.F / RAD2DEG;

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

    float_t x_center;
    float_t y_center;

//    float_t rect_w = 0.F;
//    float_t rect_h = 0.F;

//    float_t min_x;
//    float_t min_y;
//    float_t max_x;
//    float_t max_y;
//    bool has_one_wheel = false;

    void ResizeTop(QPointF mouse_pos);
    void ResizeLeft(QPointF mouse_pos);
    void ResizeRight(QPointF mouse_pos);
    void ResizeBottom(QPointF mouse_pos);

    void updateGtInfo();

    bool multiple_chosen;

    float_t calculateAngleInImage(float_t orig_angle){
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
