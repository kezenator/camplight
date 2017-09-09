namespace bbox.debug.api
{
    export class ChildEntry
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::ChildEntry", ChildEntry)
            .addMember("name", "std::string")
            .addMember("path", "std::string")
            .addMember("short_description", "std::string");

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
