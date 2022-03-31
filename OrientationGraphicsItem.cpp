#include "OrientationGraphicsItem.h"

OrientationGraphicsItem::OrientationGraphicsItem(LabelManager* label_manager, GtInfo& gt_info, int zValue):
    m_gtInfo(gt_info)
{
    m_rect.setX(gt_info.bbox.x1);
    m_rect.setY(gt_info.bbox.y1);
    m_rect.setWidth(gt_info.bbox.x2 - gt_info.bbox.x1);
    m_rect.setHeight(gt_info.bbox.y2 - gt_info.bbox.y1);

    setFlags(ItemIsMovable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
//    setAcceptDrops(true);

    setZValue(zValue);
    this->m_labelmanager = label_manager;

    m_itemController = eNone;

    x_center = (m_gtInfo.bbox.x1+m_gtInfo.bbox.x2)/2.F;
    y_center = (m_gtInfo.bbox.y1+m_gtInfo.bbox.y2)/2.F;

    multiple_chosen = false;
}

//OrientationGraphicsItem::~OrientationGraphicsItem(){
//    delete m_labelmanager;
//}

void OrientationGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if(m_gtInfo.is_background==true){
        m_labelmanager->setBottomLayer(this);
    }
    if(m_gtInfo.draw_enabled==true && m_gtInfo.is_background==false){
        QPen pen = painter->pen();
        QColor box_color = Qt::green;
//        if(m_gtInfo.obj_cls.endsWith("ignored"))
//            box_color = Qt::magenta;

        pen.setWidth(2);
//        if(m_gtInfo.move_resize==true)
//            pen.setWidth(1);
        if (m_gtInfo.is_chosen == true) {// && m_gtInfo.move_resize == false) {
            QBrush brush(Qt::white);
            painter->setBrush(brush);
            pen.setColor(Qt::white);
            pen.setWidth(4);
            painter->setPen(pen);
            painter->drawRect(m_gtInfo.x[0] - 6,m_gtInfo.y[0] - 6, 12, 12);
            painter->drawRect(m_gtInfo.x[1] - 6,m_gtInfo.y[1] - 6, 12, 12);
            painter->drawRect(m_gtInfo.x[2] - 6,m_gtInfo.y[2] - 6, 12, 12);
            painter->drawRect(m_gtInfo.x[3] - 6,m_gtInfo.y[3] - 6, 12, 12);
            painter->drawRect(m_gtInfo.x[4] - 6,m_gtInfo.y[4] - 6, 12, 12);
            painter->drawRect(m_gtInfo.x[5] - 6,m_gtInfo.y[5] - 6, 12, 12);
            painter->drawRect(m_gtInfo.x[6] - 6,m_gtInfo.y[6] - 6, 12, 12);
            painter->drawRect(m_gtInfo.x[7] - 6,m_gtInfo.y[7] - 6, 12, 12);
//            m_LabelManager->setTopLayer(this);
        }
        pen.setColor(box_color);

        painter->setPen(pen);

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

        pen.setColor(Qt::red);
        painter->setPen(pen);
        painter->drawText(x_center, y_center, QString::number(m_gtInfo.dir_angle));
        float draw_angle = calculateAngleInImage(m_gtInfo.dir_angle);
        float x_end = x_center+50.F*cos(draw_angle);
        float y_end = y_center+50.F*sin(draw_angle);
        painter->drawLine(x_center, y_center, x_end, y_end);
        painter->drawLine(x_end, y_end, x_center+45.F*cos(calculateAngleInImage(m_gtInfo.dir_angle-10.F)), y_center+45.F*sin(calculateAngleInImage(m_gtInfo.dir_angle-10.F)));
        painter->drawLine(x_end, y_end, x_center+45.F*cos(calculateAngleInImage(m_gtInfo.dir_angle+10.F)), y_center+45.F*sin(calculateAngleInImage(m_gtInfo.dir_angle+10.F)));

//        painter->drawRect(m_rect);
////        updateGtInfo();

//        if(m_gtInfo.show_cls==true && m_gtInfo.move_resize==false && m_gtInfo.is_chosen==true){
//            pen.setColor(Qt::black);
//            pen.setWidth(5);
//            painter->setPen(pen);
//            painter->drawText(m_rect.x()-4, m_rect.y()-8, m_gtInfo.obj_cls);
//            pen.setColor(box_color);
//            pen.setWidth(3);
//            painter->setPen(pen);
//            painter->drawText(m_rect.x()-4, m_rect.y()-8, m_gtInfo.obj_cls);
//        }

//        if (m_gtInfo.is_chosen == true && m_gtInfo.move_resize == false) {
//            QBrush brush(Qt::white);
//            painter->setBrush(brush);
//            pen.setColor(Qt::white);
//            painter->setPen(pen);
//            painter->drawRect(m_rect.x() - 6, m_rect.y() - 6, 12, 12);
//            painter->drawRect(m_rect.x() + m_rect.width() - 6, m_rect.y() - 6, 12, 12);
//            painter->drawRect(m_rect.x() - 6, m_rect.y() + m_rect.height() - 6, 12, 12);
//            painter->drawRect(m_rect.x() + m_rect.width() - 6, m_rect.y() + m_rect.height() - 6, 12, 12);
////            m_LabelManager->setTopLayer(this);
//        }

        widget->update();
    }
}

QRectF OrientationGraphicsItem::boundingRect() const{
    return this->m_rect;
}

