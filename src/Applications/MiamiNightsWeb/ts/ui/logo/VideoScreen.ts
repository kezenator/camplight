/// <reference path="../CanvasScreen.ts" />
/// <reference path="../../util/Math.ts" />
/// <reference path="LogoCommon.ts" />


namespace ui.logo
{
    export class VideoScreen extends CanvasScreen
    {
        private video: HTMLVideoElement;
        private instagram: HTMLImageElement;
        private youtube: HTMLImageElement;
        private video_index: number;
        private duration: number;
        private youtube_channel: string;

        private fading: boolean;
        private button_pressed: boolean;
        private fade_start_time: number;

        public constructor(app: App)
        {
            super(app);

            this.video = document.createElement('video');
            this.video.classList.add('video');
            this.video.volume = 0;
            this.video.width = 1920;
            this.video.height = 1080;

            this.getDiv().appendChild(this.video);

            this.instagram = document.createElement('img');
            this.instagram.src = '/res/imgs/instagram.png';

            this.youtube= document.createElement('img');
            this.youtube.src = '/res/imgs/youtube.png';

            this.video_index = 0;
            this.duration = 0;
            this.youtube_channel = '';

            this.fading = false;
            this.button_pressed = false;
            this.fade_start_time = 0;
        }

        public show()
        {
            super.show();

            var index = this.video_index;
            this.video_index = (this.video_index + 1) % 6;

            switch (index)
            {
                case 0:
                    this.video.src = '/res/video/MiamiNights1984-Accelerated.mp4';
                    this.duration = 80;
                    this.youtube_channel = 'NewRetroWave';
                    break;

                case 1:
                    this.video.src = '/res/video/SOUTHBEACHMIAMIDRONEPHANTOM4PRO4KFLORIDA-CANON1DXII-TRAVELOGFLORIDALIFESTYLE.mp4';
                    this.duration = 92;
                    this.youtube_channel = 'Artipica Media';
                    break;

                case 2:
                    this.video.src = '/res/video/MiamiNights1984-OceanDrive.mp4';
                    this.duration = 30;
                    this.youtube_channel = 'neros77';
                    break;

                case 3:
                    this.video.src = '/res/video/MiamibyDronein4K-DJIMavic2Pro_1.mp4';
                    this.duration = 57;
                    this.youtube_channel = 'Blue Mantle Films';
                    break;

                case 4:
                    this.video.src = '/res/video/MiamiNights1984-EarlySummer.mp4';
                    this.duration = 67;
                    this.youtube_channel = 'neros77';
                    break;

                case 5:
                    this.video.src = '/res/video/MiamibyDronein4K-DJIMavic2Pro_2.mp4';
                    this.duration = 78;
                    this.youtube_channel = 'Blue Mantle Films';
                    break;
            }

            this.duration = this.duration * 1000;
            this.video.play();

            this.fading = false;
            this.button_pressed = false;
            this.fade_start_time = 0;

            this.getApp().getButtons().setBackColor('#000000');
        }

        public hide()
        {
            super.hide();

            this.video.pause();
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            var buttons = this.getButtons();

            var button_pressed = buttons.isPlayClicked();
            for (var i = 0; i < Buttons.NUMBER; ++i)
            {
                if (buttons.isButtonPressed(i))
                    button_pressed = true;
            }

            if (button_pressed
                || (ms > (this.duration - LogoCommon.DURATION_FADE)))
            {
                if (button_pressed && !this.button_pressed)
                {
                    this.button_pressed = true;
                    this.getButtons().setPlayColor('#000000');
                    this.getApp().getAudio().play(
                        "/res/audio/insert_coin.ogg",
                        () => { });
                }

                if (!this.fading)
                {
                    this.fading = true;
                    this.fade_start_time = ms;
                }
            }

            if (this.fading)
            {
                if ((ms - this.fade_start_time) > LogoCommon.DURATION_FADE)
                {
                    if (this.button_pressed)
                        this.getApp().showScreen(App.MENU);
                    else
                        this.getApp().showScreen(App.LOGO);
                    return;
                }
            }

            var alpha = 0;

            if (this.fading)
            {
                alpha = util.lerp((ms - this.fade_start_time) / LogoCommon.DURATION_FADE, 0, 1);
            }
            else if (ms < LogoCommon.DURATION_FADE)
            {
                alpha = 1 - (ms / LogoCommon.DURATION_FADE);
            }

            ctx.clearRect(0, 0, 1920, 1080);

            LogoCommon.drawPressStart(ctx, ms, buttons);
            LogoCommon.colorWaveButtons(ms, alpha, buttons);
            this.drawYouTube(ctx, this.youtube_channel);
            this.drawTextCasaDelShade(ctx, ms);
            this.drawTextMiamiNights(ctx, ms);

            ctx.drawImage(this.instagram, 1421, 952);

            if (alpha != 0)
            {
                ctx.fillStyle = 'rgba(0,0,0,' + alpha + ')';
                ctx.fillRect(0, 0, 1920, 1080);
            }
        }

