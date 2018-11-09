# Project FastNoise (type StaticLibrary)

MODULES += FastNoise

SRCS_DIRECT_FastNoise := $(sort \
        ThirdParty/FastNoise/../../../external/FastNoise/FastNoise.cpp \
    )
    # End SRCS_DIRECT_FastNoise

SRCS_RECURSIVE_FastNoise := $(sort \
        $(SRCS_DIRECT_FastNoise) \
    )
    # End SRCS_RECURSIVE_FastNoise

INCS_DIRECT_FastNoise := $(sort \
        ThirdParty/FastNoise/../../../external/FastNoise \
    )
    # End INCS_DIRECT_FastNoise

INCS_RECURSIVE_FastNoise := $(sort \
        $(INCS_DIRECT_FastNoise) \
    )
    # End INCS_RECURSIVE_FastNoise

# Project LibWS281x (type StaticLibrary)

MODULES += LibWS281x

SRCS_DIRECT_LibWS281x := $(sort \
        ThirdParty/WS281x/Lib/src/dma.c \
        ThirdParty/WS281x/Lib/src/mailbox.c \
        ThirdParty/WS281x/Lib/src/pwm.c \
        ThirdParty/WS281x/Lib/src/rpihw.c \
        ThirdParty/WS281x/Lib/src/ws2811.c \
    )
    # End SRCS_DIRECT_LibWS281x

SRCS_RECURSIVE_LibWS281x := $(sort \
        $(SRCS_DIRECT_LibWS281x) \
    )
    # End SRCS_RECURSIVE_LibWS281x

INCS_DIRECT_LibWS281x := $(sort \
        ThirdParty/WS281x/Lib/src \
    )
    # End INCS_DIRECT_LibWS281x

INCS_RECURSIVE_LibWS281x := $(sort \
        $(INCS_DIRECT_LibWS281x) \
    )
    # End INCS_RECURSIVE_LibWS281x

# Project Lua (type StaticLibrary)

MODULES += Lua

SRCS_DIRECT_Lua := $(sort \
        ThirdParty/Lua/src/lapi.c \
        ThirdParty/Lua/src/lauxlib.c \
        ThirdParty/Lua/src/lbaselib.c \
        ThirdParty/Lua/src/lbitlib.c \
        ThirdParty/Lua/src/lcode.c \
        ThirdParty/Lua/src/lcorolib.c \
        ThirdParty/Lua/src/lctype.c \
        ThirdParty/Lua/src/ldblib.c \
        ThirdParty/Lua/src/ldebug.c \
        ThirdParty/Lua/src/ldo.c \
        ThirdParty/Lua/src/ldump.c \
        ThirdParty/Lua/src/lfunc.c \
        ThirdParty/Lua/src/lgc.c \
        ThirdParty/Lua/src/linit.c \
        ThirdParty/Lua/src/liolib.c \
        ThirdParty/Lua/src/llex.c \
        ThirdParty/Lua/src/lmathlib.c \
        ThirdParty/Lua/src/lmem.c \
        ThirdParty/Lua/src/loadlib.c \
        ThirdParty/Lua/src/lobject.c \
        ThirdParty/Lua/src/lopcodes.c \
        ThirdParty/Lua/src/loslib.c \
        ThirdParty/Lua/src/lparser.c \
        ThirdParty/Lua/src/lstate.c \
        ThirdParty/Lua/src/lstring.c \
        ThirdParty/Lua/src/lstrlib.c \
        ThirdParty/Lua/src/ltable.c \
        ThirdParty/Lua/src/ltablib.c \
        ThirdParty/Lua/src/ltm.c \
        ThirdParty/Lua/src/lundump.c \
        ThirdParty/Lua/src/lvm.c \
        ThirdParty/Lua/src/lzio.c \
    )
    # End SRCS_DIRECT_Lua

SRCS_RECURSIVE_Lua := $(sort \
        $(SRCS_DIRECT_Lua) \
    )
    # End SRCS_RECURSIVE_Lua

INCS_DIRECT_Lua := $(sort \
        ThirdParty/Lua/src \
    )
    # End INCS_DIRECT_Lua

INCS_RECURSIVE_Lua := $(sort \
        $(INCS_DIRECT_Lua) \
    )
    # End INCS_RECURSIVE_Lua

# Project Snappy (type StaticLibrary)

MODULES += Snappy

SRCS_DIRECT_Snappy := $(sort \
        ThirdParty/Snappy/snappy/snappy-sinksource.cc \
        ThirdParty/Snappy/snappy/snappy-stubs-internal.cc \
        ThirdParty/Snappy/snappy/snappy.cc \
    )
    # End SRCS_DIRECT_Snappy

SRCS_RECURSIVE_Snappy := $(sort \
        $(SRCS_DIRECT_Snappy) \
    )
    # End SRCS_RECURSIVE_Snappy

INCS_DIRECT_Snappy := $(sort \
        ThirdParty/Snappy/. \
    )
    # End INCS_DIRECT_Snappy

INCS_RECURSIVE_Snappy := $(sort \
        $(INCS_DIRECT_Snappy) \
    )
    # End INCS_RECURSIVE_Snappy

# Project boost (type StaticLibrary)

MODULES += boost

