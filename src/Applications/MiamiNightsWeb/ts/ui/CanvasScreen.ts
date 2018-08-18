/// <reference path="Screen.ts" />

namespace ui
{
    export abstract class CanvasScreen extends Screen
    {
        private canvas: HTMLCanvasElement;

        public constructor(app: App)
        {
            super(app);
            this.canvas = document.createElement('canvas');
            this.canvas.width = 960;
            this.canvas.height = 540;
            this.getDiv().appendChild(this.canvas);
        }

        updateFrame(ms: number): void
        {
            var ctx = this.getCanvas().getContext('2d');
            if (ctx != null)
            {
                ctx.setTransform(1, 0, 0, 1, 0, 0);
                ctx.scale(0.5, 0.5);

                this.draw(ctx, ms);
            }
        }

        protected getCanvas()
        {
            return this.canvas;
        }

        protected abstract draw(ctx: CanvasRenderingContext2D, ms: number): void;
    }
}