/// <reference path="../CanvasScreen.ts" />
/// <reference path="../../util/Math.ts" />

namespace ui.logo
{
    export class LogoScreen extends CanvasScreen
    {
        private static TIME_START_CASA: number = 4000;
        private static TIME_END_CASA: number = 7000;
        private static TIME_START_MIAMI: number = 9000;
        private static TIME_END_MIAMI: number = 11000;

        private static DURATION_SUNSET: number = 12000;

        private static PERIOD_CASA_HIGHLIGHT: number = 2000;
        private static PERIOD_REFLECTION: number = 3000;

        private static HORIZON: number = 700;

        private palms: HTMLImageElement;
        private city: HTMLImageElement;

        private audio: mn.audio.Playout;
        private fade_start_time: number;

        public constructor(app: App)
        {
            super(app);

            this.palms = document.createElement('img');
            this.palms.src = '/res/imgs/palm.png';

            this.city= document.createElement('img');
            this.city.src = '/res/imgs/city.png';

            this.audio = null;
            this.fade_start_time = 0;
        }

        public show()
        {
            super.show();

            this.getApp().getButtons().setBackColor('#000000');

            this.audio = null;
            this.fade_start_time = 0;
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            if (this.getButtons().isPlayClicked()
                && (this.audio == null))
            {
                this.getButtons().setPlayColor('#000000');
                this.fade_start_time = ms;
                this.audio = this.getApp().getAudio().play(
                    "/res/audio/insert_coin.ogg",
                    () => { this.audioCompleted(); });
            }

            this.colorWaveButtons(ms);

            this.drawSky(ctx, ms);
            this.drawSea(ctx, ms);
            this.drawSun(ctx, ms);
            this.drawCity(ctx, ms);
            this.drawPalms(ctx, ms);
            this.drawText(ctx, ms);

            if (this.audio)
            {
                var alpha = util.lerp((ms - this.fade_start_time) / 1500, 0, 1);

                ctx.fillStyle = 'rgba(0,0,0,' + alpha + ')';
                ctx.fillRect(0, 0, 1920, 1080);
            }
            else
            {
                this.drawPressStart(ctx, ms);
            }
        }

        private audioCompleted()
        {
            this.audio = null;
            this.getApp().showScreen(App.MENU);
        }

        private drawSky(ctx: CanvasRenderingContext2D, ms: number)
        {
            var time = util.unlerp(ms, 0, LogoScreen.DURATION_SUNSET);

            var num_bands = 6;

            var horizon = LogoScreen.HORIZON;
            var spacing = 0.5 * horizon;
            var height = num_bands * spacing;
            var end = horizon;
            var start = horizon - height;
            var offset = util.lerp(time, 0, -start);

            var grad = ctx.createLinearGradient(960, start + offset, 960, end + offset);
            grad.addColorStop(0 / 6, 'black');
            grad.addColorStop(1 / 6, '#25143e');
            grad.addColorStop(2 / 6, '#441a48');
            grad.addColorStop(3 / 6, '#8b2857');
            grad.addColorStop(4 / 6, '#5f648e');
            grad.addColorStop(5 / 6, '#3eafe4');
            grad.addColorStop(6 / 6, '#81c6e5');
            ctx.fillStyle = grad;
            ctx.fillRect(0, 0, 1920, horizon + 1);
        }

        private drawSea(ctx: CanvasRenderingContext2D, ms: number)
        {
            var progress = util.unlerp(ms, 0, LogoScreen.DURATION_SUNSET);

            ctx.fillStyle = new util.Color(4, 68, 99, 1).lerp(progress,
                new util.Color(15, 5, 35, 1)).toString();
            
            ctx.fillRect(0, LogoScreen.HORIZON + 1, 1920, 1080);
        }

