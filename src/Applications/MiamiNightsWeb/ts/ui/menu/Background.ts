namespace ui.menu
{
    class BackgroundSprite
    {
        x: number;
        y: number;
        rotate: number;
        rotate_rate: number;
        sprite: number;
        num_in_row: number;

        constructor(x: number, y: number, num_in_row: number)
        {
            this.x = x;
            this.y = y;
            this.rotate = Math.random() * 2 * Math.PI;
            this.rotate_rate = Math.sqrt(Math.random()) * ((Math.random() > 0.5) ? 1 : -1);
            this.sprite = Math.floor(Math.random() * Background.NUM_SPRITES);
            this.num_in_row = num_in_row;
        }

        move(ms: number)
        {
            this.x += (ms / 12);
            this.y += (ms / 12);

            this.rotate += 0.0015 * ms * this.rotate_rate;

            while (this.rotate > Math.PI)
                this.rotate -= 2.0 * Math.PI;
            while (this.rotate < Math.PI)
                this.rotate += 2.0 * Math.PI;

            var spacing = Background.SPACING;

            while ((this.x > (1920 + spacing)) || (this.y > (1080 + spacing)))
            {
                this.x -= (this.num_in_row * Background.SPACING);
                this.y -= (this.num_in_row * Background.SPACING);
                this.sprite = Math.floor(Math.random() * Background.NUM_SPRITES);
                this.rotate_rate = Math.sqrt(Math.random()) * ((Math.random() > 0.5) ? 1 : -1);
            }
        }

        draw(ctx: CanvasRenderingContext2D, img: HTMLImageElement)
        {
            var sprite_size = Background.SPRITE_SIZE;

            ctx.translate(this.x, this.y);
            ctx.rotate(this.rotate);

            ctx.drawImage(
                img,
                this.sprite * sprite_size, 0,
                sprite_size, sprite_size,
                -0.5 * sprite_size, -0.5 * sprite_size,
                sprite_size, sprite_size);

            ctx.rotate(-this.rotate);
            ctx.translate(-this.x, -this.y);
        }
    }

    export class Background
    {
        static SPRITE_SIZE = 128;
        static NUM_SPRITES = 6;
        static SPACING = 256;

        private img: HTMLImageElement;
        private sprites: BackgroundSprite[];
        private last_ms: number;

        public constructor()
        {
            this.img = document.createElement('img');
            this.img.src = '/res/imgs/menu_background_sprites.png';

            this.sprites = [];
            this.last_ms = 0;

            var sprite_size = Background.SPRITE_SIZE;
            var spacing = Background.SPACING;

            var createLine = (x: number, y: number) =>
            {
                // Mov back by a full sprite so new sprites always
                // move back "off" screen before scrolling on.

                x -= spacing;
                y -= spacing;

                // Count the number of images that will fit into
                // this diagonal line

                var num_in_row = 0;

                {
                    var cx = x;
                    var cy = y;
                    while ((cx < (1920 + spacing)) && (cy < (1080 + spacing)))
                    {
                        num_in_row += 1;
                        cx += spacing;
                        cy += spacing;
                    }
                }

                // Now, create the correct number of sprites

                for (var i = 0; i < num_in_row; ++i)
                {
                    this.sprites.push(new BackgroundSprite(x, y, num_in_row));

                    x += spacing;
                    y += spacing;
                }
            };

            createLine(0, 0);
            createLine(0.5 * spacing, 0.5 * spacing);

            for (var i = spacing; i < 1920; i += spacing)
            {
                createLine(i - (0.5 * spacing), -0.5 * spacing);
                createLine(i, 0);
            }

            for (var i = spacing; i < 1080; i += spacing)
            {
                createLine(-0.5 * spacing, i - (0.5 * spacing));
                createLine(0, i);
            }
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            var change_ms = 0;
            if (ms > this.last_ms)
                change_ms = ms - this.last_ms;
            this.last_ms = ms;

            ctx.save();

            ctx.fillStyle = '#222222';
            ctx.fillRect(0, 0, 1920, 1080);

            for (var sprite of this.sprites)
            {
                sprite.move(change_ms);
                sprite.draw(ctx, this.img);
            }

            ctx.restore();
        }
    }
}