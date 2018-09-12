namespace bbox.http.debug
{
    export class Application extends bbox.ui.Application<AppState>
    {
        private websocket_uri: string;
        private websocket: bbox.net.MessageWebSocket;

        private output_view: OutputView;
        private control_pane: ControlPane;

        constructor(websocket_uri: string)
        {
            super(AppState);

            this.websocket_uri = websocket_uri;

            this.websocket = new bbox.net.MessageWebSocket(
                this.websocket_uri,
                "11.09.2018.debug.http.bbox.kezenator.com",
                (open: boolean, error: string) => { this.handleWebSocketState(open, error); });

            this.websocket.registerHandler(
                msgs.DebugReportNotification.TYPE,
                (msg: msgs.DebugReportNotification) => { this.handleDebugOutputNotification(msg); });

            this.websocket.registerHandler(
                msgs.QueryResponse.TYPE,
                (msg: msgs.QueryResponse) => { this.handleQueryResponse(msg); });

            this.output_view = new OutputView(this);

            this.control_pane = new ControlPane(this, this.output_view, this.websocket);

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

        private handleWebSocketState(open: boolean, error: string)
        {
            if (open)
            {
                this.output_view.addMessage("Connected", false);
                this.control_pane.resendRequest();
                this.queryState("/");
            }
            else
            {
                this.output_view.addMessage("Disconnected: " + error, true);
            }
        }

        private queryState(query: string): void
        {
            var msg = new msgs.QueryRequest();
            msg.request_id = "UniqueRequestID";
            msg.query = query;
            this.websocket.send(msg);
        }

        private handleQueryResponse(response: msgs.QueryResponse): void
        {
            if (response.entries.size() == 0)
            {
                this.output_view.addMessage("Not found", true);
            }
            else
            {
                for (var i = 0; i < response.entries.size(); ++i)
                {
                    this.output_view.addQueryResponse(response.entries.at(i));
                }
            }
        }

        private handleDebugOutputNotification(report: msgs.DebugReportNotification): void
        {
            this.output_view.addDebugReport(report);
        }
    }
}