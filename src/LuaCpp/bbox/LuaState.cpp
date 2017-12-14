/**
* @file
*
* Implmentation file for the bbox::LuaState class.
*/

#include <bbox/Assert.h>
#include <bbox/Format.h>
#include <bbox/TypeInfo.h>
#include <bbox/LuaException.h>
#include <bbox/LuaState.h>

#include <stdlib.h>
#include <iostream>
#include <set>

#include <lauxlib.h>
#include <lstate.h>

namespace {

    class lua_reader_state
    {
        friend const char *lua_reader_func(lua_State *L, void *_state, size_t *size);

    public:
        explicit lua_reader_state(const std::string &_text)
            : text(_text)
            , started(false)
        {
        }

    private:
        const std::string &text;
        bool started;
    };

    const char *
    lua_reader_func(lua_State *L, void *_state, size_t *size)
    {
        // Convert the state pointer into our state type

        lua_reader_state *state = static_cast<lua_reader_state *>(_state);

        // First not started, then return the entire string
        // and mark us as done

        if (!state->started)
        {
            state->started = true;
            *size = state->text.size();
            return state->text.c_str();
        }

        // Else, just return done

        *size = 0;
        return nullptr;
    }

    class LuaErrorCategory : public boost::system::error_category
    {
    public:
        virtual const char* name() const BOOST_SYSTEM_NOEXCEPT { return "Lua"; }

        virtual boost::system::error_condition default_error_condition(int ev) const BOOST_SYSTEM_NOEXCEPT
        {
            std::errc default_code = std::errc::result_out_of_range;

            switch (ev)
            {
            case LUA_OK:            default_code = std::errc(0);                        break;
            case LUA_YIELD:         default_code = std::errc::interrupted;              break;
            case LUA_ERRRUN:        default_code = std::errc::protocol_error;           break;
            case LUA_ERRSYNTAX:     default_code = std::errc::wrong_protocol_type;      break;
            case LUA_ERRMEM:        default_code = std::errc::not_enough_memory;        break;
            case LUA_ERRGCMM:       default_code = std::errc::state_not_recoverable;    break;
            case LUA_ERRERR:        default_code = std::errc::operation_would_block;    break;
            }

            return boost::system::error_condition(int(default_code), boost::system::generic_category());
        }

        virtual std::string message(int ev) const
        {
            const char *message_ptr = "Unknown";

            switch (ev)
            {
            case LUA_OK:            message_ptr = "OK";         break;
            case LUA_YIELD:         message_ptr = "Yield";      break;
            case LUA_ERRRUN:        message_ptr = "ErrRun";     break;
            case LUA_ERRSYNTAX:     message_ptr = "ErrSyntax";  break;
            case LUA_ERRMEM:        message_ptr = "ErrMem";     break;
            case LUA_ERRGCMM:       message_ptr = "ErrGcMM";    break;
            case LUA_ERRERR:        message_ptr = "ErrErr";     break;
            }

            return message_ptr;
        }
    };
    
    static LuaErrorCategory &LuaCategory()
    {
        static LuaErrorCategory instance;

        return instance;
    }

} // annonymous namespace

namespace bbox {

    class LuaState::RootDetails
    {
    public:
        explicit RootDetails(LuaState *root)
            : m_root(root)
            , m_blocks_allocated(0)
            , m_bytes_allocated(0)
        {
        }

        LuaState * const m_root;
        size_t m_blocks_allocated;
        size_t m_bytes_allocated;
        size_t m_function_metatable_addr;
        std::set<LuaState::RegisteredFunction *> m_functions;
    };

    class LuaState::RegisteredFunction
    {
        friend class LuaState;

        // Not copyable or moveable
        RegisteredFunction(const RegisteredFunction &) = delete;
        RegisteredFunction(RegisteredFunction &&) = delete;
        void operator =(const RegisteredFunction &) = delete;
        void operator =(RegisteredFunction &&) = delete;

    public:
        RegisteredFunction(RootDetails *details, const std::string &name, boost::function<int (LuaState &)> &&function);
        ~RegisteredFunction();

