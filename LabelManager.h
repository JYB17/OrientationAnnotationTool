#ifndef LABELMANAGER_H
#define LABELMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsItem>

#include "InfoStructure.h"

constexpr float RAD2DEG = 180.F / 3.141592F;
constexpr float DEG2RAD = 1.F / RAD2DEG;

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

    void unselectOthers(int curr_idx);
    void clickBackground();
    void setMultiChosen();
    void selectDraggedArea(Bbox &dragged_area);
    void setRiderPoint(float x, float y);
    void setAddWheelMode(int curr_idx);

    void setNewGtMode();

private slots:
    void drawAll(QVector<GtInfo> &gts_info, bool change_gt_mode);
//    void drawAll(QVector<GtInfo> &gts_info);

};

#endif // LABELMANAGER_H

