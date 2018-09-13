namespace mn.msgs
{
    export class StartEmulator extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("mn::msgs::StartEmulator", StartEmulator)
            .addMember("game", "std::string")
            ;

        public game: string;

        constructor()
        {
            super(StartEmulator.TYPE);
            this.game = "";
        }
    };
}
