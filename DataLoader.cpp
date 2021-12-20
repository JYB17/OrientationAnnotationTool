#include "DataLoader.h"

DataLoader::DataLoader(QGraphicsView* mainWindow, QLabel* LoadTypeText, QLabel *rear_coord_txt, QLabel *front_coord_txt, QObject* parent) : QObject(parent)//,
//    ptr_draw_info{nullptr}
{
    num_frames = 0;
    curr_frame_no = 0;
    this->viewText = LoadTypeText;
    this->viewWindow = mainWindow;

    this->rear_coord_txt = rear_coord_txt;
    this->front_coord_txt = front_coord_txt;

    curr_img = nullptr;
    initialized = false;

    unselect_target = false;

    is_vehicle_mode = true;
    add_rider = false;

    is_zoom_mode = false;

    rear_enabled = false;
    front_enabled = false;

    this->viewWindow->installEventFilter(this);
}

DataLoader::~DataLoader()
{
    delete viewWindow;
    delete viewText;
    delete curr_img;
    delete rear_coord_txt;
    delete front_coord_txt;
}

//void DataLoader::SetDrawInfo(DrawInfo const* _draw_info){
//    this->ptr_draw_info = _draw_info;
//}

void DataLoader::setAllPath(QString const all_path)
{
    setGTPath(all_path);
    setImgPath(all_path);
}

void DataLoader::setGTPath(QString const gt_path)
{
    this->gt_path = gt_path;
    files_list.clear();

    QDir dir(gt_path);
    QStringList temp_list = dir.entryList(QDir::Filter::Files);
    int32_t curr_idx = 0;
    for (int i = 0; i < temp_list.size(); i++) {
        if (!temp_list.at(i).endsWith(".json")) {
            continue;
        }
        files_list.append(temp_list.at(i).split(".jso")[0]);
        viewText->setText("Loading... " + QString::number(++curr_idx));
    }
    std::sort(files_list.begin(), files_list.end());
    num_frames = files_list.size();

    viewText->setText("Successfully loaded " + QString::number(num_frames) + " GT files!");
    initialized = false;
}

void DataLoader::setImgPath(QString const img_path)
{
    this->img_path = img_path;
    curr_frame_no = 0;

    showFrame();
//    viewText->setText("Successfully loaded " + QString::number(num_frames) + " GT files and frame images!");
}

void DataLoader::setSavePath(QString const save_path){
    this->save_path = save_path;
}

void DataLoader::showFrame(){
    curr_img_name = img_path + "\\" + files_list[curr_frame_no] + ".png";
//    curr_gt_name = gt_path + "\\" + files_list[curr_frame_no] + ".json";
    prev_done_gts.clear();
    undo_flag = false;
    undo_redo_idx = -1;

    curr_frame = cv::imread(curr_img_name.toStdString());
    if(initialized==false){
        frame_width = curr_frame.cols;
        frame_height = curr_frame.rows;
        emit SetNewVideo(frame_width, frame_height);

        initialized = true;
    }

    createQImage();
    emit UpdateFrontImage(curr_img);

    readJson();
    emit updateGtInfos(curr_gt_infos, false);

    QStringList splitted = curr_gt_name.split("\\");
    viewText->setText(splitted[splitted.size()-1] + " (" + QString::number(curr_frame_no+1) + "/" + QString::number(num_frames) + ")");
}

void DataLoader::savePrevInfos()
{
    while(prev_done_gts.size()!=(undo_redo_idx+1)){
        prev_done_gts.removeLast();
    }

    QVector<GtInfo> prev_gts;
    prev_gts.clear();
    for(int32_t i=0; i<curr_gt_infos.size(); i++){
        GtInfo prev_gt;
        prev_gt.dir_angle = curr_gt_infos[i].dir_angle;
        prev_gt.X = curr_gt_infos[i].X;
        prev_gt.Y = curr_gt_infos[i].Y;
        for(int32_t j=0; j<8; j++){
            prev_gt.x[j] = curr_gt_infos[i].x[j];
            prev_gt.y[j] = curr_gt_infos[i].y[j];
        }
//        prev_gt.obj_cls = curr_gt_infos[curr_gt_idx][i].obj_cls;
        prev_gt.bbox.x1 = curr_gt_infos[i].bbox.x1;
        prev_gt.bbox.y1 = curr_gt_infos[i].bbox.y1;
        prev_gt.bbox.x2 = curr_gt_infos[i].bbox.x2;
        prev_gt.bbox.y2 = curr_gt_infos[i].bbox.y2;
        prev_gt.draw_enabled = curr_gt_infos[i].draw_enabled;
        prev_gt.move_resize = curr_gt_infos[i].move_resize;
        prev_gt.is_chosen = curr_gt_infos[i].is_chosen;
        prev_gt.is_first = curr_gt_infos[i].is_first;
//        prev_gt.show_cls = curr_gt_infos[curr_gt_idx][i].show_cls;
        prev_gt.is_background = curr_gt_infos[i].is_background;
        prev_gt.is_2_wheeler = curr_gt_infos[i].is_2_wheeler;
        prev_gt.rear_x = curr_gt_infos[i].rear_x;
        prev_gt.rear_y = curr_gt_infos[i].rear_y;
        prev_gt.front_x = curr_gt_infos[i].front_x;
        prev_gt.front_y = curr_gt_infos[i].front_y;
        prev_gt.is_svnet_rider = curr_gt_infos[i].is_svnet_rider;
        prev_gts.append(prev_gt);
    }
    prev_done_gts.append(prev_gts);
    undo_redo_idx++;

    undo_flag = false;
}