        static int __gc_function(lua_State *state);
        static int __call_function(lua_State *state);

    private:
        RootDetails *m_details;
        std::string m_name;
        boost::function<int(LuaState &)> m_function;
    };

    LuaState::LuaState()
        : m_state(nullptr)
        , m_details(nullptr)
    {
        // Create the root details for us

        m_details = new RootDetails(this);

        // Create a default lua state

        m_state = lua_newstate(&LuaState::Alloc, m_details);

        BBOX_ASSERT(m_state != nullptr);

        // Create the function metatable that specifies
        // the "__gc", "__call" and "__metatable" handlers.

        {
            // Create the new table

            lua_createtable(m_state, 0, 2);

            BBOX_ASSERT(lua_gettop(m_state) == 1);

            // Set the "__gc" member to the
            // RegisteredFunction::gc_function
            // method

            lua_pushstring(m_state, "__gc");
            lua_pushcfunction(m_state, &RegisteredFunction::__gc_function);
            lua_rawset(m_state, 1);

            // Set the "__call" member to the
            // RegisteredFunction::__call_function
            // method

            lua_pushstring(m_state, "__call");
            lua_pushcfunction(m_state, &RegisteredFunction::__call_function);
            lua_rawset(m_state, 1);

            // Set the "__metatable" field to
            // a null value

            lua_pushstring(m_state, "__metatable");
            lua_pushnil(m_state);
            lua_rawset(m_state, 1);

            // Store it into the registry
            // using the address as an index

            lua_rawsetp(m_state, LUA_REGISTRYINDEX, &m_details->m_function_metatable_addr);

            BBOX_ASSERT(lua_gettop(m_state) == 0);
        }
    }

    LuaState::LuaState(lua_State *state, RootDetails *root_details)
        : m_state(state)
        , m_details(root_details)
    {
    }

    LuaState::~LuaState()
    {
        BBOX_ASSERT(m_state);
        BBOX_ASSERT(m_details);

        if (m_details->m_root == this)
        {
            lua_close(m_state);

            BBOX_ASSERT(m_details->m_blocks_allocated == 0);
            BBOX_ASSERT(m_details->m_bytes_allocated == 0);
            BBOX_ASSERT(m_details->m_functions.empty());

            delete m_details;
        }

        m_state = nullptr;
        m_details = nullptr;
    }

    void *
    LuaState::Alloc(void *ud, void *ptr, size_t osize, size_t nsize)
    {
        // Convert the state pointer into the LuaState::RootDetails C++ object

        RootDetails *details = static_cast<RootDetails *>(ud);
        BBOX_ASSERT(details);

        // If already allocated, remove the old size

        if ((ptr != nullptr) && (osize != 0))
        {
            details->m_blocks_allocated--;
            details->m_bytes_allocated -= osize;
        }

        // Use re-alloc to allocated the requested new size

        void *result = realloc(ptr, nsize);

        // If now allocated, add the new size

        if ((result != nullptr) && (nsize != 0))
        {
            details->m_blocks_allocated++;
            details->m_bytes_allocated += nsize;
        }

        //std::cout << "LuaState::Alloc " << ptr << " " << osize << " " << nsize << " => " << result << ", total " << details->m_blocks_allocated << '/' << details->m_bytes_allocated << std::endl;

        return result;
    }

    Error
    LuaState::ErrorFromCode(int code)
    {
        return boost::system::error_code(code, LuaCategory());
    }

    void
    LuaState::LoadStandardLibraries()
    {
        luaL_openlibs(m_state);
    }

    Error
    LuaState::CompileText(std::string &error_str,
                          const std::string &module_name,
                          const std::string &module_text)
    {
        error_str.clear();

        lua_reader_state reader_state(module_text);

        Error err = ErrorFromCode(
           lua_load(m_state, &lua_reader_func, &reader_state, module_name.c_str(), "t"));

        ExtractPushedError(err, error_str);

        return err;
    }

