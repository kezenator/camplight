namespace mn.audio
{
    export class AudioManager
    {
        private free_elements: HTMLAudioElement[];

        constructor()
        {
            this.free_elements = new Array(0);
        }

        play(resource: string, on_complete: { (): void }): Playout
        {
            var element: HTMLAudioElement = null;

            if (this.free_elements.length > 0)
            {
                element = this.free_elements.pop();
            }
            else
            {
                element = document.createElement('audio');
            }

            return new Playout(this, element, resource, on_complete);
        }

        recycle(element: HTMLAudioElement): void
        {
            this.free_elements.push(element);
        }
    }
}