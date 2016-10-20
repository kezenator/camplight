namespace bbox.ui
{
    export abstract class Control
    {
        private m_control_element: HTMLElement;

        constructor(element: HTMLElement)
        {
            this.m_control_element = element;
        }

        htmlElement(): HTMLElement
        {
            return this.m_control_element;
        }
    }
}