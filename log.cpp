#include "log.h"
#include <fstream>

struct Logger {
    Logger(const std::string filename)
        : out(filename.c_str(), std::fstream::out | std::fstream::app)
    {
        out << std::endl;
        log("STARTED LOG");
    }
    ~Logger()
    {
        log("CLOSED LOG");
    }
    void log(const std::string & message)
    {
        time_t rawtime;
        time(&rawtime);
        char buffer[80];
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&rawtime));
        out << buffer << ": " << message << std::endl;
    }
private:
    std::fstream out;
};

void log_line(const std::string & s)
{
    static Logger logger("blamerl.log");
    logger.log(s);
}

//------------------------------------------------------------------------------

Format log(const std::string & message)
{
    return Format(message, log_line);
}
