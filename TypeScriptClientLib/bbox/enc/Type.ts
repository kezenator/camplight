namespace bbox.enc
{
    export abstract class Type
    {
        constructor(lib: TypeLibrary, name: string)
        {
            this.type_lib = lib;
            this.type_name = name;
        }

        public getTypeLibrary(): TypeLibrary
        {
            return this.type_lib;
        }

        public getName(): string
        {
            return this.type_name;
        }

        public abstract isValue(): boolean;
        public abstract toString(val: any, encoder: ToString): void;
        public abstract toXml(val: any, encoder: ToXml): void;
        public abstract fromXml(decoder: FromXml): any;

        private type_lib: TypeLibrary;
        private type_name: string;
    }
}