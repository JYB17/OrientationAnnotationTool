#include "TwoWheelerGraphicsItem.h"
#include <cmath>

TwoWheelerGraphicsItem::TwoWheelerGraphicsItem(LabelManager* label_manager, GtInfo& gt_info, int zValue):
    m_gtInfo(gt_info)
{
    float rect_w = gt_info.bbox.x2 - gt_info.bbox.x1;
    float rect_h = gt_info.bbox.y2 - gt_info.bbox.y1;

    m_rect.setX(gt_info.bbox.x1);
    m_rect.setY(gt_info.bbox.y1);
    m_rect.setWidth(rect_w);
    m_rect.setHeight(rect_h);

    x_center = (gt_info.bbox.x2 + gt_info.bbox.x1)/2.F;
    y_center = (gt_info.bbox.y2 + gt_info.bbox.y1)/2.F;

    setFlags(ItemIsMovable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);

    setZValue(zValue);
    this->m_labelmanager = label_manager;

    m_itemController = eNone;

    multiple_chosen = false;
}

void TwoWheelerGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if(m_gtInfo.is_background==true){
        m_labelmanager->setBottomLayer(this);
    }
    if(m_gtInfo.draw_enabled==true && m_gtInfo.is_background==false){
        QPen pen = painter->pen();
        painter->setPen(pen);

        float rect_w = m_gtInfo.bbox.x2 - m_gtInfo.bbox.x1;
        float rect_h = m_gtInfo.bbox.y2 - m_gtInfo.bbox.y1;

        if(m_gtInfo.is_chosen==true){
//            box_color = Qt::magenta;
            QBrush brush(Qt::white);
            painter->setBrush(brush);
            pen.setColor(Qt::white);
            pen.setWidth(4);
            painter->setPen(pen);
            if(m_gtInfo.is_svnet_rider==true){
                painter->drawRect(m_gtInfo.bbox.x1 - 6,m_gtInfo.bbox.y1 - 6, 12, 12);
                painter->drawRect(m_gtInfo.bbox.x1 - 6,m_gtInfo.bbox.y2 - 6, 12, 12);
                painter->drawRect(m_gtInfo.bbox.x2 - 6,m_gtInfo.bbox.y1 - 6, 12, 12);
                painter->drawRect(m_gtInfo.bbox.x2 - 6,m_gtInfo.bbox.y2 - 6, 12, 12);
            }
            else{
                painter->drawRect(m_gtInfo.x[0] - 6,m_gtInfo.y[0] - 6, 12, 12);
                painter->drawRect(m_gtInfo.x[1] - 6,m_gtInfo.y[1] - 6, 12, 12);
                painter->drawRect(m_gtInfo.x[2] - 6,m_gtInfo.y[2] - 6, 12, 12);
                painter->drawRect(m_gtInfo.x[3] - 6,m_gtInfo.y[3] - 6, 12, 12);
                painter->drawRect(m_gtInfo.x[4] - 6,m_gtInfo.y[4] - 6, 12, 12);
                painter->drawRect(m_gtInfo.x[5] - 6,m_gtInfo.y[5] - 6, 12, 12);
                painter->drawRect(m_gtInfo.x[6] - 6,m_gtInfo.y[6] - 6, 12, 12);
                painter->drawRect(m_gtInfo.x[7] - 6,m_gtInfo.y[7] - 6, 12, 12);
            }

            m_rect.setX(m_gtInfo.bbox.x1-0.5F*rect_w);
            m_rect.setY(m_gtInfo.bbox.y1);
            m_rect.setWidth(rect_w*2.F);
            m_rect.setHeight(rect_h*1.25F);

            pen.setWidth(1);
            pen.setColor(Qt::gray);
            painter->setPen(pen);
            painter->setBrush(Qt::transparent);
            painter->drawRect(m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height());
        }
        else{
            m_rect.setX(m_gtInfo.bbox.x1);
            m_rect.setY(m_gtInfo.bbox.y1);
            m_rect.setWidth(rect_w);
            m_rect.setHeight(rect_h);
        }

        pen.setColor(Qt::yellow);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(Qt::transparent);

        if(m_gtInfo.is_svnet_rider==true){
            painter->drawRect(m_gtInfo.bbox.x1, m_gtInfo.bbox.y1, m_gtInfo.bbox.x2-m_gtInfo.bbox.x1, m_gtInfo.bbox.y2-m_gtInfo.bbox.y1);

            QBrush brush(Qt::magenta);
            pen.setColor(Qt::magenta);
            painter->setBrush(brush);
            painter->setPen(pen);

            if(!(m_gtInfo.front_x==-1.F && m_gtInfo.front_y==-1.F)){
                painter->drawRect(m_gtInfo.front_x-2.F, m_gtInfo.front_y-2.F, 4, 4);
            }
            if(!(m_gtInfo.rear_x==-1.F && m_gtInfo.rear_y==-1.F)){
                painter->drawRect(m_gtInfo.rear_x-2.F, m_gtInfo.rear_y-2.F, 4, 4);
            }

            if(!(m_gtInfo.front_x==-1.F && m_gtInfo.front_y==-1.F) && !(m_gtInfo.rear_x==-1.F && m_gtInfo.rear_y==-1.F)){
                painter->setPen(pen);
                painter->drawLine(m_gtInfo.front_x, m_gtInfo.front_y, m_gtInfo.rear_x, m_gtInfo.rear_y);
            }
        }
        else{
            painter->drawLine(m_gtInfo.x[0], m_gtInfo.y[0], m_gtInfo.x[1], m_gtInfo.y[1]);
            painter->drawLine(m_gtInfo.x[1], m_gtInfo.y[1], m_gtInfo.x[3], m_gtInfo.y[3]);
            painter->drawLine(m_gtInfo.x[2], m_gtInfo.y[2], m_gtInfo.x[0], m_gtInfo.y[0]);
            painter->drawLine(m_gtInfo.x[3], m_gtInfo.y[3], m_gtInfo.x[2], m_gtInfo.y[2]);

            painter->drawLine(m_gtInfo.x[4], m_gtInfo.y[4], m_gtInfo.x[5], m_gtInfo.y[5]);
            painter->drawLine(m_gtInfo.x[5], m_gtInfo.y[5], m_gtInfo.x[7], m_gtInfo.y[7]);
            painter->drawLine(m_gtInfo.x[6], m_gtInfo.y[6], m_gtInfo.x[4], m_gtInfo.y[4]);
            painter->drawLine(m_gtInfo.x[7], m_gtInfo.y[7], m_gtInfo.x[6], m_gtInfo.y[6]);

            painter->drawLine(m_gtInfo.x[0], m_gtInfo.y[0], m_gtInfo.x[4], m_gtInfo.y[4]);
            painter->drawLine(m_gtInfo.x[1], m_gtInfo.y[1], m_gtInfo.x[5], m_gtInfo.y[5]);
            painter->drawLine(m_gtInfo.x[2], m_gtInfo.y[2], m_gtInfo.x[6], m_gtInfo.y[6]);
            painter->drawLine(m_gtInfo.x[3], m_gtInfo.y[3], m_gtInfo.x[7], m_gtInfo.y[7]);

            pen.setColor(Qt::magenta);
            painter->setPen(pen);
            painter->drawText(x_center, y_center, QString::number(m_gtInfo.dir_angle));
            float draw_angle = calculateAngleInImage(m_gtInfo.dir_angle);
            float x_end = x_center+50.F*std::cos(draw_angle);
            float y_end = y_center+50.F*std::sin(draw_angle);
            painter->drawLine(x_center, y_center, x_end, y_end);
            painter->drawLine(x_end, y_end, x_center+45.F*cos(calculateAngleInImage(m_gtInfo.dir_angle-10.F)), y_center+45.F*sin(calculateAngleInImage(m_gtInfo.dir_angle-10.F)));
            painter->drawLine(x_end, y_end, x_center+45.F*cos(calculateAngleInImage(m_gtInfo.dir_angle+10.F)), y_center+45.F*sin(calculateAngleInImage(m_gtInfo.dir_angle+10.F)));
        }

        widget->update();
    }
}

