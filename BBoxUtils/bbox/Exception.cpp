/**
 * @file
 *
 * Implementation file for the bbox::Exception class.
 */

#include <bbox/Exception.h>
#include <bbox/Format.h>

namespace bbox {

    Exception::Exception()
        : m_what("Unknown")
    {
    }

    Exception::Exception(const Error &err)
        : m_what(Format("Error %s", err))
    {
    }

    Exception::Exception(const std::string &_what)
        : m_what(_what)
    {
    }

    Exception::Exception(std::string &&_what)
        : m_what(std::move(_what))
    {
    }

    Exception::Exception(const char *_what)
        : m_what(_what)
    {
    }

    Exception::Exception(const Error &err, const std::string &description)
        : m_what(Format("%s (Error %s)", description, err))
    {
    }

    Exception::Exception(const Exception &other)
        : m_what(other.m_what)
    {
    }
    
    Exception::Exception(Exception &&other)
        : m_what(std::move(other.m_what))
    {
    }
    
    Exception::~Exception()
    {
    }
    
    Exception & Exception::operator =(const Exception &other)
    {
        if (&other != this)
        {
            m_what = other.m_what;
        }

        return *this;
    }

    Exception & Exception::operator =(Exception &&other)
    {
        if (&other != this)
        {
            m_what = std::move(other.m_what);
        }

        return *this;
    }

    const char * Exception::what() const BOOST_SYSTEM_NOEXCEPT
    {
        return m_what.c_str();
    }

} // namespace bbox
