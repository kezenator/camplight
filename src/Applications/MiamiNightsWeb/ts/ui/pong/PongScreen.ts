/// <reference path="../CanvasScreen.ts" />
/// <reference path="../../util/Math.ts" />

namespace ui.pong
{
    enum PongState
    {
        INSTRUCTIONS,
        PLAYING,
        SCORE,
        WIN,
    }

    export class PongScreen extends CanvasScreen
    {
        private static FRAMEX: number = 140;
        private static FRAMEY: number = 140;
        private static FRAMEW: number = 1750;
        private static FRAMEH: number = 910;

        private static PADDLE_SIZE: number = 200;
        private static PADDLE_OFFSET: number = 30;

        private static BALL_SIZE: number = 50;

        private static VOL_X: number = 0.55;
        private static MAX_VOL_Y: number = 0.3;

        private state: PongState;
        private delayms: number;
        private lastms: number;
        private pos1: number;
        private pos2: number;
        private score1: number;
        private score2: number;
        private ballx: number;
        private bally: number;
        private volx: number;
        private voly: number;
        private curfade: number;
        private targetfade: number;

        private logoImage: HTMLImageElement;

        public constructor(app: App)
        {
            super(app);

            this.state = PongState.INSTRUCTIONS;
            this.delayms = 8000;
            this.lastms = 0;
            this.pos1 = PongScreen.FRAMEX + 0.5 * PongScreen.FRAMEH;
            this.pos2 = this.pos1;
            this.score1 = 0;
            this.score2 = 0;
            this.ballx = PongScreen.FRAMEX + 0.5 * PongScreen.FRAMEW;
            this.bally = PongScreen.FRAMEY + 0.5 * PongScreen.FRAMEH;
            this.volx = 0;
            this.voly = 0;
            this.curfade = 0;
            this.targetfade = 0;

            this.logoImage = document.createElement('img');
            this.logoImage.src = 'res/imgs/pong_logo.png';
        }

        public show()
        {
            super.show();

            this.state = PongState.INSTRUCTIONS;
            this.delayms = 8000;
            this.lastms = 0;
            this.pos1 = PongScreen.FRAMEX + 0.5 * PongScreen.FRAMEH;
            this.pos2 = this.pos1;
            this.score1 = 0;
            this.score2 = 0;
            this.ballx = PongScreen.FRAMEX + 0.5 * PongScreen.FRAMEW;
            this.bally = PongScreen.FRAMEY + 0.5 * PongScreen.FRAMEH;
            this.volx = PongScreen.VOL_X * ((Math.random() < 0.5) ? 1 : -1);
            this.voly = util.lerp(Math.random(), -PongScreen.MAX_VOL_Y, PongScreen.MAX_VOL_Y);
            this.curfade = 0.5;
            this.targetfade = 0.5;
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            var span_ms = ms - this.lastms;
            this.lastms = ms;

            if (!this.handleButtons(span_ms))
                return;

            if (!this.updateState(span_ms))
                return;

            if (this.state == PongState.PLAYING)
                this.updateBall(span_ms);

            this.updateFade(span_ms);

            ctx.fillStyle = 'black';
            ctx.fillRect(0, 0, 1920, 1080);

            this.drawFrame(ctx);
            this.drawScore(ctx);

            if (this.state == PongState.PLAYING)
                this.drawBall(ctx);
        }

