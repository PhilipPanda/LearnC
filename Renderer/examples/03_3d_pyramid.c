#include "../src/renderer.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void) {
    Renderer rend = {0};
    
    if (!renderer_init(&rend, "Example 04: 3D Colored Pyramid", 800, 600)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }

    // Pyramid with apex at top, square base at bottom
    Vec3 pyramid_vertices[5] = {
        { 0,  1,  0},   // 0: Top point
        {-1, -1,  1},   // 1-4: Bottom square
        { 1, -1,  1},
        { 1, -1, -1},
        {-1, -1, -1}
    };

    // 6 triangular faces - 4 sides + 2 for the bottom square
    // Each face is 3 vertex indices in counter-clockwise order
    int pyramid_faces[6][3] = {
        {0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 1},  // 4 sides
        {1, 4, 3}, {1, 3, 2}                          // Bottom (2 triangles)
    };

    // Base colors for each face (lighting will adjust these)
    Color face_colors[6] = {
        {255, 100, 100, 255}, {100, 255, 100, 255},  // Sides: red, green
        {100, 100, 255, 255}, {255, 255, 100, 255},  // Sides: blue, yellow
        {255, 100, 255, 255}, {100, 255, 255, 255}   // Bottom: magenta, cyan
    };

    clock_t start = clock();

    while (rend.is_running) {
        renderer_handle_events(&rend);
        float time = (float)(clock() - start) / CLOCKS_PER_SEC;

        renderer_clear(&rend, color_make(8, 10, 15, 255));
        
        renderer_draw_text(&rend, "3D Pyramid with Lighting", 10, 10, color_make(255, 255, 255, 255), 2);
        renderer_draw_text(&rend, "Surfaces facing the light are brighter", 10, 30, color_make(150, 150, 150, 255), 1);

        // Slower rotation for better 3D perception
        Mat4 model = mat4_identity();
        model = mat4_mul(model, mat4_rotate_y(time * 0.7f));
        model = mat4_mul(model, mat4_rotate_x(sin(time * 0.5f) * 0.4f));
        model = mat4_mul(model, mat4_translate(0, 0, -3.5f));  // Closer

        Mat4 view = mat4_look_at(
            vec3_make(0, 0, 0),
            vec3_make(0, 0, -1),
            vec3_make(0, 1, 0)
        );

        Mat4 proj = mat4_perspective(M_PI / 3.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        Mat4 transform = mat4_mul(proj, mat4_mul(view, model));

        // Light from top-right
        Vec3 light_dir = vec3_normalize(vec3_make(0.5f, 0.8f, 0.6f));

        // Draw faces back to front for proper visibility
        for (int i = 0; i < 6; i++) {
            Vec3 v1 = pyramid_vertices[pyramid_faces[i][0]];
            Vec3 v2 = pyramid_vertices[pyramid_faces[i][1]];
            Vec3 v3 = pyramid_vertices[pyramid_faces[i][2]];
            
            // Calculate normal in model space
            Vec3 edge1 = vec3_sub(v2, v1);
            Vec3 edge2 = vec3_sub(v3, v1);
            Vec3 normal = vec3_normalize(vec3_cross(edge1, edge2));
            
            // Lighting calculation
            float diffuse = vec3_dot(normal, light_dir);
            if (diffuse < 0.0f) diffuse = 0.0f;
            
            // Ambient + diffuse
            float brightness = 0.3f + diffuse * 0.7f;
            
            Color col = face_colors[i];
            col.r = (uint8_t)(col.r * brightness);
            col.g = (uint8_t)(col.g * brightness);
            col.b = (uint8_t)(col.b * brightness);
            
            renderer_draw_triangle_3d(&rend, v1, v2, v3, transform, col);
            
            // Draw edges for better depth perception
            renderer_draw_line_3d(&rend, v1, v2, transform, color_make(20, 20, 20, 255));
            renderer_draw_line_3d(&rend, v2, v3, transform, color_make(20, 20, 20, 255));
            renderer_draw_line_3d(&rend, v3, v1, transform, color_make(20, 20, 20, 255));
        }

        renderer_present(&rend);
        renderer_sleep(16);
    }

    renderer_cleanup(&rend);
    return 0;
}
