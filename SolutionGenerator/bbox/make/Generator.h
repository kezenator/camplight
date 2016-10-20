/**
 * @file
 *
 * Header file for the bbox::make::Generator class.
 */

#ifndef __BBOX__MAKE__GENERATOR_H__
#define __BBOX__MAKE__GENERATOR_H__

#pragma once

#include <string>

namespace bbox {
    namespace make {

        // Forward declarations
        class Solution;
        class Project;

        /**
         * Base class for the file generator that
         * generates the output files from the solution.
         */
        class Generator
        {
        public:
            Generator(const Solution &solution);
            ~Generator();

            bool AnyFilesChanged() const { return m_any_files_changed; }

            virtual void UpdateFiles() = 0;

        protected:
            const Solution &GetSolution() { return m_solution; }

            void WriteFile(const std::string &relative_path, const std::string &contents);

        private:
            const Solution &m_solution;
            bool m_any_files_changed;
        };

    } // namesoace bbox::make
} // namespace bbox

#endif // __BBOX__MAKE__GENERATOR_H__