        private handleButtons(span_ms: number): boolean
        {
            var buttons = this.getButtons();

            if (buttons.isBackClicked())
            {
                this.getApp().showScreen(App.MENU);
                return false;
            }

            buttons.setBackColor('#ff0000');
            buttons.setPlayColor('#000000');

            var p1_on = true;
            var p2_on = true;

            if (this.state == PongState.INSTRUCTIONS)
            {
                var step = Math.floor(this.delayms / 1000);
                var on = (this.delayms % 250) < 125;

                if (step == 6)
                {
                    p1_on = p2_on = on;
                }
                else if (step == 5)
                {
                    p1_on = on;
                    p2_on = false;
                }
                else if (step == 4)
                {
                    p1_on = false;
                    p2_on = on;
                }
            }

            buttons.setButtonColor(2, '#000000');
            buttons.setButtonColor(3, '#000000');

            if (p1_on)
            {
                buttons.setButtonColor(0, '#ff0000');
                buttons.setButtonColor(1, '#ff0000');
            }
            else
            {
                buttons.setButtonColor(0, '#000000');
                buttons.setButtonColor(1, '#000000');
            }

            if (p2_on)
            {
                buttons.setButtonColor(4, '#ffff00');
                buttons.setButtonColor(5, '#ffff00');
            }
            else
            {
                buttons.setButtonColor(4, '#000000');
                buttons.setButtonColor(5, '#000000');
            }

            // Player movement

            function updatePaddle(orig: number, up: boolean, down: boolean)
            {
                var min = PongScreen.FRAMEY + 0.5 * PongScreen.PADDLE_SIZE;
                var max = PongScreen.FRAMEY + PongScreen.FRAMEH - 0.5 * PongScreen.PADDLE_SIZE;

                if (!up && !down)
                    return orig;
                else if (up && down)
                    return orig;
                else if (up)
                    return util.constrain(orig - 0.8 * span_ms, min, max);
                else
                    return util.constrain(orig + 0.8 * span_ms, min, max);
            };

            this.pos1 = updatePaddle(this.pos1, buttons.isButtonPressed(0), buttons.isButtonPressed(1));
            this.pos2 = updatePaddle(this.pos2, buttons.isButtonPressed(4), buttons.isButtonPressed(5));

            return true;
        }

        private updateBall(span_ms: number)
        {
            this.ballx += this.volx * span_ms;
            this.bally += this.voly * span_ms;

            var minx = PongScreen.FRAMEX + PongScreen.PADDLE_OFFSET + PongScreen.BALL_SIZE;
            var maxx = PongScreen.FRAMEX + PongScreen.FRAMEW - PongScreen.PADDLE_OFFSET - PongScreen.BALL_SIZE;

            var miny = PongScreen.FRAMEY + PongScreen.BALL_SIZE;
            var maxy = PongScreen.FRAMEY + PongScreen.FRAMEH - PongScreen.BALL_SIZE;

            // Reflect off top and bottom walls

            if (this.bally > maxy)
            {
                this.bally = maxy - (this.bally - maxy);
                this.voly = -this.voly;
            }
            else if (this.bally < miny)
            {
                this.bally = miny + (miny - this.bally);
                this.voly = -this.voly;
            }

            // Reflect off paddles, or score

            var restart: number = 0;

            function hitsPaddle(pos: number, bally: number): boolean
            {
                var offset = pos - bally;

                if (offset < (-0.65 * PongScreen.PADDLE_SIZE))
                    return false;
                else if (offset > (0.65 * PongScreen.PADDLE_SIZE))
                    return false;
                else
                    return true;
            }

            function paddleBump(pos: number, bally: number): number
            {
                var factor = (pos - bally) / (0.5 * PongScreen.PADDLE_SIZE);
                var sign = factor / Math.abs(factor);
                factor = -sign * factor * factor;   
                return factor * PongScreen.MAX_VOL_Y;
            }

            if (this.ballx > maxx)
            {
                if (hitsPaddle(this.pos2, this.bally))
                {
                    this.ballx = maxx - (this.ballx - maxx);
                    this.volx = -this.volx;
                    this.voly = util.constrain(
                        this.voly + paddleBump(this.pos2, this.bally),
                        -PongScreen.MAX_VOL_Y,
                        PongScreen.MAX_VOL_Y);
                }
                else
                {   
                    this.score1 += 1;
                    restart = 1;
                }
            }
            else if (this.ballx < minx)
            {
                if (hitsPaddle(this.pos1, this.bally))
                {
                    this.ballx = minx + (minx - this.ballx);
                    this.volx = -this.volx;
                    this.voly = util.constrain(
                        this.voly + paddleBump(this.pos1, this.bally),
                        -PongScreen.MAX_VOL_Y,
                        PongScreen.MAX_VOL_Y);
                }
                else
                {
                    this.score2 += 1;
                    restart = -1;
                }
            }

            if (restart != 0)
            {
                this.ballx = PongScreen.FRAMEX + 0.5 * PongScreen.FRAMEW;
                this.bally = PongScreen.FRAMEY + 0.5 * PongScreen.FRAMEH;
                this.volx = PongScreen.VOL_X * restart;
                this.voly = util.lerp(Math.random(), -PongScreen.MAX_VOL_Y, PongScreen.MAX_VOL_Y);

                if ((this.score1 >= 11)
                    || (this.score2 >= 11))
                {
                    this.state = PongState.WIN;
                    this.delayms = 15000;
                }
                else
                {
                    this.state = PongState.SCORE;
                    this.delayms = 1500;
                }
            }
        }

