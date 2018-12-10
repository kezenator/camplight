/// <reference path="../CanvasScreen.ts" />
/// <reference path="../../util/Math.ts" />
/// <reference path="LogoCommon.ts" />


namespace ui.logo
{
    export class VideoScreen extends CanvasScreen
    {
        private video: HTMLVideoElement;
        private instagram: HTMLImageElement;

        public constructor(app: App)
        {
            super(app);

            this.video = document.createElement('video');
            this.video.classList.add('video');
            this.video.volume = 0;
            this.video.width = 1920;
            this.video.height = 1080;
            this.video.src = '/res/video/MiamiNights1984-Accelerated.mp4';

            this.getDiv().appendChild(this.video);

            this.instagram = document.createElement('img');
            this.instagram.src = '/res/imgs/instagram.png';
        }

        public show()
        {
            super.show();

            this.video.play();

            this.getApp().getButtons().setBackColor('#000000');
        }

        public hide()
        {
            super.hide();

            this.video.pause();
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            if (ms > 30000)
            {
                this.getApp().showScreen(App.LOGO);
                return;
            }

            var buttons = this.getButtons();

            ctx.clearRect(0, 0, 1920, 1080);

            var alpha = 0;

            if (ms < LogoCommon.DURATION_FADE)
            {
                alpha = 1 - (ms / LogoCommon.DURATION_FADE);
            }
            else if (ms > (30000 - LogoCommon.DURATION_FADE))
            {
                alpha = (ms - (30000 - LogoCommon.DURATION_FADE)) / LogoCommon.DURATION_FADE;
            }

            LogoCommon.drawPressStart(ctx, ms, buttons);
            LogoCommon.colorWaveButtons(ms, alpha, buttons);
            ctx.drawImage(this.instagram, 1421, 952);

            if (alpha != 0)
            {
                ctx.fillStyle = 'rgba(0,0,0,' + alpha + ')';
                ctx.fillRect(0, 0, 1920, 1080);
            }
        }
    }
}