//
// Created by Владислав Харин on 21.12.2021.
//

#include "image.h"
#include "rotator.h"
#include <stdio.h>

/* Создает копию изображения, которая повёрнута на 90 градусов */
struct image* rotate(struct image* source) {
    if(source == NULL) return NULL;
    
    uint64_t sourceWidth = source -> width;
    uint64_t sourceHeight = source -> height;

    struct image* result = create_image(sourceHeight, sourceWidth);

    if(result == NULL) return NULL;

    for (size_t i = 0; i < sourceHeight; i++) {
        for (size_t j = 0; j < sourceWidth; j++) {
            struct pixel pixel = getPixel(source, i, j);
            setPixel(result, pixel, j, sourceHeight - i - 1);
    }
    }

    return result;
}