        private drawSun(ctx: CanvasRenderingContext2D, ms: number)
        {
            var progress = util.unlerp(ms, 0, LogoScreen.DURATION_SUNSET);
            var y = util.lerp(util.arrive(progress), 0, 800);

            var horizon = LogoScreen.HORIZON;

            // Colors

            var col_sun_center = new util.Color(200, 140, 0, 1).lerp(progress,
                                 new util.Color(162, 1, 9, 1)).toString();
            var col_sun_edge   = new util.Color(255, 255, 0, 1).lerp(progress,
                                 new util.Color(213, 20, 5, 1)).toString();
            var col_glow_touch = new util.Color(255, 255, 255, 0.5).lerp(progress,
                                 new util.Color(87, 20, 63, 1)).toString();
            var col_glow_fade  = new util.Color(255, 255, 255, 0).lerp(progress,
                                 new util.Color(87, 20, 63, 0)).toString();

            // Routines to draw the sun

            function doDraw(y: number)
            {
                var glowGrad = ctx.createRadialGradient(960, y, 400, 960, y, 450);
                glowGrad.addColorStop(0, col_glow_touch);
                glowGrad.addColorStop(1, col_glow_fade);
                ctx.fillStyle = glowGrad;
                ctx.beginPath();
                ctx.arc(960, y, 450, 0, 2 * Math.PI);
                ctx.fill();

                var sunGrad = ctx.createRadialGradient(960, y, 200, 960, y, 400);
                sunGrad.addColorStop(0, col_sun_center);
                sunGrad.addColorStop(1, col_sun_edge);
                ctx.fillStyle = sunGrad;
                ctx.beginPath();
                ctx.arc(960, y, 400, 0, 2 * Math.PI);
                ctx.fill();
            }

            // Draw the part above the horizon

            ctx.save();

            ctx.beginPath();
            ctx.rect(0, 0, 1920, horizon);
            ctx.clip();

            doDraw(y);

            ctx.restore();

            // Draw the reflection

            ctx.save();

            ctx.beginPath();

            var factor = 1.2;
            var startHeight = 2;

            var loop = (ms % LogoScreen.PERIOD_REFLECTION) / LogoScreen.PERIOD_REFLECTION;
            var secondHeight = startHeight * factor;
            var height = util.lerp(loop, startHeight, secondHeight);
            var pos = horizon - startHeight + util.lerp(loop, 0, 2 * startHeight);
            while (pos < 1080)
            {
                var sy = pos;
                var ey = pos + height;

                if ((sy < horizon)
                    && (ey < horizon))
                {
                    // All above horizon - don't clip
                }
                else
                {
                    if (sy < horizon)
                        sy = horizon;
                    ctx.moveTo(0, sy);
                    ctx.lineTo(1920, sy);
                    ctx.lineTo(1920, ey);
                    ctx.lineTo(0, ey);
                    ctx.lineTo(0, sy);
                }

                pos += 2 * height;
                height *= factor;
            }
            ctx.clip();

            doDraw(LogoScreen.HORIZON + (LogoScreen.HORIZON - y));

            ctx.restore();
        }

        private drawCity(ctx: CanvasRenderingContext2D, ms: number)
        {
            var imageWidth = 1067;
            var imageHeight = 500;

            // Work out the position of the city

            var cityStartX = -Math.floor(imageWidth * ((ms % 20000) / 20000));
            var cityHorion = util.lerp(util.arrive(util.unlerp(ms, 0, LogoScreen.DURATION_SUNSET)), 2080 + imageHeight, 770);

            // Draw the reflections

            ctx.save();

            ctx.beginPath();

            var horizon = LogoScreen.HORIZON;
            var factor = 1.2;
            var startHeight = 2;

            var loop = (ms % LogoScreen.PERIOD_REFLECTION) / LogoScreen.PERIOD_REFLECTION;
            var secondHeight = startHeight * factor;
            var height = util.lerp(loop, startHeight, secondHeight);
            var pos = horizon - startHeight + util.lerp(loop, 0, 2 * startHeight);
            while (pos < 1080)
            {
                var sy = pos;
                var ey = pos + height;

                if ((sy < horizon)
                    && (ey < horizon))
                {
                    // All above horizon - don't clip
                }
                else
                {
                    if (sy < horizon)
                        sy = horizon;
                    ctx.moveTo(0, sy);
                    ctx.lineTo(1920, sy);
                    ctx.lineTo(1920, ey);
                    ctx.lineTo(0, ey);
                    ctx.lineTo(0, sy);
                }

                pos += 2 * height;
                height *= factor;
            }
            ctx.clip();

            ctx.translate(0, cityHorion);
            ctx.scale(1, -0.5);

            (<any>ctx).filter = 'blur(2px)';

            for (var x = cityStartX; x < 1920; x += imageWidth)
            {
                ctx.drawImage(this.city, x, -imageHeight);
            }

            ctx.restore();

            // Finally, draw the city images

            for (var x = cityStartX; x < 1920; x += imageWidth)
            {
                ctx.drawImage(this.city, x, cityHorion - imageHeight + 1);
            }
        }

