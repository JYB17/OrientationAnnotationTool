#include "FrontView.h"

FrontView::FrontView(QGraphicsScene* scene)
{
    this->sceneItem = scene->addPixmap(qpixmap);
}

void FrontView::UpdateFrontImage(QImage* img)
{
    qpixmap.convertFromImage(*img);
    this->sceneItem->setPixmap(qpixmap);
}
