namespace bbox.http.debug.msgs
{
    export class QueryResponseChild
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::http::debug::msgs::QueryResponseChild", QueryResponseChild)
            .addMember("name", "std::string")
            .addMember("path", "std::string")
            .addMember("short_description", "std::string")
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
