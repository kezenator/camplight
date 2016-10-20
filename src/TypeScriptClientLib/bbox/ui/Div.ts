namespace bbox.ui
{
    export class Div extends Container
    {
        private m_div_element: HTMLDivElement;

        constructor()
        {
            var div = document.createElement("div");

            super(div);
            this.m_div_element = div;
        }
    }
}