void DataLoader::unDo()
{
    if(undo_flag==false)
        savePrevInfos();

    if(undo_redo_idx>0){
        curr_gt_infos.clear();
        for(int32_t i=0; i<prev_done_gts[undo_redo_idx-1].size(); i++){
            curr_gt_infos.append(prev_done_gts[undo_redo_idx-1][i]);
        }
        undo_redo_idx = std::max(0, undo_redo_idx-1);
    }
    undo_flag = true;

    emit updateGtInfos(curr_gt_infos, true);
}
void DataLoader::reDo()
{
    if(undo_redo_idx<(prev_done_gts.size()-1)){
        curr_gt_infos.clear();
        for(int32_t i=0; i<prev_done_gts[undo_redo_idx+1].size(); i++){
            curr_gt_infos.append(prev_done_gts[undo_redo_idx+1][i]);
        }
        undo_redo_idx = std::min(undo_redo_idx+1, prev_done_gts.size()-1);
    }

    emit updateGtInfos(curr_gt_infos, true);
}

void DataLoader::copyOrigGT(){
    orig_gt_infos.clear();

    int32_t num_gts = curr_gt_infos.size();
    for(int32_t i=0; i<num_gts; i++){
        GtInfo orig_gt;
        orig_gt.bbox.x1 = curr_gt_infos[i].bbox.x1;
        orig_gt.bbox.y1 = curr_gt_infos[i].bbox.y1;
        orig_gt.bbox.x2 = curr_gt_infos[i].bbox.x2;
        orig_gt.bbox.y2 = curr_gt_infos[i].bbox.y2;
        for(int32_t j=0; j<8; j++){
            orig_gt.x[j] = curr_gt_infos[i].x[j];
            orig_gt.y[j] = curr_gt_infos[i].y[j];
        }
        orig_gt.rear_x = curr_gt_infos[i].rear_x;
        orig_gt.rear_y = curr_gt_infos[i].rear_y;
        orig_gt.front_x = curr_gt_infos[i].front_x;
        orig_gt.front_y = curr_gt_infos[i].front_y;

        orig_gt.dir_angle = curr_gt_infos[i].dir_angle;
        orig_gt.score = curr_gt_infos[i].score;

        orig_gt.draw_enabled = curr_gt_infos[i].draw_enabled;
        orig_gt.is_first = curr_gt_infos[i].is_first;
        orig_gt.is_chosen = curr_gt_infos[i].is_chosen;
        orig_gt.move_resize = curr_gt_infos[i].move_resize;
        orig_gt.vehicle_mode = curr_gt_infos[i].vehicle_mode;
        orig_gt.zoom_mode = curr_gt_infos[i].zoom_mode;
        orig_gt.is_2_wheeler = curr_gt_infos[i].is_2_wheeler;
        orig_gt.is_svnet_rider = curr_gt_infos[i].is_svnet_rider;
        orig_gt.curr_idx = curr_gt_infos[i].curr_idx;
        orig_gt.multi_chosen = curr_gt_infos[i].multi_chosen;
        orig_gt.is_background = curr_gt_infos[i].is_background;
        orig_gt.is_not_dragging = curr_gt_infos[i].is_not_dragging;
        orig_gt.is_adding_wheel = curr_gt_infos[i].is_adding_wheel;
        orig_gt.is_valid = curr_gt_infos[i].is_valid;

        orig_gt_infos.append(orig_gt);
    }
}

