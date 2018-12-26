/// <reference path="../CanvasScreen.ts" />
/// <reference path="../../util/Math.ts" />

namespace ui.menu
{
    export class MenuScreen extends CanvasScreen
    {
        static CYCLE_TIME: number = 2000;
        static MOVE_TIME: number = 250;
        static BUTTON_REVEAL_TIME: number = 250;
        static TIMEOUT_TIME: number = 7000;

        private background: Background;
        private entries: MenuEntry[];
        private selected: boolean;
        private selected_index: number;
        private selected_ms: number;

        private music_resources: string[];
        private music_playout: mn.audio.Playout;

        public constructor(app: App, background: Background)
        {
            super(app);

            this.background = background;

            this.entries = [
                new MenuEntry(App.STREET_FIGHTER_II),
                new MenuEntry(App.PONG),
                new MenuEntry(App.SUPER_MARIO_WORLD),
                new MenuEntry(App.TETRIS),
                new MenuEntry(App.SONIC),
                new MenuEntry(App.ALTERED_BEAST),
            ];

            this.selected = false;
            this.selected_index = 0;
            this.selected_ms = 0;

            this.music_resources = new Array(0);

            if (!app.isSafeMode())
                this.music_resources.push('/res/audio/mii-channel-my-neck.ogg');

            this.music_resources.push('/res/audio/mii-channel-fresh-prince.ogg');
            this.music_resources.push('/res/audio/mii-channel-insaneintherainmusic.ogg');

            this.music_playout = null;
        }

        public show(): void
        {
            super.show();

            var buttons = this.getButtons();

            buttons.setPlayColor('#000000');
            buttons.setBackColor('#FF0000');
            buttons.setWashColor('#000000');
            for (var i = 0; i < Buttons.NUMBER; ++i)
                buttons.setButtonColor(i, '#000000');

            for (var entry of this.entries)
                entry.show = false;

            this.selected = false;

            var music = this.music_resources.shift();
            this.music_resources.push(music);

            this.music_playout = this.getApp().getAudio().play(
                music,
                () => { this.handleMusicCompleted(); });
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            var buttons = this.getButtons();

            if (!this.selected)
            {
                if (buttons.isBackClicked())
                {
                    this.music_playout.stop();
                    this.getApp().showScreen(App.LOGO);
                    return;
                }

                for (var i = 0; i < this.entries.length; ++i)
                {
                    if (this.entries[i].show
                        && buttons.isButtonClicked(i))
                    {
                        this.selected = true;
                        this.selected_index = i;
                        this.selected_ms = ms;

                        var entry = this.entries[i];

                        for (var j = 0; j < this.entries.length; ++j)
                            this.entries[j].show = false;
                        entry.show = true;

                        for (var j = 0; j < Buttons.NUMBER; ++j)
                            buttons.setButtonColor(j, entry.buttons[j].color);
                        buttons.setBackColor(entry.buttons[6].color);
                        buttons.setPlayColor(entry.buttons[7].color);
                        buttons.setWashColor('hsl(' + entry.hue + ',100%,50%)');

                        break;
                    }
                }
            }
            else // selected
            {
                if (buttons.isPlayClicked()
                    || ((ms - this.selected_ms) >= MenuScreen.TIMEOUT_TIME))
                {
                    this.music_playout.stop();
                    this.getApp().showScreen(this.entries[this.selected_index].screen);
                    return;
                }
            }

            this.background.draw(ctx, ms);

            if (ms >= MenuScreen.CYCLE_TIME)
            {
                for (var i = 0; i < this.entries.length; ++i)
                {
                    var entry = this.entries[i];
                    this.drawMenu(ctx, ms, i, entry);
                }

                if (this.selected)
                {
                    this.drawHelp(ctx, ms, this.entries[this.selected_index]);
                }
                else
                {
                    // Scroll the wash color with the selected index
                    // Each light goes from 0 to 0.2625, offset by 0.125 amounts
                    // So the brightest point of each is at 0.13125

                    var cycle = (ms % MenuScreen.CYCLE_TIME) / MenuScreen.CYCLE_TIME;
                    var peak_first = 0.13125;
                    var peak_last = 0.13125 + (5 * 0.125);
                    var end = 0.125 * 7.1;

                    if (cycle < peak_first)
                    {
                        buttons.setWashColor('hsl('
                            + this.entries[0].hue
                            + ',100%,'
                            + util.lerp(cycle / peak_first, 0, 50).toString()
                            + '%)');
                    }
                    else if (cycle > peak_last)
                    {
                        buttons.setWashColor('hsl('
                            + this.entries[5].hue
                            + ',100%,'
                            + util.lerp(util.unlerp(cycle, peak_last, end), 50, 0).toString()
                            + '%)');
                    }
                    else
                    {
                        var index_float = 5 * util.unlerp(cycle, peak_first, peak_last);
                        var index = Math.floor(index_float);
                        var sweep = index_float - index;

                        buttons.setWashColor('hsl('
                            + util.lerp(sweep, this.entries[index].hue, this.entries[index + 1].hue).toString()
                            + ',100%,50%)');
                    }
                }
            }
            else
            {
                // Fade in
                ctx.fillStyle = 'rgba(0,0,0,' + (1 - (ms / MenuScreen.CYCLE_TIME)) + ')';
                ctx.fillRect(0, 0, 1920, 1080);
            }
        }

