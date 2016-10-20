namespace bbox.enc.details
{
    export class DequeType extends Type
    {
        private member_type: Type;
        private uint32_type: Type;

        constructor(lib: TypeLibrary, name: string, member_type: Type)
        {
            super(lib, name);
            this.member_type = member_type;
            this.uint32_type = lib.findType("uint32_t");
        }

        public isValue(): boolean
        {
            return false;
        }

        public toString(val: any, encoder: ToString): void
        {
            if (typeof val != "object")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as vector/deque " + this.getName());
                return;
            }

            var deque = <bbox.ds.Deque<any>>(val);

            encoder.append("[");
            var prefix = "";

            for (var i = 0; i < deque.size(); ++i)
            {
                encoder.append(prefix);
                prefix = ", ";
                this.member_type.toString(deque.at(i), encoder);
            }

            encoder.append("]");
        }

        public toXml(val: any, encoder: ToXml): void
        {
            if (typeof val != "object")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as vector/deque " + this.getName());
                return;
            }

            var deque = <bbox.ds.Deque<any>>(val);

            encoder.startObject();
            encoder.startNamedValue("size");
            this.uint32_type.toXml(deque.size(), encoder);
            encoder.endNamedValue();

            for (var i = 0; i < deque.size(); ++i)
            {
                encoder.startNamedValue("entry");
                this.member_type.toXml(deque.at(i), encoder);
                encoder.endNamedValue();
            }

            encoder.endObject();
        }

        public fromXml(decoder: FromXml): any
        {
            var result = new bbox.ds.Deque<any>();

            decoder.startObject();
            
            decoder.startNamedValue("size");
            var size: number = <number>this.uint32_type.fromXml(decoder);
            decoder.endNamedValue();

            var count = 0;

            while (!decoder.hasError()
                && (count < size))
            {
                decoder.startNamedValue("entry");
                result.push_back(this.member_type.fromXml(decoder));
                decoder.endNamedValue();

                count += 1;
            }

            decoder.endObject();

            return result;
        }
    }
}