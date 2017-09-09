namespace bbox.debug.api
{
    export class QueryResponseEntry
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::QueryResponseEntry", QueryResponseEntry)
            .addMember("path", "std::string")
            .addMember("parent", "std::string")
            .addMemberByType("children", bbox.enc.TypeLibrary.stdVectorAsDeque("bbox::debug::api::ChildEntry"))
            .addMember("debug_report", "bbox::debug::api::DebugReport");

        public path: string;
        public parent: string;
        public children: bbox.ds.Deque<ChildEntry>;
        public debug_report: DebugReport;

        constructor()
        {
            this.path = "";
            this.parent = "";
            this.children = new bbox.ds.Deque<ChildEntry>();
            this.debug_report = new DebugReport();
        }
    };
}
