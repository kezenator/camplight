namespace ui.tetris
{
    export class Cell
    {
        public static TILE_SIZE = 48;

        public hue: number;

        constructor(hue: number)
        {
            this.hue = hue;
        }

        public draw(ctx: CanvasRenderingContext2D, x: number, y: number)
        {
            ctx.fillStyle = 'hsl(' + this.hue + ',100%,50%)';
            ctx.fillRect(x, y, Cell.TILE_SIZE, Cell.TILE_SIZE);

            ctx.strokeStyle = 'black';
            ctx.lineWidth = 3;
            ctx.strokeRect(x, y, Cell.TILE_SIZE, Cell.TILE_SIZE);
        }
    }
}