        private drawYouTube(ctx: CanvasRenderingContext2D, channel: string)
        {
            ctx.drawImage(this.youtube, 0, 952);

            ctx.font = '42px "Segoe UI"';
            ctx.fillStyle = 'white';
            ctx.fillText(channel, 120, 1031);
        }

        private drawTextCasaDelShade(ctx: CanvasRenderingContext2D, ms: number)
        {
            ctx.save();

            var text = "Casa del Shade";
            ctx.font = '250px "ThirdRail"';
            var offset = 8;

            var metrics = ctx.measureText(text);
            var x = 0.5 * (1920 - metrics.width);
            var y = 200;

            ctx.translate(0, 30);
            ctx.scale(0.25, 0.25);
            ctx.translate(x, y);

            ctx.fillStyle = 'rgb(30,0,128)';
            ctx.fillText(text, 2 * offset, 2 * offset);

            ctx.fillStyle = 'rgb(192,0,192)';
            ctx.fillText(text, offset, offset);

            ctx.fillStyle = 'rgb(255,0,255)';
            ctx.fillText(text, 0, 0);

            var hightlight_offset = 200 * (ms % LogoCommon.PERIOD_CASA_HIGHLIGHT) / LogoCommon.PERIOD_CASA_HIGHLIGHT;
            var grad = ctx.createLinearGradient(0, -400 + hightlight_offset, 0, 0 + hightlight_offset);
            var c1 = 'rgb(255,255,0)';
            var c2 = 'rgb(255,128,0)';
            grad.addColorStop(0, c1);
            grad.addColorStop(0.25, c2);
            grad.addColorStop(0.5, c1);
            grad.addColorStop(0.75, c2);
            grad.addColorStop(1, c1);

            ctx.strokeStyle = grad;
            ctx.lineWidth = 6;
            ctx.strokeText(text, 0, 0);

            ctx.restore();
        }

        private drawTextMiamiNights(ctx: CanvasRenderingContext2D, ms: number)
        {
            ctx.save();

            ctx.font = '300px "Still Time"';
            var offset = 8;

            var time = 1;
            var t1 = util.arrive(util.unlerp(time, 0, 0.8));
            var t2 = util.arrive(util.unlerp(time, 0.2, 1));

            function drawSimple(x: number, y: number, amount: number, text: string)
            {
                var metrics = ctx.measureText(text);

                x = x + util.lerp(amount, 2200, 0);

                ctx.save();
                ctx.translate(x - 0.5 * metrics.width, y + 300);
                ctx.rotate(-15 / 180 * Math.PI);
                ctx.fillText(text, -0.5 * metrics.width, -150);
                ctx.restore();
            }

            function drawWithShadow(x: number, y: number, amount: number, text: string)
            {
                ctx.fillStyle = 'rgb(255,0,255)';
                ctx.shadowColor = 'white';
                ctx.shadowBlur = 30;
                ctx.shadowOffsetX = 0;
                ctx.shadowOffsetY = 0;
                drawSimple(x + offset, y + offset, amount, text);
                drawSimple(x + offset, y + offset, amount, text);
                ctx.fillStyle = 'rgb(0,255,255)';
                ctx.shadowBlur = 0;
                ctx.shadowColor = 'transparent';
                drawSimple(x, y, amount, text);
            }

            ctx.translate(0, 30);
            ctx.scale(0.25, 0.25);

            drawWithShadow(1080, 350, t1, "Miami");
            drawWithShadow(1480, 500, t2, "Nights");

            ctx.restore();
        }
    }
}