//
// Created by Владислав Харин on 18.12.2021.
//

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdio.h>

struct pixel { uint8_t b, g, r; } __attribute__((packed));

struct image {
    uint64_t width, height;
    struct pixel* data;
};

struct pixel getPixel(struct image const* source, size_t i, size_t j);
void setPixel(struct image const* source, struct pixel const pixel, size_t i, size_t j);

struct image* create_image(uint64_t sourceWidth, uint64_t sourceHeight);

#endif
