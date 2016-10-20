/**
* @file
*
* Header file for the bbox::LuaState class.
*/

#ifndef __BBOX__LUA_STATE_H__
#define __BBOX__LUA_STATE_H__

#pragma once

#include <lua.h>
#include <lualib.h>

#include <bbox/Error.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace bbox {

    /**
     * Represents a RAII reference to a Lua state
     * managed in C++.
     *
     * A number of helper functions are provided,
     * but this is not a complete wrapper of
     * the Lua API - you still need to be aware of
     * the C API and it's concepts - for example
     * the Stack.
     *
     * This class just simplifies using the
     * API with exceptions (when compiled as C++)
     * as it provides the RAII state, and
     * simplifies wrapping functions and
     * C++ class methods in the correct
     * exception handling state as exceptions
     * cannot pass the Lua/C boundary.
     *
     * Note that for all functions that push
     * an error string (e.g. lua_load, lua_pcall etc.)
     * this error string is automatically extracted and
     * removed from the stack in the case of error -
     * so these functions either succeed and push on
     * results, or fail and leave the stack unchanged.
     */
    class LuaState
    {
        // Is not copyable
        LuaState(const LuaState &) = delete;
        void operator =(const LuaState &) = delete;

        // Wish we could make it moveable - but
        // the lua library doesn't allow us to update
        // the alloc user data pointer
        LuaState(LuaState &&) = delete;
        void operator =(LuaState &&) = delete;

    public:

        /**
         * Constructor.
         */
        LuaState();

        /**
         * Destructor.
         */
        ~LuaState();

        /**
         * Converts a Lua C API error code into
         * a bbox::Error instance.
         */
        static Error ErrorFromCode(int code);

        /**
         * Gets the state pointer associated with this Lua state.
         */
        lua_State *GetState()
        {
            return m_state;
        }

        /**
         * Loads the standard Lua libraries into the state.
         *
         * Should only be called when the state is first
         * constructed.
         */
        void LoadStandardLibraries();

        /**
         * Registers the specified function and pushes
         * the function value onto the top.
         *
         * A copy of the provided boost::function
         * is stored in user-data associated with the
         * function value, and is subject to
         * normal Lua garbage collection.
         *
         * The function is run as a normal
         * Lua function - i.e. arguments are on the Lua stack,
         * and you need to return the number of
         * return values left on the Lua stack.
         *
         * Up values are not supported - store
         * associated values directly in the
         * boost::function (through boost::bind)
         * rather than in Lua up values.
         *
         * All C++ exceptions are caught
         * and raised as Lua exceptions. This
         * behaviour is required by the Lua runtime.
         */
        void PushFunction(const std::string &name, boost::function<int(LuaState &)> &&function);

        /**
         * Stores a function into the global table.
         *
         * Equivilent to a call to PushFunction
         * and then storing that pushed value into
         * the global table with the specified value.
         */
        void RegisterGlobalFunction(const std::string &name, boost::function<int(LuaState &)> &&function);

        /**
         * Loads the specified file and places it as the top
         * member of the stack.
         *
         * @param error_str   receives the error message in case of
         *                    an error.
         * @param module_name the name of the module, for debugging
         *                    and error handling.
         * @param module_text the text to compile.
         *
         * @return An error, or success.
         */
        Error CompileText(std::string &error_str,
                          const std::string &module_name,
                          const std::string &module_text);

        /**
         * Loads the specified file and places it as the top
         * member of the stack, or throws a LuaException
         * if an error occurs.
         *
         * @param module_name the name of the module, for debugging
         *                    and error handling.
         * @param module_text the text to compile.
         */
        void CompileTextOrThrow(const std::string &module_name,
                                const std::string &module_text);

        /**
         * Calls a Lua function - equivilent to
         * calling lua_pcall. An error code is returned,
         * and if an error occurs, the error_str variable
         * is filled in with the Lua error details.
         *
         * @param error_str string to receive the Lua error details (if any).
         * @param nargs the number of arguments on the stack - see lua_pcall.
         * @param nresults the number of results to return - see lua_pcall.
         * @param msgh the messaging handler index - see lua_pcall.
         * @return an error code, or success.
         */
        Error PCall(std::string &error_str, int nargs, int nresults, int msgh = 0);

        /**
         * Calls a Lua function - equivilent to calling
         * lua_pcall. A LuaException is raised if any
         * error occurs while running the function.
         *
         * @param nargs the number of arguments on the stack - see lua_pcall.
         * @param nresults the number of results to return - see lua_pcall.
         * @param msgh the messaging handler index - see lua_pcall.
         */
        void PCallOrThrow(int nargs, int nresults, int msgh = 0);

        /**
         * Checks the number of arguments to the function are
         * exactly the provided number.
         */
        void ArgumentCountExactlyOrThrow(int num_args);

        /**
         * Converts a lua argument to a string value, or
         * throws an exception if not possible.
         */
        std::string ArgumentToStringOrThrow(int arg_num);

    private:

        class RootDetails;
        class RegisteredFunction;

        static void *Alloc(void *ud, void *ptr, size_t osize, size_t nsize);

        LuaState(lua_State *state, RootDetails *root_details);
        void ExtractPushedError(const Error &err, std::string &error_str);

        lua_State *m_state;
        RootDetails *m_details;
    };

} // namespace bbox

#endif // __BBOX__LUA_STATE_H__