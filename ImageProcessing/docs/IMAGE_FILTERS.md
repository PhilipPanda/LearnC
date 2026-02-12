# Image Filters

## The Big Picture

Image filters transform images by applying mathematical operations to pixels. Most filters use **convolution** - combining a pixel with its neighbors using a kernel (small matrix).

## Convolution

Convolution slides a kernel (filter matrix) over the image, multiplying overlapping values and summing them.

```
Image:              Kernel (3×3):        Result:
┌───┬───┬───┐      ┌───┬───┬───┐
│ 1 │ 2 │ 3 │      │ k0│ k1│ k2│
├───┼───┼───┤  ×   ├───┼───┼───┤   =   new pixel value
│ 4 │ 5 │ 6 │      │ k3│ k4│ k5│
├───┼───┼───┤      ├───┼───┼───┤
│ 7 │ 8 │ 9 │      │ k6│ k7│ k8│
└───┴───┴───┘      └───┴───┴───┘

Result = 1*k0 + 2*k1 + 3*k2 + 
         4*k3 + 5*k4 + 6*k5 + 
         7*k6 + 8*k7 + 9*k8
```

### Convolution Implementation

```c
typedef struct {
    float values[9];  // 3×3 kernel
} Kernel3x3;

uint8_t apply_kernel_channel(Image* img, int x, int y, int channel, Kernel3x3* kernel) {
    float sum = 0.0f;
    int idx = 0;
    
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            int px = x + kx;
            int py = y + ky;
            
            // Handle borders (clamp to edge)
            if (px < 0) px = 0;
            if (px >= img->width) px = img->width - 1;
            if (py < 0) py = 0;
            if (py >= img->height) py = img->height - 1;
            
            int pixel_idx = (py * img->width + px) * 3 + channel;
            sum += img->pixels[pixel_idx] * kernel->values[idx++];
        }
    }
    
    // Clamp result to [0, 255]
    if (sum < 0) sum = 0;
    if (sum > 255) sum = 255;
    return (uint8_t)sum;
}
```

## Common Kernels

### Identity (No Change)

```c
Kernel3x3 identity = {{
    0, 0, 0,
    0, 1, 0,
    0, 0, 0
}};
```

### Box Blur (Average)

```c
Kernel3x3 box_blur = {{
    1/9.0f, 1/9.0f, 1/9.0f,
    1/9.0f, 1/9.0f, 1/9.0f,
    1/9.0f, 1/9.0f, 1/9.0f
}};
```

### Gaussian Blur (Smooth)

```c
Kernel3x3 gaussian_blur = {{
    1/16.0f, 2/16.0f, 1/16.0f,
    2/16.0f, 4/16.0f, 2/16.0f,
    1/16.0f, 2/16.0f, 1/16.0f
}};
```

### Sharpen

```c
Kernel3x3 sharpen = {{
     0, -1,  0,
    -1,  5, -1,
     0, -1,  0
}};
```

### Edge Detection (Sobel X)

```c
Kernel3x3 sobel_x = {{
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
}};
```

### Edge Detection (Sobel Y)

```c
Kernel3x3 sobel_y = {{
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1
}};
```

### Emboss

```c
Kernel3x3 emboss = {{
    -2, -1, 0,
    -1,  1, 1,
     0,  1, 2
}};
```

## Filter Functions

### Blur Filter

```c
void blur(Image* img) {
    Kernel3x3 kernel = {{
        1/9.0f, 1/9.0f, 1/9.0f,
        1/9.0f, 1/9.0f, 1/9.0f,
        1/9.0f, 1/9.0f, 1/9.0f
    }};
    
    uint8_t* output = malloc(img->width * img->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int idx = (y * img->width + x) * 3;
            output[idx + 0] = apply_kernel_channel(img, x, y, 0, &kernel);  // R
            output[idx + 1] = apply_kernel_channel(img, x, y, 1, &kernel);  // G
            output[idx + 2] = apply_kernel_channel(img, x, y, 2, &kernel);  // B
        }
    }
    
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}
```

### Sharpen Filter

```c
void sharpen(Image* img) {
    Kernel3x3 kernel = {{
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0
    }};
    
    uint8_t* output = malloc(img->width * img->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int idx = (y * img->width + x) * 3;
            output[idx + 0] = apply_kernel_channel(img, x, y, 0, &kernel);
            output[idx + 1] = apply_kernel_channel(img, x, y, 1, &kernel);
            output[idx + 2] = apply_kernel_channel(img, x, y, 2, &kernel);
        }
    }
    
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}
```

### Edge Detection

```c
void edge_detect(Image* img) {
    Kernel3x3 sobel_x = {{
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    }};
    
    Kernel3x3 sobel_y = {{
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    }};
    
    uint8_t* output = malloc(img->width * img->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            // Apply both Sobel operators
            float gx_r = apply_kernel_channel(img, x, y, 0, &sobel_x);
            float gy_r = apply_kernel_channel(img, x, y, 0, &sobel_y);
            float magnitude = sqrtf(gx_r*gx_r + gy_r*gy_r);
            
            if (magnitude > 255) magnitude = 255;
            
            int idx = (y * img->width + x) * 3;
            output[idx + 0] = (uint8_t)magnitude;
            output[idx + 1] = (uint8_t)magnitude;
            output[idx + 2] = (uint8_t)magnitude;
        }
    }
    
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}
```

## Non-Convolution Filters

### Median Filter (Noise Reduction)