        private drawMenu(ctx: CanvasRenderingContext2D, ms: number, i: number, entry: MenuEntry): void
        {
            // Work out the fade amount for this entry

            var swipe = 0;

            {
                var cycle = (ms % MenuScreen.CYCLE_TIME) / MenuScreen.CYCLE_TIME;
                var start = 0.125 * i;
                var end = 0.125 * (i + 2.1);

                swipe = util.unlerp(cycle, start, end);
            }

            // Work out if it should be shown

            if (!this.selected
                && (swipe > 0))
            {
                entry.show = true;
            }

            if (!entry.show)
            {
                return;
            }

            // Work out the location for this entry

            var width = 505;
            var height = 390;

            var x = entry.x;
            var y = entry.y;

            if (this.selected)
            {
                var move = util.lerp((ms - this.selected_ms) / MenuScreen.MOVE_TIME, 0, 1);
                x = util.lerp(util.arrive(move), entry.x, entry.selected_x);
                y = util.lerp(util.arrive(move), entry.y, entry.selected_y);
            }

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

            if (!this.selected)
            {
                this.getButtons().setButtonColor(i, 'hsl(' + entry.hue + ',100%,' + util.lerp(fade, 15, 50) + '%)');
            }
        }

        private drawHelp(ctx: CanvasRenderingContext2D, ms: number, entry: MenuEntry): void
        {
            if ((ms - this.selected_ms) <= MenuScreen.MOVE_TIME)
            {
                // Still moving - don't show
                return;
            }

            function drawButton(x: number, y: number, but: ButtonEntry)
            {
                ctx.fillStyle = 'black';
                ctx.beginPath();
                ctx.arc(x, y + 70, 50, 0, 2 * Math.PI);
                ctx.fill();

                ctx.fillStyle = but.color;
                ctx.beginPath();
                ctx.arc(x, y + 70, 43, 0, 2 * Math.PI);
                ctx.fill();

                ctx.font = '50px "KarmaticArcade"';
                ctx.fillStyle = 'white';

                var meas = ctx.measureText(but.help1);

                ctx.fillText(but.help1, x - 0.5 * meas.width, y + 180);

                meas = ctx.measureText(but.help2);
                ctx.fillText(but.help2, x - 0.5 * meas.width, y + 240);
            }

            var width = 1920 / 6;
            var offset = 0.5 * width;

            var clipped = (ms - this.selected_ms) < (MenuScreen.MOVE_TIME + MenuScreen.BUTTON_REVEAL_TIME);

            if (clipped)
            {
                ctx.save();

                var clip_width =
                    util.lerp(
                        util.unlerp(
                            ms - this.selected_ms,
                            MenuScreen.MOVE_TIME,
                            MenuScreen.MOVE_TIME + MenuScreen.BUTTON_REVEAL_TIME),
                        0,
                        1920);

                if (clip_width > 0)
                {
                    ctx.beginPath();
                    ctx.rect(0, entry.help_y, clip_width, 540);
                    ctx.clip();
                }
            }

            for (var i = 0; i < 6; ++i)
            {
                drawButton(
                    offset + (i * width),
                    entry.help_y,
                    entry.buttons[i]);
            }

            drawButton(
                offset + (2 * width),
                entry.help_y + 270,
                entry.buttons[6]);
            drawButton(
                offset + (3 * width),
                entry.help_y + 270,
                entry.buttons[7]);

            // Now also draw the count down timer

            var end_ms = this.selected_ms + MenuScreen.TIMEOUT_TIME;
            var start_ms = this.selected_ms + MenuScreen.MOVE_TIME;

            var range = util.unlerp(ms, start_ms, end_ms);

            ctx.lineWidth = 30;
            ctx.lineCap = 'butt';
            ctx.strokeStyle = 'cornflowerblue';
            ctx.font = '50px "KarmaticArcade"';

            var label = Math.floor(1 + (MenuScreen.TIMEOUT_TIME - (ms - start_ms)) / 1000).toString();

            var meas = ctx.measureText(label);

            ctx.beginPath();
            ctx.arc(
                1567, entry.selected_y + 200,
                100,
                1.5 * Math.PI,
                (1.5 + range * 2) * Math.PI);
            ctx.stroke();
            ctx.fillText(label, 1567 - 0.5 * meas.width, entry.selected_y + 225);

            if (clipped)
            {
                ctx.restore();
            }
        }

        handleMusicCompleted(): void
        {
            this.getApp().showScreen(App.LOGO);
        }
    }
}