namespace bbox.debug.api
{
    export class QueryRequest
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::QueryRequest", QueryRequest)
            .addMember("query", "std::string");

        public query: string;

        constructor()
        {
            this.query = "";
        }
    };
}
