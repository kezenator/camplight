namespace bbox.enc
{
    export interface TypePrototype<Type>
    {
        type: enc.Type;
        new (): Type;
    }
}