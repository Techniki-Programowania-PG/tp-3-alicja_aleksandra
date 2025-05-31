#include <vector>
#include <complex>

using namespace std;

// vector<complex<double>> dft(vector<double> values) {
//     int N = values.size();
//     vector<complex<double>> result(N);
//     for(int k = 0; k < N; k++) {
//         complex<double> sum = 0;
//         for(int n = 0; n < N; n++) {
//             double ix = -2 * PI * (k/N) * n;
//             sum += values[n] * complex<double>(cos(ix), sin(ix));
//         }
//         result[k] = sum;
//     }
//     return result;
// }


// vector<double> dft(vector<complex<double>> values) {
//     int N = values.size();
//     vector<double> result(N);
//     for(int k = 0; k < N; k++) {
//         complex<double> sum = 0;
//         for(int n = 0; n < N; n++) {
//             double ix = 2 * PI * (k/(double)N) * n;
//             sum += values[n] * complex<double>(cos(ix), sin(ix));
//         }
//         result[k] = sum.real() / N;
//     }
//     return result;
// }
