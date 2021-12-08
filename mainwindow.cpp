#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)//, draw_info{1.0F}
{
    ui->setupUi(this);


    m_scene = new QGraphicsScene();
    m_dataloader = new DataLoader(ui->canvas, ui->LoadTypeText, ui->lbl_rear_txt, ui->lbl_front_txt);
    m_frontview = new FrontView(m_scene);
    m_timer = new QTimer(this);
    m_labelmanager = new LabelManager(m_scene);
    m_timer->setInterval(20);
//    m_timer->setInterval(30);

//    m_dataloader->SetDrawInfo(&draw_info);

    ui->canvas->setScene(m_scene);

    connect(m_dataloader, SIGNAL(UpdateFrontImage(QImage*)), m_frontview, SLOT(UpdateFrontImage(QImage*)));
    connect(m_dataloader, SIGNAL(SetNewVideo(int32_t, int32_t)), this, SLOT(SetNewVideo(int32_t, int32_t)));
    connect(m_dataloader, SIGNAL(updateGtInfos(QVector<GtInfo>&)), m_labelmanager, SLOT(drawAll(QVector<GtInfo>&)));

    connect(m_labelmanager, SIGNAL(unselectOthers(int32_t)), m_dataloader, SLOT(unselectOthers(int32_t)));
    connect(m_labelmanager, SIGNAL(setMultiChosen()), m_dataloader, SLOT(setMultiChosen()));
    connect(m_labelmanager, SIGNAL(setAddWheelMode(int32_t)), this, SLOT(setAddWheelMode(int32_t)));
    connect(m_labelmanager, SIGNAL(selectDraggedArea(Bbox &)), m_dataloader, SLOT(selectDraggedArea(Bbox &)));
    connect(m_labelmanager, SIGNAL(setRiderPoint(float_t, float_t)), m_dataloader, SLOT(setRiderPoint(float_t, float_t)));
//    connect(m_labelmanager, SIGNAL(dragZoomFocusedArea(Bbox &)), m_dataloader, SLOT(dragZoomFocusedArea(Bbox &)));
//    connect(m_labelmanager, SIGNAL(dragZoomFocusedArea(float_t, float_t)), m_dataloader, SLOT(dragZoomFocusedArea(float_t, float_t)));
//    connect(m_labelmanager, SIGNAL(setStartXY(float_t, float_t)), m_dataloader, SLOT(setStartXY(float_t, float_t)));

    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));

    QPalette rear_palette = ui->lbl_rear_txt->palette();
    rear_palette.setColor(ui->lbl_rear_txt->backgroundRole(), Qt::white);
    rear_palette.setColor(ui->lbl_rear_txt->foregroundRole(), Qt::black);
    ui->lbl_rear_txt->setAutoFillBackground(true);
    ui->lbl_rear_txt->setPalette(rear_palette);
    ui->lbl_rear_txt->setFixedWidth(100);

    QPalette front_palette = ui->lbl_front_txt->palette();
    front_palette.setColor(ui->lbl_front_txt->backgroundRole(), Qt::white);
    front_palette.setColor(ui->lbl_front_txt->foregroundRole(), Qt::black);
    ui->lbl_front_txt->setAutoFillBackground(true);
    ui->lbl_front_txt->setPalette(front_palette);
    ui->lbl_front_txt->setFixedWidth(100);

//    ui->edit_gt_path->setText("C:\\Users\\JYB\\Desktop\\ODP\\orientation_annotation\\inputs_temp\\FN-ZF-8884_2020-11-11-14-02-32");
//    ui->edit_img_path->setText("V:\\C_19_ZF_L4_ZFL4TRM\\Data\\Customer_Mass\\Mass_data\\FN-ZF-8884_2020-11-11-14-02-32\\CAM-WA-FM-FORWARD\\images");

}

MainWindow::~MainWindow()
{
    delete ui;

    delete m_scene;
    delete m_dataloader;
    delete m_frontview;
    delete m_timer;
}

