//
// Created by Владислав Харин on 19.12.2021.
//

#include "bmp.h"
#include <stdlib.h>

const char empty[4] = { 0 };

static uint8_t calculatePadding(struct image img) {
    return img.width % 4;
}

static uint8_t validateHeader(struct bmp_header bmpHeader) {
    return bmpHeader.bfType == bfTypeDefault || bmpHeader.biBitCount == biBitCountDefault;
}

struct bmp_header createDefaultBmpHeader(uint64_t width, uint64_t height) {
    struct bmp_header header = {0};
    header.bfType = bfTypeDefault;
    header.bfileSize = sizeof(struct bmp_header) + (width * height * sizeof(struct pixel));
    header.bfReserved = 0;
    header.bOffBits = sizeof(struct bmp_header);
    header.biSize = 40;
    header.biWidth = width;
    header.biHeight = height;
    header.biPlanes = 1;
    header.biBitCount = biBitCountDefault;
    header.biCompression = 0;
    header.biSizeImage = header.bfileSize - header.bOffBits;
    header.biXPelsPerMeter = 0;
    header.biYPelsPerMeter = 0;
    header.biClrUsed = 0;
    header.biClrImportant = 0;
    return header;
}

enum read_status from_bmp(FILE *in, struct image* img) {
    if(in == NULL || img == NULL) return READ_ERROR;

    struct bmp_header bmpHeader;

    if (fread(&bmpHeader, sizeof(struct bmp_header), 1, in) != 1)
        return READ_INVALID_SIGNATURE;

    if(validateHeader(bmpHeader) != 1)
        return READ_INVALID_HEADER;

    uint64_t width = img -> width = bmpHeader.biWidth;
    uint64_t height = img -> height = bmpHeader.biHeight;

    struct pixel* data = malloc(width * height * sizeof(struct pixel));

    if(data == NULL) return READ_OUT_OF_MEMORY;

    uint8_t padding = calculatePadding(*img);
    for(size_t i = 0; i < height; i++) {
        if(fread(data + i * width, width * sizeof(struct pixel), 1, in) != 1) {
            free(data);
            return READ_INVALID_BITS;
        }
        if(padding != 0) {
            if(fseek(in, padding, SEEK_CUR) != 0) {
                free(data);
                return READ_ERROR;
            }
        }
    }

    if(img -> data != NULL) {
        free(img -> data);
    }

    img -> data = data;

    return READ_OK;
}

enum write_status to_bmp(FILE* out, struct image const* img) {
    if(out == NULL || img == NULL) return WRITE_ERROR;

    uint64_t width = img -> width;
    uint64_t height = img -> height;

    uint8_t padding = calculatePadding(*img);

    struct bmp_header header = createDefaultBmpHeader(width, height);

    if(fwrite(&header, sizeof(struct bmp_header), 1, out) != 1)
        return WRITE_ERROR;

    for(size_t i = 0; i < height; i++) {
        if(fwrite(img -> data + i * width, sizeof(struct pixel), width, out) != width) {
            return WRITE_ERROR;
        }
        if(padding != 0) {
            if(fwrite(&empty, sizeof(char), padding, out) != padding) {
                return WRITE_ERROR;
            }
        }
    }

    return WRITE_OK;
}
