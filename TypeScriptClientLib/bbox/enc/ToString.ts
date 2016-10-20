namespace bbox.enc
{
    export class ToString
    {
        private result: string;
        private has_error: boolean;
        private error: string;

        constructor()
        {
            this.result = "";
            this.has_error = false;
            this.error = "";
        }

        getStringalue(): string
        {
            return this.result;
        }

        hasError(): boolean
        {
            return this.has_error;
        }

        getErrorString(): string
        {
            return this.error;
        }

        raiseError(err: string): void
        {
            if (!this.has_error)
            {
                this.has_error = true;
                this.error = err;
            }
        }

        append(str: string): void
        {
            this.result += str;
        }
    }
}