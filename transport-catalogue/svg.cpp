#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream &operator<<(std::ostream &out, StrokeLineCap elem) {
        switch (elem) {
            case StrokeLineCap::BUTT:
                out << "butt"sv;
                break;
            case StrokeLineCap::ROUND:
                out << "round"sv;
                break;
            case StrokeLineCap::SQUARE:
                out << "square"sv;
                break;
        }
        return out;
    }

    std::ostream &operator<<(std::ostream &out, StrokeLineJoin elem) {
        switch (elem) {
            case StrokeLineJoin::ARCS:
                out << "arcs"sv;
                break;
            case StrokeLineJoin::BEVEL:
                out << "bevel"sv;
                break;
            case StrokeLineJoin::MITER:
                out << "miter"sv;
                break;
            case StrokeLineJoin::MITER_CLIP:
                out << "miter-clip"sv;
                break;
            case StrokeLineJoin::ROUND:
                out << "round"sv;
                break;
        }
        return out;
    }

    struct PrintColor {
        std::ostream &out;

        void operator()(std::monostate) {
            out << "none"sv;
        }

        void operator()(std::string const &color) {
            out << color;
        }

        void operator()(Rgb color) {
            out << "rgb("sv << +color.red << ',' << +color.green << ',' << +color.blue << ')';
        }

        void operator()(Rgba color) {
            out << "rgba("sv << +color.red << ',' << +color.green << ',' << +color.blue << ',' << color.opacity << ')';
        }

    };

    std::ostream &operator<<(std::ostream &out, Color elem) {
        std::visit(PrintColor{out}, elem);
        return out;
    }

    void Object::Render(const RenderContext &context) const {

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

// ---------- Circle ------------------

    Circle &Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle &Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << R"(<circle cx=")" << center_.x << R"(" cy=")" << center_.y << R"(" )";
        out << R"(r=")" << radius_ << R"(")";
        RenderAttrs(out);
        out << R"(/>)";
    }

//----------- Polyline ---------------

    Polyline &Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << R"(<polyline points=")";
        bool is_first = true;
        for (const Point &point: points_) {
            if (!is_first) {
                out << ' ';
            } else {
                is_first = false;
            }
            out << point.x << ","sv << point.y;
        }
        out << R"(")";

        RenderAttrs(out);

        out << "/>"sv;
    }

//------------ Text ------------------

    Text &Text::SetPosition(Point pos) {
        position_ = pos;
        return *this;
    }

    Text &Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text &Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text &Text::SetFontFamily(std::string_view font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text &Text::SetFontWeight(std::string_view font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text &Text::SetData(std::string_view data) {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<text"sv;
        RenderAttrs(out);
        out << R"( x=")" << position_.x << R"(" y=")" << position_.y << R"(" dx=")";
        out << offset_.x << R"(" dy=")" << offset_.y << R"(" font-size=")" << font_size_;
        if (!font_family_.empty()) {
            out << R"(" font-family=")" << font_family_;
        }

        if (!font_weight_.empty()) {
            out << R"(" font-weight=")" << font_weight_;
        }
        out << R"(">)";

        for (char c: data_) {
            switch (c) {
                case '"':
                    out << "&quot;"sv;
                    break;
                case '\'': //символ '
                    out << "&apos;"sv;
                    break;
                case '<':
                    out << "&lt;"sv;
                    break;
                case '>':
                    out << "&gt;"sv;
                    break;
                case '&':
                    out << "&amp;"sv;
                    break;
                default:
                    out << c;
                    break;
            }
        }
        out << "</text>"sv;
    }

//---------- Document ---------------

    void Document::AddPtr(std::unique_ptr<Object> &&obj) {
        objects_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream &out) const {
        RenderContext context(out, 1);
        out << R"(<?xml version="1.0" encoding="UTF-8" ?>)" << std::endl;
        out << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)" << std::endl;
        for (const auto &object: objects_) {
            context.Indented().RenderIndent();
            object->Render(context);
        }
        out << "</svg>"sv;
    }

}  // namespace svg