namespace bbox.ui
{
    export interface ButtonOnClickCallback
    {
        (): void;
    }
    
    export class Button extends TextControl
    {
        private m_button_element: HTMLButtonElement;
        private m_on_click: ButtonOnClickCallback;

        constructor(text: string)
        {
            var button = document.createElement("button");
            button.textContent = text;

            super(button);
            this.m_button_element = button;
            this.m_on_click = null;
            
            button.onclick = () =>
                {
                    this.handleClick();
                };
        }
        
        onClick(handler: ButtonOnClickCallback): this
        {
            this.m_on_click = handler;
            return this;
        }
        
        private handleClick(): void
        {
            if (this.m_on_click)
            {
                this.m_on_click();
            }
        }
    }
}