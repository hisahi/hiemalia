/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// load2d.cc: implementation of 2D loaders

#include "load2d.hh"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "file.hh"
#include "logger.hh"
#include "sbuf.hh"
#include "shape.hh"
#include "str.hh"

namespace hiemalia {
Color parseColor(std::string s) {
    uint8_t r = 255, g = 255, b = 255, a;
    float af = 1.0f;
    std::sscanf(s.c_str(), "%hhu,%hhu,%hhu,%f", &r, &g, &b, &af);
    a = static_cast<uint8_t>(af * 255);
    return Color{r, g, b, a};
}

static ShapePoint parseShapePoint(std::string s, coord_t scale) {
    coord_t x = 0, y = 0;
    std::sscanf(s.c_str(), FMT_coord_t "," FMT_coord_t, &x, &y);
    return ShapePoint(x * scale, y * scale);
}

static ShapeSheet loadStream2D(std::istream& in) {
    std::vector<Shape> shapes;
    coord_t scale = 1;
    Shape shape;
    std::optional<ShapeFragment> fragment;
    Color color{255, 255, 255, 255};

    for (std::string line; std::getline(in, line);) {
        if (line.empty() || line[0] == '#') continue;
        auto command = line.substr(0, line.find(' '));
        std::string value;
        if (command.size() < line.size()) {
            value = line.substr(line.find(' ') + 1);
        }

        if (command == "begin") {
            shape = Shape{};
        } else if (command == "move") {
            if (fragment) shape.parts.push_back(*fragment);
            fragment.emplace(color, parseShapePoint(value, scale));
        } else if (command == "line") {
            fragment->points.push_back(parseShapePoint(value, scale));
        } else if (command == "scale") {
            scale = fromString<coord_t>(value);
        } else if (command == "color") {
            color = parseColor(value);
            dynamic_assert(fragment.has_value(),
                           "must have fragment for color");
            if (!fragment->points.empty()) {
                shape.parts.push_back(*fragment);
                fragment.emplace(color, fragment->points.back());
            } else
                fragment->color = color;
        } else if (command == "end") {
            if (fragment) shape.parts.push_back(*fragment);
            fragment.reset();
            shapes.push_back(shape);
        }
    }

    return ShapeSheet(std::move(shapes));
}

static Font loadStreamFont(std::istream& in) {
    bool is_font = false;
    coord_t width = 0;
    coord_t height = 0;
    char minChar = 0;
    for (std::string line; std::getline(in, line);) {
        if (line.empty() || line[0] == '#') continue;
        auto command = line.substr(0, line.find(' '));
        std::string value;
        if (command.size() < line.size()) {
            value = line.substr(line.find(' ') + 1);
        }

        if (command == "font") {
            is_font = true;
        } else if (!is_font) {
            throw std::runtime_error("not a font (" + command + ")");
        } else if (command == "fontwidth") {
            width = fromString<coord_t>(value);
        } else if (command == "fontheight") {
            height = fromString<coord_t>(value);
        } else if (command == "fontmin") {
            minChar = fromString<coord_t>(value);
        } else if (command == "fontend") {
            break;
        }
    }

    return Font(loadStream2D(in), width, height, minChar);
}

ShapeSheet load2D(const std::string& filename) {
    LOG_TRACE("loading shape sheet from %s", filename);
    std::ifstream stream = openAssetFileRead(filename, false);
    if (stream.fail())
        throw std::runtime_error("failed to open shape sheet file");
    stream.exceptions(std::ifstream::badbit);
    return loadStream2D(stream);
}

Font loadFont(const std::string& filename) {
    LOG_TRACE("loading font from %s", filename);
    std::ifstream stream = openAssetFileRead(filename, false);
    if (stream.fail()) throw std::runtime_error("failed to open font file");
    stream.exceptions(std::ifstream::badbit);
    return loadStreamFont(stream);
}
}  // namespace hiemalia
