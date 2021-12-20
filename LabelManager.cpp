#include "LabelManager.h"
#include "OrientationGraphicsItem.h"
#include "TwoWheelerGraphicsItem.h"

LabelManager::LabelManager(QGraphicsScene* scene, QObject* parent) : QObject(parent)
{
    this->m_scene = scene;
}

LabelManager::~LabelManager()
{
}

void LabelManager::drawAll(QVector<GtInfo> &gts_info, bool change_gt_mode)
{
    for (auto& item : m_items) {
        delete item;
    }
    m_items.clear();
//    int32_t invalid_idx = -1;
//    TwoWheelerGraphicsItem *invalid_item = nullptr;
    for (int i = 0; i < gts_info.size(); i++) {
//        OrientationGraphicsItem *item = nullptr;
        if(gts_info[i].is_2_wheeler==false){
            OrientationGraphicsItem *item = new OrientationGraphicsItem(this, gts_info[i], i);
            m_items.append(item);
            m_scene->addItem(item);
        }
        else{
            TwoWheelerGraphicsItem *item = new TwoWheelerGraphicsItem(this, gts_info[i], i);
            m_items.append(item);
            m_scene->addItem(item);
//            if(gts_info[i].is_valid==false){
//                invalid_idx = i;
////                invalid_item = item;
//            }
        }
        gts_info[i].curr_idx = i;
//        m_items.append(item);
//        m_scene->addItem(item);
    }

    if(change_gt_mode==true){
        emit setNewGtMode();
    }
}

void LabelManager::setTopLayer(QGraphicsItem* item)
{
    m_items.move(m_items.indexOf(item), m_items.size() - 1);
    updateZValue();
}

void LabelManager::setBottomLayer(QGraphicsItem* item)
{
    m_items.move(m_items.indexOf(item), 0);
    updateZValue();
}

void LabelManager::updateZValue()
{
    int i = 0;
    for (auto& item : m_items) {
        item->setZValue(i);
        i++;
    }
}
