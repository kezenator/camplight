#
# Top level Makefile for the BrainBox project
#

.PHONY: all clean distclean help app_all test_all run_test_all

# Make all the default target

all:

# Setup for and include the modules file

APPS :=
TESTS :=
MODULES :=

include modules.make

# Default compiler flags

CFLAGS := 
CXXFLAGS := --std=c++14 -Werror -Wall -Wpedantic -Wno-error=unused-local-typedefs -Wno-unused-local-typedefs
OUTPUT_DIR := build

###################################################
## TODO - put these in the .bbmake files
## just haven't been bothered implementing it
##

$(patsubst %.c,$(OUTPUT_DIR)/%.o,$(SRCS_DIRECT_Lua)): CFLAGS += -x c++ $(CXXFLAGS)
$(patsubst %.cc,$(OUTPUT_DIR)/%.o,$(SRCS_DIRECT_Snappy)): CXXFLAGS += -Wno-error=unknown-pragmas -Wno-error=sign-compare

## END TODO
###################################################

BOOST_VER := 1_59_0

all: app_all test_all
app_all: $(APPS)
test_all: $(TESTS)
run_test_all: $(patsubst %,run_%,$(TESTS))

clean:
	rm -rf $(OUTPUT_DIR)

distclean: clean
	rm -f ThirdParty/boost/boost_$(BOOST_VER)/success
	rm -rf ThirdParty/boost/boost_$(BOOST_VER)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

.PHONY: boost
boost: ThirdParty/boost/boost_$(BOOST_VER)/success

ThirdParty/boost/boost_$(BOOST_VER)/success:
	rm -f ThirdParty/boost/boost_$(BOOST_VER)/success
	rm -rf ThirdParty/boost/boost_$(BOOST_VER)
	cd ThirdParty/boost && tar -vxjf boost_$(BOOST_VER).tar.bz2
	cd ThirdParty/boost/boost_$(BOOST_VER) && ../apply_linux_patches
	touch ThirdParty/boost/boost_$(BOOST_VER)/success
	echo "Boost libraries extracted - please run make again"
	false

$(OUTPUT_DIR)/%.o: %.cpp
	@echo "Compiling $@"
	mkdir -p $(dir $@)
	g++ $(CFLAGS) $(CXXFLAGS) -o $@ -c $< -MMD -MP -MT $@

$(OUTPUT_DIR)/%.o: %.cc
	@echo "Compiling $@"
	mkdir -p $(dir $@)
	g++ $(CFLAGS) $(CXXFLAGS) -o $@ -c $< -MMD -MP -MT $@

$(OUTPUT_DIR)/%.o: %.c
	@echo "Compiling $@"
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -o $@ -c $< -MMD -MP -MT $@

define SRC_TO_OBJ
$(filter %.o,$(patsubst %.c,$(OUTPUT_DIR)/%.o,$(1)) $(patsubst %.cpp,$(OUTPUT_DIR)/%.o,$(1)) $(patsubst %.cc,$(OUTPUT_DIR)/%.o,$(1)))
endef

define GENERATE_MODULE

-include $$(patsubst %.o,%.d,$$(call SRC_TO_OBJ,$$(SRCS_DIRECT_$(1))))

$$(if $$(SRCS_DIRECT_$(1)), $$(call SRC_TO_OBJ,$$(SRCS_DIRECT_$(1))): CFLAGS += $$(patsubst %,-I %/,$(INCS_RECURSIVE_$(1))),)

$$(if $$(SRCS_DIRECT_$(1)), $$(call SRC_TO_OBJ,$$(SRCS_DIRECT_$(1))): Makefile | boost,)
endef

$(foreach module,$(MODULES),$(eval $(call GENERATE_MODULE,$(module))))

define GENERATE_APP
.PHONY: $(1)
$(1) : $(OUTPUT_DIR)/$$(shell echo $(1) | tr [:upper:] [:lower:])

$(OUTPUT_DIR)/$$(shell echo $(1) | tr [:upper:] [:lower:]): $$(call SRC_TO_OBJ,$$(SRCS_RECURSIVE_$(1))) | $(OUTPUT_DIR)
	@echo "Linking $$@"
	g++ -o $$@ $$^ -lssl -lcrypto -luuid -pthread -lrt

endef

define GENERATE_TEST
.PHONY: $(1)
$(1) : $(OUTPUT_DIR)/$$(shell echo $(1) | tr [:upper:] [:lower:])

.PHONY: run_$(1)
run_$(1): $(1)
	$(OUTPUT_DIR)/$$(shell echo $(1) | tr [:upper:] [:lower:])

$(OUTPUT_DIR)/$$(shell echo $(1) | tr [:upper:] [:lower:]): $$(call SRC_TO_OBJ,$$(SRCS_RECURSIVE_$(1))) | $(OUTPUT_DIR)
	@echo "Linking $$@"
	g++ -o $$@ $$^ -lssl -lcrypto -luuid -pthread -lrt

endef

$(foreach app,$(APPS),$(eval $(call GENERATE_APP,$(app))))
$(foreach app,$(TESTS),$(eval $(call GENERATE_TEST,$(app))))

#####################################################################################
##
## HELP
##
##

help:
	@echo Support Targets:
	@echo
	@echo all             - Makes all applications and unit tests
	@echo clean           - Cleans application build files
	@echo distclean       - Cleans application build files and third-party library files
	@echo help            - This help
	@echo app_all         - Makes all appplications
	@echo test_all        - Makes all unit tests
	@echo run_test_all    - Makes all unit tests
	@echo [app_name]      - Makes a single application
	@echo [test_name]     - Makes a single unit test
	@echo run_[test_name] - Makes and runs a single unit test
	@echo
	@echo Applications:
	@echo     $(APPS)
	@echo
	@echo Unit Tests:
	@echo     $(TESTS)