void MainWindow::setAddWheelMode(int32_t curr_idx){
    ui->btn_add_rider->setEnabled(true);
    ui->btn_enter_rear->setEnabled(false);
    ui->lbl_rear_txt->setText("");
    ui->btn_enter_front->setEnabled(false);
    ui->lbl_front_txt->setText("");

    emit m_labelmanager->unselectOthers(curr_idx);
}

void MainWindow::changeFrame(int32_t frame_no){
    if(saved_flag==false){
        m_dataloader->saveCurrGT();
        saved_flag = true;
    }
    else{
        saved_flag = false;
    }
    ui->check_show_bbox->setChecked(true);
    ui->check_selected_bbox->setChecked(false);
    ui->check_show_bbox->setEnabled(true);
    ui->check_selected_bbox->setEnabled(true);
    ui->btn_enter_rear->setEnabled(false);
    ui->lbl_rear_txt->setText("");
    ui->btn_enter_front->setEnabled(false);
    ui->lbl_front_txt->setText("");

    m_dataloader->setFrameNo(std::max(0, std::min(frame_no, num_frames-1)));
    m_dataloader->showFrame();
    ui->video_navigator->setValue(m_dataloader->getFrameNo());

    selected_checked = false;

    if(ui->radio_normal->isChecked()){
        m_dataloader->setVehicleMode();
    }
    else if(ui->radio_zoom->isChecked()){
        m_dataloader->zoomMode();
    }
    else{
        m_dataloader->setRiderMode();
    }

    m_dataloader->setRearEnabled(false);
    m_dataloader->setFrontEnabled(false);
}

void MainWindow::timeout(){
    changeFrame(m_dataloader->getFrameNo()+1);
    if(m_dataloader->getFrameNo()==(num_frames-1)){
        pause_video();
        m_dataloader->saveCurrGT();
    }
}

void MainWindow::on_actionLoad_All_triggered()
{
    QString all_path = QFileDialog::getExistingDirectory(nullptr, "Select data path", "..\\");
//    QString all_path = "";
    m_dataloader->setAllPath(all_path);
}

void MainWindow::on_actionLoad_GTs_triggered()
{
#ifdef _DEBUG
//    QString gt_path = "C:\\Users\\JYB\\Desktop\\ODP\\orientation_annotation\\lidar_3d_GTs\\GTs_1007\\FN-ZF-8884_2020-11-11-14-02-32\\FM\\GTs";
//    QString gt_path = "C:\\Users\\JYB\\Desktop\\ODP\\orientation_annotation\\lidar_3d_GTs\\temp_small\\GTs";
    QString gt_path = "C:\\Users\\JYB\\Desktop\\ODP\\orientation_annotation\\lidar_3d_GTs\\temp_rider\\GTs";
#else
    QString gt_path = QFileDialog::getExistingDirectory(nullptr, "Select GT path", "..\\");
#endif
    m_dataloader->setGTPath(gt_path);
    ui->actionLoad_All->setEnabled(false);
    ui->actionLoad_Frame_Images->setEnabled(true);
}

void MainWindow::on_actionLoad_Frame_Images_triggered()
{
#ifdef _DEBUG
    QString img_path = "V:\\C_19_ZF_L4_ZFL4TRM\\Data\\Customer_Mass\\Mass_data\\FN-ZF-8884_2020-11-11-14-02-32\\CAM-WA-FM-FORWARD\\images";
#else
    QString img_path = QFileDialog::getExistingDirectory(nullptr, "Select image path", "..\\");
#endif
    m_dataloader->setImgPath(img_path);
    ui->actionLoad_All->setEnabled(true);
}

void MainWindow::on_btn_load_gts_imgs_clicked()
{
    if(ui->edit_gt_path->text()!="" && ui->edit_img_path->text()!=""){
        m_dataloader->setGTPath(ui->edit_gt_path->text());
        m_dataloader->setImgPath(ui->edit_img_path->text());
    }
}

