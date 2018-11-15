/// <reference path="../CanvasScreen.ts" /> 
/// <reference path="TetrisGame.ts" />

namespace ui.tetris
{
    export class TetrisScreen extends CanvasScreen
    {
        private music_resources: string[];
        private music_playout: mn.audio.Playout;
        private pieces_img: HTMLImageElement;

        private games: TetrisGame[];

        public constructor(app: App)
        {
            super(app);

            this.music_resources = new Array(0);
            this.music_resources.push('/res/audio/tetris-8bit-remix.ogg');
            this.music_resources.push('/res/audio/tetris-trap-remix.ogg');

            this.music_playout = null;

            this.pieces_img = document.createElement('img');
            this.pieces_img.src = '/res/imgs/tetris_pieces.png';

            this.games = null;
        }

        public show()
        {
            super.show();

            var res = this.music_resources.shift();
            this.music_resources.push(res);
            this.music_playout = this.getApp().getAudio().play(
                res,
                () => { this.handleMusicCompleted(); });

            this.games = new Array(2);
            this.games[0] = new TetrisGame(0, this.pieces_img);
            this.games[1] = new TetrisGame(960, this.pieces_img);
        }

        public draw(ctx: CanvasRenderingContext2D, ms: number): void
        {
            var buttons = this.getButtons();

            if (buttons.isBackClicked())
            {
                this.music_playout.stop();
                this.getApp().showScreen(App.MENU);
                return;
            }
            else if (this.games[0].isTimedOut(ms)
                && this.games[1].isTimedOut(ms))
            {
                this.music_playout.stop();
                this.getApp().showScreen(App.LOGO);
                return;
            }

            this.games[0].handleButtons(
                ms,
                buttons.isButtonClicked(0),
                buttons.isButtonPressed(0),
                buttons.isButtonClicked(1),
                buttons.isButtonPressed(1),
                buttons.isButtonClicked(2),
                buttons.isButtonPressed(2));

            this.games[1].handleButtons(
                ms,
                buttons.isButtonClicked(3),
                buttons.isButtonPressed(3),
                buttons.isButtonClicked(4),
                buttons.isButtonPressed(4),
                buttons.isButtonClicked(5),
                buttons.isButtonPressed(5));

            this.games[0].draw(ctx);
            this.games[1].draw(ctx);
        }

        private handleMusicCompleted(): void
        {
            var res = this.music_resources.shift();
            this.music_resources.push(res);
            this.music_playout = this.getApp().getAudio().play(
                res,
                () => { this.handleMusicCompleted(); });
        }
    }
}