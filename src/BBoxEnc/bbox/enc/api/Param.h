/**
 * @file
 *
 * Header file for the bbox::enc::api::Param class.
 */

#ifndef __BBOX__ENC__API__PARAM_H__
#define __BBOX__ENC__API__PARAM_H__

#include <string>
#include <bbox/TypeInfo.h>

namespace bbox
{
    namespace enc
    {
        namespace api
        {

            /**
             * Describes a method parameter in an API.
             */
            class Param
            {
            private:
                Param(const Param &other) = delete;
                Param &operator =(const Param &other) = delete;

            protected:
                Param();

            public:
                virtual ~Param();
                virtual const std::string &GetName() const = 0;
                virtual const std::string &GetDescription() const = 0;
                virtual bbox::TypeInfo GetType() const = 0;
                virtual bool IsInput() const = 0;
                virtual bool IsOutput() const = 0;
            };

        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__PARAM_H__
