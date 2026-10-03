#include "kernel.h"
#include <string.h>

int generate_pagefault() {

    struct image* img = (struct image*)malloc(sizeof(struct image));

    img->width = 64;
    img->height = 64;

    size_t numofPixels = (size_t)img->width * img->height;

    img->pixels = (struct pixel*)malloc(numofPixels * sizeof(struct pixel));

    for(int i = 0; i< numofPixels; i++){
        img->pixels[i].r = 0;
        img->pixels[i].g = 0;
        img->pixels[i].b = 0;
    }


    char* temp_file = "fault_file.bin";

    saveimage_mmap(temp_file, img);

    free(img->pixels);
    img->pixels = NULL;

    loadimage_mmap(temp_file, img);

    volatile struct pixel fault_trigger =  img->pixels[numofPixels - 1];

    (void)fault_trigger;

    

    free(img->pixels);

    free(img);

    return 0;
}

int main(int argc, char** argv){
    // TODO: parse the arguments in argv. 
    // You can expect argv[1] to be the mode
    // You can expect argv[2] to be the filepath
    // You can expect argv[3] to be the integer width
    // You can expect argv[4] to be the integer height
    // You can expect argv[5] to be the output filepath.

    if(argc != 6) {
        printf("Incorrect number of arguments. Expected: ./cli <MODE=kernel|mmap|convert|uconvert|fault> <input_image> <width> <height> <output_image_path>\n");
        return -1;
    }

    char* fileName = argv[2];

    char* output = argv[5];

    int width = atoi(argv[3]);
    int height = atoi(argv[4]);

    // TODO: call correct function based on mode

    char* mode =  argv[1];

    struct image* img = (struct image*)malloc(sizeof(struct image));

    img->width = width;
    img->height = height;

    int kernel[3][3] = {{1,1,1},{1,1,1},{1,1,1}};

    if(strcmp(mode, "mmap")== 0){
        loadimage_mmap(fileName, img);

        struct image* kernelImage = apply_kernel(img, (int*)kernel, 3, 1.0f / 9.0f);

        saveimage_mmap(output, kernelImage);

        free(kernelImage->pixels);
        free(kernelImage);

    }
    else if(strcmp(mode, "convert")== 0){

        loadimage(fileName, img);
        saveimage_mmap(output, img);

    }
    else if(strcmp(mode, "unconvert")== 0){

        loadimage_mmap(fileName, img);

        saveimage(output, img);
    }
    else if(strcmp(mode, "fault")== 0){
        generate_pagefault();
    }
    else{
        // TODO: allocate the space needed for one image and load the image
    

        loadimage(fileName, img);


        // TODO: call apply kernel with 1/9 (as a float) as the normalization value

        struct image* kernelImage = apply_kernel(img, (int*)kernel, 3, 1.0f / 9.0f);

        saveimage(output, kernelImage);

        free(kernelImage->pixels);
        free(kernelImage);

    }

    free(img->pixels);
    free(img);

    return 0;
    
}
