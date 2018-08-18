/// <reference path="Buttons.ts" />

namespace ui
{
    export abstract class Screen
    {
        private app: App;
        private buttons: Buttons;
        private div: HTMLDivElement;

        public constructor(app: App)
        {
            this.app = app;
            this.buttons = app.getButtons();
            this.div = document.createElement('div');
            this.div.classList.add('screen');
        }

        public show()
        {
            document.body.appendChild(this.div);
        }

        public hide()
        {
            document.body.removeChild(this.div);
        }

        public abstract updateFrame(ms: number): void;

        protected getDiv(): HTMLDivElement
        {
            return this.div;
        }

        protected getApp(): App
        {
            return this.app;
        }

        protected getButtons(): Buttons
        {
            return this.buttons;
        }
    }
}