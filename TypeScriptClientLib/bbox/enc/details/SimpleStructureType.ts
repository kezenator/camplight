namespace bbox.enc.details
{
    export abstract class SimpleStructureType extends Type
    {
        private members: ds.Map<string, Type>;

        constructor(lib: TypeLibrary, name: string)
        {
            super(lib, name);
            this.members = new ds.Map<string, Type>();
        }

        protected abstract constructDefaultValue(): any;

        public addMemberByType(member_name: string, member_type: Type): SimpleStructureType
        {
            if (this.members.has(member_name))
            {
                console.assert(false, "bbox.enc.Type " + this.getName() + " already has member with name " + member_name);
                return this;
            }

            this.members.set(member_name, member_type);

            return this;
        }

        public addMember(member_name: string, type_name: string): SimpleStructureType
        {
            var type = this.getTypeLibrary().findType(type_name);

            if (type)
            {
                this.addMemberByType(member_name, type);
            }

            return this;
        }

        public isValue(): boolean
        {
            return false;
        }

        public toString(val: any, encoder: ToString): void
        {
            if (typeof val != "object")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as structure " + this.getName());
                return;
            }

            if (val == null)
            {
                encoder.raiseError("Can't encode null value as structure " + this.getName());
                return;
            }

            encoder.append("{");
            var prefix = "";

            this.members.forEach((key: string, type: Type) => 
            {
                if (!(<Object>(val)).hasOwnProperty(key))
                {
                    encoder.raiseError("Object of type " + typeof (val) + " encoding as " + this.getName() + " seems to have no member " + key);
                    return;
                }

                encoder.append(prefix);
                prefix = ", ";

                encoder.append(key);
                encoder.append("=");
                type.toString(val[key], encoder);
            });

            encoder.append("}");
        }

        public toXml(val: any, encoder: ToXml): void
        {
            if (typeof val != "object")
            {
                encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as structure " + this.getName());
                return;
            }

            if (val == null)
            {
                encoder.raiseError("Can't encode null value as structure " + this.getName());
                return;
            }

            encoder.startObject();

            this.members.forEach((key: string, type: Type) => 
            {
                if (!(<Object>(val)).hasOwnProperty(key))
                {
                    encoder.raiseError("Object of type " + typeof (val) + " encoding as " + this.getName() + " seems to have no member " + key);
                    return;
                }

                encoder.startNamedValue(key);
                type.toXml(val[key], encoder);
                encoder.endNamedValue();
            });

            encoder.endObject();
        }

        public fromXml(decoder: FromXml): any
        {
            var result = this.constructDefaultValue();

            decoder.startObject();

            if (decoder.hasError())
                return undefined;

            this.members.forEach((key: string, type: Type) => 
            {
                decoder.startNamedValue(key);
                result[key] = type.fromXml(decoder);
                decoder.endNamedValue();

                if (decoder.hasError())
                    return undefined;
            });

            decoder.endObject();

            if (decoder.hasError())
                return undefined;

            return result;
        }
    }

    export class GenericSimpleStructureType<T> extends SimpleStructureType
    {
        private default_constructor: { new (): T };

        constructor(lib: TypeLibrary, name: string, default_constructor: { new (): T })
        {
            super(lib, name);
            this.default_constructor = default_constructor;
        }

        protected constructDefaultValue(): any
        {
            return new this.default_constructor();
        }
    }
}