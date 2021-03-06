﻿namespace ui
{
    export class Buttons
    {
        static NUMBER: number = 6;

        private button_states: mn.msgs.ButtonStates;
        private button_colors: mn.msgs.ButtonColors;
        private send_timer: bbox.ui.Timer;
        private websocket: bbox.net.MessageWebSocket;

        private back_clicked: boolean;
        private play_clicked: boolean;
        private other_clicked: boolean[];

        constructor(websocket: bbox.net.MessageWebSocket)
        {
            this.button_states = new mn.msgs.ButtonStates();
            this.button_colors = new mn.msgs.ButtonColors();

            this.back_clicked = false;
            this.play_clicked = false;
            this.other_clicked = new Array(Buttons.NUMBER);

            for (var i = 0; i < Buttons.NUMBER; ++i)
            {
                this.button_states.button_states.push_back(false);
                this.button_colors.button_colors.push_back('black');
                this.other_clicked[i] = false;
            }

            this.send_timer = new bbox.ui.Timer(() => { this.sendTimerExpired(); });

            this.websocket = websocket;

            this.websocket.registerHandler(
                mn.msgs.ButtonStates.TYPE,
                (msg: mn.msgs.ButtonStates) => { this.handleWebsocketRxButtonStates(msg); });

            this.websocket.registerHandler(
                mn.msgs.RetransmitRequired.TYPE,
                (msg: mn.msgs.RetransmitRequired) => { this.handleWebsocketRxRetransmitRequired(msg); });
        }

        handleAppWebSocketState(state: boolean): void
        {
        }

        setPlayColor(color: string): void
        {
            if (color != this.button_colors.play_color)
            {
                this.button_colors.play_color = color;

                if (!this.send_timer.isRunning())
                    this.send_timer.startSingleShot(1);
            }
        }

        setBackColor(color: string): void
        {
            if (color != this.button_colors.back_color)
            {
                this.button_colors.back_color = color;

                if (!this.send_timer.isRunning())
                    this.send_timer.startSingleShot(1);
            }
        }

        setWashColor(color: string): void
        {
            if (color != this.button_colors.wash_color)
            {
                this.button_colors.wash_color = color;

                if (!this.send_timer.isRunning())
                    this.send_timer.startSingleShot(1);
            }
        }

        setButtonColor(btn: number, color: string): void
        {
            if (color != this.button_colors.button_colors.at(btn))
            {
                this.button_colors.button_colors.setAt(btn, color);

                if (!this.send_timer.isRunning())
                    this.send_timer.startSingleShot(1);
            }
        }

        isPlayPressed(): boolean
        {
            return this.button_states.play_state;
        }

        isPlayClicked(): boolean
        {
            return this.play_clicked;
        }

        isBackPressed(): boolean
        {
            return this.button_states.back_state;
        }

        isBackClicked(): boolean
        {
            return this.back_clicked;
        }

        isButtonPressed(btn: number): boolean
        {
            return this.button_states.button_states.at(btn);
        }

        isButtonClicked(btn: number): boolean
        {
            return this.other_clicked[btn];
        }

        frameCompleted(): void
        {
            this.back_clicked = false;
            this.play_clicked = false;

            var num = Buttons.NUMBER;
            for (var i = 0; i < num; ++i)
            {
                this.other_clicked[i] = false;
            }
       }

        private sendTimerExpired()
        {
            this.websocket.send(this.button_colors);
        }

        private handleWebsocketRxButtonStates(msg: mn.msgs.ButtonStates): void
        {
            // Determine if clicked

            var old_state = this.button_states;

            if (msg.back_state && !old_state.back_state)
                this.back_clicked = true;

            if (msg.play_state && !old_state.play_state)
                this.play_clicked = true;

            var num = Buttons.NUMBER;
            for (var i = 0; i < num; ++i)
            {
                if (msg.button_states.at(i) && !this.other_clicked[i])
                    this.other_clicked[i] = true;
            }

            // Save new state

            this.button_states = msg;
        }

        private handleWebsocketRxRetransmitRequired(msg: mn.msgs.RetransmitRequired): void
        {
            this.websocket.send(this.button_colors);
        }
    }
}