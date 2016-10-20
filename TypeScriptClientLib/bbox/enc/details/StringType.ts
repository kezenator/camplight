namespace bbox.enc.details
{
    export class StringType extends Type
    {
        constructor(lib: TypeLibrary)
        {
            super(lib, "std::string");
        }

        public isValue(): boolean
        {
            return true;
        }

        public toString(val: any, encoder: ToString): void
        {
            if (typeof val != "string")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as string");
                return;
            }

            encoder.append(JSON.stringify(val));
        }

        public toXml(val: any, encoder: ToXml): void
        {
            if (typeof val != "string")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as string");
                return;
            }

            encoder.setValue(val);
        }

        public fromXml(decoder: FromXml): any
        {
            var str = decoder.getValue();

            if (decoder.hasError())
                return undefined;

            if (typeof str != "string")
            {
                decoder.raiseError("Can't decode value of type \"" + typeof str + "\" as string");
                return;
            }

            return str;
        }
    }
}