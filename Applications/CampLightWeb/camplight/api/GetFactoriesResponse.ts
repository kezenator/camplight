namespace camplight.api
{
    export class GetFactoriesResponse
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("camplight::api::GetFactoriesResponse", GetFactoriesResponse)
            .addMemberByType("patterns", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"))
            .addMemberByType("transitions", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"));

        public patterns: bbox.ds.Deque<string>;
        public transitions: bbox.ds.Deque<string>;

        constructor()
        {
            this.patterns = new bbox.ds.Deque<string>();
            this.transitions = new bbox.ds.Deque<string>();
        }
    };
}