void MainWindow::SetNewVideo(int32_t img_width, int32_t img_height)
{
    ui->btn_backward->setEnabled(true);
    ui->btn_forward->setEnabled(true);
    ui->video_navigator->setEnabled(true);
    ui->btn_play_pause->setEnabled(true);
    ui->btn_save->setEnabled(true);
    ui->btn_reverse_angle->setEnabled(true);
    ui->btn_delete->setEnabled(true);
    ui->btn_choose_all->setEnabled(true);
    ui->btn_unchoose_all->setEnabled(true);
    ui->btn_undo->setEnabled(true);
    ui->btn_redo->setEnabled(true);

    ui->check_selected_bbox->setEnabled(true);
    ui->check_show_bbox->setEnabled(true);
    ui->radio_normal->setEnabled(true);
    ui->radio_rider->setEnabled(true);
    ui->radio_zoom->setEnabled(true);
//    ui->btn_add_rider->setEnabled(true);
//    ui->btn_zoom_fit->setEnabled(true);

    if(prev_frame_width!=img_width || prev_frame_height!= img_height) {
        qreal min_wh = std::min((ui->canvas->width()) / qreal(img_width + 4), (ui->canvas->height()) / qreal(img_height + 4));
        ui->canvas->scale(min_wh, min_wh);
    }

    if ((ui->canvas->width()) / qreal(img_width + 4) < (ui->canvas->height()) / qreal(img_height + 4)) {
        prev_canvas_width = ui->canvas->width() - 4;
        prev_canvas_height = int32_t(img_height * (prev_canvas_width / (float_t)img_width));
    }
    else {
        prev_canvas_height = ui->canvas->height() - 4;
        prev_canvas_width = int32_t(img_width * (prev_canvas_height / (float_t)img_height));
    }

    prev_frame_width = img_width;
    prev_frame_height = img_height;

    num_frames = m_dataloader->getNumFrames();
    ui->video_navigator->setMinimum(0);
    ui->video_navigator->setMaximum(num_frames-1);
    ui->video_navigator->setValue(0);
}

void MainWindow::on_btn_forward_clicked()
{
    changeFrame(m_dataloader->getFrameNo()+1);
}

void MainWindow::on_btn_backward_clicked()
{
    changeFrame(m_dataloader->getFrameNo()-1);
}

void MainWindow::on_video_navigator_valueChanged(int value)
{
    changeFrame(value);
}

void MainWindow::play_video(){
    m_timer->start();
    ui->btn_backward->setEnabled(false);
    ui->btn_forward->setEnabled(false);
    ui->btn_save->setEnabled(false);
    ui->btn_reverse_angle->setEnabled(false);
    ui->btn_delete->setEnabled(false);
    ui->btn_choose_all->setEnabled(false);
    ui->btn_unchoose_all->setEnabled(false);
    ui->btn_undo->setEnabled(false);
    ui->btn_redo->setEnabled(false);
    ui->btn_add_rider->setEnabled(false);
//    ui->btn_zoom_fit->setEnabled(false);
}

void MainWindow::pause_video(){
    m_timer->stop();
    ui->btn_backward->setEnabled(true);
    ui->btn_forward->setEnabled(true);
    ui->btn_save->setEnabled(true);
    ui->btn_reverse_angle->setEnabled(true);
    ui->btn_delete->setEnabled(true);
    ui->btn_choose_all->setEnabled(true);
    ui->btn_unchoose_all->setEnabled(true);
    ui->btn_undo->setEnabled(true);
    ui->btn_redo->setEnabled(true);
    if(ui->radio_rider->isChecked()==true){
        ui->btn_add_rider->setEnabled(true);
    }
    else if(ui->radio_normal->isChecked()==true){
        ui->btn_add_rider->setEnabled(false);
    }
//    ui->btn_zoom_fit->setEnabled(true);
}

void MainWindow::on_btn_play_pause_clicked()
{
    if(m_timer->isActive()){
        pause_video();
    }
    else{
        play_video();
    }
}

