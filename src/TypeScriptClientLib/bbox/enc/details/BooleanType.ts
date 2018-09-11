namespace bbox.enc.details
{
    export class BooleanType extends Type
    {
        constructor(lib: TypeLibrary)
        {
            super(lib, "bool");
        }

        public isValue(): boolean
        {
            return true;
        }

        public toString(val: any, encoder: ToString): void
        {
            if (typeof val != "boolean")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as boolean");
                return;
            }

            encoder.append(JSON.stringify(val));
        }

        public toXml(val: any, encoder: ToXml): void
        {
            if (typeof val != "boolean")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as boolean");
                return;
            }

            encoder.setValue(val ? 'true' : 'false');
        }

        public fromXml(decoder: FromXml): any
        {
            var str = decoder.getValue();

            if (decoder.hasError())
                return undefined;

            if (typeof str != "string")
            {
                decoder.raiseError("Can't decode value of type \"" + typeof str + "\" as boolean");
                return;
            }

            if (str == 'true')
            {
                return true;
            }
            else if (str == 'false')
            {
                return false;
            }
            else
            {
                decoder.raiseError("Can't decode string value \"" + str + "\" as boolean");
                return;
            }
        }
    }
}