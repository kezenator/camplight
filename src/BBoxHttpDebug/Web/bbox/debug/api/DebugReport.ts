namespace bbox.debug.api
{
    export class DebugReport
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::DebugReport", DebugReport)
            .addMember("contents_html", "std::string");

        public contents_html: string;

        constructor()
        {
            this.contents_html = "";
        }
    };
}
