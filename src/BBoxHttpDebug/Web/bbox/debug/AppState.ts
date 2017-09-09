namespace bbox.debug
{
    export class AppState
    {
        static type = bbox.enc.TypeLibrary.simpleStructure<AppState>("bbox::debug::AppState", AppState)
            .addMember("path", "std::string");

        AppState()
        {
            this.path = "/";
        }

        public path: string;
    }
}