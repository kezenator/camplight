/**
 * @file
 *
 * Main application for the CampLight application.
 *
 */

#include <bbox/MainWrapper.h>
#include <bbox/debug/DebugProvider.h>
#include <bbox/debug/CoutDebugTarget.h>
#include <bbox/rt/Proactor.h>
#include <bbox/rt/Service.h>
#include <bbox/rt/ConsoleShutdownService.h>
#include <bbox/http/server/HttpServer.h>
#include <bbox/http/debug/HttpDebugWebsite.h>
#include <bbox/http/Response.h>
#include <bbox/Format.h>
#include <bbox/Assert.h>

#include <bbox/enc/api/MethodSet.h>
#include <bbox/enc/api/Describe.h>
#include <bbox/enc/FromXml.h>
#include <bbox/enc/ToXml.h>

#include <camplight/Resources.h>

#include <camplight/render/Renderer.h>
#include <camplight/factory/PatternFactory.h>
#include <camplight/factory/TransitionFactory.h>

namespace camplight {

class ApplicationService : public bbox::rt::Service
{
public:

    ApplicationService(const std::string &name, 
                       bbox::rt::Proactor &parent,
                       const bbox::rt::net::TcpEndpoint &http_listen_endpoint)
        : bbox::rt::Service(name, parent)
        , m_http_listen_endpoint(http_listen_endpoint)
        , m_console_shutdown_service("console-shutdown-service", *this)
        , m_http_server("http-server", *this)
        , m_http_debug_website("http-debug-website", *this, m_http_server)
        , m_renderer("renderer", *this)
    {
        m_http_server.SetThisDependantOn(m_renderer);

        m_method_set = bbox::enc::api::MethodSet(
            bbox::enc::api::describe::Method(
                "get-leds",
                "Gets the current state of the LEDs",
                bbox::enc::api::describe::ParamPack(
                    bbox::enc::api::describe::Param<
                        uint32_t,
                        bbox::enc::api::describe::Output>(
                        "width",
                        "The width of the matricies"),
                    bbox::enc::api::describe::Param<
                        uint32_t,
                        bbox::enc::api::describe::Output>(
                        "height",
                        "The height of the matricies"),
                    bbox::enc::api::describe::Param<
                        std::vector<render::Color>,
                        bbox::enc::api::describe::Output>(
                        "main_leds",
                        "The vector (width x height long) of the colors strings (in HTML hashed color format) of each of the LEDs on the main (bottom) side"),
                    bbox::enc::api::describe::Param<
                        std::vector<render::Color>,
                        bbox::enc::api::describe::Output>(
                        "top_leds",
                        "The vector (width x height long) of the colors strings (in HTML hashed color format) of each of the LEDs on the top side")),
                this,
                &ApplicationService::Api_GetLeds),
            bbox::enc::api::describe::Method(
                "get-factories",
                "Gets the available factories",
                bbox::enc::api::describe::ParamPack(
                    bbox::enc::api::describe::Param<
                        std::vector<std::string>,
                        bbox::enc::api::describe::Output>(
                        "sequences",
                        "The list of sequences that can be displayed"),
                    bbox::enc::api::describe::Param<
                        std::vector<std::string>,
                        bbox::enc::api::describe::Output>(
                        "patterns",
                        "The list of patterns that can be used in sequences"),
                    bbox::enc::api::describe::Param<
                        std::vector<std::string>,
                        bbox::enc::api::describe::Output>(
                        "transitions",
                        "The list of transitions that can be used in sequences")),
                this,
                &ApplicationService::Api_GetFactories),
            bbox::enc::api::describe::Method(
                "apply",
                "Applies a sequence",
                bbox::enc::api::describe::ParamPack(
                    bbox::enc::api::describe::Param<
                    std::string,
                    bbox::enc::api::describe::Input>(
                    "sequence",
                    "The sequence to display")),
                this,
                &ApplicationService::Api_Apply)
            );
    }

private:

    bbox::Error Api_GetLeds(uint32_t &width, uint32_t &height, std::vector<render::Color> &main_leds, std::vector<render::Color> &top_leds)
    {
        m_renderer.GetLedsForWebDisplay(width, height, main_leds, top_leds);

        return bbox::Error();
    }

    bbox::Error Api_GetFactories(std::vector<std::string> &sequences, std::vector<std::string> &patterns, std::vector<std::string> &transitions)
    {
        sequences = m_renderer.GetSequenceFactory().GetNames();
        patterns = m_renderer.GetPatternFactory().GetNames();
        transitions = m_renderer.GetTransitionFactory().GetNames();

        return bbox::Error();
    }

