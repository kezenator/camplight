namespace ui.tetris
{
    export class Cell
    {
        public static TILE_SIZE = 48;

        private pieces_img: HTMLImageElement;
        private safe: boolean;
        private index: number;
        public num: number;
        public hue: number;
        public rotate: number;
        private orig_x_off: number;
        private orig_y_off: number;

        constructor(pieces_img: HTMLImageElement, safe: boolean, index: number, num: number, hue: number, rotate: number, orig_x_off: number, orig_y_off: number)
        {
            this.pieces_img = pieces_img;
            this.safe = safe;
            this.index = index;
            this.num = num;
            this.hue = hue;
            this.rotate = rotate;
            this.orig_x_off = orig_x_off;
            this.orig_y_off = orig_y_off;
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
            else if (this.safe)
            {
                ctx.fillStyle = 'hsl(' + this.hue + ',100%,50%)';
                ctx.fillRect(x, y, tile_size, tile_size);

                ctx.strokeStyle = 'black';
                ctx.lineWidth = 3;
                ctx.strokeRect(x, y, tile_size, tile_size);

                //ctx.font = '24px monospace';
                //ctx.fillStyle = 'black';
                //ctx.fillText((this.num + 1).toString(), x + 15, y + 30);
            }
            else
            {
                var ox = 0;
                var oy = 0;
                var orot = 0;

                switch (this.index)
                {
                    case 0:  // I
                        ox = 2 + this.orig_x_off;
                        oy = 0;
                        break;
                    case 1: // O
                        ox = 1 + this.orig_x_off;
                        oy = 2 + this.orig_y_off;
                        break;
                    case 2: // T
                        ox = 4 + this.orig_x_off;
                        oy = 2 + this.orig_y_off;
                        break;
                    case 3: // S
                        ox = 9 + this.orig_x_off;
                        oy = 2 + this.orig_y_off;
                        break;
                    case 4: // Z
                        ox = 6 + this.orig_x_off;
                        oy = 2 + this.orig_y_off;
                        break;
                    case 5: // J
                        ox = 4 - this.orig_x_off;
                        oy = 0 - this.orig_y_off;
                        orot = 2;
                        break;
                    case 6: // L
                        ox = 7 - this.orig_x_off;
                        oy = 0 - this.orig_y_off;
                        orot = 2;
                        break;
                }

                var rx = 0;
                var ry = 0;

                switch ((this.rotate + orot) % 4)
                {
                    case 0:
                        rx = ox;
                        ry = oy;
                        break;
                    case 1:
                        rx = 2 - oy;
                        ry = 6 + ox;
                        break;
                    case 2:
                        rx = 9 - ox;
                        ry = 5 - oy;
                        break;
                    case 3:
                        rx = 3 + oy;
                        ry = 15 - ox;
                        break;
                }

                ctx.drawImage(
                    this.pieces_img,
                    tile_size * rx, tile_size * ry,
                    tile_size, tile_size,
                    x, y,
                    tile_size, tile_size);
            }
        }
    }
}