        private drawPalms(ctx: CanvasRenderingContext2D, ms: number)
        {
            var imageWidth = 1118;
            var imageHeight = 800;

            // Work out the position of the city

            var progress = ((ms + 0.8 * LogoScreen.DURATION_SUNSET) % LogoScreen.DURATION_SUNSET) / LogoScreen.DURATION_SUNSET;
            var x = util.lerp(progress, 1920, -imageWidth);

            var palmTop = util.lerp(util.arrive(util.unlerp(ms, 0, LogoScreen.DURATION_SUNSET)), 1580 + imageHeight, 1080 + 2 - imageHeight);

            // Draw the palms

            ctx.drawImage(this.palms, x, palmTop);
        }

        private drawText(ctx: CanvasRenderingContext2D, ms: number)
        {
            this.drawTextCasaDelShade(ctx, ms);
            this.drawTextMiamiNights(ctx, ms);
        }

        private drawTextCasaDelShade(ctx: CanvasRenderingContext2D, ms: number)
        {
            ctx.save();

            var text = "Casa del Shade";
            ctx.font = '250px "ThirdRail"';
            var offset = 8;

            var time = util.unlerp(ms, LogoScreen.TIME_START_CASA, LogoScreen.TIME_END_CASA);

            var metrics = ctx.measureText(text);
            var x = 0.5 * (1920 - metrics.width);
            var y = util.lerp(util.arrive(time), -50, 200);

            ctx.translate(x, y);

            ctx.fillStyle = 'rgb(30,0,128)';
            ctx.fillText(text, 2 * offset, 2 * offset);

            ctx.fillStyle = 'rgb(192,0,192)';
            ctx.fillText(text, offset, offset);

            ctx.fillStyle = 'rgb(255,0,255)';
            ctx.fillText(text, 0, 0);

            var hightlight_offset = 200 * (ms % LogoScreen.PERIOD_CASA_HIGHLIGHT) / LogoScreen.PERIOD_CASA_HIGHLIGHT;
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

            var time = util.unlerp(ms, LogoScreen.TIME_START_MIAMI, LogoScreen.TIME_END_MIAMI);
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

            drawWithShadow(1080, 350, t1, "Miami");
            drawWithShadow(1480, 500, t2, "Nights"); 

            ctx.restore();
        }

        private drawPressStart(ctx: CanvasRenderingContext2D, ms: number)
        {
            var buttons = this.getButtons();
            var on = (ms % 1000) >= 500;

            if (on)
            {
                ctx.font = '55px "Consolas"';
                ctx.fillStyle = 'rgb(255,255,255)';

                var text = 'PRESS PLAY';

                var metrics = ctx.measureText(text);

                ctx.fillText(text, 960 - 0.5 * metrics.width, 1080 - 55);

                buttons.setPlayColor('#ffffff');
            }
            else
            {
                buttons.setPlayColor('#000000');
            }
        }

        private colorWaveButtons(ms: number)
        {
            var buttons = this.getButtons();

            for (var i = 0; i < Buttons.NUMBER; ++i)
            {
                var base = 360 * ((ms % 4000) / 4000);
                var offset = i * 60;
                var hue = Math.floor((base + offset) % 360);

                buttons.setButtonColor(i, 'hsl(' + hue + ',100%,50%)');
            }
        }
    }
}