namespace camplight.ui
{
    export class LedMatrix extends bbox.ui.Control
    {
        private main_div: HTMLDivElement;
        private width: number;
        private height: number;
        private spans: bbox.ds.Deque<bbox.ds.Deque<HTMLSpanElement>>;

        constructor()
        {
            var div = document.createElement("div");

            super(div);

            this.main_div = div;
            this.main_div.classList.add("ledmatrix");

            this.width = 0;
            this.height = 0;
            this.spans = null;
        }

        updateDisplay(width: number, height: number, colors: bbox.ds.Deque<string>)
        {
            if ((width != this.width)
                || (height != this.height))
            {
                this.createMatrix(width, height);   
            }

            for (var row = 0; row < height; ++row)
            {
                for (var col = 0; col < width; ++col)
                {
                    var index = col + (row * width);

                    this.spans.at(row).at(col).style.background = colors.at(index);
                }
            }
        }

        private createMatrix(width: number, height: number)
        {
            this.main_div.innerHTML = "";
            this.width = width;
            this.height = height;
            this.spans = new bbox.ds.Deque<bbox.ds.Deque<HTMLSpanElement>>();

            for (var row = 0; row < height; ++row)
            {
                var row_deque = new bbox.ds.Deque<HTMLSpanElement>();
                this.spans.push_back(row_deque);

                var row_div = document.createElement("div");
                this.main_div.appendChild(row_div);

                for (var col = 0; col < width; ++col)
                {
                    var col_span = document.createElement("span");
                    col_span.innerHTML = "&nbsp;";

                    row_deque.push_back(col_span);
                    row_div.appendChild(col_span);
                }
            }
        }
    }
}