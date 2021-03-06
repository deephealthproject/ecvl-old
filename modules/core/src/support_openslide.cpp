/*
* ECVL - European Computer Vision Library
* Version: 0.1
* copyright (c) 2020, Università degli Studi di Modena e Reggio Emilia (UNIMORE), AImageLab
* Authors:
*    Costantino Grana (costantino.grana@unimore.it)
*    Federico Bolelli (federico.bolelli@unimore.it)
*    Michele Cancilla (michele.cancilla@unimore.it)
*    Laura Canalini (laura.canalini@unimore.it)
*    Stefano Allegretti (stefano.allegretti@unimore.it)
* All rights reserved.
*/

#include "ecvl/core/support_openslide.h"

#include <iostream>
#include "openslide.h"

#include "ecvl/core/standard_errors.h"

using namespace std::filesystem;
using namespace std;

namespace ecvl
{
bool OpenSlideGetLevels(const path& filename, vector<array<int, 2>>& levels)
{
    openslide_t* osr = openslide_open(filename.string().c_str());
    levels.clear();

    if (osr == NULL || openslide_get_error(osr) != NULL) {
        return false;
    }

    int n_levels = openslide_get_level_count(osr);
    levels.resize(n_levels);
    int64_t w, h;
    for (int i = 0; i < n_levels; ++i) {
        openslide_get_level_dimensions(osr, i, &w, &h);
        levels[i] = array{ static_cast<int>(w), static_cast<int>(h) };
    }

    openslide_close(osr);
    return true;
}

bool OpenSlideRead(const path& filename, Image& dst, const int level, const vector<int>& dims)
{
    const int& x = dims[0];
    const int& y = dims[1];
    const int& w = dims[2];
    const int& h = dims[3];

    bool open_status = true;

    openslide_t* osr = openslide_open(filename.string().c_str());

    if (osr == NULL || openslide_get_error(osr) != NULL) {
        return false;
    }
    vector<uint32_t> d(sizeof(uint32_t) * w * h);
    openslide_read_region(osr, d.data(), x, y, level, w, h);

    dst.Create({ 3, w, h }, DataType::uint8, "cxy", ColorType::BGR);

    uint8_t a, r, g, b;
    uint32_t pixel;
    for (int i = 0, j = 0; i < dst.datasize_; ++j, ++i) {
        pixel = d[j];
        a = pixel >> 24;
        switch (a) {
        case 0:
            r = g = b = 0;
            break;
        case 255:
            r = (pixel >> 16) & 0xff;
            g = (pixel >> 8) & 0xff;
            b = pixel & 0xff;
            break;
        default:
            r = 255 * ((pixel >> 16) & 0xff) / a;
            g = 255 * ((pixel >> 8) & 0xff) / a;
            b = 255 * (pixel & 0xff) / a;
        }
        dst.data_[i] = b;
        dst.data_[++i] = g;
        dst.data_[++i] = r;
    }
    openslide_close(osr);
    return !dst.IsEmpty();
}
} // namespace ecvl