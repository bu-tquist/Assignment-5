#include "loader.h"
#include <stdlib.h>
#include <string.h>

/** Returns p1 with each channel multiplied by scalar. */
struct pixel mul(struct pixel p1, float scalar) {
    return (struct pixel){r: p1.r * scalar, g: p1.g * scalar, b: p1.b * scalar};
}
/** Returns the channel-wise sum of p1 and p2. */
struct pixel add(struct pixel p1, struct pixel p2) {
    return (struct pixel){r: p1.r + p2.r, g: p1.g + p2.g, b: p1.b + p2.b};
}

/**
 * Applies a square kernel to an image (cross-correlation).
 *
 * Produces a new image where each output pixel is the weighted sum of
 * the ksize x ksize neighborhood centered on the corresponding input
 * pixel, multiplied by normalize. The kernel is applied as-is (not
 * flipped), so this is technically cross-correlation; the result is
 * identical to convolution for symmetric kernels.
 *
 * The input img is padded so that kernel operations that fall outside of the 
 * original image are multiplied by a black pixel (zero padding).
 *
 * img        Source image. Not modified.
 * kernel     Kernel weights in row-major order, containing ksize * ksize elements.
 * ksize      Width and height of the kernel. Should be odd
 * normalize  Scale factor applied to each weighted sum
 *                       (e.g., 1.0f / 9 for a 3x3 box blur).
 *
 * Returns a pointer to a newly allocated image with the same dimensions as img.
 *
 */
struct image* apply_kernel(struct image* img, int* kernel, int ksize, float normalize) {
    struct image* NewImg = (struct image*)malloc(sizeof(struct image));

    NewImg->width = img->width;
    NewImg->height = img->height;

    size_t numofPixels = (size_t)img->width * img->height;

    NewImg->pixels = (struct pixel*)malloc(numofPixels * sizeof(struct pixel));

    int half_ksize = ksize / 2;

    for(int y = 0; y < NewImg->height; y++){
        for(int x = 0; x < NewImg->width; x++){
          struct pixel accum = {0, 0, 0};
          

            for(int ky = 0; ky < ksize; ky++){
                for(int kx = 0; kx < ksize; kx++){
                    int x_neighbor = x + (kx - half_ksize);
                    int y_neighbor = y + (ky - half_ksize);

                    if(x_neighbor >= 0 && x_neighbor < img->width &&
                    y_neighbor >= 0 && y_neighbor < img->height){
                       
                        int img_indice =  y_neighbor * img->width + x_neighbor;
                        int kernel_indice = ky * ksize + kx;

                        float weight = (float)kernel[kernel_indice];

                        struct pixel weight_pix = mul(img->pixels[img_indice], weight);
                         accum =  add(accum, weight_pix);
                    }

                    
                }
            }
            int out_indice = y * img->width + x;

            struct pixel normal_pix = mul(accum, normalize);

            int final_r = normal_pix.r;
            int final_g = normal_pix.g;
            int final_b = normal_pix.b;

            NewImg->pixels[out_indice].r = (final_r < 0 ? 0 : (final_r > 255 ? 255 : final_r));
            NewImg->pixels[out_indice].g = (final_g < 0 ? 0 : (final_g > 255 ? 255 : final_g));
            NewImg->pixels[out_indice].b = (final_b < 0 ? 0 : (final_b > 255 ? 255 : final_b));
        }

    }

    


    return NewImg;
}