```c
int compare_uint8(const void* a, const void* b) {
    return (*(uint8_t*)a - *(uint8_t*)b);
}

void median_filter(Image* img) {
    uint8_t* output = malloc(img->width * img->height * 3);
    uint8_t neighbors[9];
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            for (int c = 0; c < 3; c++) {  // For each color channel
                // Collect neighbors
                int idx = 0;
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int px = x + kx;
                        int py = y + ky;
                        if (px >= 0 && px < img->width && py >= 0 && py < img->height) {
                            neighbors[idx++] = img->pixels[(py*img->width + px)*3 + c];
                        }
                    }
                }
                
                // Sort and take median
                qsort(neighbors, idx, sizeof(uint8_t), compare_uint8);
                output[(y*img->width + x)*3 + c] = neighbors[idx/2];
            }
        }
    }
    
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}
```

### Bilateral Filter (Edge-Preserving Blur)

```c
float gaussian(float x, float sigma) {
    return expf(-(x*x) / (2*sigma*sigma));
}

void bilateral_filter(Image* img, float sigma_spatial, float sigma_range) {
    uint8_t* output = malloc(img->width * img->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            float sum_r = 0, sum_g = 0, sum_b = 0;
            float weight_sum = 0;
            
            int center_idx = (y * img->width + x) * 3;
            
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    
                    if (px >= 0 && px < img->width && py >= 0 && py < img->height) {
                        int idx = (py * img->width + px) * 3;
                        
                        // Spatial distance
                        float spatial_dist = sqrtf(kx*kx + ky*ky);
                        float spatial_weight = gaussian(spatial_dist, sigma_spatial);
                        
                        // Color distance
                        float dr = img->pixels[idx+0] - img->pixels[center_idx+0];
                        float dg = img->pixels[idx+1] - img->pixels[center_idx+1];
                        float db = img->pixels[idx+2] - img->pixels[center_idx+2];
                        float range_dist = sqrtf(dr*dr + dg*dg + db*db);
                        float range_weight = gaussian(range_dist, sigma_range);
                        
                        float weight = spatial_weight * range_weight;
                        
                        sum_r += img->pixels[idx+0] * weight;
                        sum_g += img->pixels[idx+1] * weight;
                        sum_b += img->pixels[idx+2] * weight;
                        weight_sum += weight;
                    }
                }
            }
            
            output[center_idx+0] = (uint8_t)(sum_r / weight_sum);
            output[center_idx+1] = (uint8_t)(sum_g / weight_sum);
            output[center_idx+2] = (uint8_t)(sum_b / weight_sum);
        }
    }
    
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}
```

## Advanced Edge Detection

### Canny Edge Detection (Simplified)

```c
void canny_edges(Image* img, float low_threshold, float high_threshold) {
    // 1. Gaussian blur
    blur(img);
    
    // 2. Sobel gradient
    edge_detect(img);
    
    // 3. Non-maximum suppression (simplified - just threshold)
    for (int i = 0; i < img->width * img->height * 3; i++) {
        if (img->pixels[i] < low_threshold) {
            img->pixels[i] = 0;  // Weak edge - remove
        } else if (img->pixels[i] < high_threshold) {
            img->pixels[i] = 128;  // Medium edge
        } else {
            img->pixels[i] = 255;  // Strong edge
        }
    }
}
```

## Motion Blur

```c
void motion_blur_horizontal(Image* img, int length) {
    uint8_t* output = malloc(img->width * img->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            float sum_r = 0, sum_g = 0, sum_b = 0;
            int count = 0;
            
            // Average pixels in horizontal line
            for (int dx = -length/2; dx <= length/2; dx++) {
                int px = x + dx;
                if (px >= 0 && px < img->width) {
                    int idx = (y * img->width + px) * 3;
                    sum_r += img->pixels[idx+0];
                    sum_g += img->pixels[idx+1];
                    sum_b += img->pixels[idx+2];
                    count++;
                }
            }
            
            int out_idx = (y * img->width + x) * 3;
            output[out_idx+0] = (uint8_t)(sum_r / count);
            output[out_idx+1] = (uint8_t)(sum_g / count);
            output[out_idx+2] = (uint8_t)(sum_b / count);
        }
    }
    
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}
```

## Unsharp Mask (Advanced Sharpening)

```c
void unsharp_mask(Image* img, float amount) {
    // Save original
    uint8_t* original = malloc(img->width * img->height * 3);
    memcpy(original, img->pixels, img->width * img->height * 3);
    
    // Blur image
    blur(img);
    
    // Subtract blurred from original and add back
    for (int i = 0; i < img->width * img->height * 3; i++) {
        int sharpened = original[i] + amount * (original[i] - img->pixels[i]);
        if (sharpened < 0) sharpened = 0;
        if (sharpened > 255) sharpened = 255;
        img->pixels[i] = (uint8_t)sharpened;
    }
    
    free(original);
}
```

## Summary

| Filter | Effect | Kernel Type |
|--------|--------|-------------|
| Blur | Smoothing, reduce detail | Averaging |
| Sharpen | Enhance edges, increase detail | Edge enhancement |
| Edge Detect | Find boundaries | Gradient |
| Emboss | 3D effect | Directional gradient |
| Median | Remove noise | Non-linear |
| Bilateral | Smooth while preserving edges | Range + spatial |

**Key Concepts:**
- Convolution = kernel sliding over image
- Most filters use 3×3 or 5×5 kernels
- Always create output buffer (don't modify in-place during loop)
- Clamp results to [0, 255]
- Handle borders (clamp, wrap, or mirror)

Image filters are the foundation of photo editing, computer vision preprocessing, and artistic effects!
