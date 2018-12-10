/// <reference path="../../util/Math.ts" />

namespace ui.logo
{
    export class LogoCommon
    {         
        public static PERIOD_WASH: number = 10000;    
        public static DURATION_FADE: number = 1500;  

        public static drawPressStart(ctx: CanvasRenderingContext2D, ms: number, buttons: ui.Buttons)
        {                                                                                          
            var on = (ms % 1000) >= 500;

            if (on)
            {
                var text = 'PRESS PLAY';

                ctx.font = '65px "KarmaticArcade"';

                var metrics = ctx.measureText(text);

                var x = 960 - 0.5 * metrics.width;
                var y = 1050;

                var grad = ctx.createLinearGradient(x, y, x + metrics.width, y);
                grad.addColorStop(0, 'hsl(0,100%,50%)');
                grad.addColorStop(1 / 5, 'hsl(30,100%,50%)');
                grad.addColorStop(2 / 5, 'hsl(60,100%,50%)');
                grad.addColorStop(3 / 5, 'hsl(120,100%,50%)');
                grad.addColorStop(4 / 5, 'hsl(180,100%,50%)');
                grad.addColorStop(1, 'hsl(240,100%,50%)');

                ctx.fillStyle = grad;
                ctx.strokeStyle = 'black';
                ctx.lineWidth = 6;

                ctx.strokeText(text, x, y);
                ctx.fillText(text, x, y);

                buttons.setPlayColor('#ffffff');
            }
            else
            {
                buttons.setPlayColor('#000000');
            }
        }

        public static colorWaveButtons(ms: number, alpha: number, buttons: ui.Buttons)
        {
            var lightness = 50 * (1 - alpha);

            for (var i = 0; i < Buttons.NUMBER; ++i)
            {
                var base = 360 * ((ms % 4000) / 4000);
                var offset = i * 60;
                var hue = Math.floor((base + offset) % 360);

                buttons.setButtonColor(i, 'hsl(' + hue + ',100%,' + lightness + '%)');
            }

            var wash_hue = 360 * (ms % LogoCommon.PERIOD_WASH) / LogoCommon.PERIOD_WASH;

            buttons.setWashColor('hsl(' + wash_hue + ',100%,' + lightness + '%)');
        }
    }
}