void MainWindow::on_btn_save_clicked()
{
    m_dataloader->saveCurrGT();
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() == Qt::KeyboardModifier::ControlModifier) {
        ui->canvas->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        if (event->angleDelta().y() > 0) {
            ui->canvas->scale(1.1, 1.1);
            prev_canvas_width = int(prev_canvas_width * 1.1);
            prev_canvas_height = int(prev_canvas_height * 1.1);
        }
        else {
            ui->canvas->scale(1.F/1.1F, 1.F/1.1F);
            prev_canvas_width = int(prev_canvas_width * 1.F/1.1F);
            prev_canvas_height = int(prev_canvas_height * 1.F/1.1F);
        }
        event->accept();

//        qDebug() << prev_canvas_width << prev_canvas_height;
    }
//    ui->canvas->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
//    if (event->angleDelta().y() > 0) {
//        ui->canvas->scale(1.1, 1.1);
//        prev_canvas_width = int(prev_canvas_width * 1.1);
//        prev_canvas_height = int(prev_canvas_height * 1.1);
//    }
//    else {
//        ui->canvas->scale(0.9, 0.9);
//        prev_canvas_width = int(prev_canvas_width * 0.9);
//        prev_canvas_height = int(prev_canvas_height * 0.9);
//    }

//    event->accept();
}

void MainWindow::on_btn_zoom_fit_clicked()
{
//    qreal min_wh = std::min((ui->canvas->width()) / qreal(prev_canvas_width + 4), (ui->canvas->height()) / qreal(prev_canvas_height + 4));
//    ui->canvas->scale(min_wh, min_wh);

//    if ((ui->canvas->width()) / qreal(prev_canvas_width + 4) < (ui->canvas->height()) / qreal(prev_canvas_height + 4)) {
//        int temp_width = prev_canvas_width;
//        prev_canvas_width = ui->canvas->width() - 4;
//        prev_canvas_height = int(prev_canvas_height * (prev_canvas_width / (float)temp_width));
//    }
//    else {
//        int temp_height = prev_canvas_height;
//        prev_canvas_height = ui->canvas->height() - 4;
//        prev_canvas_width = int(prev_canvas_width * (prev_canvas_height / (float)temp_height));
//    }
}

void MainWindow::on_btn_reverse_angle_clicked()
{
    m_dataloader->editGTs(DataLoader::changeAngle);
}

void MainWindow::on_btn_delete_clicked()
{
    m_dataloader->deleteGTs();
}

void MainWindow::on_btn_choose_all_clicked()
{
    m_dataloader->editGTs(DataLoader::chooseAll);
}

void MainWindow::on_btn_unchoose_all_clicked()
{
    m_dataloader->editGTs(DataLoader::unchooseAll);

    if(ui->check_selected_bbox->isChecked()==true){
//        m_dataloader->editGTs(DataLoader::showSelectedUnchoose);
        m_dataloader->editGTs(DataLoader::enableDraw);
        ui->check_show_bbox->setEnabled(true);
        ui->check_selected_bbox->setChecked(false);
        selected_checked = !selected_checked;
    }

    if(ui->btn_enter_rear->isEnabled()){
        ui->lbl_rear_txt->setText("");
    }
    if(ui->btn_enter_front->isEnabled()){
        ui->lbl_front_txt->setText("");
    }
}

void MainWindow::on_btn_undo_clicked()
{
    m_dataloader->unDo();
}

void MainWindow::on_btn_redo_clicked()
{
    m_dataloader->reDo();
}

void MainWindow::on_check_show_bbox_clicked()
{
    m_dataloader->editGTs(DataLoader::showBbox);

    if(ui->check_show_bbox->isChecked()==false){
        ui->check_selected_bbox->setChecked(false);
        ui->check_selected_bbox->setEnabled(false);
    }
    else{
        ui->check_selected_bbox->setEnabled(true);
    }
}

void MainWindow::on_check_selected_bbox_clicked()
{
    m_dataloader->editGTs(DataLoader::showSelectedBbox);

//    if(ui->check_selected_bbox->isChecked()==true){
    if(selected_checked==true){
//        ui->check_show_bbox->setChecked(true);
//        ui->check_show_bbox->setEnabled(false);
        m_dataloader->editGTs(DataLoader::enableDraw);
        ui->check_show_bbox->setEnabled(true);
//        if(selected_checked==true){
//            m_dataloader->editGTs(DataLoader::enableDraw);
//        }
    }
    else{
//        ui->check_show_bbox->setEnabled(true);
        ui->check_show_bbox->setChecked(true);
        ui->check_show_bbox->setEnabled(false);
    }
    selected_checked = !selected_checked;
}

