namespace bbox.ui
{
    export class Span extends TextControl
    {
        private m_span_element: HTMLSpanElement;

        constructor(text: string)
        {
            var span = document.createElement("span");
            span.textContent = text;

            super(span);
            this.m_span_element = span;
        }
    }
}