SRCS_DIRECT_boost := $(sort \
        ThirdParty/boost/boost_1_68_0/libs/chrono/src/chrono.cpp \
        ThirdParty/boost/boost_1_68_0/libs/chrono/src/process_cpu_clocks.cpp \
        ThirdParty/boost/boost_1_68_0/libs/chrono/src/thread_clock.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/c_regex_traits.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/cpp_regex_traits.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/cregex.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/fileiter.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/icu.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/instances.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/posix_api.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/regex.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/regex_debug.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/regex_raw_buffer.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/regex_traits_defaults.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/static_mutex.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/usinstances.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/w32_regex_traits.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/wc_regex_traits.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/wide_posix_api.cpp \
        ThirdParty/boost/boost_1_68_0/libs/regex/src/winstances.cpp \
        ThirdParty/boost/boost_1_68_0/libs/system/src/error_code.cpp \
        ThirdParty/boost/boost_1_68_0/libs/thread/src/future.cpp \
        ThirdParty/boost/boost_1_68_0/libs/thread/src/pthread/once.cpp \
        ThirdParty/boost/boost_1_68_0/libs/thread/src/pthread/thread.cpp \
        ThirdParty/boost/boost_1_68_0/libs/thread/src/tss_null.cpp \
    )
    # End SRCS_DIRECT_boost

SRCS_RECURSIVE_boost := $(sort \
        $(SRCS_DIRECT_boost) \
    )
    # End SRCS_RECURSIVE_boost

INCS_DIRECT_boost := $(sort \
        ThirdParty/boost/boost_1_68_0 \
    )
    # End INCS_DIRECT_boost

INCS_RECURSIVE_boost := $(sort \
        $(INCS_DIRECT_boost) \
    )
    # End INCS_RECURSIVE_boost

# Project json (type StaticLibrary)

MODULES += json

SRCS_DIRECT_json := $(sort \
        ThirdParty/json/json/json.cpp \
    )
    # End SRCS_DIRECT_json

SRCS_RECURSIVE_json := $(sort \
        $(SRCS_DIRECT_json) \
    )
    # End SRCS_RECURSIVE_json

INCS_DIRECT_json := $(sort \
        ThirdParty/json/. \
    )
    # End INCS_DIRECT_json

INCS_RECURSIVE_json := $(sort \
        $(INCS_DIRECT_json) \
    )
    # End INCS_RECURSIVE_json

# Project BBoxUtils (type StaticLibrary)

MODULES += BBoxUtils

SRCS_DIRECT_BBoxUtils := $(sort \
        BBoxUtils/bbox/Base16.cpp \
        BBoxUtils/bbox/Base64.cpp \
        BBoxUtils/bbox/DebugOutput.cpp \
        BBoxUtils/bbox/Error.cpp \
        BBoxUtils/bbox/Exception.cpp \
        BBoxUtils/bbox/FileInfo.cpp \
        BBoxUtils/bbox/FileUtils.cpp \
        BBoxUtils/bbox/Format.cpp \
        BBoxUtils/bbox/MainWrapper.cpp \
        BBoxUtils/bbox/TextCoding.cpp \
        BBoxUtils/bbox/Uuid.cpp \
        BBoxUtils/bbox/crypto/Hash.cpp \
        BBoxUtils/bbox/crypto/HashStream.cpp \
        BBoxUtils/bbox/debug/CoutDebugTarget.cpp \
        BBoxUtils/bbox/debug/DebugProvider.cpp \
        BBoxUtils/bbox/debug/DebugQuery.cpp \
        BBoxUtils/bbox/debug/DebugRoot.cpp \
        BBoxUtils/bbox/debug/DebugStream.cpp \
        BBoxUtils/bbox/debug/DebugTarget.cpp \
        BBoxUtils/bbox/debug/DebugVisitor.cpp \
    )
    # End SRCS_DIRECT_BBoxUtils

SRCS_RECURSIVE_BBoxUtils := $(sort \
        $(SRCS_DIRECT_BBoxUtils) \
        $(SRCS_RECURSIVE_boost) \
    )
    # End SRCS_RECURSIVE_BBoxUtils

INCS_DIRECT_BBoxUtils := $(sort \
        BBoxUtils/. \
    )
    # End INCS_DIRECT_BBoxUtils

INCS_RECURSIVE_BBoxUtils := $(sort \
        $(INCS_DIRECT_BBoxUtils) \
        $(INCS_RECURSIVE_boost) \
    )
    # End INCS_RECURSIVE_BBoxUtils

# Project PiLightTest (type Application)

MODULES += PiLightTest

SRCS_DIRECT_PiLightTest := $(sort \
        Applications/PiLightTest/main.cpp \
    )
    # End SRCS_DIRECT_PiLightTest

SRCS_RECURSIVE_PiLightTest := $(sort \
        $(SRCS_DIRECT_PiLightTest) \
        $(SRCS_RECURSIVE_LibWS281x) \
    )
    # End SRCS_RECURSIVE_PiLightTest

INCS_DIRECT_PiLightTest := $(sort \
        Applications/PiLightTest/. \
    )
    # End INCS_DIRECT_PiLightTest