void DataLoader::reInitGtInfos(bool vehicle_mode, bool zoom_mode){
    savePrevInfos();

    curr_gt_infos.clear();

    int32_t num_gts = orig_gt_infos.size();
    for(int32_t i=0; i<num_gts; i++){
        GtInfo orig_gt;
        orig_gt.bbox.x1 = orig_gt_infos[i].bbox.x1;
        orig_gt.bbox.y1 = orig_gt_infos[i].bbox.y1;
        orig_gt.bbox.x2 = orig_gt_infos[i].bbox.x2;
        orig_gt.bbox.y2 = orig_gt_infos[i].bbox.y2;
        for(int32_t j=0; j<8; j++){
            orig_gt.x[j] = orig_gt_infos[i].x[j];
            orig_gt.y[j] = orig_gt_infos[i].y[j];
        }
        orig_gt.rear_x = orig_gt_infos[i].rear_x;
        orig_gt.rear_y = orig_gt_infos[i].rear_y;
        orig_gt.front_x = orig_gt_infos[i].front_x;
        orig_gt.front_y = orig_gt_infos[i].front_y;

        orig_gt.dir_angle = orig_gt_infos[i].dir_angle;
        orig_gt.score = orig_gt_infos[i].score;

        orig_gt.vehicle_mode = vehicle_mode;
        orig_gt.zoom_mode = zoom_mode;

        orig_gt.draw_enabled = orig_gt_infos[i].draw_enabled;
        orig_gt.is_first = orig_gt_infos[i].is_first;
        orig_gt.is_chosen = orig_gt_infos[i].is_chosen;
        orig_gt.move_resize = orig_gt_infos[i].move_resize;
//        orig_gt.vehicle_mode = orig_gt_infos[i].vehicle_mode;
//        orig_gt.zoom_mode = orig_gt_infos[i].zoom_mode;
        orig_gt.is_2_wheeler = orig_gt_infos[i].is_2_wheeler;
        orig_gt.is_svnet_rider = orig_gt_infos[i].is_svnet_rider;
        orig_gt.curr_idx = orig_gt_infos[i].curr_idx;
        orig_gt.multi_chosen = orig_gt_infos[i].multi_chosen;
        orig_gt.is_background = orig_gt_infos[i].is_background;
        orig_gt.is_not_dragging = orig_gt_infos[i].is_not_dragging;
        orig_gt.is_adding_wheel = orig_gt_infos[i].is_adding_wheel;
        orig_gt.is_valid = orig_gt_infos[i].is_valid;

        curr_gt_infos.append(orig_gt);
    }

    emit updateGtInfos(curr_gt_infos, false);
}

void DataLoader::readJson(){
    curr_gt_infos.clear();

    curr_gt_name = gt_path + "\\" + files_list[curr_frame_no] + ".json";
    if(is_new_gt_mode==true){
        curr_gt_name = save_path + "\\" + files_list[curr_frame_no] + ".json";
    }

    QFile log_file(curr_gt_name);
    if (!log_file.open(QIODevice::ReadOnly)) {
        if(is_new_gt_mode==false){
            QString outputText = "Cannot find " + curr_gt_name + "!";
            viewText->setText(outputText);
            return;
        }
        curr_gt_name = gt_path + "\\" + files_list[curr_frame_no] + ".json";

        log_file.setFileName(curr_gt_name);
        if (!log_file.open(QIODevice::ReadOnly)) {
            if(is_new_gt_mode==false){
                QString outputText = "Cannot find " + curr_gt_name + "!";
                viewText->setText(outputText);
                return;
            }
        }
    }

    QByteArray log_data = log_file.readAll();
    QJsonDocument log_doc(QJsonDocument::fromJson(log_data));
    QJsonArray curr_objs = log_doc.object()["objects"].toArray();
//    int32_t num_objs = log_doc.object()["num_objs"].toInt();
    int32_t num_objs = curr_objs.size();
    for(int32_t i=0; i<num_objs; i++){
        QJsonObject curr_obj = curr_objs[i].toObject();
        GtInfo curr_gt;
        curr_gt.bbox.x1 = curr_obj["bbox"].toArray()[0].toDouble();
        curr_gt.bbox.y1 = curr_obj["bbox"].toArray()[1].toDouble();
        curr_gt.bbox.x2 = curr_obj["bbox"].toArray()[2].toDouble();
        curr_gt.bbox.y2 = curr_obj["bbox"].toArray()[3].toDouble();

        if(curr_obj["X"].isArray())
            curr_gt.X = curr_obj["X"].toArray()[0].toDouble();
        else
            curr_gt.X = curr_obj["X"].toDouble();

        if(curr_obj["Y"].isArray())
            curr_gt.Y = curr_obj["Y"].toArray()[0].toDouble();
        else
            curr_gt.Y = curr_obj["Y"].toDouble();

        int32_t bbox_3d_size = curr_obj["bbox_3d"].toObject()["x"].toArray().size();
        if(bbox_3d_size==2){
            curr_gt.is_2_wheeler = true;
            curr_gt.is_svnet_rider = true;
            curr_gt.rear_x = curr_obj["bbox_3d"].toObject()["x"].toArray()[0].toDouble();
            curr_gt.rear_y = curr_obj["bbox_3d"].toObject()["y"].toArray()[0].toDouble();
            curr_gt.front_x = curr_obj["bbox_3d"].toObject()["x"].toArray()[1].toDouble();
            curr_gt.front_y = curr_obj["bbox_3d"].toObject()["y"].toArray()[1].toDouble();
        }
        else{
            if(curr_obj["label"].toDouble()==3.F){
                curr_gt.is_2_wheeler = true;
            }
            for(int32_t j=0; j<bbox_3d_size; j++){
                curr_gt.x[j] = curr_obj["bbox_3d"].toObject()["x"].toArray()[j].toDouble();
                curr_gt.y[j] = curr_obj["bbox_3d"].toObject()["y"].toArray()[j].toDouble();
            }
        }
        curr_gt.score = curr_obj["score"].toDouble();
        curr_gt.dir_angle = curr_obj["dir_angle"].toDouble();
        if(!(curr_gt.dir_angle<360.F)){
            curr_gt.dir_angle -= 360.F;
        }

        curr_gt_infos.push_back(curr_gt);
    }
    if(is_zoom_mode==false){
        GtInfo background;
        background.is_background = true;
        background.bbox.x1 = 0.F;
        background.bbox.y1 = 0.F;
        background.bbox.x2 = frame_width;
        background.bbox.y2 = frame_height;
        curr_gt_infos.push_back(background);
    }

    if(is_new_gt_mode==false){
        copyOrigGT();
        start_w_orig_gt = true;
    }
    else{
        start_w_orig_gt = false;
    }
}

