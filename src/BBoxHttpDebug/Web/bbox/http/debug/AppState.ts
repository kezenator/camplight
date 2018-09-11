namespace bbox.http.debug
{
    export class AppState extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::AppState", AppState)
            .addMember("path", "std::string")
            ;

        public path: string;

        constructor()
        {
            super(AppState.TYPE);
            this.path = "";
        }
    };
}
