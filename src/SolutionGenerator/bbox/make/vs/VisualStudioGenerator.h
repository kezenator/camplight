/**
 * @file
 *
 * Header file for the bbox::make::vs::VisualStudioGenerator class.
 */

#ifndef __BBOX__MAKE__VS__VISUAL_STUDIO_GENERATOR_H__
#define __BBOX__MAKE__VS__VISUAL_STUDIO_GENERATOR_H__

#pragma once

#include <string>
#include <map>

#include <bbox/make/Generator.h>

namespace bbox {
    namespace make {
        namespace vs {

            class VisualStudioGenerator: public Generator
            {
            public:
                VisualStudioGenerator(const Solution &solution);
                ~VisualStudioGenerator();

                void UpdateFiles() override;

            private:

                std::string CalcPathToSolution(const Project *project_ptr);

                void GenerateProjectGuids();
                void GenerateSolutionFile();
                void GenerateProjectFiles(const Project *project_ptr);
                void GenerateProjectVcxproj(const Project *project_ptr);
                void GenerateProjectFilters(const Project *project_ptr);
                void GenerateTypescriptCsproj(const Project *project_ptr);
                void GenerateTypescriptWebConfigs(const Project *project_ptr);

                std::map<std::string, std::string> m_project_guids;
				uint16_t m_iis_port;
            };

        } // namespace bbox::make::vs
    } // namesoace bbox::make
} // namespace bbox

#endif // __BBOX__MAKE__VS__VISUAL_STUDIO_GENERATOR_H__
