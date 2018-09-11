/// <reference path="QueryResponseEntry.ts" />

namespace bbox.http.debug.msgs
{
    export class QueryResponse extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::QueryResponse", QueryResponse)
            .addMember("request_id", "std::string")
            .addMember("entries", bbox.enc.TypeLibrary.stdVectorAsDeque("bbox::http::debug::msgs::QueryResponseEntry"))
            ;

        public request_id: string;
        public entries: bbox.ds.Deque<bbox.http.debug.msgs.QueryResponseEntry>;

        constructor()
        {
            super(QueryResponse.TYPE);
            this.request_id = "";
            this.entries = new bbox.ds.Deque<bbox.http.debug.msgs.QueryResponseEntry>();
        }
    };
}
