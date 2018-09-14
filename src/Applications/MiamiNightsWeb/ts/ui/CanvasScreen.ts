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
            this.canvas.width = app.getWidth();
            this.canvas.height = app.getHeight();
            this.getDiv().appendChild(this.canvas);
        }

        updateFrame(ms: number): void
        {
            var ctx = this.getCanvas().getContext('2d');
            if (ctx != null)
            {
                ctx.setTransform(1, 0, 0, 1, 0, 0);

                var scale = this.getApp().getScale();
                if (scale != 1)
                    ctx.scale(scale, scale);

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