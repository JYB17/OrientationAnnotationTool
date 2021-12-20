#ifndef INFOSTRUCTURE_H
#define INFOSTRUCTURE_H

#include <QString>
#include <QVector>

struct GtInfo;

struct Bbox {
    float_t x1;
    float_t y1;
    float_t x2;
    float_t y2;
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
    float_t dir_angle{};
    float_t x[8]{}, y[8]{};
    float_t X{}, Y{};
    float_t rear_x{-1.F}, rear_y{-1.F};
    float_t front_x{-1.F}, front_y{-1.F};
    float_t score{};

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
