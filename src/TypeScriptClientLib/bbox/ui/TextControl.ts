/// <reference path="./Control.ts"/>

namespace bbox.ui
{
    export class TextControl extends Control
    {
        constructor(element: HTMLElement)
        {
            super(element);
        }

        setText(text: string): this
        {
            this.htmlElement().textContent = text;
            return this;
        }
    }
}