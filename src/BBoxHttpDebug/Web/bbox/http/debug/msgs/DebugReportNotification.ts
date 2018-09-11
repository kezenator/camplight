namespace bbox.http.debug.msgs
{
    export class DebugReportNotification extends bbox.enc.MsgAnyPtr
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::DebugReportNotification", DebugReportNotification)
            .addMember("contents_html", "std::string")
            ;

        public contents_html: string;

        constructor()
        {
            super(DebugReportNotification.TYPE);
            this.contents_html = "";
        }
    };
}
