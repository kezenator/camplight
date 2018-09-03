/// <reference path="Cell.ts" />

namespace ui.tetris
{
    export class GameBoard
    {
        public static NUM_ROWS = 22;
        public static HIDDEN_ROWS = 2;
        public static NUM_COLS = 10;

        public cells: Cell[][];

        public constructor()
        {
            var num_cols = GameBoard.NUM_COLS;
            var num_rows = GameBoard.NUM_ROWS;

            this.cells = new Array(num_cols);

            for (var i = 0; i < num_cols; ++i)
                this.cells[i] = new Array(num_rows);
        }

        public draw(ctx: CanvasRenderingContext2D, game_x: number, game_y: number)
        {
            var num_cols = GameBoard.NUM_COLS;
            var hidden_rows = GameBoard.HIDDEN_ROWS;
            var num_rows = GameBoard.NUM_ROWS;
            var tile_size = Cell.TILE_SIZE;

            var cells = this.cells;

            for (var i = 0; i < num_cols; ++i)
            {
                for (var j = hidden_rows; j < num_rows; ++j)
                {
                    var cell = cells[i][j];

                    if (cell)
                    {
                        cell.draw(ctx, game_x + i * tile_size, game_y + (j - hidden_rows) * tile_size);
                    }
                }
            }
        }
    }
}