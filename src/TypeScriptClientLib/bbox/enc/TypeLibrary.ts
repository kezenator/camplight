namespace bbox.enc
{
    export class TypeLibrary
    {
        public static findType(name: string)
        {
            return TypeLibrary.instance().findType(name);
        }

        public static simpleStructure<T>(name: string, type_constructor: { new (): T }): details.SimpleStructureType
        {
            return TypeLibrary.instance().simpleStructure(name, type_constructor);
        }

        public static stdVectorAsDeque(member_type_name: string): Type
        {
            return TypeLibrary.instance().stdVectorAsDeque(member_type_name);
        }

        public static instance(): TypeLibrary
        {
            if (!TypeLibrary._instance)
                TypeLibrary._instance = new TypeLibrary();
            return TypeLibrary._instance;
        }

        public findType(name: string): Type
        {
            if (!this.by_name.has(name))
            {
                console.assert(false, "bbox.enc.TypeLibrary - no such type " + name);
                return null;
            }
            return this.by_name.get(name);
        }

        public simpleStructure<T>(name: string, type_constructor: { new (): T }): details.SimpleStructureType
        {
            var result = new details.GenericSimpleStructureType<T>(this, name, type_constructor);

            this.addType(result);

            return result;
        }

        public stdVectorAsDeque(member_type_name: string): Type
        {
            var type_name = "std::vector<" + member_type_name + ">";

            if (this.by_name.has(type_name))
                return this.by_name.get(type_name);

            // First time it was called - create and
            // add a new type for this vector type

            var result = new details.DequeType(this, type_name, this.findType(member_type_name));

            this.addType(result);

            return result;
        }

        private static _instance: TypeLibrary = null;
        private by_name: ds.Map<string, Type>;

        constructor()
        {
            this.by_name = new ds.Map<string, Type>();

            this.addType(new details.StringType(this));
            this.addType(new details.BooleanType(this));
            this.addType(new details.IntType(this, "uint8_t", 0, 255));
            this.addType(new details.IntType(this, "uint16_t", 0, 65535));
            this.addType(new details.IntType(this, "uint32_t", 0, 4294967295));
            this.addType(new details.IntType(this, "uint64_t", 0, parseInt("FFFFFFFFFFFFFFFF", 16)));
            this.addType(new details.IntType(this, "int8_t", -128, 127));
            this.addType(new details.IntType(this, "int16_t", -32768, 32767));
            this.addType(new details.IntType(this, "int32_t", -21474836478, 2147483647));
            this.addType(new details.IntType(this, "int64_t", parseInt("-9223372036854775808"), parseInt("9223372036854775807")));
        }

        private addType(type: Type): void
        {
            var name: string = type.getName();

            if (this.by_name.has(name))
            {
                console.assert(false, "bbox.enc.TypeLibrary - duplicate type " + name + " defined");
            }
            else if (type.getTypeLibrary() !== this)
            {
                console.assert(false, "bbox.enc.TypeLibrary - adding type that is not owned by this type library");
            }
            else
            {
                this.by_name.set(name, type);
            }
        }
    }
}