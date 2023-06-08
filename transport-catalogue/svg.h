#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {

    struct Rgb {
        Rgb(uint8_t r, uint8_t g, uint8_t b)
                :red(r), green(g), blue(b) {
        }

        Rgb()
                :red(0), green(0), blue(0) {
        }

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba {
        Rgba(uint8_t r, uint8_t g, uint8_t b, double op)
                :red(r), green(g), blue(b), opacity(op) {
        }

        Rgba()
                :red(0), green(0), blue(0), opacity(1.0) {
        }

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.0;
    };

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

// Объявив в заголовочном файле константу со спецификатором inline,
// мы сделаем так, что она будет одной на все единицы трансляции,
// которые подключают этот заголовок.
// В противном случае каждая единица трансляции будет использовать свою копию этой константы
    inline const Color NoneColor{"none"};

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    inline std::string to_string(StrokeLineCap value) {
        switch (value) {
            case StrokeLineCap::BUTT:   return "butt";
            case StrokeLineCap::ROUND:  return "round";
            case StrokeLineCap::SQUARE: return "square";
            default:                    return "Unknown StrokeLineCap";
        }
    }

    inline std::string to_string(StrokeLineJoin value) {
        switch (value) {
            case StrokeLineJoin::ARCS:       return "arcs";
            case StrokeLineJoin::BEVEL:      return "bevel";
            case StrokeLineJoin::MITER:      return "miter";
            case StrokeLineJoin::MITER_CLIP: return "miter_clip";
            case StrokeLineJoin::ROUND:      return "round";
            default:                         return "Unknown StrokeLineJoin";
        }
    }

    inline std::ostream& operator<<(std::ostream& out, const StrokeLineCap line_cap) {
        switch (line_cap) {
            case StrokeLineCap::BUTT:
                out << "butt";
                break;
            case StrokeLineCap::ROUND:
                out << "round";
                break;
            case StrokeLineCap::SQUARE:
                out << "square";
                break;
        }
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, const StrokeLineJoin line_join) {
        switch (line_join) {
            case StrokeLineJoin::ARCS:
                out << "arcs";
                break;
            case StrokeLineJoin::BEVEL:
                out << "bevel";
                break;
            case StrokeLineJoin::MITER:
                out << "miter";
                break;
            case StrokeLineJoin::MITER_CLIP:
                out << "miter-clip";
                break;
            case StrokeLineJoin::ROUND:
                out << "round";
                break;
        }
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, const Rgb& rgb) {
        out << "rgb(" << static_cast<int>(rgb.red) << ","
            << static_cast<int>(rgb.green) << ","
            << static_cast<int>(rgb.blue) << ")";
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, const Rgba& rgba) {
        out << "rgba(" << static_cast<int>(rgba.red) << ","
            << static_cast<int>(rgba.green) << ","
            << static_cast<int>(rgba.blue) << ","
            << rgba.opacity << ")";
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, const Color& color) {
        std::visit([&out](const auto& c) {
            using T = std::decay_t<decltype(c)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                out << "none";
            } else if constexpr (std::is_same_v<T, std::string>) {
                out << c;
            } else if constexpr (std::is_same_v<T, Rgb>) {
                out << c;
            } else if constexpr (std::is_same_v<T, Rgba>) {
                out << c;
            }
        }, color);
        return out;
    }

    struct Point {
        Point() = default;
        Point(double x, double y)
                : x(x)
                , y(y) {
        }
        double x = 0;
        double y = 0;
    };

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
    struct RenderContext {
        RenderContext(std::ostream& out)
                : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
                : out(out)
                , indent_step(indent_step)
                , indent(indent) {
        }

        RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

        //virtual std::unique_ptr<Object> Clone() const = 0;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    template <typename Owner>
    class PathProps {
    public:

        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            stroke_line_cap_ = line_cap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            stroke_line_join_ = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << R"( fill=")";
                std::visit([&out](const auto& color) {
                    if constexpr (!std::is_same_v<std::decay_t<decltype(color)>, std::monostate>) {
                        out << color;
                    }
                }, *fill_color_);
                out << R"(")";
            }
            if (stroke_color_) {
                out << R"( stroke=")";
                std::visit([&out](const auto& color) {
                    if constexpr (!std::is_same_v<std::decay_t<decltype(color)>, std::monostate>) {
                        out << color;
                    }
                }, *stroke_color_);
                out << R"(")";
            }
            if (stroke_width_) {
                out << R"( stroke-width=")" << stroke_width_ << R"(")";
            }
            if (stroke_line_cap_) {
                out << R"( stroke-linecap=")" << to_string(stroke_line_cap_.value()) << R"(")";
            }
            if (stroke_line_join_) {
                out << R"( stroke-linejoin=")" << to_string(stroke_line_join_.value()) << R"(")";
            }
        }


    private:
        Owner& AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        double stroke_width_{};
        std::optional<StrokeLineCap> stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;
    };

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);
        //[[nodiscard]] std::unique_ptr<Object> Clone() const override;

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

        //[[nodiscard]] std::unique_ptr<Object> Clone() const override;
        /*
         * Прочие методы и данные, необходимые для реализации элемента <polyline>
         */

    private:
        void RenderObject(const RenderContext& context) const override;

        std::vector<Point> points_;
    };

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
    class Text final : public Object, public PathProps<Text> {
    public:


        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

        // Прочие данные и методы, необходимые для реализации элемента <text>

        //[[nodiscard]] std::unique_ptr<Object> Clone() const override;

    private:
        void RenderObject(const RenderContext& context) const override;

        Point pos_ = {0, 0};
        Point offset_ = {0, 0};
        uint32_t fontsize_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;
    };

    class ObjectContainer {
    public:

        template <typename Obj>
        void Add(Obj obj);

        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    protected:
        ~ObjectContainer() = default;
    };

    class Drawable {
    public:

        virtual void Draw(ObjectContainer& container) const = 0;

        virtual ~Drawable() = default;
    };

    class Document : public ObjectContainer{
    public:
        /*
         Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
         Пример использования:
         Document doc;
         doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
        */

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;

        // Прочие методы и данные, необходимые для реализации класса Document
    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };

    template <typename Obj>
    void ObjectContainer::Add(Obj obj) {
        static_assert(std::is_base_of<Object, Obj>::value, "Object must be derived from svg::Object");
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }

}  // namespace svg