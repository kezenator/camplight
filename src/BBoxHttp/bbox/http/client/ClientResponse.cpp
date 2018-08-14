/**
 * @file
 *
 * Implementation for the bbox::http::client::ClientResponse class.
 */

#include <bbox/http/client/ClientResponse.h>

namespace bbox
{
    namespace http
    {
        namespace client
        {

            ClientResponse::ClientResponse(std::unique_ptr<boost::beast::http::response<boost::beast::http::string_body>> &&ptr)
                : m_response_ptr(std::move(ptr))
            {
            }

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox
