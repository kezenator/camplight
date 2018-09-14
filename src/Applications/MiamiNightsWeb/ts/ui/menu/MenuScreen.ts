/// <reference path="../CanvasScreen.ts" />
/// <reference path="../../util/Math.ts" />

namespace ui.menu
{
    class MenuScreenEntry
    {
        show: boolean;
        hue: number;
        screen: string;
        imageUri: string;
        imageElement: HTMLImageElement;

        constructor(_hue: number, _screen: string, _image_uri: string)
        {
            this.show = false;
            this.hue = _hue;
            this.screen = _screen;
            this.imageUri = _image_uri;
            this.imageElement = document.createElement('img');
            this.imageElement.src = _image_uri;
        }
    };

    export class MenuScreen extends CanvasScreen
    {
        private static TIME_EXPIRY: number = 60000;

        private background: Background;
        private entries: MenuScreenEntry[];

        public constructor(app: App, background: Background)
        {
            super(app);

            this.background = background;

            this.entries = [
                new MenuScreenEntry(0, App.FORTUNE, "res/imgs/menu_fortune.png"),
                new MenuScreenEntry(30, App.PONG, "res/imgs/menu_pong.png"),
                new MenuScreenEntry(120, App.LOGO, "res/imgs/menu_todo.png"),
                new MenuScreenEntry(180, App.TETRIS, "res/imgs/menu_tetris.png"),
                new MenuScreenEntry(240, App.SONIC, "res/imgs/menu_sonic.png"),
                new MenuScreenEntry(300, App.ALTERED_BEAST, "res/imgs/menu_altered_beast.png"),
            ];
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            if (ms > MenuScreen.TIME_EXPIRY)
            {
                this.getApp().showScreen(App.LOGO);
                return;
            }

            var buttons = this.getButtons();

            for (var i = 0; i < this.entries.length; ++i)
            {
                if (buttons.isButtonClicked(i))
                {
                    this.getApp().showScreen(this.entries[i].screen);
                    return;
                }
            }

            buttons.setPlayColor('#000000');

            this.background.draw(ctx, ms);

            for (var i = 0; i < this.entries.length; ++i)
            {
                var entry = this.entries[i];
                this.drawMenu(ctx, ms, i, entry);
            } 
        }

        private drawMenu(ctx: CanvasRenderingContext2D, ms: number, i: number, entry: MenuScreenEntry): void
        {
            // Work out the fade amount for this entry

            var swipe = 0;

            {
                var cycle = (ms % 2000) / 2000;
                var start = 0.125 * i;
                var end = 0.125 * (i + 2.1);

                swipe = util.unlerp(cycle, start, end);
            }

            // Work out if it should be shown

            if (swipe > 0)
            {
                entry.show = true;
            }

            if (!entry.show)
                return;

            // Work out the location for this entry

            var row = Math.floor(i / 3);
            var col = Math.floor(i % 3);

            var marginx = 100;
            var marginy = 100;

            var width = 505;
            var height = 390;

            var x = marginx + (col * (width + marginx));
            var y = marginy + (row * (height + marginy));

            // Fill in the body

            ctx.fillStyle = 'hsl(' + entry.hue + ',100%,50%)';
            ctx.fillRect(x, y, width, height);    

            ctx.save();

            ctx.beginPath();
            ctx.rect(x, y, width, height);
            ctx.clip();

            ctx.strokeStyle = 'hsl(' + entry.hue + ',100%,30%)';
            ctx.lineWidth = 10;

            ctx.beginPath();

            for (var j = 0; j < (width + height); j += 25)
            {                 
                ctx.moveTo(x - height + j, y + height);
                ctx.lineTo(x + j, y);
            }

            ctx.stroke();

            ctx.restore();

            // Draw the image

            ctx.drawImage(entry.imageElement, x, y);

            // Draw a white wash across the button

            if ((swipe > 0) && (swipe < 1))
            {
                var shine_width = 200;

                var width_frac = shine_width / (shine_width + width + height);

                var offset = util.lerp(swipe, 0, 1 - width_frac);

                var grad = ctx.createLinearGradient(x - shine_width, y - shine_width, x + width + height, y + width + height);

                grad.addColorStop(offset, 'rgba(255,255,255,0)');
                grad.addColorStop(offset + 0.5 * width_frac, 'rgba(255,255,255,255)');
                grad.addColorStop(offset + width_frac, 'rgba(255,255,255,0)');

                ctx.fillStyle = grad;
                ctx.fillRect(x, y, width, height);
            }

            // Draw the border
            // (color changing, expanding with fade)

            var fade = util.there_and_back(swipe);

            ctx.strokeStyle = 'hsl(' + entry.hue + ',100%,' + Math.floor(util.lerp(fade, 30, 50)) + '%)';
            ctx.lineWidth = 10 + util.lerp(fade, 0, 20);
            ctx.strokeRect(x, y, width, height);

            // Set the button color

            this.getButtons().setButtonColor(i, 'hsl(' + entry.hue + ',100%,' + util.lerp(fade, 30, 50) + '%)');
        }
    }
}