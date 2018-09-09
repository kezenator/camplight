namespace bbox.http.debug.msgs
{
    export class QueryResponse
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::QueryResponse", QueryResponse)
            .addMember("request_id", "string")
            .addMember("entries", "QueryResponseEntry[]")
            ;

        public request_id: string;
        public entries: QueryResponseEntry[];

        constructor()
        {
            this.request_id = "";
            this.entries = new Array(0);
        }
    };
}
