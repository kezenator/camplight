namespace ui
{
    export abstract class Buttons
    {
        static NUMBER: number = 6;

        abstract setPlayColor(color: string): void;
        abstract setButtonColor(btn: number, color: string): void;

        abstract isPlayPressed(): boolean;
        abstract isPlayClicked(): boolean;

        abstract isButtonPressed(btn: number): boolean;
        abstract isButtonClicked(btn: number): boolean;

        abstract frameCompleted(): void;
    }
}