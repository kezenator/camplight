namespace bbox.http.debug.msgs
{
    export class QueryResponseChild
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::QueryResponseChild", QueryResponseChild)
            .addMember("name", "string")
            .addMember("path", "string")
            .addMember("short_description", "string")
            ;

        public name: string;
        public path: string;
        public short_description: string;

        constructor()
        {
            this.name = "";
            this.path = "";
            this.short_description = "";
        }
    };
}