void DataLoader::addNewRider(GtInfo &new_rider_gt){
    savePrevInfos();

    if((new_rider_gt.rear_x!=0 && new_rider_gt.rear_y!=0) || (new_rider_gt.front_x!=0 && new_rider_gt.front_y!=0)){
        curr_gt_infos.append(new_rider_gt);
    }

    emit updateGtInfos(curr_gt_infos, true);
}

void DataLoader::saveCurrGT(){
    std::ofstream out_file((save_path + "\\" + files_list[curr_frame_no] + ".json").toStdString());
    if (out_file.is_open()) {
        Json::Value out_json;
        out_json["frame_id"] = curr_frame_no + 1;
//        out_json["num_objs"] = curr_gt_infos.size();
        const int32_t obj_size = curr_gt_infos.size();
        bool has_background = false;
        for (int32_t i = 0; i < obj_size; i++) {
            Json::Value object;
            if(curr_gt_infos[i].is_background==true){
                has_background = true;
                continue;
            }
            object["score"] = curr_gt_infos[i].score;
            object["X"] = curr_gt_infos[i].X;
            object["Y"] = curr_gt_infos[i].Y;
            if(curr_gt_infos[i].is_2_wheeler==false){
                object["label"] = 1.F;
//                object["X"] = curr_gt_infos[i].X;
//                object["Y"] = curr_gt_infos[i].Y;
                object["dir_angle"] = curr_gt_infos[i].dir_angle;
                Json::Value obj_coords;
                obj_coords.append(curr_gt_infos[i].bbox.x1);
                obj_coords.append(curr_gt_infos[i].bbox.y1);
                obj_coords.append(curr_gt_infos[i].bbox.x2);
                obj_coords.append(curr_gt_infos[i].bbox.y2);
                object["bbox"] = obj_coords;
                Json::Value od_3d_coords_x;
                Json::Value od_3d_coords_y;
                for (int32_t j = 0; j < 8; j++) {
                    od_3d_coords_x.append(curr_gt_infos[i].x[j]);
                    od_3d_coords_y.append(curr_gt_infos[i].y[j]);
                }
                object["bbox_3d"]["x"] = od_3d_coords_x;
                object["bbox_3d"]["y"] = od_3d_coords_y;
            }
            else{
                object["label"] = 3.F;
                Json::Value obj_coords;
                obj_coords.append(curr_gt_infos[i].bbox.x1);
                obj_coords.append(curr_gt_infos[i].bbox.y1);
                obj_coords.append(curr_gt_infos[i].bbox.x2);
                obj_coords.append(curr_gt_infos[i].bbox.y2);
                object["bbox"] = obj_coords;

                Json::Value bbox_3d_x, bbox_3d_y;
                if(curr_gt_infos[i].dir_angle<0){
                    object["dir_angle"] = -1.F;
                    bbox_3d_x.append(curr_gt_infos[i].rear_x);
                    bbox_3d_x.append(curr_gt_infos[i].front_x);
                    bbox_3d_y.append(curr_gt_infos[i].rear_y);
                    bbox_3d_y.append(curr_gt_infos[i].front_y);
                }
                else{
                    object["dir_angle"] = curr_gt_infos[i].dir_angle;
                    for(int j=0; j<8; j++){
                        bbox_3d_x.append(curr_gt_infos[i].x[j]);
                        bbox_3d_y.append(curr_gt_infos[i].y[j]);
                    }
                }
//                if(curr_gt_infos[i].rear_x==0 && curr_gt_infos[i].front_x==0){
//                    bbox_3d_x.append(-1);
//                    bbox_3d_x.append(-1);
//                }
//                else{
//                    bbox_3d_x.append(curr_gt_infos[i].rear_x);
//                    bbox_3d_x.append(curr_gt_infos[i].front_x);
//                }
//                if(curr_gt_infos[i].rear_y==0 && curr_gt_infos[i].front_y==0){
//                    bbox_3d_y.append(-1);
//                    bbox_3d_y.append(-1);
//                }
//                else{
//                    bbox_3d_y.append(curr_gt_infos[i].rear_y);
//                    bbox_3d_y.append(curr_gt_infos[i].front_y);
//                }
                object["bbox_3d"]["x"] = bbox_3d_x;
                object["bbox_3d"]["y"] = bbox_3d_y;
            }
            out_json["objects"].append(object);
        }
        out_json["num_objs"] = has_background==false ? curr_gt_infos.size() : curr_gt_infos.size()-1;
        out_file << out_json.toStyledString();
        out_file.close();
    }
}

