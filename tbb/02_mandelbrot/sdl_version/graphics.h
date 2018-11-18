/*
 * Alessandra Fais
 * SPD course - MCSN - University of Pisa
 * A.A. 2017/18
 */

#ifndef EX02_GRAPHICS_H
#define EX02_GRAPHICS_H


#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

#include "SDL2/SDL.h"

class SDL_Mandelbrot {
public:
    SDL_Mandelbrot(int mi) : max_iter(mi) {}

    /*
     * Sequential computation of the Mandelbrot set.
     */
    int draw() {
        SDL_Init(SDL_INIT_EVERYTHING);

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Event event;

        int screen_w = 1440;
        int screen_h = 900;

        SDL_CreateWindowAndRenderer(screen_w, screen_h, 0, &window, &renderer);
        SDL_RenderSetLogicalSize(renderer, width, height);

        int count = 0;
        bool go = true;
        while (go) {

            min += 0.15 * scale_factor;
            max -= 0.1 * scale_factor;
            scale_factor *= 0.9349;
            max_iter += 5;

            if (count > 30) {
                max_iter *= 1.02;
            }

            SDL_RenderPresent(renderer);

            // Computation of the single pixel of the matrix.
            for (int x = 0; x < width; ++x) {

                // Key event handler.
                if (SDL_PollEvent(&event) &&
                    (event.type == SDL_QUIT ||          // Exit when quit button is pressed or when q is pressed.
                     (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q))) {
                    return 0;
                }

                for (int y = 0; y < height; ++y) {

                    // Map (x, y) values into window coordinates.
                    long double a = screen_coord_map(x, 0, width, min, max);
                    long double b = screen_coord_map(y, 0, height, min, max);

                    int color = comp_pixel(a, b, max_iter);

                    draw_pixel(color, x, y, renderer);

                }
            }

            // Number of iterations.
            count++;
        }

        return 0;
    }


private:
    const int width = 800;
    const int height = 800;

    long double min = -2.84;
    long double max = 1.0;
    long double scale_factor = 1.0;

    int max_iter;

    // ---------------------------------------- COMPUTE ----------------------------------------------

    int comp_pixel(long double a, long double b, int max_iter) {

        int count = 0;              // Number of iterations (determines the color of the pixel).

        long double c_real = a;     // Coordinates of the point in the complex plane.
        long double c_imag = b;

        // Iterate until max_iter number if iterations is reached or the computation diverges.
        for (int i = 0; (i < max_iter) && ((a + b) < 2); ++i) {

            /*
             * Mandelbrot formula
             *
             * z(0) = 0
             * c = a + bi (initial point)
             *
             * ITERATION 1:
             * z(1) = 0 + c = a + bi
             *
             * z(1).real = a
             * z(1).imag = b
             *
             * ITERATION n > 1:
             * z^2 = (a + bi)^2 = a^2 - b^2 + 2ab
             *
             * z(n).real = a^2 - b^2 + c.real = a^2 - b^2 + a   (new value of a)
             * z(n).img = 2ab + c.imag = 2ab + b                (new value of b)
             *
             */

            long double temp_a = a * a - b * b;
            long double temp_b = 2 * a * b;
            a = temp_a + c_real;
            b = temp_b + c_imag;

            count++;
        }

        return count;
    }

    // ------------------------------------------ DRAW -----------------------------------------------

    long double screen_coord_map(long double x,
                                 long double in_min, long double in_max,
                                 long double out_min, long double out_max) {

        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

    }

    /*
     * The number of iterations determines the color of the pixel, until infinity is reached
     * (until the length of the vector z becomes greater than 2, i.e. its square value becomes greater than 4)
     */
    void draw_pixel(int color, int x, int y, SDL_Renderer *renderer) {
        int bright = screen_coord_map(color, 0, max_iter, 0, 255); // Pixel brightness.

        if (color == max_iter || bright < 20) {
            bright = 0;
        }

        int red = screen_coord_map(bright * bright, 0, 6502, 0, 255); // 255 * 255
        int green = bright;
        int blue = screen_coord_map(std::sqrt(bright), 0, std::sqrt(255), 0, 255);

        SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
        SDL_RenderDrawPoint(renderer, x, y);
    }
};

#endif //EX02_GRAPHICS_H
