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

        renderer_clear(&rend, color_make(10, 12, 18, 255));
        
        renderer_draw_text(&rend, "3D Wireframe Cube", 10, 10, color_make(255, 255, 255, 255), 2);
        renderer_draw_text(&rend, "Notice how edges farther away are darker", 10, 30, color_make(150, 150, 150, 255), 1);

        // Rotate cube slowly - simple rotation looks more 3D
        Mat4 model = mat4_identity();
        model = mat4_mul(model, mat4_rotate_y(time * 0.8f));
        model = mat4_mul(model, mat4_rotate_x(time * 0.5f));
        model = mat4_mul(model, mat4_translate(0, 0, -3.5f));  // Closer for better depth

        Mat4 view = mat4_look_at(
            vec3_make(0, 0, 0),
            vec3_make(0, 0, -1),
            vec3_make(0, 1, 0)
        );

        Mat4 proj = mat4_perspective(M_PI / 3.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        Mat4 transform = mat4_mul(proj, mat4_mul(view, model));

        // Draw edges with depth-based coloring for better 3D effect
        for (int i = 0; i < 12; i++) {
            Vec3 p1 = cube_vertices[cube_edges[i][0]];
            Vec3 p2 = cube_vertices[cube_edges[i][1]];
            
            // Project to get depth
            int x1, y1, x2, y2;
            float depth1, depth2;
            renderer_project_point(&rend, p1, transform, &x1, &y1, &depth1);
            renderer_project_point(&rend, p2, transform, &x2, &y2, &depth2);
            
            // Color based on depth - closer edges are brighter
            float avg_depth = (depth1 + depth2) * 0.5f;
            float brightness = 1.0f - (avg_depth - 2.0f) / 3.0f;  // Map depth to 0-1
            if (brightness < 0.3f) brightness = 0.3f;
            if (brightness > 1.0f) brightness = 1.0f;
            
            uint8_t b = (uint8_t)(brightness * 255);
            renderer_draw_line_3d(&rend, p1, p2, transform, color_make(b/2, b/2 + 50, b, 255));
        }

        // Draw vertex dots with depth-based size
        for (int i = 0; i < 8; i++) {
            int x, y;
            float depth;
            renderer_project_point(&rend, cube_vertices[i], transform, &x, &y, &depth);
            
            // Closer vertices are bigger
            float size = 8.0f - depth * 0.8f;
            if (size < 3.0f) size = 3.0f;
            
            float brightness = 1.0f - (depth - 2.0f) / 3.0f;
            if (brightness < 0.5f) brightness = 0.5f;
            uint8_t b = (uint8_t)(brightness * 255);
            
            renderer_draw_filled_circle(&rend, x, y, (int)size, color_make(255, b, b/2, 255));
        }

        renderer_present(&rend);
        renderer_sleep(16);
    }

    renderer_cleanup(&rend);
    return 0;
}
