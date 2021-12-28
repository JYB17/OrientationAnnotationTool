#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QWheelEvent>

#include "DataLoader.h"
#include "FrontView.h"
#include "LabelManager.h"
#include "QFileDialog"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void timeout();

    void SetNewVideo(int32_t img_width, int32_t img_height);

    void on_actionLoad_All_triggered();

    void on_actionLoad_GTs_triggered();

    void on_actionLoad_Frame_Images_triggered();

    void on_btn_forward_clicked();

    void on_btn_backward_clicked();

    void on_video_navigator_valueChanged(int value);

    void on_btn_play_pause_clicked();

    void on_btn_save_clicked();

    void on_btn_zoom_fit_clicked();

    void on_btn_reverse_angle_clicked();

    void on_btn_delete_clicked();

    void on_btn_choose_all_clicked();

    void on_btn_unchoose_all_clicked();

    void on_btn_undo_clicked();

    void on_btn_redo_clicked();

    void on_check_show_bbox_clicked();

    void on_check_selected_bbox_clicked();

    void on_radio_rider_clicked();

    void on_radio_normal_clicked();

    void on_btn_add_rider_clicked();

    void on_btn_enter_rear_clicked();

    void on_btn_enter_front_clicked();

    void on_radio_zoom_clicked();

    void on_btn_load_gts_imgs_clicked();

    void on_btn_delete_wheel_points_clicked();

    void on_btn_init_save_clicked();

    void on_check_show_orig_clicked();

    void setAddWheelMode(int32_t curr_idx);
    void clickBackground();
    void setNewGtMode();

    void on_btn_backward_30_clicked();

    void on_btn_forward_30_clicked();

    void on_btn_move_frame_clicked();

private:
    void changeFrame(int32_t frame_no);
    void play_video();
    void pause_video();

    void fit_frame_to_canvas();

    Ui::MainWindow *ui;

    QGraphicsScene *m_scene;
    DataLoader *m_dataloader;
    FrontView *m_frontview;
    QTimer *m_timer;
    LabelManager *m_labelmanager;

    int32_t prev_canvas_width;
    int32_t prev_canvas_height;

    float_t prev_frame_width;
    float_t prev_frame_height;
    float_t curr_scale_ratio;

    int32_t frame_width;
    int32_t frame_height;

    int32_t num_frames;

    bool saved_flag = false;
    bool selected_checked = false;

    bool is_initialized = false;

    GtInfo new_rider;

//    DrawInfo draw_info;

protected:
    virtual void wheelEvent(QWheelEvent* event) override;
};
#endif // MAINWINDOW_H
