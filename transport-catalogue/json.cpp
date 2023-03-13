#include "json.h"

using namespace std;

namespace json {

    namespace {

//---------------------------------------Load-------------------------------------------

        Node LoadNode(istream &input);

        Node LoadNumber(std::istream &input) {

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            } else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    return stoi(parsed_num);
                }
                return stod(parsed_num);
            } catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadString(std::istream &input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                } else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *it;
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                        case 'n':
                            s.push_back('\n');
                            break;
                        case 't':
                            s.push_back('\t');
                            break;
                        case 'r':
                            s.push_back('\r');
                            break;
                        case '"':
                            s.push_back('"');
                            break;
                        case '\\':
                            s.push_back('\\');
                            break;
                        default:
                            // Встретили неизвестную escape-последовательность
                            throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                } else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                } else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }
            return s;
        }

        Node LoadNull(istream& input) {
            string s;
            char c;
            for (int i = 0; input >> c && i < 4; ++i) {
                s += c;
            }
            if (s != "null"s) {
                throw ParsingError("Unexpected value"s);
            }
            return {};
        }

        Node LoadBool(istream& input) {
            string s;
            char c;

            for (; input >> c && s.size() < 5 && c != 'e';s += c);
            s += c;

            if (s == "true"s) {
                return {true};
            }
            if (s == "false"s) {
                return {false};
            }
            throw ParsingError("Unexpected value"s);
        }

        Node LoadArray(istream &input) {
            Array result;
            char c;
            for (; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }

            if (c != ']') {
                throw ParsingError("] not expected"s);
            }

            return result;
        }

        Node LoadDict(istream &input) {
            Dict result;
            char c;
            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({std::move(key), LoadNode(input)});
            }

            if (c != '}') {
                throw ParsingError("} not expected"s);
            }

            return result;
        }

        Node LoadNode(istream &input) {
            char c;
            if (input >> c) {
                if (c == '[') {
                    return LoadArray(input);
                } else if (c == '{') {
                    return LoadDict(input);
                } else if (c == '"') {
                    return LoadString(input);
                } else if (c == 'n') {
                    input.putback(c);
                    return LoadNull(input);
                } else if (c == 't' || c == 'f') {
                    input.putback(c);
                    return LoadBool(input);
                } else {
                    input.putback(c);
                    return LoadNumber(input);
                }
            } else return {};
        }

    }  // namespace

//------------------------------------Node-------------------------------------

    Node::Node(bool value)
            : data_(value) {}

    Node::Node(const int value)
            : data_(value) {}

    Node::Node(const double value)
            : data_(value) {}

    Node::Node(const string &value)
            : data_(value) {}

    Node::Node(const Array &value)
            : data_(value) {}

    Node::Node(const Dict &map)
            : data_(map) {}

//--------------------------------------As-------------------------------------------

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::invalid_argument("Is not Bool"s);
        }
        return get<bool>(data_);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::invalid_argument("Is not Int"s);
        }
        return get<int>(data_);
    }

    double Node::AsDouble() const {
        if (holds_alternative<double>(data_)) {
            return get<double>(data_);
        }
        if (IsInt()) {
            return static_cast<double>(AsInt());
        }
        throw std::invalid_argument("Is not Double"s);
    }

    const string &Node::AsString() const {
        if (!IsString()) {
            throw std::invalid_argument("Is not String"s);
        }
        return get<string>(data_);
    }

    const Array &Node::AsArray() const {
        if (!IsArray()) {
            throw std::invalid_argument("Is not Array"s);
        }
        return get<Array>(data_);
    }

    const Dict &Node::AsMap() const {
        if (!IsMap()) {
            throw std::invalid_argument("Is not Map"s);
        }
        return get<Dict>(data_);
    }

//----------------------------------------------Document---------------------------------------

    Document::Document(Node root)
            : root_(std::move(root)) {
    }



    Node &Document::GetRoot() {
        return root_;
    }

    Document Load(istream &input) {
        return Document{LoadNode(input)};
    }

//----------------------------------------------Print-------------------------------------------

    struct PrintContext {
        std::ostream &out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        PrintContext Indented() const {
            return {out, indent_step, indent_step + indent};
        }
    };

    void PrintNode(const Node &node, const PrintContext &ctx, bool enter = true);

    void PrintValue(nullptr_t, const PrintContext &ctx, bool) {
        ctx.out << "null"sv;
    }

    void PrintValue(const string &value, const PrintContext &ctx, bool) {
        ctx.out << '"';
        for (char c: value) {
            switch (c) {
                case '"':
                    ctx.out << R"(\")";
                    break;
                case '\n':
                    ctx.out << R"(\n)";
                    break;
                case '\t':
                    ctx.out << R"(\t)";
                    break;
                case '\r':
                    ctx.out << R"(\r)";
                    break;
                case '\\':
                    ctx.out << R"(\\)";
                    break;
                default:
                    ctx.out << c;
                    break;
            }
        }
        ctx.out << '"';
    }

    template<typename Number>
    void PrintValue(const Number value, const PrintContext &ctx, bool) {
        ctx.out << boolalpha;
        ctx.out << value;
    }

    void PrintValue(const Array &value, const PrintContext &ctx, bool) {

        auto print_elems = [&value](const PrintContext &ctx) {
            bool is_first = false;
            for (const auto &elem: value) {
                if (is_first) {
                    ctx.out << ",\n"sv;
                }
                is_first = true;
                ctx.PrintIndent();
                PrintNode(elem, ctx, false);
            }
        };


        ctx.PrintIndent();

        ctx.out << "[\n"sv;

        print_elems(ctx.Indented());

        ctx.out << '\n';
        ctx.PrintIndent();
        ctx.out << "]"sv;
    }

    void PrintValue(const Dict &value, const PrintContext &ctx, bool enter = true) {

        auto print_elems = [&value](const PrintContext &ctx) {
            bool is_first = false;
            for (const auto &elem: value) {
                if (is_first) {
                    ctx.out << ",\n"sv;
                }
                is_first = true;
                ctx.PrintIndent();
                ctx.out << "\""sv << elem.first << "\": "sv;
                PrintNode(elem.second, ctx, false);
            }
        };
        if (enter) {
            ctx.PrintIndent();
        }

        ctx.out << "{\n"sv;

        print_elems(ctx.Indented());

        ctx.out << '\n';
        ctx.PrintIndent();
        ctx.out << "}"sv;
    }

    void PrintNode(const Node &node, const PrintContext &ctx, bool enter) {
        visit([&ctx, enter](const auto &value) { PrintValue(value, ctx, enter); }, node.GetValue());

    }

    void Print(Document &doc, std::ostream &out) {
        PrintContext ctx({out, TAB, 0});
        PrintNode(doc.GetRoot(), ctx);
        out << endl;
    }

}  // namespace json