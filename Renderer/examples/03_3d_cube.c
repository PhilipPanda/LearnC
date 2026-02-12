#include "../src/renderer.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void) {
    Renderer rend = {0};
    
    if (!renderer_init(&rend, "Example 03: 3D Wireframe Cube", 800, 600)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }

    // Cube vertices in 3D space (unit cube centered at origin)
    // Coordinates go from -1 to 1 in each dimension
    Vec3 cube_vertices[8] = {
        {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},  // Front face
        {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1}   // Back face
    };

    // Edges connect vertices - each edge is a pair of vertex indices
    // 12 edges total: 4 on front face, 4 on back face, 4 connecting them
    int cube_edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // Front face
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Back face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}   // Connections
    };

    clock_t start = clock();

    while (rend.is_running) {
        renderer_handle_events(&rend);
        float time = (float)(clock() - start) / CLOCKS_PER_SEC;

        renderer_clear(&rend, color_make(15, 20, 30, 255));
        
        renderer_draw_text(&rend, "3D Wireframe Cube", 10, 10, color_make(255, 255, 255, 255), 2);
        renderer_draw_text(&rend, "Matrices transform 3D points to screen", 10, 30, color_make(200, 200, 200, 255), 1);

        // MODEL matrix - positions and orients the cube in 3D space
        Mat4 model = mat4_identity();
        model = mat4_mul(model, mat4_rotate_x(time * 0.7f));    // Spin around X axis
        model = mat4_mul(model, mat4_rotate_y(time));           // Spin around Y axis
        model = mat4_mul(model, mat4_rotate_z(time * 0.5f));    // Spin around Z axis
        model = mat4_mul(model, mat4_translate(0, 0, -5));      // Move back 5 units

        // VIEW matrix - camera position and orientation
        // Camera at origin, looking down negative Z, Y is up
        Mat4 view = mat4_look_at(
            vec3_make(0, 0, 0),    // Eye position
            vec3_make(0, 0, -1),   // Look at point
            vec3_make(0, 1, 0)     // Up direction
        );

        // PROJECTION matrix - creates perspective (distant = smaller)
        // FOV 60 degrees, aspect ratio 4:3, near/far clip planes
        Mat4 proj = mat4_perspective(M_PI / 3.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        
        // Combine matrices: apply model, then view, then projection
        Mat4 transform = mat4_mul(proj, mat4_mul(view, model));

        // Draw all 12 edges of the cube
        for (int i = 0; i < 12; i++) {
            Vec3 p1 = cube_vertices[cube_edges[i][0]];
            Vec3 p2 = cube_vertices[cube_edges[i][1]];
            
            // Animated color for visual interest
            uint8_t intensity = (uint8_t)(128 + 127 * sin(time + i * 0.5f));
            renderer_draw_line_3d(&rend, p1, p2, transform, color_make(100, intensity, 255, 255));
        }

        // Draw circles at each vertex so you can see the points
        for (int i = 0; i < 8; i++) {
            int x, y;
            renderer_project_point(&rend, cube_vertices[i], transform, &x, &y, NULL);
            renderer_draw_filled_circle(&rend, x, y, 5, color_make(255, 200, 100, 255));
        }

        renderer_present(&rend);
        renderer_sleep(16);
    }

    renderer_cleanup(&rend);
    return 0;
}
