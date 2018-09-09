namespace bbox.http.debug.msgs
{
    export class QueryResponseEntry
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::QueryResponseEntry", QueryResponseEntry)
            .addMember("path", "string")
            .addMember("parent_path", "string")
            .addMember("children", "QueryResponseChild[]")
            .addMember("report", "DebugReportNotification")
            ;

        public path: string;
        public parent_path: string;
        public children: QueryResponseChild[];
        public report: DebugReportNotification;

        constructor()
        {
            this.path = "";
            this.parent_path = "";
            this.children = new Array(0);
            this.report = new DebugReportNotification();
        }
    };
}
