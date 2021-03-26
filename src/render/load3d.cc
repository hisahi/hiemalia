/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// load3d.cc: implementation of 3D loader

#include "load3d.hh"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "file.hh"
#include "load2d.hh"
#include "logger.hh"
#include "sbuf.hh"
#include "str.hh"

namespace hiemalia {
static ModelPoint parseModelPoint(std::string s, coord_t sx, coord_t sy,
                                  coord_t sz) {
    coord_t x = 0, y = 0, z = 0;
    std::sscanf(s.c_str(), FMT_coord_t " " FMT_coord_t " " FMT_coord_t, &x, &y,
                &z);
    return ModelPoint(x * sx, y * sy, z * sz);
}

static ModelFragment parseModelFragment(std::string s, size_t n, Color color) {
    std::istringstream in(s);
    size_t start;
    int num;
    std::vector<size_t> points;

    in >> num;
    start = num < 0 ? num + n : num;
    while (in >> num) points.push_back(num < 0 ? num + n : num);

    return ModelFragment(color, start, std::move(points));
}

static Model loadStream3D(std::istream& in) {
    std::vector<ModelPoint> vertices;
    std::vector<ModelFragment> fragments;
    coord_t scale = 1;
    coord_t scaleX = 1;
    coord_t scaleY = 1;
    coord_t scaleZ = 1;
    Color color{255, 255, 255, 255};

    for (std::string line; std::getline(in, line);) {
        if (line.empty() || line[0] == '#') continue;
        auto command = line.substr(0, line.find(' '));
        std::string value;
        if (command.size() < line.size()) {
            value = line.substr(line.find(' ') + 1);
        }

        if (command == "v") {
            vertices.push_back(parseModelPoint(value, scale * scaleX,
                                               scale * scaleY, scale * scaleZ));
        } else if (command == "l") {
            fragments.push_back(
                parseModelFragment(value, vertices.size(), color));
        } else if (command == "c") {
            color = parseColor(value);
        } else if (command == "s") {
            scale = fromString<coord_t>(value);
        } else if (command == "sx") {
            scaleX = fromString<coord_t>(value);
        } else if (command == "sy") {
            scaleY = fromString<coord_t>(value);
        } else if (command == "sz") {
            scaleZ = fromString<coord_t>(value);
        }
    }

    return Model(std::move(vertices), std::move(fragments));
}

Model load3D(const std::string& filename) {
    LOG_TRACE("loading 3D model from assets/%s", filename);
    std::ifstream stream = openAssetFileRead(filename, false);
    if (stream.fail()) throw std::runtime_error("failed to open model file");
    stream.exceptions(std::ifstream::badbit);
    return loadStream3D(stream);
}

Model load3D(const std::string& folder, const std::string& filename) {
    std::string f = buildAssetFilePath(folder, filename);
    LOG_TRACE("loading 3D model from %s", f);
    std::ifstream stream = openFileRead(f, false);
    if (stream.fail()) throw std::runtime_error("failed to open model file");
    stream.exceptions(std::ifstream::badbit);
    return loadStream3D(stream);
}
}  // namespace hiemalia
