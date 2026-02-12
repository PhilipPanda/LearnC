# Image Transformations

## Flip Horizontal

```c
void flip_horizontal(Image* img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width / 2; x++) {
            int left_idx = (y * img->width + x) * 3;
            int right_idx = (y * img->width + (img->width - 1 - x)) * 3;
            
            for (int c = 0; c < 3; c++) {
                uint8_t temp = img->pixels[left_idx + c];
                img->pixels[left_idx + c] = img->pixels[right_idx + c];
                img->pixels[right_idx + c] = temp;
            }
        }
    }
}
```

## Flip Vertical

```c
void flip_vertical(Image* img) {
    for (int y = 0; y < img->height / 2; y++) {
        for (int x = 0; x < img->width; x++) {
            int top_idx = (y * img->width + x) * 3;
            int bottom_idx = ((img->height - 1 - y) * img->width + x) * 3;
            
            for (int c = 0; c < 3; c++) {
                uint8_t temp = img->pixels[top_idx + c];
                img->pixels[top_idx + c] = img->pixels[bottom_idx + c];
                img->pixels[bottom_idx + c] = temp;
            }
        }
    }
}
```

## Rotate 90° Clockwise

```c
Image* rotate_90_cw(Image* img) {
    Image* rotated = malloc(sizeof(Image));
    rotated->width = img->height;
    rotated->height = img->width;
    rotated->pixels = malloc(rotated->width * rotated->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int src_idx = (y * img->width + x) * 3;
            int dst_idx = (x * rotated->width + (img->height - 1 - y)) * 3;
            
            rotated->pixels[dst_idx] = img->pixels[src_idx];
            rotated->pixels[dst_idx+1] = img->pixels[src_idx+1];
            rotated->pixels[dst_idx+2] = img->pixels[src_idx+2];
        }
    }
    
    return rotated;
}
```

## Scale (Nearest Neighbor)

```c
Image* scale_nearest(Image* img, int new_width, int new_height) {
    Image* scaled = malloc(sizeof(Image));
    scaled->width = new_width;
    scaled->height = new_height;
    scaled->pixels = malloc(new_width * new_height * 3);
    
    float x_ratio = (float)img->width / new_width;
    float y_ratio = (float)img->height / new_height;
    
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int src_x = (int)(x * x_ratio);
            int src_y = (int)(y * y_ratio);
            int src_idx = (src_y * img->width + src_x) * 3;
            int dst_idx = (y * new_width + x) * 3;
            
            scaled->pixels[dst_idx] = img->pixels[src_idx];
            scaled->pixels[dst_idx+1] = img->pixels[src_idx+1];
            scaled->pixels[dst_idx+2] = img->pixels[src_idx+2];
        }
    }
    
    return scaled;
}
```

## Scale (Bilinear Interpolation)

```c
Image* scale_bilinear(Image* img, int new_width, int new_height) {
    Image* scaled = malloc(sizeof(Image));
    scaled->width = new_width;
    scaled->height = new_height;
    scaled->pixels = malloc(new_width * new_height * 3);
    
    float x_ratio = (float)(img->width - 1) / new_width;
    float y_ratio = (float)(img->height - 1) / new_height;
    
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            float src_x = x * x_ratio;
            float src_y = y * y_ratio;
            
            int x1 = (int)src_x;
            int y1 = (int)src_y;
            int x2 = (x1 < img->width - 1) ? x1 + 1 : x1;
            int y2 = (y1 < img->height - 1) ? y1 + 1 : y1;
            
            float x_diff = src_x - x1;
            float y_diff = src_y - y1;
            
            for (int c = 0; c < 3; c++) {
                uint8_t p11 = img->pixels[(y1 * img->width + x1) * 3 + c];
                uint8_t p12 = img->pixels[(y2 * img->width + x1) * 3 + c];
                uint8_t p21 = img->pixels[(y1 * img->width + x2) * 3 + c];
                uint8_t p22 = img->pixels[(y2 * img->width + x2) * 3 + c];
                
                float interpolated = p11 * (1-x_diff) * (1-y_diff) +
                                   p21 * x_diff * (1-y_diff) +
                                   p12 * (1-x_diff) * y_diff +
                                   p22 * x_diff * y_diff;
                
                scaled->pixels[(y * new_width + x) * 3 + c] = (uint8_t)interpolated;
            }
        }
    }
    
    return scaled;
}
```

## Crop

```c
Image* crop(Image* img, int x, int y, int width, int height) {
    Image* cropped = malloc(sizeof(Image));
    cropped->width = width;
    cropped->height = height;
    cropped->pixels = malloc(width * height * 3);
    
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int src_idx = ((y + row) * img->width + (x + col)) * 3;
            int dst_idx = (row * width + col) * 3;
            
            cropped->pixels[dst_idx] = img->pixels[src_idx];
            cropped->pixels[dst_idx+1] = img->pixels[src_idx+1];
            cropped->pixels[dst_idx+2] = img->pixels[src_idx+2];
        }
    }
    
    return cropped;
}
```

Geometric transformations are essential for image manipulation, computer vision, and creating image processing pipelines!
