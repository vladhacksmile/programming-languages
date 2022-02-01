#include "bmp.h"
#include "rotator.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if(argc < 3) {
        fprintf(stderr, "Укажите все аргументы! Первым - путь к исходной картинке, второй - путь, куда будет сохранена повернутая картинка!\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "rb");

    if(in == NULL) {
        fprintf(stderr, "Не удалось открыть файл %s для чтения!\n", argv[1]);
        return 2;
    }

    struct image* img = malloc(sizeof(struct image));

    if(img == NULL) {
        fprintf(stderr, "Не удалось выделить достаточное количество памяти для картинки!\n");
        fclose(in);
        return 3;
    }

    img -> data = NULL;

    enum read_status read_status = from_bmp(in, img);

    fclose(in);

    if(read_status != READ_OK) {
        switch (read_status) {
            case READ_INVALID_SIGNATURE:
                fprintf(stderr, "Не удалось прочитать сигнатуру!");
                break;
            case READ_INVALID_BITS:
                fprintf(stderr, "Не удалось прочитать биты! Допустимое значение для BMP - %d!", biBitCountDefault);
                break;
            case READ_INVALID_HEADER:
                fprintf(stderr, "Не удалось прочитать заголовки!");
                break;
            case READ_OUT_OF_MEMORY:
                fprintf(stderr, "Не удалось аллоцировать память при чтении!");
                break;
            default:
                fprintf(stderr, "Неизвестная ошибка при чтении!");
                break;
        }
        return 4;
    }

    struct image* rotated_img = rotate(img);

    free(img -> data);
    free(img);
    img = NULL;

    if(rotated_img == NULL) {
        fprintf(stderr, "Не удалось повернуть картинку!\n");
        return 5;
    }

    FILE *out = fopen(argv[2], "wb");

    if(out == NULL) {
        fprintf(stderr, "Не удалось создать файл для записи!\n");
        free(rotated_img);
        return 6;
    }

    enum write_status write_status = to_bmp(out, rotated_img);

    fclose(out);

    free(rotated_img -> data);
    free(rotated_img);

    if(write_status != WRITE_OK) {
        switch(write_status) {
            case WRITE_ERROR:
                fprintf(stderr, "Не удалось преобразовать картинку в BMP!\n");
                break;
            case WRITE_OUT_OF_MEMORY_ERROR:
                fprintf(stderr, "Не удалось аллоцировать память при записи!\n");
                break;
            default:
                fprintf(stderr, "Неизвестная ошибка при записи!");
                break;
        }
        return 7;
    }

    fprintf(stdout, "Успешно! Картинка повернута и сохранена в файле %s!\n", argv[2]);

    return 0;
}
