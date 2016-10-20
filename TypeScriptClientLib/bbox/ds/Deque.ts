module bbox.ds
{
    export class Deque<Type>
    {
        private array: Type[];

        constructor()
        {
            this.array = [];
        }

        empty(): boolean
        {
            return this.array.length == 0;
        }

        size(): number
        {
            return this.array.length;
        }

        back(): Type
        {
            console.assert(this.array.length > 0, "bbox.ds.Deque.back() called but empty");

            return this.array[this.array.length - 1];
        }

        front(): Type
        {
            console.assert(this.array.length > 0, "bbox.ds.Deque.front() called but empty");

            return this.array[0];
        }

        at(index: number): Type
        {
            console.assert((index >= 0) && (index < this.array.length), "bbox.ds.Deque.at() called with invalid index " + index + ", expected 0.." + (this.array.length - 1));

            return this.array[index];
        }

        push_back(val: Type): void
        {
            this.array.push(val);
        }

        pop_back(): Type
        {
            console.assert(this.array.length > 0, "bbox.ds.Deque.pop_back() called but empty");

            return this.array.pop();
        }

        pop_front(): Type
        {
            console.assert(this.array.length > 0, "bbox.ds.Deque.pop_front() called but empty");

            return this.array.shift();
        }
    }
}