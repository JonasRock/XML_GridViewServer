#ifndef XMLEXCEPTIONS_H
#define XMLEXCEPTIONS_H

namespace xmlServer
{


struct parserErrorException : public std::exception
{
    const char* what() const throw()
    {
        return "Parsing error";
    }
};

    
}

#endif /* XMLEXCEPTIONS_H */
