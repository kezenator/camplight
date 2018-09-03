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
            var num_rows = GameBoard.NUM_ROWS;

            this.cells = new Array(num_rows);

            for (var i = 0; i < num_rows; ++i)
                this.cells[i] = this.newRow();
        }

        private newRow()
        {
            var num_cols = GameBoard.NUM_COLS;

            var result: Cell[] = new Array(num_cols);

            for (var i = 0; i < num_cols; ++i)
                result[i] = null;

            return result;
        }

        public clearCompleteRows(): number
        {
            var cells = this.cells;
            var num_cols = GameBoard.NUM_COLS;
            var num_rows = GameBoard.NUM_ROWS;

            var rows_cleared = 0;

            for (var i = num_rows - 1; (i >= 0) && (rows_cleared < 4); /* nothing */)
            {
                var filled = true;

                for (var j = 0; j < num_cols; ++j)
                {
                    if (cells[i][j] === null)
                    {
                        filled = false;
                        break;
                    }
                }

                if (filled)
                {
                    rows_cleared += 1;

                    for (var j = i; j > 0; j -= 1)
                        cells[j] = cells[j - 1];

                    cells[0] = this.newRow();
                }
                else
                {
                    i -= 1;
                }   
            }

            return [0, 100, 300, 500, 800][rows_cleared];
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
                    var cell = cells[j][i];

                    if (cell)
                    {
                        cell.draw(ctx, game_x + i * tile_size, game_y + (j - hidden_rows) * tile_size);
                    }
                }
            }
        }
    }
}