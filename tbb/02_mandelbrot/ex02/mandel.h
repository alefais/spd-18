/*
 * Alessandra Fais
 * SPD course - MCSN - University of Pisa
 * A.A. 2017/18
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

using namespace tbb;
using namespace std;

class MandelBrot {
public:
    MandelBrot(const size_t& w, const size_t& h, const size_t& par_deg, const int& mi) :
                nw(par_deg), matrix(w, h), image(w, h), max_iter(mi),
                min(-2.0), max(2.0) { }

    void compute() {
        if (nw == 0) {  // Sequential implementation.

            for (size_t y = 0; y < matrix.n_rows(); ++y) {      // y = b
                for (size_t x = 0; x < matrix.n_cols(); ++x) {  // x = a

                    // Map (x, y) values into window coordinates.
                    long double a = map(x, 0, matrix.n_cols(), min, max);
                    long double b = map(y, 0, matrix.n_rows(), min, max);

                    int iter = comp_pixel(a, b);   // Number of iterations required for (x, y).
                    matrix[x + y * matrix.n_cols()] = iter;

                    draw_pixel(iter, x, y);
                }
            }
        } else {        // Parallel TBB implementation.

            tbb::parallel_for(
                    tbb::blocked_range<size_t >(0, matrix.n_rows()),
                    [&](const tbb::blocked_range<size_t>& r) {

                        for(size_t y = r.begin(); y != r.end(); ++y) {      // y = b
                            for (size_t x = 0; y < matrix.n_cols(); ++x) {  // x = a

                                // Map (x, y) values into window coordinates.
                                long double a = map(x, 0, matrix.n_cols(), min, max);
                                long double b = map(y, 0, matrix.n_rows(), min, max);

                                int iter = comp_pixel(a, b);   // Number of iterations required for (x, y).
                                matrix[x + y * matrix.n_cols()] = iter;

                                draw_pixel(iter, x, y);
                            }

                        }
                    }
            );
        }
        string filename("mandel.ppm");
        save_img(filename);
    }

    /*
     * Print the matrix as 2D vector.
     */
    void print_matrix() {
        std::stringstream ss;
        for (size_t y = 0; y < matrix.n_rows(); ++y) {
            ss << "| ";
            for (size_t x = 0; x < matrix.n_cols(); ++x) {
                ss << matrix[x + y * matrix.n_cols()] << " ";
            }
            ss << "|\n";
        }
        cout << "Matrix:\n" << ss.str();
    }

    /*
     * Print the image as 2D vector.
     */
    void print_image() {
        std::stringstream ss;
        for (size_t y = 0; y < matrix.n_rows(); ++y) {
            ss << "| ";
            for (size_t x = 0; x < matrix.n_cols(); ++x) {
                image[x + y * matrix.n_cols()].operator<<(ss);
                ss << " ";
            }
            ss << "|\n";
        }
        cout << "Image:\n" << ss.str();
    }

private:

    // --------------------------------------- Subclasses ---------------------------------------

    /*
     * Matrix defined as array of doubles.
     */
    class Matrix {
    public:

        Matrix(const size_t& _w, const size_t& _h) : w(_w), h(_h) {

            m = new long double[w * h];
            for (int i = 0; i < w * h; ++i)
                m[i] = 0;
        }

        ~Matrix() {
            if (m != nullptr)
                delete [] m;
        }

        size_t n_rows() {
            return h;
        }

        size_t n_cols() {
            return w;
        }

        const long double& operator [] (const size_t& i) const {
            return m[i];
        }

        long double& operator [] (const size_t& i) {
            return m[i];
        }

    private:
        long double* m;
        size_t w, h;
    };

    /*
     * Rgb structure, i.e. definition of a pixel.
     */
    struct Rgb {
        Rgb() : r(0), g(0), b(0) {}
        Rgb(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}

        Rgb& operator = (const Rgb& rgb) {
            r = rgb.r;
            g = rgb.g;
            b = rgb.b;
            return *this;
        }

        ostream& operator << (ostream& os) {
            return os << "(" << r << " " << g << " " << b << ")";
        }

        float r, g, b;
    };

    /*
     * Image defined as array of pixels.
     */
    class Image {
    public:

        Image() : w(0), h(0), pixels(nullptr) { /* empty image */ }

        Image(const size_t& _w, const size_t& _h, const Rgb& c = Rgb(0, 0, 0)) :
                w(_w), h(_h), pixels(nullptr) {

            pixels = new Rgb[w * h];
            for (int i = 0; i < w * h; ++i)
                pixels[i] = c;
        }

        size_t n_rows() {
            return h;
        }

        size_t n_cols() {
            return w;
        }

        const Rgb& operator [] (const size_t& i) const {
            return pixels[i];
        }

        Rgb& operator [] (const size_t& i) {
            return pixels[i];
        }

        ~Image() {
            if (pixels != nullptr)
                delete [] pixels;
        }

    private:
        Rgb* pixels; // 1D array of pixels.
        size_t w, h; // Image resolution.
    };

    // --------------------------------------- Variables ----------------------------------------

    Matrix matrix;
    Image image;
    size_t nw;

    int max_iter;
    long double min;
    long double max;

    // ------------------------------------ Auxiliary Methods -----------------------------------

    /*
     * Computation of a single pixel.
     */
    int comp_pixel(long double a, long double b) {

        int count = 0;              // Number of iterations (determines the color of the pixel).

        long double c_real = a;     // Coordinates of the point in the complex plane.
        long double c_imag = b;

        // Iterate until max_iter number if iterations is reached or the computation diverges.
        for (int i = 0; i < max_iter; ++i) {

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

            if (a * a + b * b > 4) break;
            count++;
        }

        return count;
    }

    long double map(long double value,
            long double in_min, long double in_max, long double out_min, long double out_max) {

        return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

    }

    /*
     * Computation of the color of the single pixel.
     */
    void draw_pixel(long double color, long double x, long double y) {
        float bright = map(color, 0, max_iter, 0, 255); // Pixel brightness.

        if (color == max_iter /*|| bright < 20*/) {
            bright = 0;
        }

        float red = map(bright * bright, 0, 6502, 0, 255); // 6502 violet color, 255 * 255 blue color
        float green = bright;
        float blue = map(sqrt(bright), 0, sqrt(255), 0, 255);

        image[x + y * matrix.n_cols()] = Rgb(red, green, blue);
    }

    /*
     * Save the plot of the Mandelbrot set into a .ppm file.
     */
    void save_img(const string& filename) {
        if (image.n_rows() == 0 || image.n_cols() == 0) {
            cerr << "Can't save an empty image\n";
            return;
        }

        ofstream ofs;
        try {
            ofs.open(filename.c_str(), ios::binary);
            if (ofs.fail())
                throw("Can't open output file");

            ofs << "P6\n" << image.n_cols() << " " << image.n_rows() << "\n255\n";

            // Loop over each pixel in the image and convert to byte format.
            unsigned char r, g, b;
            for (int i = 0; i < image.n_cols() * image.n_rows(); ++i) {
                r = static_cast<unsigned char>(image[i].r);
                g = static_cast<unsigned char>(image[i].g);
                b = static_cast<unsigned char>(image[i].b);
                ofs << r << g << b;
            }
            ofs.close();
        }
        catch (const char* err) {
            cerr << err << endl;
            ofs.close();
        }
    }
};