INCS_RECURSIVE_PiLightTest := $(sort \
        $(INCS_DIRECT_PiLightTest) \
        $(INCS_RECURSIVE_LibWS281x) \
    )
    # End INCS_RECURSIVE_PiLightTest

LIBS_RECURSIVE_PiLightTest := \
    # End LIBS_RECURSIVE_PiLightTest

APPS += PiLightTest

# Project BBoxRt (type StaticLibrary)

MODULES += BBoxRt

SRCS_DIRECT_BBoxRt := $(sort \
        BBoxRt/bbox/rt/ConsoleShutdownService.cpp \
        BBoxRt/bbox/rt/DebugEnable.cpp \
        BBoxRt/bbox/rt/NetworkChangeRef.cpp \
        BBoxRt/bbox/rt/OneShotWork.cpp \
        BBoxRt/bbox/rt/Proactor.cpp \
        BBoxRt/bbox/rt/Resource.cpp \
        BBoxRt/bbox/rt/RunLevel.cpp \
        BBoxRt/bbox/rt/Service.cpp \
        BBoxRt/bbox/rt/ServiceFolder.cpp \
        BBoxRt/bbox/rt/ThreadPool.cpp \
        BBoxRt/bbox/rt/ThreadPoolRef.cpp \
        BBoxRt/bbox/rt/TimeSpan.cpp \
        BBoxRt/bbox/rt/Timer.cpp \
        BBoxRt/bbox/rt/WallClock.cpp \
        BBoxRt/bbox/rt/details/GenericServiceReference.cpp \
        BBoxRt/bbox/rt/details/ResourceBase.cpp \
        BBoxRt/bbox/rt/net/AdapterInfo.cpp \
        BBoxRt/bbox/rt/net/IpAddress.cpp \
        BBoxRt/bbox/rt/net/TcpEndpoint.cpp \
        BBoxRt/bbox/rt/net/UdpEndpoint.cpp \
        BBoxRt/bbox/rt/net/UdpSocket.cpp \
        BBoxRt/bbox/rt/win32/IpHelperAccess.cpp \
        BBoxRt/bbox/rt/win32/NetworkChangeService.cpp \
    )
    # End SRCS_DIRECT_BBoxRt

SRCS_RECURSIVE_BBoxRt := $(sort \
        $(SRCS_DIRECT_BBoxRt) \
        $(SRCS_RECURSIVE_BBoxUtils) \
        $(SRCS_RECURSIVE_boost) \
    )
    # End SRCS_RECURSIVE_BBoxRt

INCS_DIRECT_BBoxRt := $(sort \
        BBoxRt/. \
    )
    # End INCS_DIRECT_BBoxRt

INCS_RECURSIVE_BBoxRt := $(sort \
        $(INCS_DIRECT_BBoxRt) \
        $(INCS_RECURSIVE_BBoxUtils) \
        $(INCS_RECURSIVE_boost) \
    )
    # End INCS_RECURSIVE_BBoxRt

# Project LuaCpp (type StaticLibrary)

MODULES += LuaCpp

SRCS_DIRECT_LuaCpp := $(sort \
        LuaCpp/bbox/LuaState.cpp \
    )
    # End SRCS_DIRECT_LuaCpp

SRCS_RECURSIVE_LuaCpp := $(sort \
        $(SRCS_DIRECT_LuaCpp) \
        $(SRCS_RECURSIVE_BBoxUtils) \
        $(SRCS_RECURSIVE_Lua) \
        $(SRCS_RECURSIVE_boost) \
    )
    # End SRCS_RECURSIVE_LuaCpp

INCS_DIRECT_LuaCpp := $(sort \
        LuaCpp/. \
    )
    # End INCS_DIRECT_LuaCpp

INCS_RECURSIVE_LuaCpp := $(sort \
        $(INCS_DIRECT_LuaCpp) \
        $(INCS_RECURSIVE_BBoxUtils) \
        $(INCS_RECURSIVE_Lua) \
        $(INCS_RECURSIVE_boost) \
    )
    # End INCS_RECURSIVE_LuaCpp

# Project ResourceBuilder (type Application)

MODULES += ResourceBuilder

SRCS_DIRECT_ResourceBuilder := $(sort \
        ResourceBuilder/ResourceBuilder.cpp \
    )
    # End SRCS_DIRECT_ResourceBuilder

SRCS_RECURSIVE_ResourceBuilder := $(sort \
        $(SRCS_DIRECT_ResourceBuilder) \
        $(SRCS_RECURSIVE_BBoxUtils) \
    )
    # End SRCS_RECURSIVE_ResourceBuilder

INCS_DIRECT_ResourceBuilder := $(sort \
        ResourceBuilder/. \
    )
    # End INCS_DIRECT_ResourceBuilder

INCS_RECURSIVE_ResourceBuilder := $(sort \
        $(INCS_DIRECT_ResourceBuilder) \
        $(INCS_RECURSIVE_BBoxUtils) \
    )
    # End INCS_RECURSIVE_ResourceBuilder

LIBS_RECURSIVE_ResourceBuilder := \
        uuid \
        rt \
        pthread \
        crypto \
    # End LIBS_RECURSIVE_ResourceBuilder

APPS += ResourceBuilder

# Project TinyXml2 (type StaticLibrary)

MODULES += TinyXml2

