/**
 * @file
 *
 * Header file for the bbox::LuaException class.
 */

#ifndef __BBOX__LUA_EXCEPTION_H__
#define __BBOX__LUA_EXCEPTION_H__

#pragma once

#include <bbox/Exception.h>

namespace bbox {

    class LuaException :
        public Exception
    {
    public:
        LuaException()                                                  : Exception() {}
        explicit LuaException(const Error &err)                         : Exception(err) {}
        explicit LuaException(const std::string &_what)                 : Exception(_what) {}
        explicit LuaException(std::string &&_what)                      : Exception(std::move(_what)) {}
        LuaException(const Error &err, const std::string &description)  : Exception(err, description) {}
    };

} // namespace bbox

#endif // __BBOX__LUA_EXCEPTION_H__
