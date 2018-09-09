namespace bbox.http.debug.msgs
{
    export class QueryRequest
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::QueryRequest", QueryRequest)
            .addMember("request_id", "string")
            .addMember("query", "string")
            ;

        public request_id: string;
        public query: string;

        constructor()
        {
            this.request_id = "";
            this.query = "";
        }
    };
}
