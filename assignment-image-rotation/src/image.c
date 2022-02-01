//
// Created by Владислав Харин on 18.12.2021.
//

#include "image.h"
#include <stdlib.h>

struct pixel getPixel(struct image const* source, size_t i, size_t j) {
    return *(source -> data + i * source -> width + j);
}

void setPixel(struct image const* source, struct pixel const pixel, size_t i, size_t j) {
    *(source -> data + i * source -> width + j) = pixel;
}

struct image* create_image(uint64_t sourceWidth, uint64_t sourceHeight) {
    struct image* result = malloc(sizeof(struct image));

    if(result == NULL) return NULL;

    result -> data = malloc(sourceWidth * sourceHeight * sizeof(struct pixel));

    // Если не смогли аллоцировать память под пиксели, смысла возвращать структуру нет
    if(result -> data == NULL) {
        free(result);
        return NULL;
    }

    result -> width = sourceWidth;
    result -> height = sourceHeight;

    return result;
}
