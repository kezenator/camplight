namespace bbox.ui
{
    export interface TimerHandler
    {
        (): void;
    }

    export class Timer
    {
        private handler: TimerHandler;
        private is_started: boolean;
        private is_periodic: boolean;
        private want_periodic: boolean;
        private id: number;
        private period: number;

        constructor(handler: TimerHandler)
        {
            this.handler = handler;
            this.is_started = false;
        }

        isRunning(): boolean
        {
            return this.is_started;
        }

        stop(): void
        {
            if (this.is_started)
            {
                this.is_started = false;

                if (this.is_periodic)
                    window.clearInterval(this.id);
                else
                    window.clearTimeout(this.id);
            }
        }

        startSingleShot(timeout_ms: number): void
        {
            this.stop();

            this.is_started = true;
            this.is_periodic = false;
            this.want_periodic = false;

            this.id = window.setTimeout(
                this.doTimeout.bind(this),
                timeout_ms);
        }

        startPeriodic(initial_ms: number, period_ms: number): void
        {
            this.stop();

            this.is_started = true;
            this.is_periodic = false;
            this.want_periodic = true;
            this.period = period_ms;

            this.id = window.setTimeout(
                this.doTimeout.bind(this),
                initial_ms);
        }

        private doTimeout(): void
        {
            if (!this.is_periodic)
            {
                if (this.want_periodic)
                {
                    this.is_periodic = true;

                    this.id = window.setInterval(
                        this.doTimeout.bind(this),
                        this.period);
                }
                else
                {
                    this.is_started = false;
                }
            }

            this.handler();
        }
    }
}