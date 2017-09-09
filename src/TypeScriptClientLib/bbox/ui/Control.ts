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

        public addClass(class_name: string): void
        {
            this.m_control_element.classList.add(class_name);
        }

        public removeClass(class_name: string): void
        {
            this.m_control_element.classList.remove(class_name);
        }
    }
}