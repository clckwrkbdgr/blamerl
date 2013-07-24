#include "format.h"

Format format(const std::string & pattern)
{
    return Format(pattern);
}

//------------------------------------------------------------------------------

Formatter::Formatter(const std::string & pattern)
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

const std::string & Formatter::str() const
{
    return result;
}

void Formatter::run_replace()
{
    int shift = 0;
    for(std::vector<Field>::iterator field = fields.begin(); field != fields.end(); ++field) {
        field->begin += shift;
        field->end += shift;
        if(field->should_be_replaced) {
            std::string & repr = field->repr;
            if(field->width > 0 && repr.size() > field->width) {
                repr = repr.substr(0, field->width);
            }
            size_t old_size = result.size();
            result.replace(field->begin - 1, field->end + 2 - field->begin, repr);
            shift += result.size() - old_size;
        }
    }
    ++i;
}

//------------------------------------------------------------------------------

Formatter::Field::Field(int _begin, int _end, const std::string & field)
    : begin(_begin), end(_end), fill(' '), width(0), precision(0)
{
    size_t colon = field.find(':');
    if(colon == std::string::npos) {
        name = field;
    } else {
        name = field.substr(0, colon);
        size_t dot = field.find('.', colon);
        bool is_dot = dot != std::string::npos;
        std::string width_spec = is_dot ? field.substr(colon + 1, dot - (colon + 1)) : field.substr(colon + 1);
        if(width_spec.size() > 1) {
            bool has_fill = !isdigit(width_spec[0]) || (width_spec[0] == '0' && isdigit(width_spec[1]) );
            if(has_fill) {
                fill = width_spec[0];
                ++colon;
            }
        }
        if(is_dot) {
            width = atoi(field.substr(colon + 1, dot - (colon + 1)).c_str());
            precision = atoi(field.substr(dot + 1).c_str());
        } else {
            width = atoi(field.substr(colon + 1).c_str());
        }
    }
}

std::string Formatter::Field::basic_repr(const std::string & repr) const
{
    if(width > 0) {
        std::ostringstream out;
        out.fill(fill);
        out << std::setw(width) << repr;
        return out.str();
    }
    return repr;
}

std::string Formatter::Field::double_repr(double value) const
{
    std::ostringstream out;
    if(width > 0) {
        out.fill(fill);
        out.width(width);
    }
    if(precision > 0) {
        out.setf(std::ios::fixed);
        out.precision(precision);
    }
    out << value;
    return out.str();
}

//------------------------------------------------------------------------------

Format::Data::Data(const std::string & pattern, FormatAction::Action action_after)
    : refCount(1), formatter(pattern), action(action_after)
{
}

Format::Format(const std::string & pattern, FormatAction::Action action_after)
    : data(new Data(pattern, action_after))
{
}

Format::Format(const Format & other)
    : data(other.data)
{
    data->refCount++;
}

Format & Format::operator=(const Format & other)
{
    data->refCount--;
    if(data->refCount <= 0) {
        data->action(data->formatter.str());
        delete data;
    }

    data = other.data;
    data->refCount++;
    return *this;
}

Format::~Format()
{
    data->refCount--;
    if(data->refCount <= 0) {
        data->action(data->formatter.str());
        delete data;
    }
}

Format::operator std::string() const
{
    return str();
}

const std::string & Format::str() const
{
    return data->formatter.str();
}

//------------------------------------------------------------------------------

void FormatAction::no_action(const std::string & message)
{
}

