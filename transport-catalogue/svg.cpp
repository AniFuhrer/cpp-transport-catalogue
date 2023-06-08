#include "svg.h"

#include <utility>

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

// ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
        radius_ = radius;
        return *this;
    }

    /*std::unique_ptr<Object> Circle::Clone() const {
        return std::make_unique<Circle>(*this);
    }*/

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << R"(<circle cx=")" << center_.x << R"(" cy=")" << center_.y << R"(" )";
        out << R"(r=")" << radius_ << R"(")";
        // Выводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back((point));
        return *this;
    }

    /*std::unique_ptr<Object> Polyline::Clone() const {
        return std::make_unique<Polyline>(*this);
    }*/

    void Polyline::RenderObject(const RenderContext &context) const {
        auto& out = context.out;
        out << R"(<polyline points=")";
        bool  first = true;
        for (const auto& point : points_) {
            if (!first) {
                out << " "sv;
            }
            out << point.x << ","sv << point.y;
            first = false;
        }
        out << R"(")";
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Text ------------------

    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        fontsize_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    /*std::unique_ptr<Object> Text::Clone() const {
        return std::make_unique<Text>(*this);
    }*/

    void Text::RenderObject(const RenderContext &context) const {
        auto& out = context.out;
        out << R"(<text x=")" << pos_.x << R"(" y=")" << pos_.y << R"(" )";
        out << R"(dx=")" << offset_.x << R"(" dy=")" << offset_.y << R"(" )";
        out << R"(font-size=")" << fontsize_ << R"(")";
        if(!font_family_.empty()) {
            out << R"( font-family=")" << font_family_ << R"(")";
        }
        if (!font_weight_.empty()) {
            out << R"( font-weight=")" << font_weight_ << R"(")";
        }
        RenderAttrs(out);
        out << ">"sv;
        out << data_ << "</text>"sv;
    }

    // ---------- Document ------------------


    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        std::cout << R"(<?xml version="1.0" encoding="UTF-8" ?>)" << std::endl;
        std::cout << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)" << std::endl;
        RenderContext const ctx(out, 2, 2);
        for (const auto& obj : objects_) {
            obj->Render(ctx);
        }
        std::cout << "</svg>"sv;
    }

}  // namespace svg