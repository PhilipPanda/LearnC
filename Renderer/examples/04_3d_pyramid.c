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

        renderer_clear(&rend, color_make(10, 15, 25, 255));
        
        renderer_draw_text(&rend, "3D Pyramid with Basic Lighting", 10, 10, color_make(255, 255, 255, 255), 2);
        renderer_draw_text(&rend, "Faces facing light are brighter", 10, 30, color_make(200, 200, 200, 255), 1);

        // Transform matrices - same pattern as the cube example
        Mat4 model = mat4_identity();
        model = mat4_mul(model, mat4_rotate_x(time * 0.5f + 0.3f));
        model = mat4_mul(model, mat4_rotate_y(time));
        model = mat4_mul(model, mat4_translate(0, 0, -5));

        Mat4 view = mat4_look_at(
            vec3_make(0, 0, 0),
            vec3_make(0, 0, -1),
            vec3_make(0, 1, 0)
        );

        Mat4 proj = mat4_perspective(M_PI / 3.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        Mat4 transform = mat4_mul(proj, mat4_mul(view, model));

        // Draw each triangular face
        for (int i = 0; i < 6; i++) {
            Vec3 v1 = pyramid_vertices[pyramid_faces[i][0]];
            Vec3 v2 = pyramid_vertices[pyramid_faces[i][1]];
            Vec3 v3 = pyramid_vertices[pyramid_faces[i][2]];
            
            // Calculate surface normal (perpendicular to the face)
            // Cross product of two edges gives a perpendicular vector
            Vec3 edge1 = vec3_sub(v2, v1);
            Vec3 edge2 = vec3_sub(v3, v1);
            Vec3 normal = vec3_cross(edge1, edge2);
            
            // Simple diffuse lighting - dot product with light direction
            // Faces pointing toward light are brighter
            Vec3 light_dir = vec3_make(0, 0, 1);
            float brightness = vec3_dot(vec3_normalize(normal), light_dir);
            if (brightness < 0.2f) brightness = 0.2f;  // Minimum ambient light
            
            // Apply lighting to the base color
            Color col = face_colors[i];
            col.r = (uint8_t)(col.r * brightness);
            col.g = (uint8_t)(col.g * brightness);
            col.b = (uint8_t)(col.b * brightness);
            
            renderer_draw_triangle_3d(&rend, v1, v2, v3, transform, col);
        }

        renderer_present(&rend);
        renderer_sleep(16);
    }

    renderer_cleanup(&rend);
    return 0;
}
