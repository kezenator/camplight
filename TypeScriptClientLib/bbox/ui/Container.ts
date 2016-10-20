namespace bbox.ui
{             
    export abstract class Container extends Control
    {
        private m_container_element: HTMLElement;

        constructor(element: HTMLElement)
        {
            super(element);
            this.m_container_element = element;
        }

        add(control: Control): this
        {
            this.m_container_element.appendChild(control.htmlElement());
            return this;
        }
    }
}