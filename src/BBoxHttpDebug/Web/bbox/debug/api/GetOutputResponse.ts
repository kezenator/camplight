namespace bbox.debug.api
{
    export class GetOutputResponse
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::GetOutputResponse", GetOutputResponse)
            .addMember("next_id", "uint64_t")
            .addMemberByType("reports", bbox.enc.TypeLibrary.stdVectorAsDeque("bbox::debug::api::DebugReport"));

        public next_id: number;
        public reports: bbox.ds.Deque<DebugReport>;

        constructor()
        {
            this.next_id = 0;
            this.reports = new bbox.ds.Deque<DebugReport>();
        }
    };
}
