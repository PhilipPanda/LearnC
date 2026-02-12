# Color Operations

## Brightness

Adjust overall lightness by adding/subtracting from all channels:

```c
void adjust_brightness(Image* img, int amount) {
    for (int i = 0; i < img->width * img->height * 3; i++) {
        int value = img->pixels[i] + amount;
        img->pixels[i] = (value < 0) ? 0 : (value > 255) ? 255 : value;
    }
}
```

## Contrast

Stretch or compress the range of values:

```c
void adjust_contrast(Image* img, float factor) {
    // factor > 1.0 increases contrast, < 1.0 decreases
    for (int i = 0; i < img->width * img->height * 3; i++) {
        int value = (int)((img->pixels[i] - 128) * factor + 128);
        img->pixels[i] = (value < 0) ? 0 : (value > 255) ? 255 : value;
    }
}
```

## Grayscale Conversion

```c
void grayscale(Image* img) {
    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * 3;
        uint8_t gray = (uint8_t)(0.299f * img->pixels[idx] + 
                                 0.587f * img->pixels[idx+1] + 
                                 0.114f * img->pixels[idx+2]);
        img->pixels[idx] = img->pixels[idx+1] = img->pixels[idx+2] = gray;
    }
}
```

## Sepia Tone

```c
void sepia(Image* img) {
    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * 3;
        uint8_t r = img->pixels[idx];
        uint8_t g = img->pixels[idx+1];
        uint8_t b = img->pixels[idx+2];
        
        int tr = (int)(0.393*r + 0.769*g + 0.189*b);
        int tg = (int)(0.349*r + 0.686*g + 0.168*b);
        int tb = (int)(0.272*r + 0.534*g + 0.131*b);
        
        img->pixels[idx] = (tr > 255) ? 255 : tr;
        img->pixels[idx+1] = (tg > 255) ? 255 : tg;
        img->pixels[idx+2] = (tb > 255) ? 255 : tb;
    }
}
```

## Invert (Negative)

```c
void invert(Image* img) {
    for (int i = 0; i < img->width * img->height * 3; i++) {
        img->pixels[i] = 255 - img->pixels[i];
    }
}
```

## Saturation

```c
void adjust_saturation(Image* img, float factor) {
    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * 3;
        uint8_t r = img->pixels[idx];
        uint8_t g = img->pixels[idx+1];
        uint8_t b = img->pixels[idx+2];
        
        // Convert to HSL, adjust S, convert back (simplified version)
        uint8_t gray = (r + g + b) / 3;
        
        img->pixels[idx] = (uint8_t)(gray + (r - gray) * factor);
        img->pixels[idx+1] = (uint8_t)(gray + (g - gray) * factor);
        img->pixels[idx+2] = (uint8_t)(gray + (b - gray) * factor);
    }
}
```

## Gamma Correction

```c
void gamma_correction(Image* img, float gamma) {
    uint8_t lookup[256];
    for (int i = 0; i < 256; i++) {
        lookup[i] = (uint8_t)(powf(i / 255.0f, gamma) * 255.0f);
    }
    
    for (int i = 0; i < img->width * img->height * 3; i++) {
        img->pixels[i] = lookup[img->pixels[i]];
    }
}
```

## Threshold (Binary)

```c
void threshold(Image* img, uint8_t thresh) {
    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * 3;
        uint8_t gray = (img->pixels[idx] + img->pixels[idx+1] + img->pixels[idx+2]) / 3;
        uint8_t value = (gray >= thresh) ? 255 : 0;
        img->pixels[idx] = img->pixels[idx+1] = img->pixels[idx+2] = value;
    }
}
```

## Color Tint

```c
void tint(Image* img, uint8_t r_tint, uint8_t g_tint, uint8_t b_tint) {
    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * 3;
        img->pixels[idx] = (img->pixels[idx] * r_tint) / 255;
        img->pixels[idx+1] = (img->pixels[idx+1] * g_tint) / 255;
        img->pixels[idx+2] = (img->pixels[idx+2] * b_tint) / 255;
    }
}
```

Color operations form the basis of photo editing - exposure, color grading, filters, and artistic effects!