void MainWindow::on_radio_rider_clicked()
{
    m_dataloader->setRiderMode();
    m_dataloader->editGTs(DataLoader::unchooseAll);
//    ui->btn_add_rider->setEnabled(true);
}

void MainWindow::on_radio_normal_clicked()
{
    m_dataloader->setVehicleMode();
    ui->btn_add_rider->setEnabled(false);
    ui->btn_enter_rear->setEnabled(false);
    ui->btn_enter_front->setEnabled(false);

    m_dataloader->setRearEnabled(false);
    m_dataloader->setFrontEnabled(false);

    ui->lbl_rear_txt->setText("");
    ui->lbl_front_txt->setText("");
}

void MainWindow::on_radio_zoom_clicked()
{
    m_dataloader->zoomMode();

    ui->btn_add_rider->setEnabled(false);
    ui->btn_enter_rear->setEnabled(false);
    ui->btn_enter_front->setEnabled(false);

    m_dataloader->setRearEnabled(false);
    m_dataloader->setFrontEnabled(false);

    ui->lbl_rear_txt->setText("");
    ui->lbl_front_txt->setText("");
}

void MainWindow::on_btn_add_rider_clicked()
{
    m_dataloader->editGTs(DataLoader::enableAddWheel);
    ui->btn_enter_rear->setEnabled(true);
    ui->btn_enter_front->setEnabled(false);

    m_dataloader->setRearEnabled(true);
    ui->lbl_rear_txt->setText("");
    ui->lbl_front_txt->setText("");

    new_rider.is_2_wheeler = true;
    new_rider.vehicle_mode = false;
}

void MainWindow::on_btn_enter_rear_clicked()
{
    ui->btn_enter_rear->setEnabled(false);
    ui->btn_enter_front->setEnabled(true);

    m_dataloader->setRearEnabled(false);
    m_dataloader->setFrontEnabled(true);

    if(ui->lbl_rear_txt->text().size()!=0){// || ui->lbl_rear_txt->text()=="N/A"){
        QStringList x_y_coords = ui->lbl_rear_txt->text().split(", ");
        m_dataloader->setRearWheelPoint(x_y_coords[0].toFloat(), x_y_coords[1].toFloat());
//        new_rider.rear_x = x_y_coords[0].toFloat();
//        new_rider.rear_y = x_y_coords[1].toFloat();
    }
    else{
        ui->lbl_rear_txt->setText("N/A");
        m_dataloader->setRearWheelPoint(-1.F, -1.F);
//        new_rider.rear_x = -1.F;
//        new_rider.rear_y = -1.F;
    }
}

void MainWindow::on_btn_enter_front_clicked()
{
    ui->btn_enter_front->setEnabled(false);
    ui->btn_add_rider->setEnabled(true);

    m_dataloader->setFrontEnabled(false);

    if(ui->lbl_front_txt->text().size()!=0){// || ui->lbl_front_txt->text()=="N/A"){
        QStringList x_y_coords = ui->lbl_front_txt->text().split(", ");
        m_dataloader->setFrontWheelPoint(x_y_coords[0].toFloat(), x_y_coords[1].toFloat());
//        new_rider.front_x = x_y_coords[0].toFloat();
//        new_rider.front_y = x_y_coords[1].toFloat();
    }
    else{
        ui->lbl_front_txt->setText("N/A");
        m_dataloader->setFrontWheelPoint(-1.F, -1.F);
//        new_rider.front_x = -1.F;
//        new_rider.front_y = -1.F;
    }

//    m_dataloader->addNewRider(new_rider);

    ui->lbl_rear_txt->setText("");
    ui->lbl_front_txt->setText("");

//    m_dataloader->showCurrRiderGTs();
}

void MainWindow::on_btn_delete_wheel_points_clicked()
{
    m_dataloader->deleteWheelPoint();
}
