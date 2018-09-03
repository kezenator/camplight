/// <reference path="Cell.ts" />
/// <reference path="GameBoard.ts" />

namespace ui.tetris
{
    class Part
    {
        public cell: Cell;
        public x_off: number;
        public y_off: number;

        constructor(cell: Cell, x_off: number, y_off: number)
        {
            this.cell = cell;
            this.x_off = x_off;
            this.y_off = y_off;
        }
    };

    export class Piece
    {
        public static NUM_PIECES = 7;

        private static PART_OFFSETS: number[][][] = [
            // Part 0: I: Cyan
            [
                [-2, 0, -1, 0, 0, 0, 1, 0],
            ],
            // Part 1: O: Yellow
            [
                [-1, 0, 0, 0, -1, -1, 0, -1],
            ],
            // Part 2: T: Purple
            [
                [-2, 0, -1, 0, 0, 0, -1, -1],
            ],
            // Part 3: S: Green
            [
                [-2, 0, -1, 0, -1, -1, 0, -1],
            ],
            // Part 4: Z: Red
            [
                [-2, -1, -1, -1, -1, 0, 0, 0],
            ],
            // Part 5: J: Blue
            [
                [-2, -1, -2, 0, -1, 0, 0, 0],
            ],
            // Part 6: L: Orange
            [
                [-2, 0, -1, 0, 0, 0, 0, -1],
            ],
        ];

        private num: number;
        private x: number;
        private y: number;
        private parts: Part[];

        public constructor(num: number)
        {
            this.num = num;
            this.x = Math.floor(0.5 * GameBoard.NUM_COLS);
            this.y = GameBoard.HIDDEN_ROWS - 1;
            this.parts = new Array(4);

            // Hues - Cyan, Yellow, Purple, Green, Red, Blue, Orange
            var hue = [180, 60, 300, 120, 0, 240, 30][num];

            var offsets = Piece.PART_OFFSETS[num][0];

            this.parts[0] = new Part(new Cell(hue), offsets[0], offsets[1]);
            this.parts[1] = new Part(new Cell(hue), offsets[2], offsets[3]);
            this.parts[2] = new Part(new Cell(hue), offsets[4], offsets[5]);
            this.parts[3] = new Part(new Cell(hue), offsets[6], offsets[7]);
        }

        public move(x: number, y: number)
        {
            this.x += x;
            this.y += y;
        }

        public fits(board: GameBoard): boolean
        {
            for (var i = 0; i < 4; ++i)
            {
                var part = this.parts[i];
                var x = this.x + part.x_off;
                var y = this.y + part.y_off;

                if ((x < 0)
                    || (x >= GameBoard.NUM_COLS)
                    || (y >= GameBoard.NUM_ROWS)
                    || board.cells[x][y])
                {
                    return false;
                }                
            }

            return true;
        }

        public lock(board: GameBoard)
        {
            for (var i = 0; i < 4; ++i)
            {
                var part = this.parts[i];
                var x = this.x + part.x_off;
                var y = this.y + part.y_off;

                board.cells[x][y] = part.cell;
            }

            this.parts = null;
        }

        public draw(ctx: CanvasRenderingContext2D, game_x: number, game_y: number)
        {
            var tile_size = Cell.TILE_SIZE;
            var hidden_rows = GameBoard.HIDDEN_ROWS;

            for (var i = 0; i < 4; ++i)
            {
                var part = this.parts[i];
                var part_y = this.y + part.y_off;

                if (part_y >= hidden_rows)
                {
                    var part_x = this.x + part.x_off;

                    part.cell.draw(
                        ctx,
                        game_x + tile_size * part_x,
                        game_y + tile_size * (part_y - hidden_rows));
                }
            } 
        }
    }
}