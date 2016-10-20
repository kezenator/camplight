namespace bbox.ds
{
    export class Map<K, V>
    {
        constructor()
        {
            this.map = {};
        }

        public has(key: K): boolean
        {
            return this.map.hasOwnProperty(<any>(key));
        }

        public set(key: K, value: V): Map<K, V>
        {
            (<any>(this.map))[<any>(key)] = <any>(value);
            return this;
        }

        public get(key: K): V
        {
            if (!this.map.hasOwnProperty(<any>(key)))
                return undefined;

            return (<any>(this.map))[<any>(key)];
        }

        public forEach(func: (key: K, value: V) => void, this_inst? : any): Map<K, V>
        {
            var passed_this = this_inst;
            if (!passed_this)
                passed_this = this;

            for (var key in <any>(this.map))
            {
                func.apply(passed_this, [<K>(<any>(key)), <V>((<any>(this.map))[key])]);
            }
            return this;
        }

        private map: Object;
    }
}