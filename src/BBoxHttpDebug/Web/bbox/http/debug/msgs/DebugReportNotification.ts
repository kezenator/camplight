namespace bbox.http.debug.msgs
{
    export class DebugReportNotification
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::DebugReportNotification", DebugReportNotification)
            .addMember("contents_html", "string")
            ;

        public contents_html: string;

        constructor()
        {
            this.contents_html = "";
        }
    };
}
