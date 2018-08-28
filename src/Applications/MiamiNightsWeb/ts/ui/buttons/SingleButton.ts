﻿namespace ui.buttons
{
    export class SingleButton
    {
        private title: string;
        private span: HTMLSpanElement;
        private color: string;
        private pressed: boolean;
        private clicked: boolean;

        public constructor(parent: HTMLElement, title: string)
        {
            this.title = title;

            this.pressed = false;
            this.clicked = false;

            this.span = document.createElement('span');

            parent.appendChild(this.span);

            this.span.innerText = title;
            this.span.style.background = 'black';

            this.color = 'black';
        }


        public setColor(color: string): void
        {
            if (color != this.color)
            {
                this.color = color;
                this.span.style.backgroundColor = color;
            }
        }

        public matchesKey(key: string): boolean
        {
            return this.title.charAt(0) == key;
        }

        public setPressed(pressed: boolean): void
        {
            this.pressed = pressed;

            if (pressed)
            {
                this.clicked = true;
                this.span.classList.add('pressed');
            }
            else
            {
                this.span.classList.remove('pressed');
            }
        }

        public isPressed(): boolean
        {
            return this.pressed;
        }

        public isClicked(): boolean
        {
            return this.clicked;
        }

        public setNotClicked(): void
        {
            this.clicked = false;
        }
    }
}