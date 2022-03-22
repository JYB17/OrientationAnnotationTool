#ifndef INFOSTRUCTURE_H
#define INFOSTRUCTURE_H

#include <QString>
#include <QVector>

struct GtInfo;

struct Bbox {
    float x1;
    float y1;
    float x2;
    float y2;
};

//struct Bbox_int{
//    int32_t x1{};
//    int32_t y1{};
//    int32_t x2{};
//    int32_t y2{};
//};

struct GtInfo {
//    QString obj_cls;
    Bbox bbox;
    float dir_angle{};
    float x[8]{}, y[8]{};
    float X{}, Y{};
    float rear_x{-1.F}, rear_y{-1.F};
    float front_x{-1.F}, front_y{-1.F};
    float score{};

    bool draw_enabled = true;
    bool is_first = true;
    bool is_chosen = false;
    bool move_resize = false;

    bool vehicle_mode = true;
    bool zoom_mode = false;
    bool is_2_wheeler = false;
    bool is_svnet_rider = false;

    int32_t curr_idx = -1;
    bool multi_chosen = false;

    bool is_background = false;
    Bbox drag_chosen_area;
//    Bbox_int zoom_drag_bbox;
    bool is_not_dragging = false;
//    bool show_cls = false;

    bool is_adding_wheel = false;

    bool is_valid = true;
};


#endif // INFOSTRUCTURE_H