        private updateState(span_ms: number): boolean
        {
            if (span_ms > this.delayms)
                this.delayms = 0;
            else
                this.delayms -= span_ms;

            if (this.delayms == 0)
            {
                switch (this.state)
                {
                    case PongState.INSTRUCTIONS:
                    case PongState.SCORE:
                        this.state = PongState.PLAYING;
                        break;

                    case PongState.WIN:
                        this.getApp().showScreen(App.LOGO);
                        return false;

                    case PongState.PLAYING:
                        // Nothing
                        break;
                }
            }

            return true;
        }

        private updateFade(span_ms: number)
        {
            if (this.score1 >= 11)
                this.targetfade = 1;
            else if (this.score2 >= 11)
                this.targetfade = 0;
            else
                this.targetfade = util.unlerp(this.score1 - this.score2, -5, 5);
                                            
            if (this.curfade != this.targetfade)
            {
                this.curfade = util.lerp(
                    util.constrain(span_ms / 1000, 0, 1),
                    this.curfade,
                    this.targetfade);
            }
        }

        private drawScore(ctx: CanvasRenderingContext2D): void
        {
            function drawText(x: number, y: number, align: number, text: string)
            {
                var metrics = ctx.measureText(text);

                if (align < 0)
                    x -= metrics.width;
                else if (align == 0)
                    x -= 0.5 * metrics.width;
               
                ctx.strokeText(text, x, y);
                ctx.fillText(text, x, y);
            };

            ctx.font = 'bold 300px monospace';
            ctx.lineWidth = 15;

            ctx.strokeStyle = 'hsl(30,100%,60%)';
            ctx.fillStyle = 'hsl(0,100%,50%)';
            drawText(
                PongScreen.FRAMEX + (2 * PongScreen.PADDLE_OFFSET),
                230 + PongScreen.FRAMEY,
                1,
                this.score1.toString());

            ctx.strokeStyle = 'hsl(30,100%,60%)';
            ctx.fillStyle = 'hsl(60,100%,50%)';
            drawText(
                PongScreen.FRAMEX + PongScreen.FRAMEW - (2 * PongScreen.PADDLE_OFFSET),
                230 + PongScreen.FRAMEY,
                -1,
                this.score2.toString());

            var text: string | null = null;
            switch (this.state)
            {
                case PongState.INSTRUCTIONS:
                    switch (Math.floor(this.delayms / 1000))
                    {
                        case 7:
                            text = "--- PONG ---";
                            break;
                        case 6:
                            text = "Move Paddles";
                            break;
                        case 5:
                            text = "P1 Red                  ";
                            break;
                        case 4:
                            text = "               Yellow P2";
                            break;
                        case 3:
                            text = "First to 11 wins";
                            break;
                        case 2:
                            text = "--- Three ---";
                            break;
                        case 1:
                            text = "--- Two ---";
                            break;
                        case 0:
                            text = "--- One ---";
                            break;
                    } 
                    break;

                case PongState.PLAYING:
                    break;

                case PongState.SCORE:    
                    if (this.volx > 0)
                        text = "P1 Scores";
                    else
                        text = "P2 Scores";
                    break;

                case PongState.WIN:
                    if (this.score1 >= 11)
                        text = "--- P1 Wins ---";
                    else
                        text = "--- P2 Wins ---";
                    break;
            }

            if (text != null)
            {
                ctx.font = '120px "KarmaticArcade"';
                ctx.lineWidth = 4;
                ctx.fillStyle = 'hsl(205,100%,58%)';
                ctx.strokeStyle = 'hsl(195,100%,58%)';
                drawText(
                    PongScreen.FRAMEX + 0.5 * PongScreen.FRAMEW,
                    630 + PongScreen.FRAMEY,
                    0,
                    text);
            }
        }

