#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <vector>

template<class T>
std::string to_string(const T & t) { std::ostringstream out; out << t; return out.str(); }

class Formatter {
	class Field;
public:
    Formatter(const std::string & pattern)
        : i(0), result(pattern)
    {
        for(size_t pos = result.find('{'); pos != std::string::npos; pos = result.find('{', pos)) {
            if(pos < result.size() - 1 && result[pos + 1] == '{') {
                result.erase(pos, 1);
                ++pos;
                continue;
            }
            size_t placeholderEnd = result.find("}", pos);
            fields.push_back(Field(pos + 1, placeholderEnd, result.substr(pos + 1, placeholderEnd - pos - 1)));
            pos = placeholderEnd + 1;
        }
    }
    const std::string & str() const { return result; }

    template<class T>
    Formatter & arg(const T & t) {
        return arg(to_string(i++), t);
    }

    template<class T>
    std::string arg_repr(const T & t, const Field & field) {
		std::string repr = to_string(t);
		if(field.width > 0) {
			std::ostringstream out;
			out << std::setw(field.width) << repr;
			repr = out.str();
		}
		return repr;
	}

	// TODO arg repr for int and take precision as padding with zeroes.

    std::string arg_repr(double t, const Field & field) {
		std::ostringstream out;
		if(field.width > 0) {
			out.width(field.width);
		}
		if(field.precision > 0) {
			out.setf(std::ios::fixed);
			out.precision(field.precision);
		}
		out << t;
		return out.str();
	}

    template<class T>
    Formatter & arg(const std::string & name, const T & t) {
        int shift = 0;
        for(std::vector<Field>::iterator field = fields.begin(); field != fields.end(); ++field) {
            field->begin += shift;
            field->end += shift;
            if(field->name == name) {
				std::string repr = arg_repr(t, *field);
                size_t old_size = result.size();
                result.replace(field->begin - 1, field->end + 2 - field->begin, repr);
                shift += result.size() - old_size;
            }
        }
        return *this;
    }
private:
    struct Field {
        int begin, end;
        std::string name;
		int width, precision;
        Field(int _begin = 0, int _end = 0, const std::string & field = 0) : begin(_begin), end(_end), width(0), precision(0)
        {
            size_t colon = field.find(':');
            if(colon == std::string::npos) {
                name = field;
            } else {
                name = field.substr(0, colon);
                size_t dot = field.find('.', colon);
                if(dot == std::string::npos) {
                    width = atoi(field.substr(colon + 1).c_str());
                } else {
                    width = atoi(field.substr(colon + 1, dot - colon - 1).c_str());
                    precision = atoi(field.substr(dot + 1).c_str());
                }
            }
			// TODO filling.
            //std::cout << field << " -> " << "name=[" << name << "] width=[" << width << "] precision=[" << precision << "]\n";
        }
    };
    std::vector<Field> fields;

    int i;
    std::string result;
};

class Format {
    struct Data {
        int refCount;
        Formatter formatter;
        Data(const std::string & pattern) : refCount(1), formatter(pattern) {}
    };
    Data * data;
public:
    Format(const std::string & pattern) : data(new Data(pattern)) {}
    Format(const Format & other) : data(other.data) { data->refCount++;}
    Format & operator=(const Format & other) { data = other.data; data->refCount++; }
    virtual ~Format() {
        data->refCount--;
        if(data->refCount <= 0) {
            delete data;
        }
    }

    operator std::string() const { return str(); }
    const std::string & str() const { return data->formatter.str(); }

    template<class T>
    Format arg(const T & t) const { data->formatter.arg(t); return *this; }

    template<class T>
    Format arg(const std::string & name, const T & t) const { data->formatter.arg(name, t); return *this; }
};

Format format(const std::string & pattern)
{
    return Format(pattern);
}

// ------------------------- USAGE ------------------------

struct Date {
    int day, month, year;
    Date(int _day, int _month, int _year) : day(_day), month(_month), year(_year) {}
};
std::string to_string(const Date & date)
{
	// TODO fix test for padding using precision.
	return format("{day:02}/{month:02}/{year:04}").arg("day", date.day).arg("month", date.month).arg("year", date.year);
}

#define COMPARE(a, b) { std::string r = (a); if(r != (b)) { \
    std::cout << "Not equal: '" << r << "' and '" << b << "'!" << std::endl; \
} else { \
    std::cout << "Good: '" << r << "' == '" << b << "'." << std::endl; \
} }

int main()
{
    COMPARE(format("{0}").arg(1), "1");
    COMPARE(format("{0}{0}").arg(1), "11");
    COMPARE(format("Hello, {0}!").arg("world"), "Hello, world!");
    COMPARE(format("{0}{1}").arg(1).arg("m"), "1m");
    COMPARE(format("{hello} {world}").arg("world", "world").arg("hello", "hello"), "hello world");
    COMPARE(format("{0}{1}{0}").arg("abra").arg("cad"), "abracadabra");
    COMPARE(format("{0}").arg(1.1), "1.1");
    COMPARE(format("{0}").arg(Date(17, 7, 2013)), "17/07/2013");
    COMPARE(format("{{0}").arg(1), "{0}");
    COMPARE(format("{0:10}").arg(1), "         1");
    COMPARE(format("{0:10.2}").arg(1.0), "      1.00");
	// TODO fix test for padding using precision.
	// TODO add type specifier: d or f or s.
    COMPARE(format("{0:010}").arg(1), "0000000001");
    COMPARE(format("{0:0}").arg(1), "1");
    COMPARE(format("{0: 10}").arg(1), "         1");

}

