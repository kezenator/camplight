/// <reference path="PieceFactory.ts" />
/// <reference path="GameBoard.ts" />

namespace ui.tetris
{
    export class TetrisGame
    {
        private x: number;
        private y: number;
                                     
        private gameBoard: GameBoard;
        private pieceFactory: PieceFactory;
        private nextPiece: Piece;
        private curPiece: Piece;
        private lastMoveMs: number;
        private gameOver: boolean;

        public constructor(x: number, y: number)
        {
            this.x = x;
            this.y = y;

            this.gameBoard = new GameBoard();
            this.pieceFactory = new PieceFactory();

            this.curPiece = this.pieceFactory.newPiece();
            this.nextPiece = this.pieceFactory.newPiece();

            this.lastMoveMs = 0;
            this.gameOver = false;
        }       

        public handleButtons(ms: number, left: boolean, down: boolean, right: boolean): void
        {
            while (!this.gameOver)
            {
                var need_lock = false;
                var need_break = false;

                if (left || right)
                {
                    var move = false;
                    var offset = 0;

                    if (left && !right)
                    {
                        move = true;
                        offset = -1;
                    }
                    else if (!left && right)
                    {
                        move = true;
                        offset = 1;
                    }

                    if (move)
                    {
                        this.curPiece.move(offset, 0);

                        if (!this.curPiece.fits(this.gameBoard))
                            this.curPiece.move(-offset, 0);
                    }

                    this.lastMoveMs = ms;
                    left = false;
                    right = false;
                }

                if (down)
                {
                    this.lastMoveMs = ms;
                    down = false;

                    this.curPiece.move(0, 1);
                    if (!this.curPiece.fits(this.gameBoard))
                    {
                        this.curPiece.move(0, -1);
                        need_lock = true;
                    }
                }

                var speed = 250;
                var delay = ms - this.lastMoveMs;

                if (delay < speed)
                {
                    need_break = true;
                }
                else
                {
                    // Move down automatically

                    this.lastMoveMs += speed;

                    this.curPiece.move(0, 1);

                    if (!this.curPiece.fits(this.gameBoard))
                    {
                        this.curPiece.move(0, -1);
                        need_lock = true;
                    }
                }

                if (need_lock)
                {
                    this.curPiece.lock(this.gameBoard);

                    this.curPiece = this.nextPiece;
                    this.nextPiece = this.pieceFactory.newPiece();

                    if (!this.curPiece.fits(this.gameBoard))
                    {
                        // Doesn't fit above the screen???
                        // Don't know how this can happen - but anyway...
                        this.gameOver = true;
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
                        }
                    }
                }

                if (need_break)
                {
                    break;
                }
            }
        }

        public draw(ctx: CanvasRenderingContext2D): void
        {
            // Save some information as local vars

            var x = this.x;
            var y = this.y;

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

            // Draw the game board pices

            this.gameBoard.draw(ctx, x, y);

            // Draw the current piece

            this.curPiece.draw(ctx, x, y);

            // Draw the game over text

            if (this.gameOver)
            {
                ctx.font = '50px sans-serif';
                ctx.fillStyle = 'white';

                ctx.fillText("GAME", x, y + 50);
                ctx.fillText("OVER", x, y + 100);
            }
        }
    }
}