namespace bbox.enc
{
    export class MsgAnyPtr
    {
        protected constructor(type: Type)
        {
            this.type = type;
        }

        public getType(): Type
        {
            return this.type;
        }

        private type: Type;
    };
}