#ifndef DATALOADER_H
#define DATALOADER_H

#include <QObject>
#include <QDir>
#include <QVector>
#include <QGraphicsView>
#include <QLabel>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMouseEvent>
#include <QScrollBar>

#include <fstream>
#include <iostream>

#include "InfoStructure.h"
#include "json/json.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//struct DrawInfo{
//    float zoom_ratio;
//};


class DataLoader : public QObject
{
    Q_OBJECT

public:
    explicit DataLoader(QGraphicsView* mainWindow, QLabel* LoadTypeText, QLabel *rear_coord_txt, QLabel *front_coord_txt, QObject* parent = nullptr);
    ~DataLoader();

    enum editMode {
        chooseAll=0,
        unchooseAll,
        showBbox,
        showSelectedBbox,
        showSelectedUnchoose,
        changeAngle,
        enableDraw,
        enableAddWheel
    };

    void readJson();

    void setAllPath(QString const all_path);
    void setGTPath(QString const gt_path);
    void setImgPath(QString const img_path);
    void setSavePath(QString const save_path);

    void setRearWheelPoint(float x, float y);
    void setFrontWheelPoint(float x, float y);
    void deleteWheelPoint();

    bool checkOrigGtLoaded(){
        return start_w_orig_gt;
    }

    void setNewGtMode(bool mode){
        this->is_new_gt_mode = mode;
    }

    void mouseClickBackground(){
        unselectOthers(-1);
    }

    void setFrameNo(int frame_no){
        this->curr_frame_no = frame_no;
    }
    int getFrameNo(){
        return this->curr_frame_no;
    }
    int getNumFrames(){
        return this->num_frames;
    }
    GtInfo getGtInfo(int idx){
        return this->curr_gt_infos[idx];
    }

    void setRearEnabled(bool t_f){
        this->rear_enabled = t_f;
    }
    void setFrontEnabled(bool t_f){
        this->front_enabled = t_f;
    }

//    void setPrevWidthHeight(float width, float height){
//        this->prev_frame_width = width;
//        this->prev_frame_height = height;
//    }
//    void getPrevWidthHeight(float& width, float& height){
//        width = this->prev_frame_width;
//        height = this->prev_frame_height;
//    }

    void addNewRider(GtInfo &new_rider_gt);

//    void showCurrRiderGTs();
    QString getSelectedRiderWheelPoints();

    void showFrame();
    void saveCurrGT();

//    void reverseDirAngle();
    void deleteGTs();
    void editGTs(editMode mode);

    void unDo();
    void reDo();

    void setVehicleMode();
    void setRiderMode();

    void zoomMode();

    void reInitGtInfos(bool vehicle_mode, bool zoom_mode);
//    void SetDrawInfo(DrawInfo const* _draw_info);

private:
    QString gt_path;
    QString img_path;
    QString save_path;

    int frame_width;
    int frame_height;

    cv::Mat curr_frame;
    QImage* curr_img;

    cv::VideoCapture *vc;

    bool initialized;

    QString curr_gt_name;
    QString curr_img_name;

    QVector<QString> files_list;
    int num_frames;
    int curr_frame_no;

    QVector<GtInfo> curr_gt_infos;
    QVector<GtInfo> orig_gt_infos;

    QVector<QVector<GtInfo>> prev_done_gts;
    int undo_redo_idx;
    bool undo_flag;

    QGraphicsView* viewWindow;
    QLabel* viewText;// = nullptr;
    QLabel* rear_coord_txt;// = nullptr;
    QLabel* front_coord_txt;// = nullptr;

    int move_start_x;
    int move_start_y;

    int drag_start_x;
    int drag_start_y;
    int drag_end_x;
    int drag_end_y;
    bool is_drag_mode = false;

    bool is_new_gt_mode = false;

    void createQImage();

    void copyOrigGT();

//    void readJson();
    void savePrevInfos();

    bool unselect_target;

    bool is_vehicle_mode;
    bool add_rider;

    bool rear_enabled;
    bool front_enabled;

    bool is_zoom_mode;

    bool start_w_orig_gt = true;

//    void Zoom(float factor);
    bool is_mouse_pressed = false;
    bool is_mouse_moving = false;
//    float prev_frame_width;
//    float prev_frame_height;

//    QPointF target_scene_pos;
//    QPointF target_viewport_pos;
//    float zoom_factor;

//    DrawInfo const* ptr_draw_info;

signals:
    void UpdateFrontImage(QImage* img);
    void SetNewVideo(int img_width, int img_height);
    void updateGtInfos(QVector<GtInfo> &gts_info, bool change_gt_mode);
//    void zoomed();
//    void updateGtInfos(QVector<GtInfo> &gts_info);

protected:
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void unselectOthers(int curr_idx);
    void setMultiChosen();
    void selectDraggedArea(Bbox &dragged_area);
    void setRiderPoint(float x, float y);

    void removeBackground();
//    void addBackground();
//    void dragZoomFocusedArea(float x, float y);
//    void setStartXY(float x, float y);

};

#endif // DATALOADER_H