void DataLoader::createQImage()
{
//    cv::putText(curr_frame, std::to_string(curr_frame_no + 1) + "/" + std::to_string(num_frames), cv::Point(20, 40), 2, 1, cv::Scalar(0, 0, 0), 3);
    cv::putText(curr_frame, std::to_string(curr_frame_no + 1) + "/" + std::to_string(num_frames), cv::Point(20, 40), 2, 1, cv::Scalar(), 3);
    cv::putText(curr_frame, std::to_string(curr_frame_no + 1) + "/" + std::to_string(num_frames), cv::Point(20, 40), 2, 1, cv::Scalar(255, 255, 255), 1);
//    cv::putText(curr_frame, std::to_string(curr_frame_no + 1) + "/" + std::to_string(num_frames) + " (" + curr_gt_name.toStdString() + ")", cv::Point(20, 40), 2, 1, cv::Scalar(255, 255, 255), 2);
    if (curr_img == nullptr) {
        curr_img = new QImage(curr_frame.data, curr_frame.cols, curr_frame.rows, QImage::Format_BGR888);
    }
    else {// if (curr_img ->width() != curr_frame.cols || curr_img ->height() != curr_frame.rows) {
        delete curr_img;
        curr_img = new QImage(curr_frame.data, curr_frame.cols, curr_frame.rows, QImage::Format_BGR888);
    }
}

void DataLoader::unselectOthers(int32_t curr_idx){
    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=0; i<obj_size; i++){
        if(curr_idx!=i){
            curr_gt_infos[i].is_chosen = false;
            curr_gt_infos[i].is_adding_wheel = false;
        }
        curr_gt_infos[i].multi_chosen = false;
    }

//    emit updateGtInfos(curr_gt_infos);
}

void DataLoader::setMultiChosen(){
    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=0; i<obj_size; i++){
        if(curr_gt_infos[i].is_chosen==true && curr_gt_infos[i].multi_chosen==false){
            curr_gt_infos[i].multi_chosen = true;
            break;
        }
    }
}

void DataLoader::selectDraggedArea(Bbox &dragged_area){
//    unselectOthers(-1);
    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=0; i<obj_size; i++){
        if(curr_gt_infos[i].is_background==true){// || curr_gt_infos[i].is_2_wheeler==true){
            continue;
        }
//        if(dragged_area.x1<=curr_gt_infos[i].bbox.x1 && curr_gt_infos[i].bbox.x2<=dragged_area.x2 && dragged_area.y1<=curr_gt_infos[i].bbox.y1 && curr_gt_infos[i].bbox.y2<=dragged_area.y2){
//        if(curr_gt_infos[i].is_2_wheeler==false && ((dragged_area.x1<=curr_gt_infos[i].bbox.x1 && curr_gt_infos[i].bbox.x1<=dragged_area.x2 \
//            && dragged_area.y1<=curr_gt_infos[i].bbox.y1 && curr_gt_infos[i].bbox.y1<=dragged_area.y2) \
//                || (dragged_area.x1<=curr_gt_infos[i].bbox.x2 && curr_gt_infos[i].bbox.x2<=dragged_area.x2 \
//                    && dragged_area.y1<=curr_gt_infos[i].bbox.y2 && curr_gt_infos[i].bbox.y2<=dragged_area.y2))){
//            curr_gt_infos[i].is_chosen = true;
//            curr_gt_infos[i].multi_chosen = true;
//            curr_gt_infos[i].is_first = false;
//        }
//        else if(curr_gt_infos[i].is_2_wheeler==true \
//                && (dragged_area.x1<=curr_gt_infos[i].rear_x && curr_gt_infos[i].rear_x<=dragged_area.x2 \
//                    && dragged_area.y1<=curr_gt_infos[i].rear_y && curr_gt_infos[i].rear_y<=dragged_area.y2) \
//                && (dragged_area.x1<=curr_gt_infos[i].front_x && curr_gt_infos[i].front_x<=dragged_area.x2 \
//                    && dragged_area.y1<=curr_gt_infos[i].front_y && curr_gt_infos[i].front_y<=dragged_area.y2)){
//            curr_gt_infos[i].is_chosen = true;
//            curr_gt_infos[i].multi_chosen = true;
//            curr_gt_infos[i].is_first = false;
//        }
        if((dragged_area.x1<=curr_gt_infos[i].bbox.x1 && curr_gt_infos[i].bbox.x1<=dragged_area.x2 \
            && dragged_area.y1<=curr_gt_infos[i].bbox.y1 && curr_gt_infos[i].bbox.y1<=dragged_area.y2) \
                || (dragged_area.x1<=curr_gt_infos[i].bbox.x2 && curr_gt_infos[i].bbox.x2<=dragged_area.x2 \
                    && dragged_area.y1<=curr_gt_infos[i].bbox.y2 && curr_gt_infos[i].bbox.y2<=dragged_area.y2)){
            curr_gt_infos[i].is_chosen = true;
            curr_gt_infos[i].multi_chosen = true;
            curr_gt_infos[i].is_first = false;
        }
    }
}

