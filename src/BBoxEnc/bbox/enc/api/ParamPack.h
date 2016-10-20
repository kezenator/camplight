/**
 * @file
 *
 * Header file for the bbox::enc::api::ParamPack class.
 */

#ifndef __BBOX__ENC__API__PARAM_PACK_H__
#define __BBOX__ENC__API__PARAM_PACK_H__

#include <string>
#include <boost/any.hpp>

#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>
#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>

#include <bbox/enc/api/Param.h>

namespace bbox
{
    namespace enc
    {
        namespace api
        {

            /**
             * Describes a collection of parameters.
             */
            class ParamPack
            {
            private:
                ParamPack(const ParamPack &other) = delete;
                ParamPack &operator =(const ParamPack &other) = delete;

            protected:
                ParamPack();

            public:
                virtual ~ParamPack();

                virtual size_t GetNumParameters() const = 0;
                virtual const bbox::enc::api::Param &GetParameter(size_t index) const = 0;

                virtual bbox::TypeInfo GetTupleType() const = 0;
                virtual bool IsTupleCorrectType(const boost::any &tuple) const = 0;
                virtual boost::any CreateDefaultConstructedTuple() const = 0;

                virtual std::string TupleToDebugString(bool for_input, const boost::any &tuple) const = 0;
                virtual void TupleToBinary(ToBinary &m, bool for_input, const boost::any &tuple) const = 0;
                virtual void TupleToTextFormat(ToTextFormat &m, bool for_input, const boost::any &tuple) const = 0;
                virtual void TupleFromBinary(FromBinary &m, bool for_input, boost::any &tuple) const = 0;
                virtual void TupleFromTextFormat(FromTextFormat &m, bool for_input, boost::any &tuple) const = 0;
            };

        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__PARAM_H__