SRCS_DIRECT_TinyXml2 := $(sort \
        ThirdParty/TinyXml2/bbox/xml/Error.cpp \
        ThirdParty/TinyXml2/bbox/xml/NSAwareContext.cpp \
        ThirdParty/TinyXml2/bbox/xml/NSAwareElement.cpp \
        ThirdParty/TinyXml2/tinyxml2/tinyxml2.cpp \
    )
    # End SRCS_DIRECT_TinyXml2

SRCS_RECURSIVE_TinyXml2 := $(sort \
        $(SRCS_DIRECT_TinyXml2) \
        $(SRCS_RECURSIVE_BBoxUtils) \
    )
    # End SRCS_RECURSIVE_TinyXml2

INCS_DIRECT_TinyXml2 := $(sort \
        ThirdParty/TinyXml2/. \
    )
    # End INCS_DIRECT_TinyXml2

INCS_RECURSIVE_TinyXml2 := $(sort \
        $(INCS_DIRECT_TinyXml2) \
        $(INCS_RECURSIVE_BBoxUtils) \
    )
    # End INCS_RECURSIVE_TinyXml2

# Project UnitTest (type StaticLibrary)

MODULES += UnitTest

SRCS_DIRECT_UnitTest := $(sort \
        UnitTest/linux/unittest/UnitTest.cpp \
    )
    # End SRCS_DIRECT_UnitTest

SRCS_RECURSIVE_UnitTest := $(sort \
        $(SRCS_DIRECT_UnitTest) \
        $(SRCS_RECURSIVE_BBoxUtils) \
        $(SRCS_RECURSIVE_boost) \
    )
    # End SRCS_RECURSIVE_UnitTest

INCS_DIRECT_UnitTest := $(sort \
        UnitTest/common \
        UnitTest/linux \
    )
    # End INCS_DIRECT_UnitTest

INCS_RECURSIVE_UnitTest := $(sort \
        $(INCS_DIRECT_UnitTest) \
        $(INCS_RECURSIVE_BBoxUtils) \
        $(INCS_RECURSIVE_boost) \
    )
    # End INCS_RECURSIVE_UnitTest

# Project BBoxEnc (type StaticLibrary)

MODULES += BBoxEnc

SRCS_DIRECT_BBoxEnc := $(sort \
        BBoxEnc/bbox/enc/FromBinary.cpp \
        BBoxEnc/bbox/enc/FromJson.cpp \
        BBoxEnc/bbox/enc/FromTextFormat.cpp \
        BBoxEnc/bbox/enc/FromXml.cpp \
        BBoxEnc/bbox/enc/MsgType.cpp \
        BBoxEnc/bbox/enc/MsgTypeLibrary.cpp \
        BBoxEnc/bbox/enc/ToBinary.cpp \
        BBoxEnc/bbox/enc/ToJson.cpp \
        BBoxEnc/bbox/enc/ToTextFormat.cpp \
        BBoxEnc/bbox/enc/ToXml.cpp \
        BBoxEnc/bbox/enc/api/Method.cpp \
        BBoxEnc/bbox/enc/api/MethodSet.cpp \
        BBoxEnc/bbox/enc/api/Param.cpp \
        BBoxEnc/bbox/enc/api/ParamPack.cpp \
    )
    # End SRCS_DIRECT_BBoxEnc

SRCS_RECURSIVE_BBoxEnc := $(sort \
        $(SRCS_DIRECT_BBoxEnc) \
        $(SRCS_RECURSIVE_BBoxUtils) \
        $(SRCS_RECURSIVE_TinyXml2) \
        $(SRCS_RECURSIVE_boost) \
        $(SRCS_RECURSIVE_json) \
    )
    # End SRCS_RECURSIVE_BBoxEnc

INCS_DIRECT_BBoxEnc := $(sort \
        BBoxEnc/. \
    )
    # End INCS_DIRECT_BBoxEnc

INCS_RECURSIVE_BBoxEnc := $(sort \
        $(INCS_DIRECT_BBoxEnc) \
        $(INCS_RECURSIVE_BBoxUtils) \
        $(INCS_RECURSIVE_TinyXml2) \
        $(INCS_RECURSIVE_boost) \
        $(INCS_RECURSIVE_json) \
    )
    # End INCS_RECURSIVE_BBoxEnc

# Project BBoxHttp (type StaticLibrary)

MODULES += BBoxHttp

SRCS_DIRECT_BBoxHttp := $(sort \
        BBoxHttp/bbox/http/Request.cpp \
        BBoxHttp/bbox/http/ResourceFile.cpp \
        BBoxHttp/bbox/http/ResourceFileSet.cpp \
        BBoxHttp/bbox/http/Response.cpp \
        BBoxHttp/bbox/http/client/ClientRequest.cpp \
        BBoxHttp/bbox/http/client/ClientResponse.cpp \
        BBoxHttp/bbox/http/client/HttpClient.cpp \
        BBoxHttp/bbox/http/client/WebSocketClient.cpp \
        BBoxHttp/bbox/http/server/HttpServer.cpp \
        BBoxHttp/bbox/http/server/RequestHandler.cpp \
        BBoxHttp/bbox/http/server/ServerWebSocket.cpp \
        BBoxHttp/bbox/http/server/details/Connection.cpp \
        BBoxHttp/bbox/http/server/details/WebSocketConnection.cpp \
    )
    # End SRCS_DIRECT_BBoxHttp

