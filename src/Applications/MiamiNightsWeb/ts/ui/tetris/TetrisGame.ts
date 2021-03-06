/// <reference path="PieceFactory.ts" />
/// <reference path="GameBoard.ts" />

namespace ui.tetris
{
    class TetrisButton
    {
        static SPEED: number = 250;
        static HOLD_TIME: number = 500;
        static RECLICK_TIME: number = 250;

        want_held: boolean;
        click: boolean;
        pressed: boolean;
        held: boolean;
        down_time: number;
        last_click_ms: number;
        waiting: boolean;

        constructor(want_held: boolean)
        {
            this.want_held = want_held;
            this.click = false;
            this.pressed = false;
            this.held = false;
            this.down_time = 0;
            this.last_click_ms = 0;
            this.waiting = false;
        }

        update(ms: number, clicked: boolean, pressed: boolean)
        {
            // If waiting and not pressed, then clear waiting.
            // Otherwise we're not pressed

            if (this.waiting
                && pressed)
            {
                this.click = false;
                this.pressed = false;
                this.held = false;
                return;
            }

            this.waiting = false;

            // If "clicked" then treat as if it
            // wasn't pressed an now is

            if (clicked)
            {
                this.pressed = false;
                pressed = true;
            }

            if (pressed
                && !this.pressed)
            {
                // Just pressed down

                this.down_time = ms;
                this.last_click_ms = ms;
                this.click = !this.want_held;
                this.held = false;
            }
            else if (pressed
                && this.pressed)
            {
                // Continuing to be pressed

                if (!this.held
                    && (ms - this.down_time) >= TetrisButton.HOLD_TIME)
                {
                    this.click = true;
                    this.held = true;
                    this.last_click_ms += TetrisButton.HOLD_TIME;
                }
                else if (this.held
                    && ((ms - this.last_click_ms) >= TetrisButton.RECLICK_TIME))
                {
                    this.click = true;
                    this.last_click_ms += TetrisButton.RECLICK_TIME;
                }
                else
                {
                    this.click = false;
                }
            }
            else
            {
                // Not pressed

                if (this.want_held
                    && this.pressed)
                {
                    this.click = true;
                    this.held = false;
                }
                else
                {
                    this.click = false;
                    this.held = false;
                }
            }

            this.pressed = pressed;
        }

        clicked(): boolean
        {
            return this.click;
        }

        is_held(): boolean
        {
            return this.held;
        }

        clearClicked(): void
        {
            this.click = false;
        }

        waitForRepress(): void
        {
            this.waiting = true;
            this.click = false;
            this.held = false;
        }
    };

    export class TetrisGame
    {
        private x: number;
                                     
        private gameBoard: GameBoard;
        private pieceFactory: PieceFactory;
        private nextPiece: Piece;
        private curPiece: Piece;
        private shadowPiece: Piece | null;
        private lastMoveMs: number;
        private gameOver: boolean;
        private gameOverTime: number;
        private level: number;
        private score: number;
        private left_btn: TetrisButton;
        private right_btn: TetrisButton;
        private other_btn: TetrisButton;

        public constructor(x: number, pieces_img: HTMLImageElement, safe: boolean)
        {
            this.x = x;

            this.gameBoard = new GameBoard();
            this.pieceFactory = new PieceFactory(pieces_img, safe);

            this.curPiece = this.pieceFactory.newPiece();
            this.nextPiece = this.pieceFactory.newPiece();
            this.shadowPiece = null;

            this.lastMoveMs = 0;
            this.gameOver = false;
            this.gameOverTime = 0;
            this.level = 1;
            this.score = 0;
            this.left_btn = new TetrisButton(false);
            this.right_btn = new TetrisButton(false);
            this.other_btn = new TetrisButton(true);
        }       

        public isTimedOut(ms: number): boolean
        {
            return this.gameOver
                && (ms > (this.gameOverTime + 5000));
        }