    bbox::Error Api_Apply(const std::string &sequence)
    {
        m_renderer.ChangeToSequence(sequence);

        return bbox::Error();
    }

    void HandleStarting() override
    {
        srand(unsigned(time(0)));

        m_http_server.AddServer(
            m_http_listen_endpoint,
            std::bind(&ApplicationService::HttpRequestHandler, this, std::placeholders::_1));

        NotifyStarted();
    }

    void HandleStopping() override
    {
        RequestStopAllChildren();
        NotifyStopped();
    }

    void PrintState(bbox::DebugOutput &out) const override
    {
        out.Format("HTTP Listen Endpoint: %s\n", m_http_listen_endpoint.ToString());
    }

    void HttpRequestHandler(bbox::http::Request &request)
    {
        if (request.RespondWithResource(g_resource_files()))
        {
            return;
        }

        if (request.GetResource() == "/")
        {
            request.RespondWithTemporaryRedirect("/index.html");
            return;
        }

        if (request.GetResource().substr(0, 5) == "/api/")
        {
            HandleApiRequest(request);
            return;
        }

        request.RespondWithNotFoundError();
    }

    void HandleApiRequest(bbox::http::Request &request)
    {
        if (request.GetMethod() != bbox::http::Request::Method::post)
        {
            request.RespondWithMethodNotAllowedError("POST");
            return;
        }

        std::string method_name = request.GetResource().substr(5);

        if (!m_method_set.HasMethodWithName(method_name))
        {
            request.RespondWithNotFoundError();
            return;
        }

        const bbox::enc::api::Method &method = m_method_set.GetMethodByName(method_name);

        bbox::enc::FromXml from_xml(request.GetContent());

        boost::any params_tuple = method.GetParamPack().CreateDefaultConstructedTuple();

        from_xml.StartNamedItem("params");
        method.GetParamPack().TupleFromTextFormat(from_xml, true, params_tuple);
        from_xml.CompleteNamedItem();
        from_xml.EnsureAllDecoded();

        if (from_xml.HasError())
        {
            request.RespondWithBadRequestError(from_xml.GetErrorString());
            return;
        }

        bbox::Error error = method.Call(params_tuple);

        if (error)
        {
            request.RespondWithServerError(error.ToString());
            return;
        }

        tinyxml2::XMLDocument result_doc;
        bbox::enc::ToXml to_xml(result_doc, "results");

        method.GetParamPack().TupleToTextFormat(to_xml, false, params_tuple);

        tinyxml2::XMLPrinter printer;
        result_doc.Print(&printer);

        bbox::http::Response response(request);

        response.SetHeader_ContentType("text/xml");
        response.SetHeader_NoCache();
        response.SetResponse_OK();
        response.SetContent(std::string(printer.CStr()));

#ifdef _DEBUG
        response.SetHeader("Access-Control-Allow-Origin", "*");
#endif

        response.Send();
    }

    bbox::rt::net::TcpEndpoint m_http_listen_endpoint;
    bbox::rt::ConsoleShutdownService m_console_shutdown_service;
    bbox::http::server::HttpServer m_http_server;
    bbox::http::debug::HttpDebugWebsite m_http_debug_website;
    bbox::enc::api::MethodSet m_method_set;

    render::Renderer m_renderer;

    factory::PatternFactory m_pattern_factory;
    factory::TransitionFactory m_transition_factory;

}; // class ApplicationService

} // namespace camplight

int camplight_main(int argc, char *argv[])
{
	bbox::rt::net::TcpEndpoint endpoint;

    if ((argc != 2)
		|| !bbox::rt::net::TcpEndpoint::FromString(argv[1], endpoint)
		|| !endpoint.GetAddress().is_v4()
		|| endpoint.GetAddress().is_multicast()
		|| (endpoint.GetPort() == 0))
    {
        std::cerr << "Usage:  " << argv[0] << " ipv4-listen-addr:port" << std::endl;
		std::cerr << "   e.g. " << argv[0] << " 127.0.0.1:8080" << std::endl;
        return 1;
    }

	bbox::debug::DebugProvider debug_provider;
	bbox::debug::CoutDebugTarget console_target;
    bbox::rt::Proactor proactor;
    camplight::ApplicationService app("app", proactor, endpoint);

    proactor.Run();

    return 0;
}

BBOX_MAIN_WRAPPER(camplight_main)