//void DataLoader::dragZoomFocusedArea(float_t x, float_t y){
//    // TODO: implement this function
//    int32_t curr_hor = viewWindow->horizontalScrollBar()->value() - ((int)x - move_start_x);
//    int32_t curr_ver = viewWindow->verticalScrollBar()->value() - ((int)y - move_start_y);
////    viewWindow->horizontalScrollBar()->setValue(std::max(0, curr_hor));
////    viewWindow->verticalScrollBar()->setValue(std::max(0, curr_ver));
//    viewWindow->horizontalScrollBar()->setValue(0);
//    viewWindow->verticalScrollBar()->setValue(0);

//    move_start_x = x;
//    move_start_y = y;
////    viewText->setText("("+QString::number(dragged_area.x1)+","+QString::number(dragged_area.y1)+") -> ("+QString::number(dragged_area.x1)+","+QString::number(dragged_area.y1)+")");
////    qDebug() << "X" << dragged_area.x1 << "-->" << dragged_area.x2;
////    qDebug() << "Y" << dragged_area.y1 << "-->" << dragged_area.y2 << "\n";
//}

//void DataLoader::setStartXY(float_t x, float_t y){
//    move_start_x = x;
//    move_start_y = y;
//}

void DataLoader::setRiderPoint(float_t x, float_t y){
    GtInfo temp_rider_point;
    temp_rider_point.is_valid = false;
    temp_rider_point.is_2_wheeler = true;
    temp_rider_point.vehicle_mode = false;
    if(rear_enabled==true){
        rear_coord_txt->setText(QString::number((int32_t)x)+", "+QString::number((int32_t)y));
        temp_rider_point.rear_x = x;
        temp_rider_point.rear_y = y;
    }
    else if(front_enabled==true){
        front_coord_txt->setText(QString::number((int32_t)x)+", "+QString::number((int32_t)y));
        temp_rider_point.front_x = x;
        temp_rider_point.front_y = y;
    }

//    curr_gt_infos.push_back(temp_rider_point);
    emit updateGtInfos(curr_gt_infos, true);
}

void DataLoader::deleteGTs(){
    savePrevInfos();

    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=obj_size-1; i>-1; i--){
        if(curr_gt_infos[i].is_chosen==true){
            curr_gt_infos.removeAt(i);
        }
//        curr_gt_infos[i].multi_chosen = false;
    }

    emit updateGtInfos(curr_gt_infos, true);
}

void DataLoader::editGTs(editMode mode){
    if(mode==changeAngle){
        savePrevInfos();
    }
    bool change_gt_mode = false;

    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=0; i<obj_size; i++){
        if(mode==chooseAll){
            curr_gt_infos[i].is_chosen = true;
            curr_gt_infos[i].is_first = false;
            curr_gt_infos[i].multi_chosen = true;
        }
        else if(mode==unchooseAll && curr_gt_infos[i].is_adding_wheel==false){
            curr_gt_infos[i].is_chosen = false;
            curr_gt_infos[i].is_first = true;
            curr_gt_infos[i].multi_chosen = false;
//            if(curr_gt_infos[i].is_adding_wheel==true){
//            }
//            curr_gt_infos[i].is_adding_wheel = false;
        }
        else if(mode==showSelectedUnchoose){
            curr_gt_infos[i].draw_enabled = false;
        }
        else if(mode==showBbox){
            curr_gt_infos[i].draw_enabled = !(curr_gt_infos[i].draw_enabled);
        }
        else if(mode==showSelectedBbox && curr_gt_infos[i].is_chosen==false){
            curr_gt_infos[i].draw_enabled = !(curr_gt_infos[i].draw_enabled);
        }
        else if(mode==changeAngle && curr_gt_infos[i].is_chosen==true && curr_gt_infos[i].dir_angle>-0.1F){
            float_t rotated_angle = curr_gt_infos[i].dir_angle + 90.F;
            curr_gt_infos[i].dir_angle = rotated_angle<360.F ? rotated_angle : (rotated_angle-360.F);
            change_gt_mode = true;
        }
        else if(mode==enableDraw){
            curr_gt_infos[i].draw_enabled = true;
        }
        else if(mode==enableAddWheel && curr_gt_infos[i].is_chosen==true && curr_gt_infos[i].is_2_wheeler==true){
            curr_gt_infos[i].is_adding_wheel = true;
        }
    }

    emit updateGtInfos(curr_gt_infos, change_gt_mode);
}

void DataLoader::setRearWheelPoint(float_t x, float_t y){
    savePrevInfos();

    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=0; i<obj_size; i++){
        if(curr_gt_infos[i].is_chosen==true && curr_gt_infos[i].is_2_wheeler==true && curr_gt_infos[i].is_adding_wheel==true){
            curr_gt_infos[i].rear_x = x;
            curr_gt_infos[i].rear_y = y;
            break;
        }
    }

    emit updateGtInfos(curr_gt_infos, true);
}

