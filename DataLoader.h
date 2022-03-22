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
    explicit DataLoader(QGraphicsView* mainWindow, \
                        QLabel* LoadTypeText, QLabel *rear_coord_txt, QLabel *front_coord_txt, QLabel* scene_pos_txt, QLabel* viewport_post_txt,
                        QObject* parent=nullptr);
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

    void setFrameNo(int32_t frame_no){
        this->curr_frame_no = frame_no;
    }
    int32_t getFrameNo(){
        return this->curr_frame_no;
    }
    int32_t getNumFrames(){
        return this->num_frames;
    }
    GtInfo getGtInfo(int32_t idx){
        return this->curr_gt_infos[idx];
    }

    void setRearEnabled(bool t_f){
        this->rear_enabled = t_f;
    }
    void setFrontEnabled(bool t_f){
        this->front_enabled = t_f;
    }

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

    int32_t frame_width;
    int32_t frame_height;

    cv::Mat curr_frame;
    QImage* curr_img;

    bool initialized;

    QString curr_gt_name;
    QString curr_img_name;

    QVector<QString> files_list;
    int32_t num_frames;
    int32_t curr_frame_no;

    QVector<GtInfo> curr_gt_infos;
    QVector<GtInfo> orig_gt_infos;

    QVector<QVector<GtInfo>> prev_done_gts;
    int32_t undo_redo_idx;
    bool undo_flag;

    QGraphicsView* viewWindow;
    QLabel* viewText;// = nullptr;
    QLabel* rear_coord_txt;// = nullptr;
    QLabel* front_coord_txt;// = nullptr;

    int32_t move_start_x;
    int32_t move_start_y;

    int32_t drag_start_x;
    int32_t drag_start_y;
    int32_t drag_end_x;
    int32_t drag_end_y;
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

    /*-----Youngjae.Lee-----*/
    cv::VideoCapture* vc_;
    QPointF target_scene_pos_;
    QPointF target_viewport_pos_;
    float zoom_factor_;

    QLabel* scene_pos_txt_;
    QLabel* viewport_pos_txt_;

    void Zoom(double factor);

//    DrawInfo const* ptr_draw_info;

signals:
    void UpdateFrontImage(QImage* img);
    void SetNewVideo(int32_t img_width, int32_t img_height);
    void updateGtInfos(QVector<GtInfo> &gts_info, bool change_gt_mode);
    void zoomed();
//    void updateGtInfos(QVector<GtInfo> &gts_info);

protected:
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void unselectOthers(int32_t curr_idx);
    void setMultiChosen();
    void selectDraggedArea(Bbox &dragged_area);
    void setRiderPoint(float x, float y);
//    void dragZoomFocusedArea(float x, float y);
//    void setStartXY(float x, float y);

};

#endif // DATALOADER_H
