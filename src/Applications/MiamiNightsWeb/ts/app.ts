/// <reference path="../../../TypeScriptClientLib/lib_references.ts" />

/// <reference path="mn\msgs\ButtonColors.ts" />


/// <reference path="ui/logo/LogoScreen.ts" />
/// <reference path="ui/fortune/FortuneScreen.ts" />
/// <reference path="ui/menu/Background.ts"/>
/// <reference path="ui/menu/MenuScreen.ts" />
/// <reference path="ui/pong/PongScreen.ts" />
/// <reference path="ui/tetris/TetrisScreen.ts" />
/// <reference path="ui/buttons/HTMLButtons.ts" />

class App
{
    static LOGO: string = 'logo';
    static MENU: string = 'menu';
    static PONG: string = 'pomg';
    static FORTUNE: string = 'fortune';
    static TETRIS: string = 'tetris';
    static SONIC: string = 'sonic';
    static ALTERED_BEAST: string = 'altered-beast';
    static SUPER_MARIO_WORLD: string = 'super-mario-world';

    private curScreen: ui.Screen;
    private appWebSocket: bbox.net.MessageWebSocket;
    private audio: mn.audio.AudioManager;
    private buttons: ui.Buttons;
    private htmlButtons: ui.buttons.HTMLButtons;
    private screens: any; 
    private startTime: number;

    private width: number;
    private height: number;
    private scale: number;
    private safeMode: boolean;

    constructor()
    {
        this.width = 1920;
        this.height = 1080;
        this.scale = 1;
        this.safeMode = false;

        var params = new URLSearchParams(window.location.search);

        var has_debug = params.has('debug');
        var has_buttons = params.has('buttons');
        var buttons_host = window.location.host;
        if (params.has('host'))
            buttons_host = params.get('host');

        this.safeMode = params.has('safe');

        if (has_buttons)
        {
            // Just create the HTML buttons

            this.htmlButtons = new ui.buttons.HTMLButtons(document.body, buttons_host);
        }
        else
        {
            // Run the real application

            this.appWebSocket = new bbox.net.MessageWebSocket(
                "ws://" + window.location.host + "/ws/app",
                "12.09.2018.app.miami-nights.kezenator.com",
                (state: boolean, error: string) => { this.handleAppWebSocketState(state, error); });

            this.appWebSocket.registerHandler(
                mn.msgs.EmulatorCompleted.TYPE,
                (msg: mn.msgs.EmulatorCompleted) => { this.handleWebsocketRxEmulatorCompleted(msg); });

            this.buttons = new ui.Buttons(this.appWebSocket);

            this.audio = new mn.audio.AudioManager();

            if (has_debug)
            {
                this.htmlButtons = new ui.buttons.HTMLButtons(document.body, window.location.host);

                this.width = 960;
                this.height = 540;
                this.scale = 0.5;
            }

            this.screens = {};

            var menu_background = new ui.menu.Background();

            this.screens[App.LOGO] = new ui.logo.LogoScreen(this);
            this.screens[App.MENU] = new ui.menu.MenuScreen(this, menu_background);
            this.screens[App.PONG] = new ui.pong.PongScreen(this);
            this.screens[App.FORTUNE] = new ui.fortune.FortuneScreen(this);
            this.screens[App.TETRIS] = new ui.tetris.TetrisScreen(this);

            this.curScreen = this.screens[App.LOGO];
            this.startTime = Date.now();
            window.requestAnimationFrame(() => this._doFrame());
            this.curScreen.show();
        }
    }

    getWidth(): number
    {
        return this.width;
    }

    getHeight(): number
    {
        return this.height;
    }

    getScale(): number
    {
        return this.scale;
    }

    isSafeMode(): boolean
    {
        return this.safeMode;
    }

    getButtons(): ui.Buttons
    {
        return this.buttons;
    }

    getAudio(): mn.audio.AudioManager
    {
        return this.audio;
    }

    showScreen(name: string): void
    {
        // Hide the current screen

        if (this.curScreen)
        {
            this.curScreen.hide();
            this.curScreen = null;
        }

        // See if this is a screen, or else we need to
        // start the emulator

        if (name in this.screens)
        {
            this.curScreen = this.screens[name];
            this.startTime = Date.now();
            this.curScreen.show();
        }
        else if (!this.appWebSocket.isOpen())
        {
            // No connection - just return to the menu

            this.curScreen = this.screens[App.MENU];
            this.startTime = Date.now();
            this.curScreen.show();
        }
        else
        {
            var msg = new mn.msgs.StartEmulator();
            msg.game = name;

            this.appWebSocket.send(msg);
        }
    }

    private _doFrame(): void
    {
        window.requestAnimationFrame(() => this._doFrame());

        this.curScreen.updateFrame(Date.now() - this.startTime);
        this.buttons.frameCompleted();
    }

    private handleAppWebSocketState(state: boolean, _error: string): void
    {
        this.buttons.handleAppWebSocketState(state);

        if (!state
            && (this.curScreen == null))
        {
            // We've lost out connection - return to the logo screen
            this.showScreen(App.LOGO);
        }
    }

    private handleWebsocketRxEmulatorCompleted(_msg: mn.msgs.EmulatorCompleted): void
    {
        // Return to the menu
        this.showScreen(App.MENU);
    }
}

window.addEventListener('load', () =>
{
    var doc: any;
    doc = document;
    doc.fonts.load('10pt "Still Time"')
    new App();
});