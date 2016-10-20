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

            ClientResponse::ClientResponse(const pion::http::response_ptr &ptr)
                : m_response_ptr(ptr)
            {
            }

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox
