namespace camplight.api
{
    export class ApplyRequest
    {
        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("camplight::api::ApplyRequest", ApplyRequest)
            .addMember("sequence", "std::string");

        public sequence: string;

        constructor()
        {
            this.sequence = "";
        }
    };
}
