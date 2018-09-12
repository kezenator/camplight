namespace mn.msgs
{
    export class ButtonColors extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("mn::msgs::ButtonColors", ButtonColors)
            .addMember("back_color", "std::string")
            .addMember("play_color", "std::string")
            .addMember("button_colors", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"))
            ;

        public back_color: string;
        public play_color: string;
        public button_colors: bbox.ds.Deque<string>;

        constructor()
        {
            super(ButtonColors.TYPE);
            this.back_color = "";
            this.play_color = "";
            this.button_colors = new bbox.ds.Deque<string>();
        }
    };
}
