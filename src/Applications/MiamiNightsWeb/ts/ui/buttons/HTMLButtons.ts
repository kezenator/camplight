/// <reference path="../Buttons.ts" />\
/// <reference path="SingleButton.ts" />

namespace ui.buttons
{
    export class HTMLButtons
    {    
        private backButton: SingleButton;
        private playButton: SingleButton;
        private otherButtons: SingleButton[];
        private allButtons: SingleButton[];
        private states: mn.msgs.ButtonStates;
        private websocket: bbox.net.MessageWebSocket;

        constructor(parent: HTMLElement, host: string)
        {
            this.states = new mn.msgs.ButtonStates();

            this.websocket = new bbox.net.MessageWebSocket(
                "ws://" + host + "/ws/buttons",
                "kezenator.com/uri/protocols/ws/miami-nights-buttons/2018-09-12",
                (state: boolean, error: string) => { this.handleWebsocketState(state, error); });

            this.websocket.registerHandler(
                mn.msgs.ButtonColors.TYPE,
                (msg: mn.msgs.ButtonColors) => { this.handleWebsocketRxButtonColors(msg); });

            this.allButtons = [];

            var div = document.createElement('div');
            parent.appendChild(div);
            div.classList.add('buttons');

            this.backButton = new SingleButton(div, 'Back', (state: boolean) => { this.states.back_state = state; });
            this.allButtons.push(this.backButton);
            this.playButton = new SingleButton(div, 'Play', (state: boolean) => { this.states.play_state = state; });
            this.allButtons.push(this.playButton);

            this.otherButtons = new Array(Buttons.NUMBER);

            var keys = "ASDFGH";

            for (var i = 0; i < Buttons.NUMBER; ++i)
            {
                var btn = new SingleButton(
                    div,
                    keys.charAt(i),
                    ((index: number) => ((state: boolean) => { this.states.button_states.setAt(index, state); }))(i));

                this.otherButtons[i] = btn;
                this.allButtons.push(btn);
                this.states.button_states.push_back(false);
            }

            window.addEventListener('keydown', (ev: KeyboardEvent) =>
            {
                var key = ev.key.toUpperCase();

                for (var i = 0; i < this.allButtons.length; ++i)
                {
                    if (this.allButtons[i].matchesKey(key))
                    {
                        this.allButtons[i].setPressed(true);
                        this.websocket.send(this.states);
                        break;
                    }
                }
            });

            window.addEventListener('keyup', (ev: KeyboardEvent) =>
            {
                var key = ev.key.toUpperCase();

                for (var i = 0; i < this.allButtons.length; ++i)
                {
                    if (this.allButtons[i].matchesKey(key))
                    {
                        this.allButtons[i].setPressed(false);
                        this.websocket.send(this.states);
                        break;
                    }
                }
            });
        }

        private handleWebsocketState(state: boolean, error: string): void
        {
            // Reset all colors to black
            for (var button of this.allButtons)
                button.setColor('black');

            if (state)
            {
                // Send our current state
                this.websocket.send(this.states);
            }
        }

        private handleWebsocketRxButtonColors(msg: mn.msgs.ButtonColors): void
        {
            this.backButton.setColor(msg.back_color);
            this.playButton.setColor(msg.play_color);

            var num = Buttons.NUMBER;
            for (var i = 0; i < num; ++i)
            {
                this.otherButtons[i].setColor(msg.button_colors.at(i));
            }
        }
    }
}