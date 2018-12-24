namespace ui.tetris
{
    export class Cell
    {
        public static TILE_SIZE = 48;

        private pieces_img: HTMLImageElement;
        private index: number;
        public num: number;
        public hue: number;
        public rotate: number;

        constructor(pieces_img: HTMLImageElement, index: number, num: number, hue: number, rotate: number)
        {
            this.pieces_img = pieces_img;
            this.index = index;
            this.num = num;
            this.hue = hue;
            this.rotate = rotate;
        }

        public draw(ctx: CanvasRenderingContext2D, x: number, y: number, as_shadow: boolean)
        {
            var tile_size = Cell.TILE_SIZE;

            if (as_shadow)
            {
                ctx.fillStyle = 'hsla(' + this.hue + ',100%,50%,0.3)';
                ctx.fillRect(x, y, tile_size, tile_size);

                ctx.strokeStyle = 'rgba(0,0,0,0.3)';
                ctx.lineWidth = 3;
                ctx.strokeRect(x, y, tile_size, tile_size);
            }
            /*else if (this.index < 3)
            {
                ctx.drawImage(
                    this.pieces_img,
                    tile_size * (this.num + (4 * this.index)), tile_size * this.rotate,
                    tile_size, tile_size,
                    x, y,
                    tile_size, tile_size);
            }*/
            else
            {
                ctx.fillStyle = 'hsl(' + this.hue + ',100%,50%)';
                ctx.fillRect(x, y, tile_size, tile_size);

                ctx.strokeStyle = 'black';
                ctx.lineWidth = 3;
                ctx.strokeRect(x, y, tile_size, tile_size);

                ctx.font = '24px monospace';
                ctx.fillStyle = 'black';
                ctx.fillText((this.num + 1).toString(), x + 15, y + 30);
            }
        }
    }
}