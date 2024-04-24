//
// Created by Ludw on 4/23/2024.
//

#include "camera.h"

// these are just default values
// hence they are private to this file
#define FOV 60.0f

#define SPEED 0.5f
#define SENSITIVITY 0.1f

#define CAM_NEAR 0.1f
#define CAM_FAR 100.0f

#define MIN_PITCH (-89.0f)
#define MAX_PITCH 89.0f
#define MIN_YAW (-180.0f)
#define MAX_YAW 180.0f

VCW_Camera create_default_cam(VkExtent2D res) {
    VCW_Camera cam;

    glm_vec3_copy((vec3) {0.0f, 0.0f, 0.0f}, cam.pos);
    glm_vec3_copy((vec3) {0.0f, 0.0f, 0.0f}, cam.front);
    glm_vec3_copy((vec3) {0.0f, 1.0f, 0.0f}, cam.up);

    cam.yaw = 0.0f;
    cam.pitch = 0.0f;

    cam.fov = FOV;
    cam.speed = SPEED;
    cam.sensitivity = SENSITIVITY;

    cam.aspect_ratio = (float) res.width / (float) res.height;
    cam.near = CAM_NEAR;
    cam.far = CAM_FAR;

    glm_perspective(glm_rad(cam.fov), cam.aspect_ratio, cam.near, cam.far, cam.proj);

    return cam;
}

void update_proj(VCW_Camera *cam, VkExtent2D res) {
    cam->aspect_ratio = (float) res.width / (float) res.height;
    glm_perspective(glm_rad(cam->fov), cam->aspect_ratio, cam->near, cam->far, cam->proj);
}

void update_cam_rotation(VCW_Camera *cam, float dx, float dy) {
    cam->yaw += dx * cam->sensitivity;
    cam->pitch += dy * cam->sensitivity;

    cam->pitch = glm_clamp(cam->pitch, MIN_PITCH, MAX_PITCH);
    cam->yaw = glm_clamp(cam->yaw, MIN_YAW, MAX_YAW);

    cam->front[0] = cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    cam->front[1] = sinf(glm_rad(cam->pitch));
    cam->front[2] = sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    glm_vec3_normalize(cam->front);

    cam->right[0] = sinf(glm_rad(cam->yaw));
    cam->right[1] = 0.0f;
    cam->right[2] = -cosf(glm_rad(cam->yaw));
    glm_vec3_normalize(cam->right);

    glm_vec3_scale(cam->front, cam->speed, cam->mov_lin);
    glm_vec3_scale(cam->right, cam->speed, cam->mov_lat);
}

void write_view_proj_mat(VCW_Camera cam, mat4 *view_pro) {
    vec3 absolute_front;
    glm_vec3_add(cam.pos, cam.front, absolute_front);

    mat4 look_at;
    glm_lookat(cam.pos, absolute_front, cam.up, look_at);

    glm_mat4_mul(cam.proj, look_at, *view_pro);
}