        public handleButtons(ms: number,
            left_clicked: boolean,
            left_pressed: boolean,
            other_clicked: boolean,
            other_pressed: boolean,
            right_clicked: boolean,
            right_pressed: boolean): void
        {
            this.left_btn.update(ms, left_clicked, left_pressed);
            this.right_btn.update(ms, right_clicked, right_pressed);
            this.other_btn.update(ms, other_clicked, other_pressed);

            var need_new_shadow = false;

            while (!this.gameOver)
            {
                var need_lock = false;
                var need_break = false;

                if (this.left_btn.clicked() || this.right_btn.clicked())
                {
                    var move = false;
                    var offset = 0;

                    if (this.left_btn.clicked() && !this.right_btn.clicked())
                    {
                        move = true;
                        offset = -1;
                    }
                    else if (!this.left_btn.clicked() && this.right_btn.clicked())
                    {
                        move = true;
                        offset = 1;
                    }

                    if (move)
                    {
                        this.curPiece.move(offset, 0);

                        if (this.curPiece.fits(this.gameBoard))
                        {
                            this.lastMoveMs = ms;
                            need_new_shadow = true;
                        }
                        else
                            this.curPiece.move(-offset, 0);
                    }

                    this.left_btn.clearClicked();
                    this.right_btn.clearClicked();
                }

                if (this.other_btn.is_held())
                {
                    // Replace the current piece with the
                    // shadow piece, lock it, and
                    // ignore the other button until re-pressed

                    this.curPiece = this.shadowPiece;
                    need_lock = true;
                    this.other_btn.waitForRepress();
                }
                else if (this.other_btn.clicked())
                {
                    this.curPiece.rotate(1);

                    var kicks = this.curPiece.getKickData();
                    var accept = false;

                    for (var i = 0; i < kicks.length; ++i)
                    {
                        var kick = kicks[i];

                        this.curPiece.move(kick.x, kick.y);

                        if (this.curPiece.fits(this.gameBoard))
                        {
                            accept = true;
                            break;
                        }

                        this.curPiece.move(-kick.x, -kick.y);
                    }

                    if (accept)
                    {
                        this.lastMoveMs = ms;
                        this.other_btn.clearClicked();
                        need_new_shadow = true;
                    }
                    else // revert
                    {
                        this.curPiece.rotate(-1);
                    }
                }

                var speed = TetrisButton.SPEED;
                var delay = ms - this.lastMoveMs;

                if (delay < speed)
                {
                    need_break = true;
                }
                else
                {
                    // Update time

                    this.lastMoveMs += speed;

                    // Move down automatically

                    if (!need_lock)
                    {
                        need_new_shadow = true;

                        this.curPiece.move(0, 1);

                        if (!this.curPiece.fits(this.gameBoard))
                        {
                            this.curPiece.move(0, -1);
                            need_lock = true;
                        }
                    }
                }

                if (need_lock)
                {
                    this.curPiece.lock(this.gameBoard);

                    this.score += this.gameBoard.clearCompleteRows();

                    this.curPiece = this.nextPiece;
                    this.nextPiece = this.pieceFactory.newPiece();
                    need_new_shadow = true;

                    if (!this.curPiece.fits(this.gameBoard))
                    {
                        // Doesn't fit above the screen???
                        // Don't know how this can happen - but anyway...
                        this.gameOver = true;
                        this.gameOverTime = ms;
                    }
                    else
                    {
                        // Try and move it down onto the screen
                        this.curPiece.move(0, 1);
                        if (!this.curPiece.fits(this.gameBoard))
                        {
                            // Hide it an end the game
                            this.curPiece.move(0, -1);
                            this.gameOver = true;
                            this.gameOverTime = ms;
                        }
                    }

                    // Reset the buttons so you need to press again
                    this.left_btn.waitForRepress();
                    this.right_btn.waitForRepress();
                    this.other_btn.waitForRepress();
                }

                if (need_break)
                {
                    break;
                }
            }

            if ((this.shadowPiece == null)
                || need_new_shadow)
            {
                this.shadowPiece = this.curPiece.clone();

                while (true)
                {
                    this.shadowPiece.move(0, 1);

                    if (!this.shadowPiece.fits(this.gameBoard))
                    {
                        this.shadowPiece.move(0, -1);
                        break;
                    }
                }
            }
        }

        public draw(ctx: CanvasRenderingContext2D): void
        {
            this.drawBoard(ctx);
            this.drawInfo(ctx);
        }

        private drawBoard(ctx: CanvasRenderingContext2D): void
        {
            // Save some information as local vars

            var x = this.x + 240;
            var y = 60;

            var num_rows = GameBoard.NUM_ROWS - GameBoard.HIDDEN_ROWS;
            var num_cols = GameBoard.NUM_COLS;
            var tile_size = Cell.TILE_SIZE;

            var center_x = x + (num_cols * 0.5 * tile_size);
            var center_y = y + (num_rows * 0.5 * tile_size);

            // Draw the background

            {
                var grad = ctx.createRadialGradient(
                    center_x, center_y, 10,
                    center_x, center_y, num_rows * 0.48 * tile_size);

                grad.addColorStop(0, '#4d74a8');
                grad.addColorStop(1, '#013765');

                ctx.fillStyle = grad;

                ctx.fillRect(
                    x, y,
                    num_cols * tile_size, num_rows * tile_size);
            }

            // Draw the grid

            {
                var grad = ctx.createRadialGradient(
                    center_x, center_y, 10,
                    center_x, center_y, num_rows * 0.48 * tile_size);

                grad.addColorStop(0, '#04d6fd');
                grad.addColorStop(1, '#0348e7');

                ctx.strokeStyle = grad;
                ctx.lineWidth = 3;

                ctx.beginPath();

                for (var i = 0; i <= num_rows; i++)
                {
                    ctx.moveTo(x, y + i * tile_size);
                    ctx.lineTo(x + num_cols * tile_size, y + i * tile_size);
                }

                for (var i = 0; i <= num_cols; i++)
                {
                    ctx.moveTo(x + i * tile_size, y);
                    ctx.lineTo(x + i * tile_size, y + num_rows * tile_size);
                }

                ctx.stroke();
            }

            // Draw the border

            {
                var border_x = x + 0.4 * num_cols * tile_size;
                var border_y = y + 0.3 * num_rows * tile_size;

                var grad = ctx.createRadialGradient(
                    border_x, border_y, 10,
                    border_x, border_y, num_rows * 0.88 * tile_size);

                grad.addColorStop(0, '#04d6fd');
                grad.addColorStop(1, '#0348e7');

                ctx.strokeStyle = grad;
                ctx.lineWidth = 3;

                ctx.strokeRect(
                    x - 10,
                    y - 10,
                    num_cols * tile_size + 20,
                    num_rows * tile_size + 20);
            }

            // Draw the game board pices

            this.gameBoard.draw(ctx, x, y);

            // Draw the current piece
            // and it's shadow

            if (this.shadowPiece != null)
                this.shadowPiece.draw(ctx, x, y, true);

            this.curPiece.draw(ctx, x, y, false);
        }