void DataLoader::setFrontWheelPoint(float_t x, float_t y){
    savePrevInfos();

    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=0; i<obj_size; i++){
        if(curr_gt_infos[i].is_chosen==true && curr_gt_infos[i].is_2_wheeler==true && curr_gt_infos[i].is_adding_wheel==true){
            curr_gt_infos[i].front_x = x;
            curr_gt_infos[i].front_y = y;
            curr_gt_infos[i].is_adding_wheel = false;
            break;
        }
    }

    emit updateGtInfos(curr_gt_infos, true);
}

void DataLoader::deleteWheelPoint(){
    savePrevInfos();

    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=0; i<obj_size; i++){
        if(curr_gt_infos[i].is_chosen==true && curr_gt_infos[i].is_2_wheeler==true){
            curr_gt_infos[i].rear_x = -1.F;
            curr_gt_infos[i].rear_y = -1.F;
            curr_gt_infos[i].front_x = -1.F;
            curr_gt_infos[i].front_y = -1.F;
            break;
        }
    }

    emit updateGtInfos(curr_gt_infos, true);
}

void DataLoader::setVehicleMode(){
    this->is_vehicle_mode = true;
    this->is_zoom_mode = false;

    int32_t obj_size = curr_gt_infos.size();
    bool has_background = false;
    for(int32_t i=0; i<obj_size; i++){
        if(curr_gt_infos[i].is_background==true){
            has_background = true;
        }
        curr_gt_infos[i].vehicle_mode = true;
        curr_gt_infos[i].zoom_mode = false;
    }
    if(has_background==false){
        GtInfo background;
        background.is_background = true;
        background.bbox.x1 = 0.F;
        background.bbox.y1 = 0.F;
        background.bbox.x2 = frame_width;
        background.bbox.y2 = frame_height;
        curr_gt_infos.push_back(background);
    }

    emit updateGtInfos(curr_gt_infos, false);
}
void DataLoader::setRiderMode(){
    this->is_vehicle_mode = false;
    this->is_zoom_mode = false;

    int32_t obj_size = curr_gt_infos.size();
    bool has_background = false;
    for(int32_t i=0; i<obj_size; i++){
        if(curr_gt_infos[i].is_background==true){
            has_background = true;
        }
        curr_gt_infos[i].vehicle_mode = false;
        curr_gt_infos[i].is_chosen = false;
        curr_gt_infos[i].multi_chosen = false;
        curr_gt_infos[i].is_first = true;
        curr_gt_infos[i].zoom_mode = false;
    }
    if(has_background==false){
        GtInfo background;
        background.is_background = true;
        background.vehicle_mode = false;
        background.bbox.x1 = 0.F;
        background.bbox.y1 = 0.F;
        background.bbox.x2 = frame_width;
        background.bbox.y2 = frame_height;
        curr_gt_infos.push_back(background);
    }

    emit updateGtInfos(curr_gt_infos, false);
}

void DataLoader::zoomMode(){
    this->is_vehicle_mode = false;
    this->is_zoom_mode = true;

    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=obj_size-1; i>-1; i--){
        if(curr_gt_infos[i].is_background==true){
            curr_gt_infos.removeAt(i);
            continue;
        }
        curr_gt_infos[i].vehicle_mode = false;
        curr_gt_infos[i].zoom_mode = true;
//        curr_gt_infos[i].is_chosen = false;
//        curr_gt_infos[i].multi_chosen = false;
//        curr_gt_infos[i].is_first = true;
    }

    emit updateGtInfos(curr_gt_infos, false);
}

QString DataLoader::getSelectedRiderWheelPoints(){
    QString out_string = "";

    int32_t obj_size = curr_gt_infos.size();
    for(int32_t i=0; i<obj_size; i++){
        if(curr_gt_infos[i].is_2_wheeler==true && curr_gt_infos[i].is_chosen==true && curr_gt_infos[i].multi_chosen==false && curr_gt_infos[i].is_svnet_rider==true){
            out_string += "Rear: (" + QString::number(curr_gt_infos[i].rear_x) + ", " + QString::number(curr_gt_infos[i].rear_y) + ") \n";
            out_string += "Front: (" + QString::number(curr_gt_infos[i].front_x) + ", " + QString::number(curr_gt_infos[i].front_y) + ") \n";
//            viewText->setText(out_string);
            break;
        }
    }

    return out_string;
}

