#ifndef LABELMANAGER_H
#define LABELMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsItem>

#include "InfoStructure.h"

class LabelManager : public QObject
{
    Q_OBJECT
public:
    explicit LabelManager(QGraphicsScene* scene, QObject* parent = nullptr);
    ~LabelManager();

    void setTopLayer(QGraphicsItem* item);
    void setBottomLayer(QGraphicsItem* item);

//    void unselectOthers(QGraphicsItem* item);

private:
    void updateZValue();

    QGraphicsScene* m_scene;
    QVector<QGraphicsItem*> m_items;

signals:
    void changeObjCls(GtInfo &gt_info);
    void saveUndoRedo();

    void unselectOthers(int32_t curr_idx);
    void setMultiChosen();
    void selectDraggedArea(Bbox &dragged_area);
    void setRiderPoint(float_t x, float_t y);
    void setAddWheelMode(int32_t curr_idx);
//    void dragZoomFocusedArea(float_t x, float_t y);
////    void dragZoomFocusedArea(Bbox &dragged_area);
//    void setStartXY(float_t x, float_t y);

private slots:
    void drawAll(QVector<GtInfo> &gts_info);

};

#endif // LABELMANAGER_H

