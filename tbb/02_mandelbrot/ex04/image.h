#include <stdlib.h>
#include <stdio.h>

/*
 * C routine to save a 2d int array as an image to a simple graphic format
 * (edited form of code from the rosetta code project https://rosettacode.org/wiki/Bitmap/Write_a_PPM_file)
 */
int save_img(int width, int height, const char* filename, int* matrix, int max_value)
{
    // Filename must end with .ppm .
    int i, j;
    FILE* fp = fopen(filename, "wb"); /* b - binary mode */

    (void) fprintf(fp, "P6\n%d %d\n255\n", width, height);

    for (j = 0; j < height; ++j) {
        for (i = 0; i < width; ++i) {

            // Scaling function.
            int val = matrix[i + j * height];
            if (max_value != 255) {
                val = (val * 255) / max_value; // scale to 0 - 255
            }

            static unsigned char color[3];
            color[0] = val % 256;  /* red */
            color[1] = val % 256;  /* green */
            color[2] = val % 256;  /* blue */
            (void) fwrite(color, 1, 3, fp);
        }
    }
    (void) fclose(fp);
    return 0;
}

