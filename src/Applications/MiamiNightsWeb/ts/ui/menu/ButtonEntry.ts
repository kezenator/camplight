namespace ui.menu
{
    export class ButtonEntry
    {
        color: string;
        help1: string;
        help2: string;

        constructor(_color: string, _help1: string, _help2: string)
        {
            this.color = _color;
            this.help1 = _help1;
            this.help2 = _help2;
        }
    };
}