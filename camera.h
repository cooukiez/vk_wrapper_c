//
// Created by Ludw on 4/23/2024.
//

#include "type_def.h"

#define CAM_SLOW 0.05f
#define CAM_FAST 0.1f

#ifndef VCW_CAMERA_H
#define VCW_CAMERA_H

VCW_Camera create_default_cam(VkExtent2D res);

void update_proj(VCW_Camera *cam, VkExtent2D res);

void update_cam_rotation(VCW_Camera *cam, float dx, float dy);

void write_view_proj_mat(VCW_Camera cam, mat4 *view_pro);

#endif //VCW_CAMERA_H