QRectF TwoWheelerGraphicsItem::boundingRect() const{
    return this->m_rect;
}

void TwoWheelerGraphicsItem::updateGtInfo()
{
//    auto updatedPos = mapRectToScene(m_rect);
//    m_gtInfo.bbox.x1 = updatedPos.left();
//    m_gtInfo.bbox.y1 = updatedPos.top();
//    m_gtInfo.bbox.x2 = updatedPos.right();
//    m_gtInfo.bbox.y2 = updatedPos.bottom();
}

void TwoWheelerGraphicsItem::ResizeTop(QPointF mouse_pos)
{
//    m_rect.setY(mouse_pos.y());
//    m_rect.setY(std::max(0, int(mouse_pos.y())));
}

void TwoWheelerGraphicsItem::ResizeLeft(QPointF mouse_pos)
{
//    m_rect.setX(mouse_pos.x());
//    m_rect.setX(std::max(0, int(mouse_pos.x())));
}

void TwoWheelerGraphicsItem::ResizeRight(QPointF mouse_pos)
{
//    mouse_pos.rx() -= m_rect.x();
//    m_rect.setWidth(mouse_pos.x());
//    m_rect.setWidth(std::max(int(mouse_pos.x()), 1920));
}

void TwoWheelerGraphicsItem::ResizeBottom(QPointF mouse_pos)
{
//    mouse_pos.ry() -= m_rect.y();
//    m_rect.setHeight(mouse_pos.y());
//    m_rect.setWidth(std::max(int(mouse_pos.y()), 1080));
}

void TwoWheelerGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
//    // Add function for undo/redo
//    if(m_gtInfo.is_chosen==true && m_gtInfo.is_first==false){
//        emit m_labelmanager->saveUndoRedo();
//    }
//    updateGtInfo();
////    qDebug() << event->pos().x();
//    if (m_gtInfo.move_resize == true) {
//        QGraphicsItem::mouseReleaseEvent(event);
//        m_gtInfo.move_resize = false;
//        m_gtInfo.is_chosen = true;
//    }
//    m_labelmanager->setTopLayer(this);
//    m_itemController = eNone;
    if(m_gtInfo.is_background==true && event->modifiers() == Qt::Modifier::CTRL && event->button() == Qt::LeftButton && m_gtInfo.vehicle_mode==false){
        m_gtInfo.drag_chosen_area.x2 = event->pos().x();
        m_gtInfo.drag_chosen_area.y2 = event->pos().y();
        if(m_gtInfo.drag_chosen_area.x1>m_gtInfo.drag_chosen_area.x2){
            m_gtInfo.drag_chosen_area.x2 = m_gtInfo.drag_chosen_area.x1;
            m_gtInfo.drag_chosen_area.x1 = event->pos().x();
        }
        if(m_gtInfo.drag_chosen_area.y1>m_gtInfo.drag_chosen_area.y2){
            m_gtInfo.drag_chosen_area.y2 = m_gtInfo.drag_chosen_area.y1;
            m_gtInfo.drag_chosen_area.y1 = event->pos().y();
        }

        emit m_labelmanager->selectDraggedArea(m_gtInfo.drag_chosen_area);
    }
    else if(m_gtInfo.is_background==true && m_gtInfo.is_not_dragging==true && event->button() == Qt::LeftButton){
        m_gtInfo.is_not_dragging = false;

        qDebug() << "unselect all!";
//        if(m_gtInfo.vehicle_mode==false){
//            // set rider points
//            emit m_labelmanager->setRiderPoint(event->pos().x(), event->pos().y());
//        }
//        else{
//            emit m_labelmanager->unselectOthers(-1);
//        }
        emit m_labelmanager->unselectOthers(-1);
    }
//    else if(m_gtInfo.is_background==true && m_gtInfo.is_not_dragging==false && event->button() == Qt::LeftButton){
//        // zoom dragging
//    }
//    if(m_gtInfo.is_chosen==true && m_gtInfo.is_first==true){
    else if(m_gtInfo.is_chosen==true && m_gtInfo.is_first==true){
        m_gtInfo.is_first = false;
    }
////    qDebug() << m_gtInfo.bbox.x1 << m_gtInfo.bbox.y1 << m_gtInfo.bbox.x2 << m_gtInfo.bbox.y2;
////    emit m_labelmanager->updateChangedInfos(m_gtInfo, zValue());
}

void TwoWheelerGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(m_gtInfo.is_background==true && event->modifiers() != Qt::Modifier::CTRL){// && event->button()==Qt::LeftButton){
        m_gtInfo.is_not_dragging = false;

////        m_gtInfo.drag_chosen_area.x2 = event->pos().x();
////        m_gtInfo.drag_chosen_area.y2 = event->pos().y();
//        float curr_x = event->pos().x();
//        float curr_y = event->pos().y();

////        emit m_labelmanager->dragZoomFocusedArea(m_gtInfo.drag_chosen_area);
//        emit m_labelmanager->dragZoomFocusedArea(curr_x, curr_y);

//        m_gtInfo.drag_chosen_area.x1 = (int32_t)event->pos().x();
//        m_gtInfo.drag_chosen_area.y1 = (int32_t)event->pos().y();
    }
//    if (m_gtInfo.is_chosen == true) {
//        m_gtInfo.move_resize = true;
//        if (event->buttons() & Qt::LeftButton && m_itemController == eNone) {
////            if(m_gtInfo.is_first==false)
////                QGraphicsItem::mouseMoveEvent(event);
//        }
//        else if (event->buttons() & Qt::LeftButton && m_itemController != eNone) {
//            auto mouse_pos = event->pos();
//            if (m_itemController == eB) {
//                ResizeBottom(mouse_pos);
//            }
//            if (m_itemController == eR) {
//                ResizeRight(mouse_pos);
//            }
//            if (m_itemController == eT) {
//                ResizeTop(mouse_pos);
//            }
//            if (m_itemController == eL) {
//                ResizeLeft(mouse_pos);
//            }
//            if (m_itemController == eLT) {
//                ResizeLeft(mouse_pos);
//                ResizeTop(mouse_pos);
//            }
//            if (m_itemController == eRT) {
//                ResizeRight(mouse_pos);
//                ResizeTop(mouse_pos);
//            }
//            if (m_itemController == eLB) {
//                ResizeLeft(mouse_pos);
//                ResizeBottom(mouse_pos);
//            }
//            if (m_itemController == eRB) {
//                ResizeRight(mouse_pos);
//                ResizeBottom(mouse_pos);
//            }

//            prepareGeometryChange();

////            qDebug() << m_gtInfo.bbox.x1 << m_gtInfo.bbox.y1 << m_gtInfo.bbox.x2 << m_gtInfo.bbox.y2;
////            emit m_labelmanager->updateChangedInfos(m_gtInfo, zValue());
//        }
//    }
}

void TwoWheelerGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
     if(m_gtInfo.is_background==true){
        m_labelmanager->setBottomLayer(this);
        if(event->modifiers() == Qt::Modifier::CTRL && event->button() == Qt::LeftButton){
            // multi select by drag
            m_gtInfo.drag_chosen_area.x1 = event->pos().x();
            m_gtInfo.drag_chosen_area.y1 = event->pos().y();
        }
        else if(event->button() == Qt::LeftButton){
            m_gtInfo.is_not_dragging = true;

        }
    }
    else if (event->modifiers() == Qt::Modifier::CTRL && event->button() == Qt::LeftButton){// && m_gtInfo.mode_edit == true) {
        if (m_gtInfo.is_chosen == false && m_gtInfo.draw_enabled==true && m_gtInfo.vehicle_mode==false){
            m_gtInfo.is_chosen = true;
            m_labelmanager->setTopLayer(this);
            m_gtInfo.multi_chosen = true;
            emit m_labelmanager->setMultiChosen();
        }
        else if (m_gtInfo.is_chosen == true && m_gtInfo.draw_enabled==true && m_gtInfo.vehicle_mode==false){
            m_gtInfo.is_chosen = false;
            if(m_gtInfo.is_first==false){
                m_gtInfo.is_first = true;
            }
            m_labelmanager->setBottomLayer(this);
            m_gtInfo.multi_chosen = false;
        }
    }
    else if (event->button() == Qt::LeftButton){
        if (m_gtInfo.is_chosen == false && m_gtInfo.draw_enabled==true && m_gtInfo.vehicle_mode==false && m_gtInfo.zoom_mode==false){
            m_gtInfo.is_chosen = true;
            m_labelmanager->setTopLayer(this);
            emit m_labelmanager->setAddWheelMode(m_gtInfo.curr_idx);
        }
        else if(m_gtInfo.is_chosen==true && m_gtInfo.draw_enabled==true && m_gtInfo.multi_chosen==true && m_gtInfo.vehicle_mode==false){
            m_gtInfo.is_chosen = true;
            m_labelmanager->setTopLayer(this);

            emit m_labelmanager->unselectOthers(m_gtInfo.curr_idx);
        }
        else if(m_gtInfo.is_chosen==true &&
                m_gtInfo.draw_enabled==true &&
                m_gtInfo.multi_chosen==false &&
                m_gtInfo.vehicle_mode==false &&
                m_gtInfo.is_adding_wheel==true)
        {
            m_labelmanager->setTopLayer(this);

            emit m_labelmanager->setRiderPoint(event->pos().x(), event->pos().y());
        }
        else if(m_gtInfo.is_chosen==true && m_gtInfo.draw_enabled==true && m_gtInfo.vehicle_mode==false){
            m_gtInfo.is_chosen = false;
            if(m_gtInfo.is_first==false){
                m_gtInfo.is_first = true;
            }
            m_labelmanager->setBottomLayer(this);

        }
    }
    else if (event->button() == Qt::RightButton) {
        QMenu myMenu;
        myMenu.addAction("Set Top Layer");
        myMenu.addAction("Set Bottom Layer");
        myMenu.setGeometry(QCursor::pos().x(), QCursor::pos().y(), myMenu.sizeHint().width(), myMenu.sizeHint().height());
        auto selectedAction = myMenu.exec();
        if (selectedAction != nullptr) {
            if (selectedAction->text() == "Set Top Layer") {
                m_labelmanager->setTopLayer(this);
            }
            else if (selectedAction->text() == "Set Bottom Layer") {
                m_labelmanager->setBottomLayer(this);
            }
        }
    }
}

void TwoWheelerGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
//    if (m_gtInfo.is_chosen == true){// && m_gtInfo.mode_edit == true) {
////        m_LabelManager->CheckMousePos(event->pos());
//        auto mouse_pos = event->pos();
//        auto rect_pos = mapRectToScene(m_rect);
//        mouse_pos.rx() -= m_rect.x();
//        mouse_pos.ry() -= m_rect.y();

//        bool TopEdgeIn = abs(mouse_pos.y()) < 5;
//        bool LeftEdgeIn = abs(mouse_pos.x()) < 5;
//        bool RightEdgeIn = abs(mouse_pos.x() - rect_pos.width()) < 5;
//        bool BottomEdgeIn = abs(mouse_pos.y() - rect_pos.height()) < 5;

//        if ((LeftEdgeIn && TopEdgeIn) ||
//            (RightEdgeIn && BottomEdgeIn)) {
//            if (LeftEdgeIn && TopEdgeIn) {
//                m_itemController = eLT;
//            }
//            else {
//                m_itemController = eRB;
//            }
//            this->setCursor(Qt::SizeFDiagCursor);

//        }
//        else if ((RightEdgeIn && TopEdgeIn) ||
//                 (LeftEdgeIn && BottomEdgeIn)) {
//            if (RightEdgeIn && TopEdgeIn) {
//                m_itemController = eRT;
//            }
//            else {
//                m_itemController = eLB;
//            }
//            this->setCursor(Qt::SizeBDiagCursor);
//        }
//        else if (TopEdgeIn || BottomEdgeIn) {
//            if (TopEdgeIn) {
//                m_itemController = eT;
//            }
//            else {
//                m_itemController = eB;
//            }
//            this->setCursor(Qt::SizeVerCursor);
//        }
//        else if (LeftEdgeIn || RightEdgeIn) {
//            if (LeftEdgeIn) {
//                m_itemController = eL;
//            }
//            else {
//                m_itemController = eR;
//            }
//            this->setCursor(Qt::SizeHorCursor);
//        }
//        else {
//            m_itemController = eNone;
//            this->setCursor(Qt::ArrowCursor);
//        }
//        QGraphicsItem::hoverMoveEvent(event);
//    }
}