SRCS_RECURSIVE_BBoxHttp := $(sort \
        $(SRCS_DIRECT_BBoxHttp) \
        $(SRCS_RECURSIVE_BBoxRt) \
    )
    # End SRCS_RECURSIVE_BBoxHttp

INCS_DIRECT_BBoxHttp := $(sort \
        BBoxHttp/. \
    )
    # End INCS_DIRECT_BBoxHttp

INCS_RECURSIVE_BBoxHttp := $(sort \
        $(INCS_DIRECT_BBoxHttp) \
        $(INCS_RECURSIVE_BBoxRt) \
    )
    # End INCS_RECURSIVE_BBoxHttp

# Project BBoxRt_UnitTests (type UnitTests)

MODULES += BBoxRt_UnitTests

SRCS_DIRECT_BBoxRt_UnitTests := $(sort \
        BBoxRt_UnitTests/BBoxRtNetAddressesAndEndpoints.cpp \
        BBoxRt_UnitTests/BboxRtTimer.cpp \
    )
    # End SRCS_DIRECT_BBoxRt_UnitTests

SRCS_RECURSIVE_BBoxRt_UnitTests := $(sort \
        $(SRCS_DIRECT_BBoxRt_UnitTests) \
        $(SRCS_RECURSIVE_BBoxRt) \
        $(SRCS_RECURSIVE_UnitTest) \
    )
    # End SRCS_RECURSIVE_BBoxRt_UnitTests

INCS_DIRECT_BBoxRt_UnitTests := $(sort \
        BBoxRt_UnitTests/. \
    )
    # End INCS_DIRECT_BBoxRt_UnitTests

INCS_RECURSIVE_BBoxRt_UnitTests := $(sort \
        $(INCS_DIRECT_BBoxRt_UnitTests) \
        $(INCS_RECURSIVE_BBoxRt) \
        $(INCS_RECURSIVE_UnitTest) \
    )
    # End INCS_RECURSIVE_BBoxRt_UnitTests

LIBS_RECURSIVE_BBoxRt_UnitTests := \
        uuid \
        rt \
        pthread \
        crypto \
    # End LIBS_RECURSIVE_BBoxRt_UnitTests

TESTS += BBoxRt_UnitTests

# Project BBoxUtils_UnitTests (type UnitTests)

MODULES += BBoxUtils_UnitTests

SRCS_DIRECT_BBoxUtils_UnitTests := $(sort \
        BBoxUtils_UnitTests/BboxUtilsTextCoding.cpp \
        BBoxUtils_UnitTests/BboxUtilsToFromString.cpp \
        BBoxUtils_UnitTests/BboxUtilsUuid.cpp \
        BBoxUtils_UnitTests/TestBboxUtilsCryptoHashStream.cpp \
    )
    # End SRCS_DIRECT_BBoxUtils_UnitTests

SRCS_RECURSIVE_BBoxUtils_UnitTests := $(sort \
        $(SRCS_DIRECT_BBoxUtils_UnitTests) \
        $(SRCS_RECURSIVE_BBoxUtils) \
        $(SRCS_RECURSIVE_UnitTest) \
    )
    # End SRCS_RECURSIVE_BBoxUtils_UnitTests

INCS_DIRECT_BBoxUtils_UnitTests := $(sort \
        BBoxUtils_UnitTests/. \
    )
    # End INCS_DIRECT_BBoxUtils_UnitTests

INCS_RECURSIVE_BBoxUtils_UnitTests := $(sort \
        $(INCS_DIRECT_BBoxUtils_UnitTests) \
        $(INCS_RECURSIVE_BBoxUtils) \
        $(INCS_RECURSIVE_UnitTest) \
    )
    # End INCS_RECURSIVE_BBoxUtils_UnitTests

LIBS_RECURSIVE_BBoxUtils_UnitTests := \
        uuid \
        rt \
        pthread \
        crypto \
    # End LIBS_RECURSIVE_BBoxUtils_UnitTests

TESTS += BBoxUtils_UnitTests

# Project SolutionGenerator (type Application)

MODULES += SolutionGenerator

SRCS_DIRECT_SolutionGenerator := $(sort \
        SolutionGenerator/SolutionGenerator.cpp \
        SolutionGenerator/bbox/make/Generator.cpp \
        SolutionGenerator/bbox/make/Project.cpp \
        SolutionGenerator/bbox/make/ProjectType.cpp \
        SolutionGenerator/bbox/make/Solution.cpp \
        SolutionGenerator/bbox/make/mf/MakefileGenerator.cpp \
        SolutionGenerator/bbox/make/vs/DodgyXmlGenerator.cpp \
        SolutionGenerator/bbox/make/vs/VisualStudioGenerator.cpp \
    )
    # End SRCS_DIRECT_SolutionGenerator

SRCS_RECURSIVE_SolutionGenerator := $(sort \
        $(SRCS_DIRECT_SolutionGenerator) \
        $(SRCS_RECURSIVE_BBoxUtils) \
        $(SRCS_RECURSIVE_LuaCpp) \
    )
    # End SRCS_RECURSIVE_SolutionGenerator

