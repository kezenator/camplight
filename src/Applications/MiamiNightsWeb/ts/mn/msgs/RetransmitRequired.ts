namespace mn.msgs
{
    export class RetransmitRequired extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("mn::msgs::RetransmitRequired", RetransmitRequired)
            ;

        constructor()
        {
            super(RetransmitRequired.TYPE);
        }
    };
}
