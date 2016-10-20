/**
 * @file
 *
 * Header file for the bbox::Exception class.
 */

#ifndef __BBOX__EXCEPTION_H__
#define __BBOX__EXCEPTION_H__

#pragma once

#include <string>
#include <exception>

#include <bbox/Error.h>

namespace bbox {

    class Exception :
        public std::exception
    {
    public:
        Exception();

        explicit Exception(const Error &err);
        explicit Exception(const std::string &_what);
        explicit Exception(std::string &&_what);
        explicit Exception(const char *_what);
        Exception(const Error &err, const std::string &description);

        Exception(const Exception &other);
        Exception(Exception &&other);
        virtual ~Exception();
        Exception &operator =(const Exception &other);
        Exception &operator =(Exception &&other);

        virtual const char *what() const BOOST_SYSTEM_NOEXCEPT override;

        const Error &error() const { return m_error; }

    private:
        Error m_error;
        std::string m_what;
    };

} // namespace bbox

#endif // __BBOX__EXCEPTION_H__
