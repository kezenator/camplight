namespace bbox.enc.details
{
    export class IntType extends Type
    {
        constructor(lib: TypeLibrary, name: string, min: number, max: number)
        {
            super(lib, name);
            this.min = min;
            this.max = max;
        }

        public isValue(): boolean
        {
            return true;
        }

        public toString(val: any, encoder: ToString): void
        {
            if (typeof val != "number")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as integer type " + this.getName());
                return;
            }

            if (!((val >= this.min)
                && (val <= this.max)))
            {
                encoder.raiseError("Can't encode value \"" + val + "\" as integer type " + this.getName() + " because it's not in the range "
                    + this.min + ".." + this.max);
                return;
            }

            encoder.append((<number>(val)).toString());
        }

        public toXml(val: any, encoder: ToXml): void
        {
            if (typeof val != "number")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as integer type " + this.getName());
                return;
            }

            if (!((val >= this.min)
                && (val <= this.max)))
            {
                encoder.raiseError("Can't encode value \"" + val + "\" as integer type " + this.getName() + " because it's not in the range "
                    + this.min + ".." + this.max);
                return;
            }

            encoder.setValue((<number>val).toString());
        }

        public fromXml(decoder: FromXml): any
        {
            var str = decoder.getValue();

            if (decoder.hasError())
                return undefined;

            if (typeof str != "string")
            {
                decoder.raiseError("Can't decode value of type \"" + typeof str + "\" as integer type " + this.getName() + " - must be encoded in JSON as a string");
                return;
            }

            var result = parseInt(str);

            if (!((result >= this.min)
                && (result <= this.max)))
            {
                decoder.raiseError("Can't decode value \"" + str+ "\" as integer type " + this.getName() + " because it's not in the range "
                    + this.min + ".." + this.max);
                return undefined;
            }

            return result;
        }

        private min: number;
        private max: number;
    }
}