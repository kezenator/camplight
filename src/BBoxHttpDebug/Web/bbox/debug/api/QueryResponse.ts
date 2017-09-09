namespace bbox.debug.api
{
    export class QueryResponse
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::QueryResponse", QueryResponse)
            .addMemberByType("results", bbox.enc.TypeLibrary.stdVectorAsDeque("bbox::debug::api::QueryResponseEntry"));

        public results: bbox.ds.Deque<QueryResponseEntry>;

        constructor()
        {
            this.results = new bbox.ds.Deque<QueryResponseEntry>();
        }
    };
}
