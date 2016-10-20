namespace camplight.api
{
    export class GetFactoriesResponse
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("camplight::api::GetFactoriesResponse", GetFactoriesResponse)
            .addMemberByType("sequences", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"))
            .addMemberByType("patterns", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"))
            .addMemberByType("transitions", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"));

        public sequences: bbox.ds.Deque<string>;
        public patterns: bbox.ds.Deque<string>;
        public transitions: bbox.ds.Deque<string>;

        constructor()
        {
            this.sequences= new bbox.ds.Deque<string>();
            this.patterns = new bbox.ds.Deque<string>();
            this.transitions = new bbox.ds.Deque<string>();
        }
    };
}
