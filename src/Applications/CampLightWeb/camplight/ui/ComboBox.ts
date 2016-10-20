namespace camplight.ui
{
    export class ComboBox extends bbox.ui.Control
    {
        private main_div: HTMLDivElement;
        private items: bbox.ds.Deque<string>;
        private spans: bbox.ds.Deque<bbox.ds.Deque<HTMLSpanElement>>;
        private cur_button: bbox.ui.Button;
        private cur_item: string;

        constructor(items: bbox.ds.Deque<string>)
        {
            var div = document.createElement("div");

            super(div);

            this.main_div = div;
            this.items = items;

            this.main_div.classList.add("combobox");

            for (var i = 0; i < items.size(); ++i)
            {
                var button = new bbox.ui.Button(items.at(i));
                this.main_div.appendChild(button.htmlElement());

                button.onClick(this.select.bind(this, i, button));

                if (i == 0)
                {
                    this.cur_button = button;
                    button.htmlElement().classList.add("selected");
                    this.cur_item = items.at(0);
                }
            }
        }

        getCurrentItem(): string
        {
            return this.cur_item;
        }

        private select(index: number, button: bbox.ui.Button)
        {
            this.cur_button.htmlElement().classList.remove("selected");
            button.htmlElement().classList.add("selected");
            this.cur_button = button;
            this.cur_item = this.items.at(index);
        }
    }
}