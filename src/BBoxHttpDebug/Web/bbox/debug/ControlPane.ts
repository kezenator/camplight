namespace bbox.debug
{
    export class ControlPane extends bbox.ui.Container
    {
        private app: Application;
        private output_view: OutputView;
        private get_output_querier: GetOutputQuerier;

        private div: HTMLDivElement;

        private disable_btn: bbox.ui.Button;
        private enable_btn: bbox.ui.Button;

        private enables_edit_box: bbox.ui.EditBox;

        constructor(app: Application, output_view: OutputView, get_output_querier: GetOutputQuerier)
        {
            var div = document.createElement('div');

            super(div);

            this.app = app;
            this.output_view = output_view;
            this.get_output_querier = get_output_querier;
            this.div = div;

            this.div.classList.add("control-pane");

            this.add(new bbox.ui.Span("Controls"));

            var home_btn = new bbox.ui.Button("Home");
            home_btn.onClick(() => { this.onHomeBtnClicked(); });
            this.add(home_btn);

            var clear_btn = new bbox.ui.Button("Clear");
            clear_btn.onClick(() => { this.onClearBtnClicked(); });
            this.add(clear_btn);

            var enable_div = new bbox.ui.Div();
            enable_div.addClass("radio-group");

            this.disable_btn = new bbox.ui.Button("Disable");
            this.disable_btn.onClick(() => { this.onDisableBtnClicked(); });
            this.disable_btn.addClass("selected");

            this.enable_btn = new bbox.ui.Button("Enable");
            this.enable_btn.onClick(() => { this.onEnableBtnClicked(); });

            this.enables_edit_box = new bbox.ui.EditBox("");
            this.enables_edit_box.onChanged(() => { this.onEnablesEditBoxChanged(); });

            enable_div.add(this.disable_btn);
            enable_div.add(this.enable_btn);
            this.add(enable_div);

            this.add(this.enables_edit_box);
        }

        private onClearBtnClicked(): void
        {
            this.output_view.clear();
        }

        private onHomeBtnClicked(): void
        {
            this.app.pushState_QueryState("/");
        }

        private onDisableBtnClicked(): void
        {
            this.disable_btn.addClass("selected");
            this.enable_btn.removeClass("selected");

            this.get_output_querier.stop();
        }

        private onEnableBtnClicked(): void
        {
            this.disable_btn.removeClass("selected");
            this.enable_btn.addClass("selected");

            this.get_output_querier.start();
        }

        private onEnablesEditBoxChanged(): void
        {
            var enables = new bbox.ds.Deque<string>();

            if (this.enables_edit_box.getValue() != "")
            {
                enables.push_back(this.enables_edit_box.getValue());
            }

            this.get_output_querier.setDebugEnables(enables);
        }
    }
}