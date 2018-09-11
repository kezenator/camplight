/// <reference path="DebugReportNotification.ts" />
/// <reference path="QueryResponseChild.ts" />

namespace bbox.http.debug.msgs
{
    export class QueryResponseEntry
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::QueryResponseEntry", QueryResponseEntry)
            .addMember("path", "std::string")
            .addMember("parent_path", "std::string")
            .addMember("children", bbox.enc.TypeLibrary.stdVectorAsDeque("bbox::http::debug::msgs::QueryResponseChild"))
            .addMember("report", "bbox::http::debug::msgs::DebugReportNotification")
            ;

        public path: string;
        public parent_path: string;
        public children: bbox.ds.Deque<bbox.http.debug.msgs.QueryResponseChild>;
        public report: bbox.http.debug.msgs.DebugReportNotification;

        constructor()
        {
            this.path = "";
            this.parent_path = "";
            this.children = new bbox.ds.Deque<bbox.http.debug.msgs.QueryResponseChild>();
            this.report = new bbox.http.debug.msgs.DebugReportNotification();
        }
    };
}
