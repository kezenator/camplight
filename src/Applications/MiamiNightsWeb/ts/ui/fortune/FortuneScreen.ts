/// <reference path="../CanvasScreen.ts" />
/// <reference path="../../util/Math.ts" />

namespace ui.fortune
{
    class FortuneColumn
    {
        prefix: boolean;
        chars: string;

        constructor(chars: string)
        {
            this.prefix = Math.random() >= 0.5;

            var extra = Math.floor(20 + 40 * Math.random());
            var extraChars = '';

            for (var i = 0; i < extra; ++i)
            {
                var index = Math.floor(27 * Math.random());

                if (index < 26)
                    extraChars += String.fromCharCode(0x41 + index)
                else
                    extraChars += ' ';
            }

            if (this.prefix)
                this.chars = extraChars + chars;
            else
                this.chars = chars + extraChars;    
        }

        public updateDest(msg: string): void
        {
            var orig = '';

            if (this.prefix)
                orig = this.chars.substr(this.chars.length - FortuneScreen.ROWS);
            else
                orig = this.chars.substr(0, FortuneScreen.ROWS);

            this.prefix = Math.random() >= 0.5;

            var extra = Math.floor(20 + 40 * Math.random());
            var extraChars = '';

            for (var i = 0; i < extra; ++i)
            {
                var index = Math.floor(27 * Math.random());

                if (index < 26)
                    extraChars += String.fromCharCode(0x41 + index)
                else
                    extraChars += ' ';
            }

            if (this.prefix)
                this.chars = orig + extraChars + msg;
            else
                this.chars = msg + extraChars + orig;
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number, x: number, height: number)
        {
            var fade = 1;
            if (ms < FortuneScreen.REVEAL_DELAY)
                fade = ms / FortuneScreen.REVEAL_DELAY;

            var extraRows = this.chars.length - FortuneScreen.ROWS;

            var start = 0;
            var offset = -util.lerp(util.depart_and_arrive(fade), 0, extraRows * height);

            if (!this.prefix)
            {
                start = -extraRows * height;
                offset = -offset;
            }

            for (var i = 0; i < this.chars.length; ++i)
            {
                var y = start + offset + (i * height) + (0.75 * height);

                if ((y >= -height) && (y <= (1080 + height)))
                {
                    var char = this.chars[i];

                    if (char != ' ')
                    {
                        ctx.fillText(char, x, y);
                    }
                }
            }
        }
    }
    export class FortuneScreen extends CanvasScreen
    {
        static COLUMNS: number = 21;
        static ROWS: number = 7;

        static REVEAL_DELAY: number = 5000;
        static EXIT_DELAY: number = 45000;

        private columns: FortuneColumn[];
        private rollStartTime: number;

        public constructor(app: App)
        {
            super(app);

            this.columns = new Array(0);
            this.rollStartTime = 0;

        }

        public show()
        {
            this.columns = this.createColumns(this.writeMessage());
            this.rollStartTime = 0;

            super.show();

            var buttons = this.getButtons();

            buttons.setBackColor('#ff0000');
            buttons.setPlayColor('#000000');
            for (var i = 0; i < Buttons.NUMBER; ++i)
                buttons.setButtonColor(0, '#000000');
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            var origms = ms;
            ms -= this.rollStartTime;

            var buttons = this.getButtons();

            if (buttons.isBackClicked())
            {
                this.getApp().showScreen(App.MENU);
                return;
            }
            else if (ms > FortuneScreen.EXIT_DELAY)
            {
                this.getApp().showScreen(App.LOGO);
                return;
            }

            if (ms < FortuneScreen.REVEAL_DELAY)
            {
                for (var i = 0; i < Buttons.NUMBER; ++i)
                    buttons.setButtonColor(i, '#000000');
            }
            else
            {
                var fade = util.there_and_back(util.unlerp((ms % 1000), 0, 1000));
                var color = 'hsl(0,0%,' + Math.floor(util.lerp(fade, 0, 50)) + '%)';
                for (var i = 0; i < Buttons.NUMBER; ++i)
                    buttons.setButtonColor(i, color);

                for (var i = 0; i < Buttons.NUMBER; ++i)
                {
                    if (buttons.isButtonClicked(i))
                    {
                        this.rollStartTime = origms;
                        ms = 0;
                        this.updateColumns(this.writeMessage());
                        break;
                    }
                }
            }

            ctx.fillStyle = 'black';
            ctx.fillRect(0, 0, 1920, 1080);

            ctx.font = 'bold 80px monospace';

            var cellw = 1920 / FortuneScreen.COLUMNS;
            var cellh = 1080 / FortuneScreen.ROWS;

            for (var i = 0; i < FortuneScreen.COLUMNS; ++i)
            {
                ctx.fillStyle = 'hsl('
                    + Math.floor(260 * i / FortuneScreen.COLUMNS)
                    + ',100%,50%)';

                this.columns[i].draw(
                    ctx,
                    ms,
                    i * cellw,
                    cellh);
            }
        }