    void
    LuaState::CompileTextOrThrow(const std::string &module_name,
                                 const std::string &module_text)
    {
        std::string error_str;

        Error err = CompileText(error_str, module_name, module_text);

        if (err)
        {
            throw LuaException(err, Format("Error compiling Lua module \"%s\" - %s", module_name, error_str));
        }
    }


    Error
    LuaState::PCall(std::string &error_str, int nargs, int nresults, int msgh)
    {
        error_str.clear();

        Error err = ErrorFromCode(
            lua_pcall(m_state, nargs, nresults, msgh));

        ExtractPushedError(err, error_str);

        return err;
    }

    void
    LuaState::PCallOrThrow(int nargs, int nresults, int msgh)
    {
        std::string error_str;

        Error err = PCall(error_str, nargs, nresults, msgh);

        if (err)
        {
            throw LuaException(err, error_str);
        }
    }

    void
    LuaState::PushFunction(const std::string &name, boost::function<int(LuaState &)> &&function)
    {
        // Check parameters are value

        BBOX_ASSERT(function);
        BBOX_ASSERT(m_state);
        BBOX_ASSERT(m_details);

        // Create a new user-data value, and create
        // a RegisteredFunction into the space, and
        // mark it as not created by clearing
        // the parent value.

        int orig_top = lua_gettop(m_state);

        void *user_data_ptr = lua_newuserdata(m_details->m_root->m_state, sizeof(RegisteredFunction));

        BBOX_ASSERT(lua_gettop(m_state) == (orig_top + 1));

        RegisteredFunction *reg_func = static_cast<RegisteredFunction *>(user_data_ptr);

        reg_func->m_details = nullptr;

        // Set the metadata for the user data to
        // the function metadata. This meta data
        // provides a call handler, as well as
        // a garbage collection handler and
        // prevents Lua code from accessing
        // the metatable. We need to get this
        // out of the registry using the
        // address of the metatable address
        // as an index into the registry

        lua_rawgetp(m_state, LUA_REGISTRYINDEX, &m_details->m_function_metatable_addr);

        BBOX_ASSERT(lua_gettop(m_state) == (orig_top + 2));
        BBOX_ASSERT(lua_type(m_state, orig_top + 2) == LUA_TTABLE);

        lua_setmetatable(m_state, orig_top + 1);

        BBOX_ASSERT(lua_gettop(m_state) == (orig_top + 1));

        // Now that we have the meta-methods set
        // for the function, we can actually go
        // ahead and create the real object
        // in the allocated memory

        reg_func = new (user_data_ptr) RegisteredFunction(m_details, name, std::move(function));
    }

    void
    LuaState::RegisterGlobalFunction(const std::string &name, boost::function<int(LuaState &)> &&function)
    {
        BBOX_ASSERT(m_state);
        BBOX_ASSERT(m_details);

        PushFunction(name, std::move(function));
        lua_setglobal(m_state, name.c_str());
    }

    void
    LuaState::ExtractPushedError(const Error &err, std::string &error_str)
    {
        error_str.clear();

        if (err)
        {
            if (lua_gettop(m_state) == 0)
            {
                error_str = "No error element on Lua stack";
            }
            else if (lua_isstring(m_state, -1))
            {
                error_str = lua_tostring(m_state, -1);
                lua_pop(m_state, 1);
            }
            else
            {
                error_str = Format("Lua error pushed is of type %s, not string", lua_typename(m_state, lua_type(m_state, -1)));
                lua_pop(m_state, 1);
            }
        }
    }

    void
    LuaState::ArgumentCountExactlyOrThrow(int num_args)
    {
        if (lua_gettop(m_state) != num_args)
        {
            luaL_error(m_state, "Expected %d arguments", num_args);
        }
    }

    std::string
    LuaState::ArgumentToStringOrThrow(int arg_num)
    {
        if (lua_gettop(m_state) < arg_num)
        {
            luaL_error(m_state, "Expected at least %d arguments", arg_num + 1);
        }

        if (lua_type(m_state, arg_num) != LUA_TSTRING)
        {
            luaL_error(m_state, "Expected string for argument %d", arg_num + 1);
        }

        size_t arg_size;
        const char *arg_val = lua_tolstring(m_state, arg_num, &arg_size);

        return std::string(arg_val, arg_size);
    }


