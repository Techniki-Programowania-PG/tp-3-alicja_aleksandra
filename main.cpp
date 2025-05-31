#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <matplot/matplot.h>
#include <vector>
#include <complex>
#include <cmath>
using namespace std;

#define PI 3.14

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

void plot_signal(vector<double> values) {
    using namespace matplot;

    vector<int> x;
    for(int i = 0; i < values.size(); i++) {
        x.push_back(i+1);
    }

    double min = values[0];
    double max = values[0];
    for(int i = 1; i < values.size(); i++) {
        if(values[i] < min) {
            min = values[i];
        } else if(values[i] > max) {
            max = values[i];
        }
    }

    plot(x, values);
    yrange({min - 0.5, max + 0.5});
    show();
}

vector<double> generate_sin_signal(int samples, double freq) {
    vector<double> result;

    for(int i = 0; i < samples; i++) {
        double t = i / (double)samples;
        double value = sin(2 * PI * t * freq);
        result.push_back(value);
    }

    return result;
}

vector<double> generate_cos_signal(int samples, double freq) {
    vector<double> result;

    for(int i = 0; i < samples; i++) {
        double t = i / (double)samples;
        double value = cos(2 * PI * t * freq);
        result.push_back(value);
    }

    return result;
}

vector<double> generate_square_signal(int samples, double freq) {
    vector<double> result;

    for(int i = 0; i < samples; i++) {
        double t = i / (double)samples;
        double value = sin(2 * PI * t * freq) >= 0 ? 1 : -1;
        result.push_back(value);
    }

    return result;
}

double sawtoothWave(double time, double frequency, double amplitude) {
    double phase = 2 * PI * frequency * time;
    return amplitude * (phase - floor(phase));
}

vector<double> generate_pilo_signal(int samples, double freq) {
    vector<double> result;

    double amplitude = 1.0;
    for (int i = 0; i < samples; i++) {
        double t = i / (double)samples;
        double sawtoothValue = sawtoothWave(t, freq, amplitude);
        result.push_back(sawtoothValue);
    }

    return result;
}

vector<double> filter_1d(vector<double> values) {
    vector<double> result;
    for(int i = 1; i < values.size() - 1; i++) {
        double avg = (values[i-1] + values[i] + values[i+1]) / 3.0;
        result.push_back(avg);
    }
    return result;
}

vector<vector<double>> filter_2d(vector<vector<double>> values) {
    vector<vector<double>> result(values.size() - 2);
    for(int i = 0; i < result.size(); i++) {
        for(int j = 0; j < values[0].size() - 2; j++) {
            result[i].push_back(0);
        }
    }

    for(int i = 1; i < values.size() - 1; i++) {
        for(int j = 1; j < values[i].size() - 1; j++) {
            double sum = 0;
            for(int pi = -1; pi <= 1; pi++) {
                for(int pj = -1; pj <= 1; pj++){
                    sum += values[i+pi][j+pj];
                }
            }
            result[i-1][j-1] = sum / 9.0;
        }
    }
    return result;
}

vector<complex<double>> dft(vector<double> values) {
    int N = values.size();
    vector<complex<double>> result(N);
    for(int k = 0; k < N; k++) {
        complex<double> sum = 0;
        for(int n = 0; n < N; n++) {
            double ix = -2 * PI * (k/(double)N) * n;
            sum += values[n] * complex<double>(cos(ix), sin(ix));
        }
        result[k] = sum;
    }
    return result;
}

vector<double> idft(vector<complex<double>> values) {
    int N = values.size();
    vector<double> result(N);
    for(int k = 0; k < N; k++) {
        complex<double> sum = 0;
        for(int n = 0; n < N; n++) {
            double ix = 2 * PI * (k/(double)N) * n;
            sum += values[n] * complex<double>(cos(ix), sin(ix));
        }
        result[k] = sum.real() / N;
    }
    return result;
}

vector<vector<double>> interpolacja(vector<vector<double>> values, int new_rows, int new_cols) {
    vector<vector<double>> result(new_rows);
    for(int i = 0; i < new_rows; i++) {
        for(int j = 0; j < new_cols; j++) {
            result[i].push_back(0);
        }
    }
    int rows = values.size();
    int cols = values[0].size();
    double scale_i = (rows - 1) / (new_rows - 1.0);
    double scale_j = (cols - 1) / (new_cols - 1.0);

    for(int i = 0; i < new_rows; i++) {
        double y = i * scale_i;
        int y1 = (int)floor(y);
        int y2 = min(y1 + 1, rows - 1);
        double dy = y - y1;

        for(int j = 0; j < new_cols; j++) {
            double x = j * scale_j;
            int x1 = (int)floor(x);
            int x2 = min(x1 + 1, cols - 1);
            double dx = x - x1;

            double Q11 = values[y1][x1];
            double Q12 = values[y1][x2];
            double Q21 = values[y2][x1];
            double Q22 = values[y2][x2];

            double interpolacja = (1 - dx) * (1 - dy) * Q11 + dx * (1 - dy) * Q12 + (1 - dx) * dy * Q21 + dx * dy * Q22;

            result[i][j] = interpolacja;
        }
    }

    return result;
}

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: scikit_build_example

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

    m.def("plot_signal", &plot_signal, "Rysuje wykres sygnalu");
    m.def("generate_sin_signal", &generate_sin_signal, "Generuje sygnał sin");
    m.def("generate_cos_signal", &generate_cos_signal, "Generuje sygnał cos");
    m.def("generate_square_signal", &generate_square_signal, "Generuje sygnał kwadratowy");
    m.def("generate_pilo_signal", &generate_pilo_signal, "Generuje sygnał piloksztaltny");
    m.def("filter_1d", &filter_1d, "Filtracja 1D");
    m.def("filter_2d", &filter_2d, "Filtracja 2D");
    m.def("dft", &dft, "DFT");
    m.def("idft", &idft, "IDFT");
     m.def("interpolacja", &interpolacja, "Interpolacja dwuliniowa na siatce");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
