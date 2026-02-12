#include "../src/renderer.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void) {
    Renderer rend = {0};
    
    if (!renderer_init(&rend, "Example 06: Textured 3D Cube", 800, 600)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }

    Image* texture = image_load_bmp("assets/test.bmp");
    if (!texture) {
        printf("Warning: Could not load test.bmp, using colored faces\n");
    }

    Vec3 cube_verts[8] = {
        {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},
        {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1}
    };

    int cube_faces[12][3] = {
        {0, 1, 2}, {0, 2, 3}, {4, 6, 5}, {4, 7, 6},
        {0, 4, 5}, {0, 5, 1}, {2, 6, 7}, {2, 7, 3},
        {0, 3, 7}, {0, 7, 4}, {1, 5, 6}, {1, 6, 2}
    };

    float uvs[12][3][2] = {
        {{0,0}, {1,0}, {1,1}}, {{0,0}, {1,1}, {0,1}},
        {{0,0}, {1,1}, {1,0}}, {{0,0}, {0,1}, {1,1}},
        {{0,0}, {1,0}, {1,1}}, {{0,0}, {1,1}, {0,1}},
        {{0,0}, {1,0}, {1,1}}, {{0,0}, {1,1}, {0,1}},
        {{0,0}, {1,0}, {1,1}}, {{0,0}, {1,1}, {0,1}},
        {{0,0}, {1,0}, {1,1}}, {{0,0}, {1,1}, {0,1}}
    };

    Color face_colors[12] = {
        {255,100,100,255}, {255,100,100,255}, {100,255,100,255}, {100,255,100,255},
        {100,100,255,255}, {100,100,255,255}, {255,255,100,255}, {255,255,100,255},
        {255,100,255,255}, {255,100,255,255}, {100,255,255,255}, {100,255,255,255}
    };

    clock_t start = clock();

    while (rend.is_running) {
        renderer_handle_events(&rend);
        float time = (float)(clock() - start) / CLOCKS_PER_SEC;

        renderer_clear(&rend, color_make(20, 25, 35, 255));
        
        renderer_draw_text(&rend, texture ? "Textured 3D Cube" : "Colored 3D Cube", 10, 10, color_make(255, 255, 255, 255), 2);

        Mat4 model = mat4_identity();
        model = mat4_mul(model, mat4_rotate_x(time * 0.6f));
        model = mat4_mul(model, mat4_rotate_y(time * 0.8f));
        model = mat4_mul(model, mat4_translate(0, 0, -4));

        Mat4 view = mat4_look_at(vec3_make(0, 0, 0), vec3_make(0, 0, -1), vec3_make(0, 1, 0));
        Mat4 proj = mat4_perspective(M_PI / 3.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        Mat4 transform = mat4_mul(proj, mat4_mul(view, model));

        for (int i = 0; i < 12; i++) {
            Vec3 v1 = cube_verts[cube_faces[i][0]];
            Vec3 v2 = cube_verts[cube_faces[i][1]];
            Vec3 v3 = cube_verts[cube_faces[i][2]];
            
            int x1, y1, x2, y2, x3, y3;
            renderer_project_point(&rend, v1, transform, &x1, &y1, NULL);
            renderer_project_point(&rend, v2, transform, &x2, &y2, NULL);
            renderer_project_point(&rend, v3, transform, &x3, &y3, NULL);
            
            if (texture) {
                renderer_draw_textured_triangle(&rend, x1, y1, x2, y2, x3, y3,
                    uvs[i][0][0], uvs[i][0][1], uvs[i][1][0], uvs[i][1][1],
                    uvs[i][2][0], uvs[i][2][1], texture);
            } else {
                renderer_draw_filled_triangle(&rend, x1, y1, x2, y2, x3, y3, face_colors[i]);
            }
        }

        renderer_present(&rend);
        renderer_sleep(16);
    }

    if (texture) image_free(texture);
    renderer_cleanup(&rend);
    return 0;
}