void OrientationGraphicsItem::updateGtInfo()
{
//    auto updatedPos = mapRectToScene(m_rect);
//    m_gtInfo.bbox.x1 = updatedPos.left();
//    m_gtInfo.bbox.y1 = updatedPos.top();
//    m_gtInfo.bbox.x2 = updatedPos.right();
//    m_gtInfo.bbox.y2 = updatedPos.bottom();
}

void OrientationGraphicsItem::ResizeTop(QPointF mouse_pos)
{
//    m_rect.setY(mouse_pos.y());
//    m_rect.setY(std::max(0, int(mouse_pos.y())));
}

void OrientationGraphicsItem::ResizeLeft(QPointF mouse_pos)
{
//    m_rect.setX(mouse_pos.x());
//    m_rect.setX(std::max(0, int(mouse_pos.x())));
}

void OrientationGraphicsItem::ResizeRight(QPointF mouse_pos)
{
//    mouse_pos.rx() -= m_rect.x();
//    m_rect.setWidth(mouse_pos.x());
//    m_rect.setWidth(std::max(int(mouse_pos.x()), 1920));
}

void OrientationGraphicsItem::ResizeBottom(QPointF mouse_pos)
{
//    mouse_pos.ry() -= m_rect.y();
//    m_rect.setHeight(mouse_pos.y());
//    m_rect.setWidth(std::max(int(mouse_pos.y()), 1080));
}

void OrientationGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
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
    if(m_gtInfo.is_background==true && event->modifiers() == Qt::Modifier::CTRL && event->button() == Qt::LeftButton){// && m_gtInfo.vehicle_mode==true){
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
//        if(m_gtInfo.vehicle_mode==false){
//            // set rider points
//            emit m_labelmanager->setRiderPoint(event->pos().x(), event->pos().y());
//        }
//        else{
//            emit m_labelmanager->unselectOthers(-1);
//        }
//        emit m_labelmanager->unselectOthers(-1);
        emit m_labelmanager->clickBackground();
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

void OrientationGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(m_gtInfo.is_background==true && event->modifiers() != Qt::Modifier::CTRL){// && event->button()==Qt::LeftButton){
        m_gtInfo.is_not_dragging = false;

//        qDebug() << event->pos().x() << event->pos().y();
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

void OrientationGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
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

//            float move_start_x = event->pos().x();
//            float move_start_y = event->pos().y();

//            emit m_labelmanager->setStartXY(move_start_x, move_start_y);

////            m_gtInfo.drag_chosen_area.x1 = event->pos().x();
////            m_gtInfo.drag_chosen_area.y1 = event->pos().y();
        }
    }
//    if (event->modifiers() == Qt::Modifier::CTRL && event->button() == Qt::LeftButton){// && m_gtInfo.mode_edit == true) {
    else if (event->modifiers() == Qt::Modifier::CTRL && event->button() == Qt::LeftButton){// && m_gtInfo.mode_edit == true) {
        if (m_gtInfo.is_chosen == false && m_gtInfo.draw_enabled==true && m_gtInfo.vehicle_mode==true){
            m_gtInfo.is_chosen = true;
            m_labelmanager->setTopLayer(this);
            m_gtInfo.multi_chosen = true;
            emit m_labelmanager->setMultiChosen();
        }
        else if (m_gtInfo.is_chosen == true && m_gtInfo.draw_enabled==true && m_gtInfo.vehicle_mode==true){
            m_gtInfo.is_chosen = false;
            if(m_gtInfo.is_first==false){
                m_gtInfo.is_first = true;
            }
            m_labelmanager->setBottomLayer(this);
            m_gtInfo.multi_chosen = false;
        }
    }
    else if (event->button() == Qt::LeftButton){// && m_gtInfo.mode_edit == true) {
        if (m_gtInfo.is_chosen == false && m_gtInfo.draw_enabled==true && m_gtInfo.vehicle_mode==true){
            m_gtInfo.is_chosen = true;
            m_labelmanager->setTopLayer(this);

            emit m_labelmanager->unselectOthers(m_gtInfo.curr_idx);
//            m_labelmanager->setTopLayer(this);
        }
        else if(m_gtInfo.is_chosen==true && m_gtInfo.draw_enabled==true && m_gtInfo.multi_chosen==true && m_gtInfo.vehicle_mode==true){// Need to add more condition
            m_gtInfo.is_chosen = true;
            m_labelmanager->setTopLayer(this);

            emit m_labelmanager->unselectOthers(m_gtInfo.curr_idx);
        }
        else if(m_gtInfo.is_chosen==true && m_gtInfo.draw_enabled==true && m_gtInfo.vehicle_mode==true){
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
//        myMenu.addAction("Set object class");
        myMenu.setGeometry(QCursor::pos().x(), QCursor::pos().y(), myMenu.sizeHint().width(), myMenu.sizeHint().height());
        auto selectedAction = myMenu.exec();
        if (selectedAction != nullptr) {
            if (selectedAction->text() == "Set Top Layer") {
                m_labelmanager->setTopLayer(this);
            }
            else if (selectedAction->text() == "Set Bottom Layer") {
                m_labelmanager->setBottomLayer(this);
            }
//            else if (selectedAction->text() == "Set object class") {
//                emit m_labelmanager->changeObjCls(m_gtInfo);
//                m_gtInfo.is_chosen = true;
//                m_labelmanager->setTopLayer(this);
////                m_labelmanager->setBottomLayer(this);
//            }
        }
    }
}

void OrientationGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
//    if(m_gtInfo.is_background==true && m_gtInfo.vehicle_mode==false){
//        this->setCursor(Qt::PointingHandCursor);
//    }
//    else{
//        this->setCursor(Qt::ArrowCursor);
//    }
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