bool DataLoader::eventFilter(QObject* obj, QEvent* event)
{
////    emit updateLabelInfos(gts_infos[curr_gt_idx]);
////    if(event->type() == QEvent::KeyPress){
////        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
////        if (keyEvent->key() == Qt::Key_N) {
////            auto pos = this->viewWindow->mapFromGlobal(QCursor::pos());
////            createNewFrameInfo(viewWindow->mapToScene(pos));
////        }
////        else if(keyEvent->key()==Qt::Key_D){
////            deleteGtInfo();
////        }
////        else if(keyEvent->key()==Qt::Key_Escape){
////            unchooseAll();
////        }

////        return true;
////    }
////    else if(event->type() == QEvent::MouseButtonPress){
    if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//        if(mouseEvent->modifiers()==Qt::Modifier::CTRL){
//            drag_start_x = mouseEvent->pos().x();
//            drag_start_y = mouseEvent->pos().y();
//            viewWindow->grabMouse();
//        }
//        else{
        move_start_x = mouseEvent->pos().x();
        move_start_y = mouseEvent->pos().y();
        viewWindow->grabMouse();

//        qDebug() << move_start_x << move_start_y;

        if(is_vehicle_mode==true){
            unselect_target = true;
        }
        else{
            add_rider = true;
        }
//        }
//        move_start_x = mouseEvent->pos().x();
//        move_start_y = mouseEvent->pos().y();
//        viewWindow->grabMouse();

//        if(is_vehicle_mode==true){
//            unselect_target = true;
//        }
//        else{
//            add_rider = true;
//        }

        return true;
    }
    else if(event->type() == QEvent::MouseMove){
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        int curr_x = mouseEvent->pos().x();
        int curr_y = mouseEvent->pos().y();
//        if(mouseEvent->modifiers()==Qt::Modifier::CTRL){
//            is_drag_mode = true;
//        }
//        else{
        viewWindow->horizontalScrollBar()->setValue(viewWindow->horizontalScrollBar()->value() - (curr_x - move_start_x));
        viewWindow->verticalScrollBar()->setValue(viewWindow->verticalScrollBar()->value() - (curr_y - move_start_y));
        move_start_x = curr_x;
        move_start_y = curr_y;

        if(is_vehicle_mode==true){
            unselect_target = false;
        }
        else{
            add_rider = false;
        }
//        }
//        viewWindow->horizontalScrollBar()->setValue(viewWindow->horizontalScrollBar()->value() - (curr_x - move_start_x));
//        viewWindow->verticalScrollBar()->setValue(viewWindow->verticalScrollBar()->value() - (curr_y - move_start_y));
//        move_start_x = curr_x;
//        move_start_y = curr_y;

//        if(is_vehicle_mode==true){
//            unselect_target = false;
//        }
//        else{
//            add_rider = false;
//        }

        return true;
    }
    else if(event->type() == QEvent::MouseButtonRelease){
////        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
////        float_t curr_x = mouseEvent->pos().x();
////        float_t curr_y = mouseEvent->pos().y();
////        if(is_drag_mode==true){
////            drag_end_x = curr_x;
////            drag_end_y = curr_y;

////            is_drag_mode = false;

////            qDebug() << drag_start_x << drag_start_y << drag_end_x << drag_end_y;
////            int32_t drag_min_x = std::min(drag_start_x, drag_end_x);
////            int32_t drag_max_x = std::max(drag_start_x, drag_end_x);
////            int32_t drag_min_y = std::min(drag_start_y, drag_end_y);
////            int32_t drag_max_y = std::max(drag_start_y, drag_end_y);


////            int32_t obj_size = curr_gt_infos.size();
////            for(int32_t i=0; i<obj_size; i++){
////                bool curr_chosen = false;
////                for(int32_t j=0; j<8; j++){
////                    if(drag_min_x<=curr_gt_infos[i].x[j] && curr_gt_infos[i].x[j]<=drag_max_x && drag_min_y<=curr_gt_infos[i].y[j] && curr_gt_infos[i].y[j]<=drag_max_y){
////                        curr_chosen = true;
////                        break;
////                    }
////                }
////                if(curr_chosen==true){
////                    curr_gt_infos[i].is_chosen = true;
////                    curr_gt_infos[i].multi_chosen = true;
////                }
////            }

////            emit updateGtInfos(curr_gt_infos);
////        }
////        else{
////            if(is_vehicle_mode==true){
////                if(unselect_target==true){
////                    editGTs(unchooseAll);
////                }
////                unselect_target = false;
////            }
////            else{
////                if(add_rider==true && (rear_enabled==true || front_enabled==true)){
//////                    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//////                    float_t curr_x = mouseEvent->pos().x();
//////                    float_t curr_y = mouseEvent->pos().y();
////    //                GtInfo new_rider_gt;
////    //                new_rider_gt.is_2_wheeler = true;
////    //                new_rider_gt.vehicle_mode = false;
////                    if(rear_enabled==true){
////                        rear_coord_txt->setText(QString::number(curr_x)+", "+QString::number(curr_y));
////    //                    new_rider_gt.rear_x = curr_x;
////    //                    new_rider_gt.rear_y = curr_y;
////                    }
////                    else if(front_enabled==true){
////                        front_coord_txt->setText(QString::number(curr_x)+", "+QString::number(curr_y));
////    //                    new_rider_gt.front_x = curr_x;
////    //                    new_rider_gt.front_y = curr_y;
////                    }
////                }
////                add_rider = false;
////            }
////        }

        viewWindow->releaseMouse();

        return true;
    }
    else{
        return false;
    }
}









