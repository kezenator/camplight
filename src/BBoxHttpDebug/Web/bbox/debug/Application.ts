namespace bbox.debug
{
    export class Application extends bbox.ui.Application<AppState>
    {
        private api_base: string;
        private query_request: bbox.net.BboxRpcRequest<api.QueryRequest, api.QueryResponse>;

        private output_view: OutputView;
        private get_output_querier: GetOutputQuerier;
        private control_pane: ControlPane;

        constructor(api_base: string)
        {
            super(AppState);

            this.api_base = api_base;
            this.query_request = null;

            this.output_view = new OutputView(this);

            this.get_output_querier = new GetOutputQuerier(api_base, this.output_view);

            this.control_pane = new ControlPane(this, this.output_view, this.get_output_querier);

            this.body().add(this.control_pane);
            this.body().add(this.output_view);
        }

        private stateToQuery(state: AppState): string
        {
            var query = "/";

            if (state
                && state.path)
            {
                query = state.path;
            }

            return query;
        }

        onLoad(state: AppState): void
        {
            this.queryState(this.stateToQuery(state));
        }

        onPopState(state: AppState): void
        {
            this.queryState(this.stateToQuery(state));
        }

        decodeUrlToState(url: string): AppState
        {
            var result = new AppState();

            if (url.substr(0, 13) == "/debug/state/")
                result.path = url.substr(12);

            return result;
        }

        public pushState_QueryState(query: string): boolean
        {
            var href = "/debug/state" + query;

            var state = new AppState();
            state.path = query;

            this.pushState(href, state);

            return false;
        }

        private queryState(query: string): void
        {
            this.query_request = new bbox.net.BboxRpcRequest<api.QueryRequest, api.QueryResponse>(
                this.api_base,
                "query_state",
                api.QueryRequest,
                api.QueryResponse,
                (req: bbox.net.BboxRpcRequest<api.QueryRequest, api.QueryResponse>) =>
                {
                    if (this.query_request === req)
                    {
                        this.query_request = null;

                        this.handleQueryResponse(req);
                    }
                }
                );

            var request = new api.QueryRequest();
            request.query = query;

            this.query_request.sendAsync(request);
        }

        private handleQueryResponse(req: bbox.net.BboxRpcRequest<api.QueryRequest, api.QueryResponse>): void
        {
            if (!req.wasSuccessul())
            {
                this.output_view.addMessage("Error: " + req.getErrorDescription(), true);
            }
            else
            {
                var response = req.getResult();

                if (response.results.size() == 0)
                {
                    this.output_view.addMessage("Not found", true);
                }
                else
                {
                    for (var i = 0; i < response.results.size(); ++i)
                    {
                        this.output_view.addQueryResponse(response.results.at(i));
                    }
                }
            }
        }
    }
}