INCS_DIRECT_SolutionGenerator := $(sort \
        SolutionGenerator/. \
    )
    # End INCS_DIRECT_SolutionGenerator

INCS_RECURSIVE_SolutionGenerator := $(sort \
        $(INCS_DIRECT_SolutionGenerator) \
        $(INCS_RECURSIVE_BBoxUtils) \
        $(INCS_RECURSIVE_LuaCpp) \
    )
    # End INCS_RECURSIVE_SolutionGenerator

LIBS_RECURSIVE_SolutionGenerator := \
        uuid \
        rt \
        pthread \
        crypto \
    # End LIBS_RECURSIVE_SolutionGenerator

APPS += SolutionGenerator

# Project BBoxEnc_UnitTests (type UnitTests)

MODULES += BBoxEnc_UnitTests

SRCS_DIRECT_BBoxEnc_UnitTests := $(sort \
        BBoxEnc_UnitTests/bbox/enc/unittest/TestEnc.cpp \
    )
    # End SRCS_DIRECT_BBoxEnc_UnitTests

SRCS_RECURSIVE_BBoxEnc_UnitTests := $(sort \
        $(SRCS_DIRECT_BBoxEnc_UnitTests) \
        $(SRCS_RECURSIVE_BBoxEnc) \
        $(SRCS_RECURSIVE_UnitTest) \
    )
    # End SRCS_RECURSIVE_BBoxEnc_UnitTests

INCS_DIRECT_BBoxEnc_UnitTests := $(sort \
        BBoxEnc_UnitTests/. \
    )
    # End INCS_DIRECT_BBoxEnc_UnitTests

INCS_RECURSIVE_BBoxEnc_UnitTests := $(sort \
        $(INCS_DIRECT_BBoxEnc_UnitTests) \
        $(INCS_RECURSIVE_BBoxEnc) \
        $(INCS_RECURSIVE_UnitTest) \
    )
    # End INCS_RECURSIVE_BBoxEnc_UnitTests

LIBS_RECURSIVE_BBoxEnc_UnitTests := \
        uuid \
        rt \
        pthread \
        crypto \
    # End LIBS_RECURSIVE_BBoxEnc_UnitTests

TESTS += BBoxEnc_UnitTests

# Project BBoxHttpDebug (type StaticLibrary)

MODULES += BBoxHttpDebug

SRCS_DIRECT_BBoxHttpDebug := $(sort \
        BBoxHttpDebug/Lib/bbox/http/debug/HttpDebugWebsite.cpp \
        BBoxHttpDebug/Lib/bbox/http/debug/Resources.cpp \
        BBoxHttpDebug/Lib/bbox/http/debug/msgs/DebugReportNotification.cpp \
        BBoxHttpDebug/Lib/bbox/http/debug/msgs/EnableRequest.cpp \
        BBoxHttpDebug/Lib/bbox/http/debug/msgs/QueryRequest.cpp \
        BBoxHttpDebug/Lib/bbox/http/debug/msgs/QueryResponse.cpp \
        BBoxHttpDebug/Lib/bbox/http/debug/msgs/QueryResponseChild.cpp \
        BBoxHttpDebug/Lib/bbox/http/debug/msgs/QueryResponseEntry.cpp \
    )
    # End SRCS_DIRECT_BBoxHttpDebug

SRCS_RECURSIVE_BBoxHttpDebug := $(sort \
        $(SRCS_DIRECT_BBoxHttpDebug) \
        $(SRCS_RECURSIVE_BBoxEnc) \
        $(SRCS_RECURSIVE_BBoxHttp) \
    )
    # End SRCS_RECURSIVE_BBoxHttpDebug

INCS_DIRECT_BBoxHttpDebug := $(sort \
        BBoxHttpDebug/Lib/. \
    )
    # End INCS_DIRECT_BBoxHttpDebug

INCS_RECURSIVE_BBoxHttpDebug := $(sort \
        $(INCS_DIRECT_BBoxHttpDebug) \
        $(INCS_RECURSIVE_BBoxEnc) \
        $(INCS_RECURSIVE_BBoxHttp) \
    )
    # End INCS_RECURSIVE_BBoxHttpDebug

BBoxHttpDebug/Lib/bbox/http/debug/Resources.cpp BBoxHttpDebug/Lib/bbox/http/debug/Resources.h: build/resourcebuilder BBoxHttpDebug/Lib/../Web/app.css BBoxHttpDebug/Lib/../Web/app.js BBoxHttpDebug/Lib/../Web/index.html
	cd BBoxHttpDebug/Lib && ../../build/resourcebuilder -i "../Web/app.css;../Web/app.js;../Web/index.html" -o "bbox/http/debug/Resources.cpp;bbox/http/debug/Resources.h" -n "bbox::http::debug" -r "../Web" -a "debug/"

build/BBoxHttpDebug/Lib/bbox/http/debug/HttpDebugWebsite.o: BBoxHttpDebug/Lib/bbox/http/debug/Resources.cpp BBoxHttpDebug/Lib/bbox/http/debug/Resources.h

# Project LibLeds (type StaticLibrary)

MODULES += LibLeds

SRCS_DIRECT_LibLeds := $(sort \
        Applications/LibLeds/leds/Color.cpp \
        Applications/LibLeds/leds/FadecandyClient.cpp \
        Applications/LibLeds/leds/GpioClient.cpp \
        Applications/LibLeds/leds/HsvColor.cpp \
    )
    # End SRCS_DIRECT_LibLeds

SRCS_RECURSIVE_LibLeds := $(sort \
        $(SRCS_DIRECT_LibLeds) \
        $(SRCS_RECURSIVE_BBoxEnc) \
        $(SRCS_RECURSIVE_BBoxRt) \
        $(SRCS_RECURSIVE_LibWS281x) \
    )
    # End SRCS_RECURSIVE_LibLeds

INCS_DIRECT_LibLeds := $(sort \
        Applications/LibLeds/. \
    )
    # End INCS_DIRECT_LibLeds

INCS_RECURSIVE_LibLeds := $(sort \
        $(INCS_DIRECT_LibLeds) \
        $(INCS_RECURSIVE_BBoxEnc) \
        $(INCS_RECURSIVE_BBoxRt) \
        $(INCS_RECURSIVE_LibWS281x) \
    )
    # End INCS_RECURSIVE_LibLeds

# Project MiamiNightsMsgs (type StaticLibrary)

MODULES += MiamiNightsMsgs

SRCS_DIRECT_MiamiNightsMsgs := $(sort \
        Applications/MiamiNightsMsgs/mn/msgs/ButtonColors.cpp \
        Applications/MiamiNightsMsgs/mn/msgs/ButtonStates.cpp \
        Applications/MiamiNightsMsgs/mn/msgs/EmulatorCompleted.cpp \
        Applications/MiamiNightsMsgs/mn/msgs/RetransmitRequired.cpp \
        Applications/MiamiNightsMsgs/mn/msgs/StartEmulator.cpp \
    )
    # End SRCS_DIRECT_MiamiNightsMsgs

SRCS_RECURSIVE_MiamiNightsMsgs := $(sort \
        $(SRCS_DIRECT_MiamiNightsMsgs) \
        $(SRCS_RECURSIVE_BBoxEnc) \
    )
    # End SRCS_RECURSIVE_MiamiNightsMsgs

INCS_DIRECT_MiamiNightsMsgs := $(sort \
        Applications/MiamiNightsMsgs/. \
    )
    # End INCS_DIRECT_MiamiNightsMsgs

INCS_RECURSIVE_MiamiNightsMsgs := $(sort \
        $(INCS_DIRECT_MiamiNightsMsgs) \
        $(INCS_RECURSIVE_BBoxEnc) \
    )
    # End INCS_RECURSIVE_MiamiNightsMsgs

# Project CampLight (type Application)

MODULES += CampLight

SRCS_DIRECT_CampLight := $(sort \
        Applications/CampLight/camplight/Application.cpp \
        Applications/CampLight/camplight/Resources.cpp \
        Applications/CampLight/camplight/factory/PatternFactory.cpp \
        Applications/CampLight/camplight/factory/SequenceFactory.cpp \
        Applications/CampLight/camplight/factory/TransitionFactory.cpp \
        Applications/CampLight/camplight/font/Jdpage_8x8.cpp \
        Applications/CampLight/camplight/pattern/DiagonalRainbow.cpp \
        Applications/CampLight/camplight/pattern/Solid.cpp \
        Applications/CampLight/camplight/pattern/TestPattern.cpp \
        Applications/CampLight/camplight/pattern/Text.cpp \
        Applications/CampLight/camplight/pattern/TransitionPattern.cpp \
        Applications/CampLight/camplight/render/Color.cpp \
        Applications/CampLight/camplight/render/ColorGrid.cpp \
        Applications/CampLight/camplight/render/HsvColor.cpp \
        Applications/CampLight/camplight/render/Pattern.cpp \
        Applications/CampLight/camplight/render/Renderer.cpp \
        Applications/CampLight/camplight/render/Sequence.cpp \
        Applications/CampLight/camplight/render/SequenceEntry.cpp \
        Applications/CampLight/camplight/render/Transition.cpp \
        Applications/CampLight/camplight/transition/Immediate.cpp \
        Applications/CampLight/camplight/transition/LinearFade.cpp \
        Applications/CampLight/camplight/transition/RandomDither.cpp \
        Applications/CampLight/camplight/transition/Swipe.cpp \
    )
    # End SRCS_DIRECT_CampLight

SRCS_RECURSIVE_CampLight := $(sort \
        $(SRCS_DIRECT_CampLight) \
        $(SRCS_RECURSIVE_BBoxEnc) \
        $(SRCS_RECURSIVE_BBoxHttp) \
        $(SRCS_RECURSIVE_BBoxHttpDebug) \
        $(SRCS_RECURSIVE_LibWS281x) \
        $(SRCS_RECURSIVE_MiamiNightsMsgs) \
    )
    # End SRCS_RECURSIVE_CampLight

INCS_DIRECT_CampLight := $(sort \
        Applications/CampLight/. \
    )
    # End INCS_DIRECT_CampLight

