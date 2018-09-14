namespace mn.audio
{
    export class AudioManager
    {
        constructor()
        {
        }

        play(resource: string, on_complete: { (): void }): Playout
        {
            var element = document.createElement('audio');
            element.src = resource;
            element.onended = () => { on_complete(); };
            element.play();

            return new Playout(this, 0, element);
        }
    }
}