/// <reference path="../CanvasScreen.ts" />
/// <reference path="../../util/Math.ts" />

namespace ui.menu
{
    class MenuScreenEntry
    {
        hue: number;
        screen: string;
        imageUri: string;
        imageElement: HTMLImageElement;

        constructor(_hue: number, _screen: string, _image_uri: string)
        {
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

        private entries: MenuScreenEntry[];

        public constructor(app: App)
        {
            super(app);

            this.entries = [
                new MenuScreenEntry(0, App.FORTUNE, "res/imgs/menu_fortune.png"),
                new MenuScreenEntry(30, App.PONG, "res/imgs/menu_pong.png"),
                new MenuScreenEntry(120, App.LOGO, "res/imgs/menu_todo.png"),
                new MenuScreenEntry(180, App.LOGO, "res/imgs/menu_tetris.png"),
                new MenuScreenEntry(240, App.LOGO, "res/imgs/menu_sonic.png"),
                new MenuScreenEntry(300, App.LOGO, "res/imgs/menu_altered_beast.png"),
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

            ctx.fillStyle = 'black';
            ctx.fillRect(0, 0, 1920, 1080);

            for (var i = 0; i < this.entries.length; ++i)
            {
                var entry = this.entries[i];
                this.drawMenu(ctx, ms, i, entry);
            } 
        }

        private drawMenu(ctx: CanvasRenderingContext2D, ms: number, i: number, entry: MenuScreenEntry): void
        {
            // Work out the fade amount for this entry

            var fade = 0;

            {
                var cycle = (ms % 2000) / 2000;
                var start = 0.125 * i;
                var end = 0.125 * (i + 2.1);

                fade = util.there_and_back(util.unlerp(cycle, start, end));
            }

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

            ctx.fillStyle = 'hsl(' + entry.hue + ',100%,' + Math.floor(util.lerp(fade, 70, 30)) + '%)';
            ctx.fillRect(x, y, width, height);

            // Draw the image

            ctx.drawImage(entry.imageElement, x, y);

            // Draw the border
            // (color changing, expanding with fade)

            ctx.strokeStyle = 'hsl(' + entry.hue + ',100%,' + Math.floor(util.lerp(fade, 25, 50)) + '%)';
            ctx.lineWidth = 10 + util.lerp(fade, 0, 20);
            ctx.strokeRect(x, y, width, height);

            // Set the button color

            this.getButtons().setButtonColor(i, 'hsl(' + entry.hue + ',100%,' + util.lerp(fade, 20, 50) + '%)');
        }
    }
}