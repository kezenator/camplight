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

    export class Kick
    {
        public x: number;
        public y: number;

        constructor(x: number, y: number)
        {
            this.x = x;
            this.y = y;
        }
    };

    export class Piece
    {
        public static NUM_PIECES = 7;

        private static PART_OFFSETS: number[][][] = [
            // Part 0: I: Cyan
            [
                [-2, 0, -1, 0, 0, 0, 1, 0],
                [0, -1, 0, 0, 0, 1, 0, 2],
                [1, 1, 0, 1, -1, 1, -2, 1],
                [-1, 2, -1, 1, -1, 0, -1, -1],
            ],
            // Part 1: O: Yellow
            [
                [-1, -1, 0, -1, 0, 0, -1, 0],
                [0, -1, 0, 0, -1, 0, -1, -1],
                [0, 0, -1, 0, -1, -1, 0, -1],
                [-1, 0, -1, -1, 0, -1, 0, 0],
            ],
            // Part 2: T: Purple
            [
                [-2, 0, -1, 0, 0, 0, -1, -1],
                [-1, -1, -1, 0, -1, 1, 0, 0],
                [0, 0, -1, 0, -2, 0, -1, 1],
                [-1, 1, -1, 0, -1, -1, -2, 0],
            ],
            // Part 3: S: Green
            [
                [-2, 0, -1, 0, -1, -1, 0, -1],
                [-1, -1, -1, 0, 0, 0, 0, 1],
                [0, 0, -1, 0, -1, 1, -2, 1],
                [-1, 1, -1, 0, -2, 0, -2, -1],
            ],
            // Part 4: Z: Red
            [
                [-2, -1, -1, -1, -1, 0, 0, 0],
                [0, -1, 0, 0, -1, 0, -1, 1],
                [0, 1, -1, 1, -1, 0, -2, 0],
                [-2, 1, -2, 0, -1, 0, -1, -1],
            ],
            // Part 5: J: Blue
            [
                [-2, -1, -2, 0, -1, 0, 0, 0],
                [0, -1, -1, -1, -1, 0, -1, 1],
                [0, 1, 0, 0, -1, 0, -2, 0],
                [-2, 1, -1, 1, -1, 0, -1, -1],
            ],
            // Part 6: L: Orange
            [
                [-2, 0, -1, 0, 0, 0, 0, -1],
                [-1, -1, -1, 0, -1, 1, 0, 1],
                [0, 0, -1, 0, -2, 0, -2, 1],
                [-1, 1, -1, 0, -1, -1, -2, -1],
            ],
        ];

        private static I_KICKS: Kick[][] = [
            // 3 => 0
            [
                new Kick(0, 0),
                new Kick(1, 0),
                new Kick(-2, 0),
                new Kick(1, 2),
                new Kick(-2, -1)
            ],
            // 0 => 1
            [
                new Kick(0, 0),
                new Kick(-2, 0),
                new Kick(1, 0),
                new Kick(-2, 1),
                new Kick(1, -2)
            ],
            // 1 => 2
            [
                new Kick(0, 0),
                new Kick(-1, 0),
                new Kick(2, 0),
                new Kick(-1, -2),
                new Kick(2, 1)
            ],
            // 2 => 3
            [
                new Kick(0, 0),
                new Kick(2, 0),
                new Kick(-1, 0),
                new Kick(2, -1),
                new Kick(-1, 2)
            ],
        ];

        private static O_KICKS: Kick[] = [
            new Kick(0, 0)
        ];

        private static OTHER_KICKS: Kick[][] = [
            // 3 => 0
            [
                new Kick(0, 0),
                new Kick(-1, 0),
                new Kick(-1, 1),
                new Kick(0, -2),
                new Kick(-1, -2)
            ],
            // 0 => 1
            [
                new Kick(0, 0),
                new Kick(-1, 0),
                new Kick(-1, -1),
                new Kick(0, 2),
                new Kick(-1, 2)
            ],
            // 1 => 2
            [
                new Kick(0, 0),
                new Kick(1, 0),
                new Kick(1, 1),
                new Kick(0, -2),
                new Kick(1, -2)
            ],
            // 2 => 3
            [
                new Kick(0, 0),
                new Kick(1, 0),
                new Kick(1, -1),
                new Kick(0, 2),
                new Kick(1, 2)
            ],
        ];

        private num: number;
        private x: number;
        private y: number;
        private rotation: number;
        private parts: Part[];
        private pieces_img: HTMLImageElement;
        private safe: boolean;

        public constructor(pieces_img: HTMLImageElement, safe: boolean, num: number)
        {
            this.num = num;
            this.x = Math.floor(0.5 * GameBoard.NUM_COLS);
            this.y = GameBoard.HIDDEN_ROWS - 1;
            this.rotation = 0;
            this.parts = new Array(4);
            this.pieces_img = pieces_img;
            this.safe = safe;

            this.createParts();
        }

        public clone(): Piece
        {
            var result = new Piece(this.pieces_img, this.safe, this.num);
            result.x = this.x;
            result.y = this.y;
            result.rotation = this.rotation;
            result.createParts();

            return result;
        }

        public move(x: number, y: number)
        {
            this.x += x;
            this.y += y;
        }

        public rotate(turns: number)
        {
            this.rotation = (this.rotation + 4 + turns) % 4;
            this.createParts();
        }

        public getKickData(): Kick[]
        {
            if (this.num == 0)
            {
                // I - special kicks
                return Piece.I_KICKS[this.rotation];
            }
            else if (this.num == 1)
            {
                // O - no kicks
                return Piece.O_KICKS;
            }
            else
            {
                // Other pices
                return Piece.OTHER_KICKS[this.rotation];
            }
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
                    || board.cells[y][x])
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

                board.cells[y][x] = part.cell;
            }

            this.parts = null;
        }

        public draw(ctx: CanvasRenderingContext2D, game_x: number, game_y: number, as_shadow: boolean)
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
                        game_y + tile_size * (part_y - hidden_rows),
                        as_shadow);
                }
            } 
        }

        public drawAsNext(ctx: CanvasRenderingContext2D, x: number, y: number)
        {
            var tile_size = Cell.TILE_SIZE;

            if (this.num >= 2)
            {
                // It's not I or O - it's three tiles across -
                // add half a tile to x so it's centered better

                x += 0.5 * tile_size;
            }

            for (var i = 0; i < 4; ++i)
            {
                var part = this.parts[i];

                var part_x = part.x_off;
                var part_y = part.y_off;

                part.cell.draw(
                    ctx,
                    x + tile_size * part_x,
                    y + tile_size * part_y,
                    false);
            } 
        }

        private createParts()
        {
            var hue = 0;

            if (this.safe)
            {
                // Hues - Cyan, Yellow, Purple, Green, Red, Blue, Orange
                hue = [180, 60, 300, 120, 0, 240, 30][this.num];
            }
            else
            {
                // Match hues in the images:
                // Cyan, Yellow, Purple, Red, Green, Pink/Magenta, Orange
                hue = [180, 60, 270, 0, 120, 300, 30][this.num];
            }

            var orig_offsets = Piece.PART_OFFSETS[this.num][0];
            var offsets = Piece.PART_OFFSETS[this.num][this.rotation];

            this.parts[0] = new Part(new Cell(this.pieces_img, this.safe, this.num, 0, hue, this.rotation, orig_offsets[0], orig_offsets[1]), offsets[0], offsets[1]);
            this.parts[1] = new Part(new Cell(this.pieces_img, this.safe, this.num, 1, hue, this.rotation, orig_offsets[2], orig_offsets[3]), offsets[2], offsets[3]);
            this.parts[2] = new Part(new Cell(this.pieces_img, this.safe, this.num, 2, hue, this.rotation, orig_offsets[4], orig_offsets[5]), offsets[4], offsets[5]);
            this.parts[3] = new Part(new Cell(this.pieces_img, this.safe, this.num, 3, hue, this.rotation, orig_offsets[6], orig_offsets[7]), offsets[6], offsets[7]);
        }
    }
}