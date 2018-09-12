namespace mn.msgs
{
    export class ButtonStates extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("mn::msgs::ButtonStates", ButtonStates)
            .addMember("back_state", "bool")
            .addMember("play_state", "bool")
            .addMember("button_states", bbox.enc.TypeLibrary.stdVectorAsDeque("bool"))
            ;

        public back_state: boolean;
        public play_state: boolean;
        public button_states: bbox.ds.Deque<boolean>;

        constructor()
        {
            super(ButtonStates.TYPE);
            this.back_state = false;
            this.play_state = false;
            this.button_states = new bbox.ds.Deque<boolean>();
        }
    };
}
