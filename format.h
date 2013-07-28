#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
class Format;

/** String formatting utility function.
 * Usage: format(FORMAT_STRING).arg(value).arg(name, value);
\code
    FIELD ::= {NAME[:SPEC]}
    NAME ::= [position | name]
    SPEC ::= [FILL] [width] [.precision]
\endcode
 *
 * Each field is either positional number:
\code{.cpp}
    format("{0}, {1}").arg(1).arg(2) => "1, 2"
\endcode
 *
 * Or name of argument:
\code{.cpp}
    format("Pi = {value}").arg("value", 3.14) => "Pi = 3.14"
\endcode
 *
 * Types could be mixed (numbers for positional args is counting even for named args):
\code{.cpp}
    format("{1} = {value}").arg("value", 3.14).arg("Pi") => "Pi = 3.14"
\endcode
 *
 * Arguments could be repeated and follow in any order:
\code{.cpp}
    format("{1} {operator} {0} = {result}").arg(2).arg(3).arg("result", 5).arg("operator", '+') => "3 + 2 = 5";
\endcode
 *
 * A brace character could be inserted by doubling it:
\code{.cpp}
    format("{{0}").arg(1) => "{{0}"
\endcode
 *
 * Field can have width and precision specificator:
\code{.cpp}
    name[:[fill][width][.precision]]
\endcode
 *
 * Width defines minimum field width (any character out of that width will be truncated).
 * If actual width is lesser than specified one, argument will be padded with spaces.
\code{.cpp}
    format("<{0:10}>, <{0:2}>").arg("123") => "<       123>, <12>"
\endcode
 *
 * If specified width is greater than actual one, argument is padded to the left and filled with `fill` character.
 * It could be any character except for digits (0-9). If width is preceeded with zero, fill character will be zero.
 * By default fill character is space.
\code{.cpp}
    format("{0:02.2}").arg(1) => "01"
    format("{0:_10}").arg(1) => "_________1"
\endcode
 *
 * Precision defines count of digist after decimal point for floating-point numbers:
\code{.cpp}
    format("{0:5.2}").arg(3.14158255358) => " 3.14"
\endcode
 *
 * When precision is specified for integer numbers, it defines arg's width and pads with leading zeroes (actual width is ignored):
\code{.cpp}
    format("{0:10.5}").arg(1) => "00001"
\endcode
 *
 * In order to use any user-defined class a `to_string` routine should be declared for it:
\code{.cpp}
    struct Date {
        int day, month, year;
    };
    std::string to_string(const Date & date) {
        return format("{day:02}/{month:02}/{year:04}").arg("day", date.day).arg("month", date.month).arg("year", date.year);
    }
\endcode
 */
Format format(const std::string & pattern);


/** String conversion routine.
 * Should be defined for each class that intended to be used as argument in format() function.
 */
template<class T>
std::string to_string(const T & t) { std::ostringstream out; out << t; return out.str(); }

/** Class for internal usage. */
class Formatter {
	class Field;
public:
    Formatter(const std::string & pattern);
    const std::string & str() const;

    template<class T> Formatter & arg(const T & t) { return arg(to_string(i), t); }
    template<class T>
    Formatter & arg(const std::string & name, const T & t) {
        for(std::vector<Field>::iterator field = fields.begin(); field != fields.end(); ++field) {
            if(field->name == name) {
				field->repr = arg_repr(t, *field);
                field->should_be_replaced = true;
            } else {
                field->should_be_replaced = false;
            }
        }
        run_replace();
        return *this;
    }
private:
    template<class T> std::string arg_repr(const T & t, const Field & field) { return field.basic_repr(to_string(t)); }
    std::string arg_repr(double t, const Field & field) { return field.double_repr(t); }
    void run_replace();

    struct Field {
        size_t begin, end;
        std::string name;
        std::string repr;
        bool should_be_replaced;
        char fill;
		size_t width, precision;
        Field(int _begin = 0, int _end = 0, const std::string & field = 0);
        std::string basic_repr(const std::string & repr) const;
        std::string double_repr(double value) const;
    };
    std::vector<Field> fields;

    unsigned int i;
    std::string result;
};

namespace FormatAction {
    void no_action(const std::string & message);
    typedef void (*Action)(const std::string &);
}

/** Class for internal usage. */
class Format {
    struct Data {
        int refCount;
        Formatter formatter;
        FormatAction::Action action;
        Data(const std::string & pattern, FormatAction::Action action_after);
    };
    Data * data;
public:
    Format(const std::string & pattern, FormatAction::Action action_after = FormatAction::no_action);
    Format(const Format & other);
    Format & operator=(const Format & other);
    virtual ~Format();
    operator std::string() const;
    const std::string & str() const;
    template<class T> Format arg(const T & t) const { data->formatter.arg(t); return *this; }
    template<class T> Format arg(const std::string & name, const T & t) const { data->formatter.arg(name, t); return *this; }
};

