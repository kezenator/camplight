/// <reference path="../Buttons.ts" />\
/// <reference path="SingleButton.ts" />

namespace ui.buttons
{
    export class HTMLButtons extends Buttons
    {    
        private playButton: SingleButton;
        private otherButtons: SingleButton[];
        private allButtons: SingleButton[];

        constructor(parent: HTMLElement)
        {
            super();

            this.allButtons = [];

            var div = document.createElement('div');
            parent.appendChild(div);
            div.classList.add('buttons');

            this.playButton = new SingleButton(div, 'Play');
            this.allButtons.push(this.playButton);

            this.otherButtons = new Array(Buttons.NUMBER);

            var keys = "ASDFGH";

            for (var i = 0; i < Buttons.NUMBER; ++i)
            {
                var btn = new SingleButton(
                    div,
                    keys.charAt(i));

                this.otherButtons[i] = btn;
                this.allButtons.push(btn);
            }

            window.addEventListener('keydown', (ev: KeyboardEvent) =>
            {
                var key = ev.key.toUpperCase();

                for (var i = 0; i < this.allButtons.length; ++i)
                {
                    if (this.allButtons[i].matchesKey(key))
                        this.allButtons[i].setPressed(true);
                }
            });

            window.addEventListener('keyup', (ev: KeyboardEvent) =>
            {
                var key = ev.key.toUpperCase();

                for (var i = 0; i < this.allButtons.length; ++i)
                {
                    if (this.allButtons[i].matchesKey(key))
                        this.allButtons[i].setPressed(false);
                }
            });
        }

        setPlayColor(color: string): void
        {
            this.playButton.setColor(color);
        }

        setButtonColor(btn: number, color: string): void
        {
            var bb = this._getButton(btn);
            if (bb)
                bb.setColor(color);
        }

        isPlayPressed(): boolean
        {
            return this.playButton.isPressed();
        }

        isPlayClicked(): boolean
        {
            return this.playButton.isClicked();
        }

        isButtonPressed(btn: number): boolean
        {
            var bb = this._getButton(btn);
            if (bb)
                return bb.isPressed();
            return false;
        }

        isButtonClicked(btn: number): boolean
        {
            var bb = this._getButton(btn);
            if (bb)
                return bb.isClicked();
            return false;
        }

        frameCompleted(): void
        {
            this.playButton.setNotClicked();
            for (var i = 0; i < Buttons.NUMBER; ++i)
                this.otherButtons[i].setNotClicked();
        }

        private _getButton(btn: number): SingleButton | null
        {
            var i = Math.floor(btn);

            if ((i >= 0) && (i < Buttons.NUMBER))
                return this.otherButtons[i]; 

            return null;
        }
    }
}