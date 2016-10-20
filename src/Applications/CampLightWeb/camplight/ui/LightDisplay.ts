namespace camplight.ui
{
    export class LightDisplay extends bbox.ui.Container
    {
        private main_div: HTMLDivElement;
        private main_matrix: LedMatrix;
        private top_matrix: LedMatrix;
        private is_started: boolean;
        private api_base: string;
        private api_request: bbox.net.BboxRpcRequest<api.GetLedsRequest, api.GetLedsResponse>;
        private timer: bbox.ui.Timer;

        constructor(api_base: string)
        {
            var div = document.createElement("div");

            super(div);

            this.main_div = div;
            this.main_div.classList.add("lightdisplay");

            this.main_matrix = new LedMatrix();
            this.top_matrix = new LedMatrix();

            this.add(this.main_matrix);
            this.add(this.top_matrix);

            this.is_started = false;
            this.api_base = api_base;
            this.api_request = null;
            this.timer = new bbox.ui.Timer(() => { this.handleTimerExpired(); });
        }

        updateDisplay(result: api.GetLedsResponse)
        {
            this.main_matrix.updateDisplay(result.width, result.height, result.main_leds);
            this.top_matrix.updateDisplay(result.width, result.height, result.top_leds);
        }

        start()
        {
            if (!this.is_started)
            {
                this.is_started = true;

                if (!this.api_request)
                {
                    this.timer.startSingleShot(1);
                }
            }
        }

        stop()
        {
            this.is_started = false;
            this.timer.stop();
        }

        private handleGetLedsResult(req: bbox.net.BboxRpcRequest<api.GetLedsRequest, api.GetLedsResponse>)
        {
            if (req.wasSuccessul)
            {
                this.updateDisplay(req.getResult());
            }
            else
            {
                this.updateDisplay(new api.GetLedsResponse());
            }

            if (this.is_started
                && !this.api_request)
            {
                this.timer.startSingleShot(10);
            }
        }

        private handleTimerExpired()
        {
            if (this.is_started
                && !this.api_request)
            {
                this.api_request = new bbox.net.BboxRpcRequest<api.GetLedsRequest, api.GetLedsResponse>(
                    this.api_base,
                    "get-leds",
                    api.GetLedsRequest,
                    api.GetLedsResponse,
                    (req: bbox.net.BboxRpcRequest<api.GetLedsRequest, api.GetLedsResponse>) =>
                    {
                        if (this.api_request === req)
                            this.api_request = null;

                        this.handleGetLedsResult(req);
                    }
                );

                this.api_request.sendAsync(new api.GetLedsRequest());
            }
        }
    }
}