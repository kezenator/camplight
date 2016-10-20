namespace camplight.api
{
    export class ApplyRequest
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("camplight::api::ApplyRequest", ApplyRequest)
            .addMember("pattern", "std::string")
            .addMember("transition", "std::string");

        public pattern: string;
        public transition: string;

        constructor()
        {
            this.pattern = "";
            this.transition = "";
        }
    };
}
