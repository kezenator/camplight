/**
* @file
*
* Header file for the bbox::enc::api::Method class.
*/

#ifndef __BBOX__ENC__API__METHOD_H__
#define __BBOX__ENC__API__METHOD_H__

#include <string>
#include <bbox/enc/api/ParamPack.h>
#include <bbox/Error.h>

namespace bbox
{
    namespace enc
    {
        namespace api
        {

            /**
             * Describes a method in an API.
             */
            class Method
            {
            private:
                Method(const Method &other) = delete;
                Method &operator =(const Method &other) = delete;

            protected:
                Method();

            public:
                virtual ~Method();
                virtual const std::string &GetName() const = 0;
                virtual const std::string &GetDescription() const = 0;
                virtual const ParamPack &GetParamPack() const = 0;
                virtual bbox::Error Call(boost::any &params_tuple) const = 0;
            };

        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__METHOD_H__
