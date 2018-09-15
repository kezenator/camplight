/// <reference path="../CanvasScreen.ts" /> 
/// <reference path="TetrisGame.ts" />

namespace ui.tetris
{
    export class TetrisScreen extends CanvasScreen
    {
        private games: TetrisGame[];

        public constructor(app: App)
        {
            super(app);

            this.games = null;
        }

        public show()
        {
            super.show();

            var buttons = this.getButtons();

            buttons.setBackColor('#ff0000');
            buttons.setPlayColor('#000000');

            this.games = new Array(2);
            this.games[0] = new TetrisGame(0);
            this.games[1] = new TetrisGame(960);
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            var buttons = this.getButtons();

            if (buttons.isBackClicked())
            {
                this.getApp().showScreen(App.MENU);
                return;
            }

            this.games[0].handleButtons(
                ms,
                buttons.isButtonClicked(0),
                buttons.isButtonClicked(1),
                buttons.isButtonClicked(2));

            this.games[1].handleButtons(
                ms,
                buttons.isButtonClicked(3),
                buttons.isButtonClicked(4),
                buttons.isButtonClicked(5));

            this.games[0].draw(ctx);
            this.games[1].draw(ctx);
        }
    }
}