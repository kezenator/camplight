namespace mn.audio
{
    export class Playout
    {
        private manager: AudioManager;
        private element: HTMLAudioElement;
        private resource: string;
        private stopped: boolean;
        private on_complete: { (): void; };

        constructor(manager: AudioManager, element: HTMLAudioElement, resource: string, on_complete: { (): void })
        {
            this.manager = manager;
            this.element = element;
            this.resource = resource;
            this.stopped = false;
            this.on_complete = on_complete;

            element.pause();
            element.volume = 1;
            element.src = resource;
            element.onended = () => { this.handleOnComplete(); };
            element.play();
        }

        public stop()
        {
            if (!this.stopped)
            {
                this.stopped = true;
                this.manager.recycle(this.element);

                this.element.pause();
            }
        }

        private handleOnComplete()
        {
            if (!this.stopped)
            {
                this.stopped = true;
                this.manager.recycle(this.element);

                this.on_complete();
            }
        }
    }
}