    LuaState::RegisteredFunction::RegisteredFunction(RootDetails *details, const std::string &name, boost::function<int(LuaState &)> &&function)
        : m_details(details)
        , m_name(name)
        , m_function(std::move(function))
    {
        BBOX_ASSERT(m_details);
        BBOX_ASSERT(m_function);

        auto insert_result = m_details->m_functions.insert(this);

        BBOX_ASSERT(insert_result.second);
    }

    LuaState::RegisteredFunction::~RegisteredFunction()
    {
        BBOX_ASSERT(m_details);

        size_t erase_result = m_details->m_functions.erase(this);

        BBOX_ASSERT(erase_result == 1);

        m_details = nullptr;
        m_function = boost::function<int(LuaState &)>();
    }

    int
    LuaState::RegisteredFunction::__call_function(lua_State *state)
    {
        int result = 0;

        // From the docs for what happens when using the "__call" metamethod,
        // we see that the original "function" object is passed as the first parameter.
        // In our case, this is the user data containing the RegisteredFunction.
        // We'll extract it's contents, remove it from the parameter stack
        // (so the actual C++ function is unaware of it), and then
        // call it.

        RootDetails *root_details = nullptr;
        boost::function<int(LuaState&)> function;
        std::string name;

        BBOX_ASSERT(lua_gettop(state) >= 1);
        BBOX_ASSERT(lua_type(state, 1) == LUA_TUSERDATA);

        RegisteredFunction *reg_func = static_cast<RegisteredFunction *>(lua_touserdata(state, 1));
        BBOX_ASSERT(reg_func);

        if (!reg_func->m_details)
        {
            // The function has already been deleted for some reason???

            return luaL_error(state, "C++ call-back has already been deleted during call");
        }

        root_details = reg_func->m_details;
        function = reg_func->m_function;
        name = reg_func->m_name;

        lua_remove(state, 1);

        // reg_func may now be an invalid pointer
        // if it's been garbage collected

        try
        {
            // Right - we have all of the details -
            // lets just try calling the function
            // and see what happens

            LuaState sub_state(state, root_details);
            result = function(sub_state);
        }
        catch (const std::exception &e)
        {
            TypeInfo ex_type(typeid(*&e));

            return luaL_error(state, "C++ exception (in \"%s\"), type=%s, what=%s", name.c_str(), ex_type.pretty_name().c_str(), e.what());
        }
#ifdef WIN32
        // TODO - get this to work in linux
        catch (struct lua_longjmp *)
        {
            // This seems to be the type that the LUAI_THROW macro throws
            // when compiled in C++ mode - we'll just let that
            // one pass through all the way up

            throw;
        }
#endif
        catch (...)
        {
            return luaL_error(state, "C++ exception (in \"%s\") of unknown non-std::exception type", name.c_str());
        }

        return result;
    }

    int
    LuaState::RegisteredFunction::__gc_function(lua_State *state)
    {
        // We expect there to be one value on the stack,
        // which is a user data that contains
        // a pointer to a LuaState::RegisteredFunction.
        // We need to clear this down - run the destructor etc.

        BBOX_ASSERT(lua_gettop(state) == 1);
        BBOX_ASSERT(lua_type(state, 1) == LUA_TUSERDATA);

        RegisteredFunction *reg_func = static_cast<RegisteredFunction *>(lua_touserdata(state, 1));

        BBOX_ASSERT(reg_func);

        if (reg_func->m_details)
        {
            // We have the details pointer - we'll want to check
            // that we're still registered and then run
            // the destructor

            BBOX_ASSERT(reg_func->m_details->m_functions.find(reg_func)
                        != reg_func->m_details->m_functions.end());

            reg_func->~RegisteredFunction();

            BBOX_ASSERT(reg_func->m_details == nullptr);
        }
        else
        {
            // The parent pointer is null - we'll assume
            // it already been delete or some other problem
        }

        return 0;
    }

} // namespace bbox
