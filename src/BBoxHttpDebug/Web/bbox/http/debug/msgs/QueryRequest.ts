namespace bbox.http.debug.msgs
{
    export class QueryRequest extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::QueryRequest", QueryRequest)
            .addMember("request_id", "std::string")
            .addMember("query", "std::string")
            ;

        public request_id: string;
        public query: string;

        constructor()
        {
            super(QueryRequest.TYPE);
            this.request_id = "";
            this.query = "";
        }
    };
}
