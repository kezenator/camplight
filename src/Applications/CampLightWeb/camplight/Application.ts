namespace camplight
{
    export class Application extends bbox.ui.Application
    {
        private api_base: string;

        private display: ui.LightDisplay;
        private sequence_combo: ui.ComboBox;

        constructor(api_base: string)
        {
            super();

            this.api_base = api_base;
        }

        onload(): void
        {
            this.tryReload();
        }

        private tryReload()
        {
            this.body().htmlElement().innerHTML = "";

            var get_factories_req = new bbox.net.BboxRpcRequest<api.GetFactoriesRequest, api.GetFactoriesResponse>(
                this.api_base,
                "get-factories",
                api.GetFactoriesRequest,
                api.GetFactoriesResponse,
                (req: bbox.net.BboxRpcRequest<api.GetFactoriesRequest, api.GetFactoriesResponse>) =>
                {
                    if (req.wasSuccessul())
                    {
                        this.displayUx(req.getResult());
                    }
                    else
                    {
                        this.displayError();
                    }
                });

            get_factories_req.sendAsync(new api.GetFactoriesRequest());
        }

        private displayUx(factories: api.GetFactoriesResponse)
        {
            this.display = new ui.LightDisplay(this.api_base);
            this.sequence_combo = new ui.ComboBox(factories.sequences);

            var body = this.body();
            body.add(this.display);
            body.add(this.sequence_combo);

            this.sequence_combo.onChanged(() =>
            {
                var req = new bbox.net.BboxRpcRequest<api.ApplyRequest, api.ApplyResponse>(
                    this.api_base,
                    "apply",
                    api.ApplyRequest,
                    api.ApplyResponse,
                    (req: bbox.net.BboxRpcRequest<api.ApplyRequest, api.ApplyResponse>) =>
                    {
                        // Ignore for now
                    });

                var input = new api.ApplyRequest();
                input.sequence = this.sequence_combo.getCurrentItem();

                req.sendAsync(input);
            });

            this.display.start();
        }

        private displayError()
        {
            var body = this.body();

            body.htmlElement().innerHTML = "";

            body.add(new bbox.ui.Span("Not available - try again later."));

            var button = new bbox.ui.Button("Retry");
            body.add(button);

            button.onClick(() =>
            {
                this.tryReload();
            });
        }
    }
}