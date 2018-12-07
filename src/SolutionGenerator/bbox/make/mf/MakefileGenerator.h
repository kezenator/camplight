/**
 * @file
 *
 * Header file for the bbox::make::mf::MakefileGenerator class.
 */

#ifndef __BBOX__MAKE__MF__MAKEFILE_GENERATOR_H__
#define __BBOX__MAKE__MF__MAKEFILE_GENERATOR_H__

#pragma once

#include <string>
#include <map>
#include <set>

#include <bbox/make/Generator.h>

namespace bbox {
    namespace make {
        namespace mf {

            class MakefileGenerator: public Generator
            {
            public:
                MakefileGenerator(const Solution &solution);
                ~MakefileGenerator();

                void UpdateFiles() override;

            private:
                void PrintIncludes(std::ostream &stream, const Project *proj);
                void PrintSources(std::ostream &stream, const Project *proj);
                void PrintResources(std::ostream &stream, const Project *proj);
                void PrintLibs(std::ostream &stream, const Project *proj);
                void PrintCustomBuildSteps(std::ostream &stream, const Project *proj);
                std::string UnixPath(std::string path);
                std::string PrefixJoinToUnix(const std::string prefix, const std::set<std::string> &files, const std::string &sep);

                struct project_sorter;
            };

        } // namespace bbox::make::mf
    } // namesoace bbox::make
} // namespace bbox

#endif // __BBOX__MAKE__MF__MAKEFILE_GENERATOR_H__
