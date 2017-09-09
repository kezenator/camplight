namespace bbox.ui
{
    export interface EditBoxOnChangedHandler
    {
        (): void;
    }

    export class EditBox extends TextControl
    {
        private m_input_element: HTMLInputElement;
        private m_on_change: EditBoxOnChangedHandler;

        public constructor(text: string)
        {
            var input = document.createElement("input");
            input.value = text;
            input.type = 'text';

            super(input);

            this.m_input_element = input;
            this.m_on_change = null;

            input.oninput = () =>
            {
                this.handleChange();
            };
        }

        public onChanged(handler: EditBoxOnChangedHandler): this
        {
            this.m_on_change = handler;
            return this;
        }

        public getValue(): string
        {
            return this.m_input_element.value;
        }

        public setValue(new_value: string): void
        {
            this.m_input_element.value = new_value;
        }

        private handleChange(): void
        {
            if (this.m_on_change)
            {
                this.m_on_change();
            }
        }
    }
}