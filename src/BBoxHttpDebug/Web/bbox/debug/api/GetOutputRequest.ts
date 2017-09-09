namespace bbox.debug.api
{
    export class GetOutputRequest
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::GetOutputRequest", GetOutputRequest)
            .addMember("req_id", "uint64_t")
            .addMemberByType("debug_enables", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"));

        public req_id: number;
        public debug_enables: bbox.ds.Deque<string>;

        constructor()
        {
            this.req_id = 0;
            this.debug_enables = new bbox.ds.Deque<string>();
        }
    };
}
