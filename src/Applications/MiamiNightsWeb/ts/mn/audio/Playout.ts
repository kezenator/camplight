namespace mn.audio
{
    export class Playout
    {
        private manager: AudioManager;
        private id: number;
        private element: HTMLAudioElement;

        constructor(manager: AudioManager, id: number, element: HTMLAudioElement)
        {
            this.manager = manager;
            this.id = id;
            this.element = element;
        }
    }
}