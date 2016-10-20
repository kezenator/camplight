namespace bbox.ui
{
    export class Pre extends TextControl
    {
        private m_pre_element: HTMLPreElement;

        constructor(text: string)
        {
            var pre = document.createElement("pre");
            pre.textContent = text;

            super(pre);
            this.m_pre_element = pre;
        }

        appendText(text:string): void
        {
            this.m_pre_element.appendChild(document.createTextNode(text));
        }

    }
}