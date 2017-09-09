namespace bbox.debug
{
    export class GetOutputQuerier
    {
        private running: boolean;
        private api_base: string;
        private output_view: OutputView;
        private next_id: number;
        private debug_enables: bbox.ds.Deque<string>;
        private query_request: bbox.net.BboxRpcRequest<api.GetOutputRequest, api.GetOutputResponse>;
        private timer: bbox.ui.Timer;

        constructor(api_base: string, output_view: OutputView)
        {
            this.running = false;
            this.api_base = api_base;
            this.output_view = output_view;
            this.next_id = 0;
            this.debug_enables = new bbox.ds.Deque<string>();
            this.query_request = null;
            this.timer = new bbox.ui.Timer(() => { this.handleTimeout(); });
        }

        public start(): void
        {
            if (!this.running)
            {
                this.running = true;
                this.next_id = 0;
                this.query_request = null;
                this.timer.stop();

                this.output_view.addMessage("Starting debug output...", false);

                this.doGetOutputQuery();
            }
        }

        public stop(): void
        {
            if (this.running)
            {
                this.running = false;
                this.next_id = 0;
                this.query_request = null;
                this.timer.stop();

                this.output_view.addMessage("Stopping debug output", false);
            }
        }

        public setDebugEnables(debug_enables: bbox.ds.Deque<string>): void
        {
            this.debug_enables = debug_enables.shallowClone();
        }

        private handleTimeout(): void
        {
            if (this.running
                && (this.query_request === null))
            {
                this.doGetOutputQuery();
            }
        }

        private doGetOutputQuery(): void
        {
            this.query_request = new bbox.net.BboxRpcRequest<api.GetOutputRequest, api.GetOutputResponse>(
                this.api_base,
                "get_output",
                api.GetOutputRequest,
                api.GetOutputResponse,
                (req: bbox.net.BboxRpcRequest<api.GetOutputRequest, api.GetOutputResponse>) =>
                {
                    if (this.query_request === req)
                    {
                        this.query_request = null;

                        this.handleQueryResponse(req);
                    }
                }
            );

            var request = new api.GetOutputRequest();
            request.req_id = this.next_id;
            request.debug_enables = this.debug_enables.shallowClone();

            this.timer.stop();

            this.query_request.sendAsync(request);
        }

        private handleQueryResponse(req: bbox.net.BboxRpcRequest<api.GetOutputRequest, api.GetOutputResponse>): void
        {
            if (!req.wasSuccessul())
            {
                this.output_view.addMessage("Debug output failed: " + req.getErrorDescription(), true);

                this.timer.startSingleShot(5 * 1000);

                // Since it's failed - we need to indicate
                // that we're starting again

                this.next_id = 0;
            }
            else
            {
                var output_result = req.getResult()

                this.next_id = output_result.next_id;

                for (var i = 0; i < output_result.reports.size(); ++i)
                {
                    this.output_view.addDebugReport(output_result.reports.at(i));
                }

                this.timer.startSingleShot(1000);
            }
        }
    }
}