        private drawInfo(ctx: CanvasRenderingContext2D): void
        {
            var x = this.x;

            var num_rows = GameBoard.NUM_ROWS - GameBoard.HIDDEN_ROWS;
            var num_cols = GameBoard.NUM_COLS;
            var tile_size = Cell.TILE_SIZE;

            function drawText(str: string, x: number, y: number, align: number)
            {
                var meas = ctx.measureText(str);

                if (align == 0)
                    x -= 0.5 * meas.width;
                else if (align < 0)
                    x -= meas.width;

                ctx.fillText(str, x, y);
                ctx.strokeText(str, x, y);
            }

            function drawBorder(x: number, y: number, width: number, height: number, offtl: number, offbl: number, offtr: number, offbr: number)
            {
                var center_x = x + 0.5 * width;
                var center_y = y + 0.5 * height;

                var size = Math.max(width, height);

                var grad = ctx.createRadialGradient(
                    center_x, center_y, 10,
                    center_x, center_y, 0.8 * size);

                grad.addColorStop(0, '#4d74a8');
                grad.addColorStop(1, '#013765');

                ctx.fillStyle = grad;

                var border_x = x + 0.2 * width;
                var border_y = y + 0.2 * height;

                grad = ctx.createRadialGradient(
                    border_x, border_y, 10,
                    border_x, border_y, 0.8 * size);

                grad.addColorStop(0, '#04d6fd');
                grad.addColorStop(1, '#0348e7');

                ctx.strokeStyle = grad;
                ctx.lineWidth = 1;

                ctx.beginPath();
                ctx.moveTo(x, y + offtl);
                ctx.lineTo(x + width, y + offtr);
                ctx.lineTo(x + width, y + height + offbr);
                ctx.lineTo(x, y + height + offbl);
                ctx.closePath();

                ctx.fill();
                ctx.stroke();
            }

            // Draw the game over text

            if (this.gameOver)
            {
                ctx.font = '150px "ThirdRail"';
                ctx.fillStyle = 'red';
                ctx.strokeStyle = 'black';
                ctx.lineWidth = 3;

                var center_x = x + 240 + 0.5 * num_cols * tile_size;
                var center_y = 60 + 0.5 * num_rows * tile_size;

                drawText("GAME", center_x, center_y - 50, 0);
                drawText("OVER", center_x, center_y + 90, 0);
            }

            // Draw the score

            drawBorder(x + 15, 45, 200, 200,
                0, 0, 0, 100);

            ctx.font = '50px "Segoe UI Black"';
            ctx.fillStyle = 'white';
            ctx.strokeStyle = 'black';
            ctx.lineWidth = 1;

            drawText("SCORE", x + 210, 100, -1);

            ctx.font = 'bold 60px "Consolas"';

            drawText(this.score.toString(), x + 210, 160, -1);

            // Draw the level

            drawBorder(x + 15, 830, 200, 200,
                0, 0, -100, 0);

            ctx.font = '50px "Segoe UI Black"';
            ctx.fillStyle = 'white';
            ctx.strokeStyle = 'black';
            ctx.lineWidth = 1;

            drawText("LEVEL", x + 210, 900, -1);

            ctx.font = 'bold 120px "Consolas"';

            drawText(this.level.toString(), x + 210, 1010, -1);

            // Draw the next pice

            drawBorder(x + 745, 45, 200, 300,
                0, 100, 0, 0);

            ctx.font = '50px "Segoe UI Black"';
            ctx.fillStyle = 'white';
            ctx.strokeStyle = 'black';
            ctx.lineWidth = 1;

            drawText("NEXT", x + 760, 100, 1);

            this.nextPiece.drawAsNext(ctx, x + 845, 200);
        }
    }
}