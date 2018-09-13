namespace mn.msgs
{
    export class EmulatorCompleted extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("mn::msgs::EmulatorCompleted", EmulatorCompleted)
            ;

        constructor()
        {
            super(EmulatorCompleted.TYPE);
        }
    };
}
