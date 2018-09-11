namespace camplight.api
{
    export class GetLedsResponse
    {
        static type: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure("camplight::api::GetLedsResponse", GetLedsResponse)
            .addMember("width", "uint32_t")
            .addMember("height", "uint32_t")
            .addMember("main_leds", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"))
            .addMember("top_leds", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"));

        public width: number;
        public height: number;
        public main_leds: bbox.ds.Deque<string>;
        public top_leds: bbox.ds.Deque<string>;

        constructor()
        {
            this.width = 0;
            this.height = 0;
            this.main_leds = new bbox.ds.Deque<string>();
            this.top_leds = new bbox.ds.Deque<string>();
        }
    };
}
