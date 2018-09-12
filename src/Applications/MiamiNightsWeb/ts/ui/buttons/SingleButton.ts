namespace ui.buttons
{
    export class SingleButton
    {
        private title: string;
        private span: HTMLSpanElement;
        private color: string;
        private pressed: boolean;
        private set_action: { (state: boolean): void };

        public constructor(parent: HTMLElement, title: string, set_action: { (state:boolean): void })
        {
            this.title = title;
            this.set_action = set_action;

            this.pressed = false;

            this.span = document.createElement('span');

            this.span.addEventListener('touchstart', () => { this.setPressed(true); });
            this.span.addEventListener('touchend', () => { this.setPressed(false); });

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
            if (pressed != this.pressed)
            {
                this.pressed = pressed;
                this.set_action(pressed);

                if (pressed)
                {
                    this.span.classList.add('pressed');
                }
                else
                {
                    this.span.classList.remove('pressed');
                }
            }
        }
   }
}