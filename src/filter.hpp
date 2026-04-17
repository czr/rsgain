#pragma once

#include <vector>
#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct BiquadFilter {
    double b0, b1, b2, a1, a2;
    std::vector<double> x1, x2, y1, y2;

    void init(int nb_channels) {
        x1.assign(nb_channels, 0.0);
        x2.assign(nb_channels, 0.0);
        y1.assign(nb_channels, 0.0);
        y2.assign(nb_channels, 0.0);
    }

    void process(short *data, size_t nb_samples, int nb_channels) {
        for (size_t i = 0; i < nb_samples; i++) {
            for (int ch = 0; ch < nb_channels; ch++) {
                size_t idx = i * nb_channels + ch;
                double x0 = static_cast<double>(data[idx]);
                double y0 = b0 * x0 + b1 * x1[ch] + b2 * x2[ch]
                           - a1 * y1[ch] - a2 * y2[ch];
                x2[ch] = x1[ch];
                x1[ch] = x0;
                y2[ch] = y1[ch];
                y1[ch] = y0;
                int val = static_cast<int>(std::round(y0));
                if (val > 32767) val = 32767;
                if (val < -32768) val = -32768;
                data[idx] = static_cast<short>(val);
            }
        }
    }
};

inline BiquadFilter make_highpass(double freq, double sample_rate, int nb_channels) {
    BiquadFilter f;
    double w0 = 2.0 * M_PI * freq / sample_rate;
    double Q = 1.0 / std::sqrt(2.0);
    double alpha = std::sin(w0) / (2.0 * Q);
    double cos_w0 = std::cos(w0);
    double a0 = 1.0 + alpha;
    f.b0 = ((1.0 + cos_w0) / 2.0) / a0;
    f.b1 = (-(1.0 + cos_w0)) / a0;
    f.b2 = ((1.0 + cos_w0) / 2.0) / a0;
    f.a1 = (-2.0 * cos_w0) / a0;
    f.a2 = (1.0 - alpha) / a0;
    f.init(nb_channels);
    return f;
}

inline BiquadFilter make_lowpass(double freq, double sample_rate, int nb_channels) {
    BiquadFilter f;
    double w0 = 2.0 * M_PI * freq / sample_rate;
    double Q = 1.0 / std::sqrt(2.0);
    double alpha = std::sin(w0) / (2.0 * Q);
    double cos_w0 = std::cos(w0);
    double a0 = 1.0 + alpha;
    f.b0 = ((1.0 - cos_w0) / 2.0) / a0;
    f.b1 = (1.0 - cos_w0) / a0;
    f.b2 = ((1.0 - cos_w0) / 2.0) / a0;
    f.a1 = (-2.0 * cos_w0) / a0;
    f.a2 = (1.0 - alpha) / a0;
    f.init(nb_channels);
    return f;
}