        private drawFrame(ctx: CanvasRenderingContext2D): void
        {
            // Tri-color curved border with logo

            function border(offset: number, radius: number)
            {
                ctx.beginPath();
                ctx.moveTo(offset, 1080);
                ctx.lineTo(offset, offset + radius);
                ctx.arcTo(offset, offset, offset + radius, offset, radius);
                ctx.lineTo(1920, offset);
                ctx.stroke();
            }

            ctx.lineWidth = 30;

            ctx.strokeStyle = 'rgb(255,0,0)';
            border(30, 90);
            ctx.strokeStyle = 'rgb(255,153,51)';
            border(60, 60);
            ctx.strokeStyle = 'rgb(255,255,0)';
            border(90, 30);

            ctx.drawImage(this.logoImage, 1600, 0);

            // Frame fill

            function createGrad(fade: number)
            {
                var radius = 1200;
                var width = 500;
                var y = PongScreen.FRAMEY + 0.5 * PongScreen.FRAMEH;

                var start = PongScreen.FRAMEX + util.lerp(fade, 0, PongScreen.FRAMEW - width);

                return ctx.createRadialGradient(
                    start - radius, y, radius,
                    start - radius, y, radius + width);
            }

            var grad = createGrad(this.curfade);
            grad.addColorStop(0, 'hsl(0,20%,15%)');
            grad.addColorStop(0.5, 'hsl(30,20%,16%)');
            grad.addColorStop(1, 'hsl(60,20%,15%)');

            ctx.fillStyle = grad;

            ctx.fillRect(PongScreen.FRAMEX, PongScreen.FRAMEY, PongScreen.FRAMEW, PongScreen.FRAMEH);

            // Frame border

            var grad = createGrad(this.curfade);
            grad.addColorStop(0, 'hsl(0,100%,50%)');
            grad.addColorStop(0.5, 'hsl(30,100%,60%)');
            grad.addColorStop(1, 'hsl(60,100%,50%)');

            ctx.strokeStyle = grad;

            ctx.lineWidth = 10;
            ctx.strokeRect(PongScreen.FRAMEX, PongScreen.FRAMEY, PongScreen.FRAMEW, PongScreen.FRAMEH);

            // Net and paddles

            var parts = 10;
            var space = 30;
            var inc = (PongScreen.FRAMEH - space) / parts;
            var length = inc - space;
            var x = PongScreen.FRAMEX + 0.5 * PongScreen.FRAMEW;

            ctx.beginPath();

            for (var pos = PongScreen.FRAMEY + space;
                pos < (PongScreen.FRAMEY + PongScreen.FRAMEH);
                pos += inc)
            {
                ctx.moveTo(x, pos);
                ctx.lineTo(x, pos + length);
            }

            function drawPaddle(x: number, y: number)
            {
                ctx.moveTo(x, y - (0.5 * PongScreen.PADDLE_SIZE));
                ctx.lineTo(x, y + (0.5 * PongScreen.PADDLE_SIZE));
            }

            drawPaddle(PongScreen.FRAMEX + PongScreen.PADDLE_OFFSET, this.pos1);
            drawPaddle(PongScreen.FRAMEX + PongScreen.FRAMEW - PongScreen.PADDLE_OFFSET, this.pos2);

            ctx.stroke();
        }

        private drawBall(ctx: CanvasRenderingContext2D)
        {
            ctx.lineWidth = 20;
            ctx.fillStyle = 'hsl(205,100%,58%)';
            ctx.strokeStyle = 'hsl(195,100%,58%)';

            ctx.beginPath();
            ctx.arc(this.ballx, this.bally, PongScreen.BALL_SIZE - 10, 0, 2 * Math.PI);
            ctx.stroke();

            ctx.beginPath();
            ctx.arc(this.ballx, this.bally, PongScreen.BALL_SIZE - 10, 0, 2 * Math.PI);
            ctx.fill();
        }
    }
}