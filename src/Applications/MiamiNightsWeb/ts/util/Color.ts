/// <reference path="Math.ts" />

namespace util
{
    export class Color
    {
        public r: number;
        public g: number;
        public b: number;
        public a: number;

        public constructor(r: number, g: number, b: number, a: number)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }

        public lerp(amount: number, c: Color): Color
        {
            return new Color(
                lerp(amount, this.r, c.r),
                lerp(amount, this.g, c.g),
                lerp(amount, this.b, c.b),
                lerp(amount, this.a, c.a));
        }

        public toString(): string
        {
            return 'rgba(' + this.r + ',' + this.g + ',' + this.b + ',' + this.a + ')';
        }
    };
}