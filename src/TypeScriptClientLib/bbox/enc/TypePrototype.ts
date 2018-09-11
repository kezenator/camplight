namespace bbox.enc
{
    export interface TypePrototype<Type>
    {
        TYPE: enc.Type;
        new (): Type;
    }
}