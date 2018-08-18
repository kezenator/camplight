namespace util
{
    export function lerp(amount: number, low: number, high: number): number
    {
        if (amount <= 0)
            return low;
        else if (amount >= 1)
            return high;
        else
            return ((1 - amount) * low) + (amount * high);
    }

    export function unlerp(amount: number, low: number, high: number): number
    {
        if (amount <= low)
            return 0;
        else if (amount >= high)
            return 1;
        else
            return (amount - low) / (high - low);
    }

    export function constrain(value: number, low: number, high: number): number
    {
        if (value < low)
            return low;
        else if (value > high)
            return high;
        return value;
    }

    export function arrive(amount: number): number
    {
        return Math.sin(amount * 0.5 * Math.PI);
    }

    export function depart_and_arrive(amount: number): number
    {
        return 0.5 - 0.5 * Math.cos(amount * Math.PI);
    }

    export function there_and_back(amount: number): number
    {
        return 0.5 - 0.5 * Math.cos(amount * 2.0 * Math.PI);
    }
}