        private writeMessage(): string
        {
            var strings: string[];

            if (this.getApp().isSafeMode())
            {
                strings = [
                    "HAPPY NEW YEAR",
                    "MERRY CHRISTMAS AND A HAPPY NEW YEAR",
                    "HAVE FUN AND STAY SAFE",
                ];
            }
            else
            {
                strings = [
                    "YOUR GRANDMOTHER WAS RIGHT - YOU NEED TO GET MORE DICK",
                    "ROSÉ IS THE SOLUTION TO ALL OF LIFE'S PROBLEMS",
                    "WHY ARE YOU ASKING ME? SORT YOUR LIFE OUT YOURSELF!",
                    "BIG DICKS AND VODKA!",
                ];
            }

            return strings[Math.floor(strings.length * Math.random())];
        }

        private createColumns(msg: string): FortuneColumn[]
        {
            var rows = this.messageToRows(msg);
            var columns = this.rowsToColumns(rows);

            var result: FortuneColumn[] = new Array(FortuneScreen.COLUMNS);

            for (var i = 0; i < FortuneScreen.COLUMNS; ++i)
            {
                result[i] = new FortuneColumn(columns[i]);
            }

            return result;
        }

        private updateColumns(msg: string)
        {
            var columns = this.rowsToColumns(this.messageToRows(msg));

            for (var i = 0; i < FortuneScreen.COLUMNS; ++i)
            {
                this.columns[i].updateDest(columns[i]);
            }
        }

        private rowsToColumns(rows: string[]): string[]
        {
            var result: string[] = new Array(FortuneScreen.COLUMNS);

            for (var i = 0; i < FortuneScreen.COLUMNS; ++i)
            {
                var string = '';
                for (var j = 0; j < FortuneScreen.ROWS; ++j)
                {
                    string += rows[j].charAt(i);
                }

                result[i] = string;
            }

            return result;
        }

        private messageToRows(msg: string): string[]
        {
            var result = new Array(0);

            {
                var line = '';
                var words = msg.split(' ');

                for (var i = 0; i < words.length; ++i)
                {
                    if ((line.length + 1 + words[i].length) <= FortuneScreen.COLUMNS)
                    {
                        if (line != '')
                            line += ' ';
                        line += words[i];
                    }
                    else
                    {
                        result.push(this.centerString(line));
                        line = words[i];
                    }
                }

                if (line != '')
                    result.push(this.centerString(line));
            }

            while (result.length < FortuneScreen.ROWS)
            {
                result.splice(0, 0, this.centerString(''));

                if (result.length < FortuneScreen.ROWS)
                    result.push(this.centerString(''));
            }

            return result;
        }

        private centerString(msg: string): string
        {
            while (msg.length < FortuneScreen.COLUMNS)
            {
                msg += ' ';

                if (msg.length < FortuneScreen.COLUMNS)
                    msg = ' ' + msg;
            }

            return msg;
        }
    }
}