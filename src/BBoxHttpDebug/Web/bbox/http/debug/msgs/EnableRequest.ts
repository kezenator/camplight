namespace bbox.http.debug.msgs
{
    export class EnableRequest extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::EnableRequest", EnableRequest)
            .addMember("enabled", "bool")
            .addMember("debug_enables", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"))
            ;

        public enabled: boolean;
        public debug_enables: bbox.ds.Deque<string>;

        constructor()
        {
            super(EnableRequest.TYPE);
            this.enabled = false;
            this.debug_enables = new bbox.ds.Deque<string>();
        }
    };
}