INCS_RECURSIVE_CampLight := $(sort \
        $(INCS_DIRECT_CampLight) \
        $(INCS_RECURSIVE_BBoxEnc) \
        $(INCS_RECURSIVE_BBoxHttp) \
        $(INCS_RECURSIVE_BBoxHttpDebug) \
        $(INCS_RECURSIVE_LibWS281x) \
        $(INCS_RECURSIVE_MiamiNightsMsgs) \
    )
    # End INCS_RECURSIVE_CampLight

LIBS_RECURSIVE_CampLight := \
        uuid \
        rt \
        pthread \
        crypto \
    # End LIBS_RECURSIVE_CampLight

Applications/CampLight/camplight/Resources.cpp Applications/CampLight/camplight/Resources.h: build/resourcebuilder Applications/CampLight/camplight/resources/app.css Applications/CampLight/camplight/resources/app.js Applications/CampLight/camplight/resources/apple-touch-icon-120x120.png Applications/CampLight/camplight/resources/apple-touch-icon-152x152.png Applications/CampLight/camplight/resources/apple-touch-icon-180x180.png Applications/CampLight/camplight/resources/apple-touch-icon-192x192.png Applications/CampLight/camplight/resources/favicon.ico Applications/CampLight/camplight/resources/index.html
	cd Applications/CampLight && ../../build/resourcebuilder -i "camplight/resources/app.css;camplight/resources/app.js;camplight/resources/apple-touch-icon-120x120.png;camplight/resources/apple-touch-icon-152x152.png;camplight/resources/apple-touch-icon-180x180.png;camplight/resources/apple-touch-icon-192x192.png;camplight/resources/favicon.ico;camplight/resources/index.html" -o "camplight/Resources.cpp;camplight/Resources.h" -n "camplight" -r "camplight\resources"

build/Applications/CampLight/camplight/Application.o: Applications/CampLight/camplight/Resources.cpp Applications/CampLight/camplight/Resources.h

APPS += CampLight

# Project LedSigns (type Application)

MODULES += LedSigns

SRCS_DIRECT_LedSigns := $(sort \
        Applications/LedSigns/ledsigns/Application.cpp \
        Applications/LedSigns/ledsigns/RenderService.cpp \
        Applications/LedSigns/ledsigns/buttonbox/ButtonBoxLayout.cpp \
        Applications/LedSigns/ledsigns/buttonbox/ButtonBoxPattern.cpp \
        Applications/LedSigns/ledsigns/casadelshade/CasaDelShadeLayout.cpp \
        Applications/LedSigns/ledsigns/casadelshade/FlickeringNeonPattern.cpp \
        Applications/LedSigns/ledsigns/casadelshade/PulsePattern.cpp \
        Applications/LedSigns/ledsigns/casadelshade/StaggerPattern.cpp \
        Applications/LedSigns/ledsigns/common/CountPattern.cpp \
        Applications/LedSigns/ledsigns/common/CyclePattern.cpp \
        Applications/LedSigns/ledsigns/common/FadeTransition.cpp \
        Applications/LedSigns/ledsigns/common/Gradient.cpp \
        Applications/LedSigns/ledsigns/common/GrowingCirclesPattern.cpp \
        Applications/LedSigns/ledsigns/common/Layout.cpp \
        Applications/LedSigns/ledsigns/common/NoisePattern.cpp \
        Applications/LedSigns/ledsigns/common/Pattern.cpp \
        Applications/LedSigns/ledsigns/common/SolidPattern.cpp \
        Applications/LedSigns/ledsigns/common/SwipeTestPattern.cpp \
        Applications/LedSigns/ledsigns/common/SwipeTransition.cpp \
        Applications/LedSigns/ledsigns/common/TransitionPattern.cpp \
        Applications/LedSigns/ledsigns/common/VectorGradient.cpp \
        Applications/LedSigns/ledsigns/common/WavePattern.cpp \
        Applications/LedSigns/ledsigns/gaysign/GaySignLayout.cpp \
        Applications/LedSigns/ledsigns/gaysign/GaySignPattern.cpp \
    )
    # End SRCS_DIRECT_LedSigns

SRCS_RECURSIVE_LedSigns := $(sort \
        $(SRCS_DIRECT_LedSigns) \
        $(SRCS_RECURSIVE_BBoxHttp) \
        $(SRCS_RECURSIVE_BBoxHttpDebug) \
        $(SRCS_RECURSIVE_FastNoise) \
        $(SRCS_RECURSIVE_LibLeds) \
        $(SRCS_RECURSIVE_MiamiNightsMsgs) \
    )
    # End SRCS_RECURSIVE_LedSigns

INCS_DIRECT_LedSigns := $(sort \
        Applications/LedSigns/. \
    )
    # End INCS_DIRECT_LedSigns

INCS_RECURSIVE_LedSigns := $(sort \
        $(INCS_DIRECT_LedSigns) \
        $(INCS_RECURSIVE_BBoxHttp) \
        $(INCS_RECURSIVE_BBoxHttpDebug) \
        $(INCS_RECURSIVE_FastNoise) \
        $(INCS_RECURSIVE_LibLeds) \
        $(INCS_RECURSIVE_MiamiNightsMsgs) \
    )
    # End INCS_RECURSIVE_LedSigns

LIBS_RECURSIVE_LedSigns := \
        uuid \
        rt \
        pthread \
        crypto \
    # End LIBS_RECURSIVE_LedSigns

APPS += LedSigns

