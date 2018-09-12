/// <reference path="../../../TypeScriptClientLib/lib_references.ts" />

/// <reference path="mn\msgs\ButtonColors.ts" />

/// <reference path="ui/logo/LogoScreen.ts" />
/// <reference path="ui/fortune/FortuneScreen.ts" />
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

    private curScreen: ui.Screen;
    private buttons: ui.Buttons;
    private htmlButtons: ui.buttons.HTMLButtons;
    private screens: any; 
    private startTime: number;

    constructor()
    {
        var params = new URLSearchParams(window.location.search);

        var has_debug = params.has('debug');
        var has_buttons = params.has('buttons');
        var buttons_host = window.location.host;
        if (params.has('host'))
            buttons_host = params.get('host');

        if (has_buttons)
        {
            // Just create the HTML buttons

            this.htmlButtons = new ui.buttons.HTMLButtons(document.body, buttons_host);
        }
        else
        {
            // Run the real application

            this.buttons = new ui.Buttons(window.location.host);

            this.screens = {};

            this.screens[App.LOGO] = new ui.logo.LogoScreen(this);
            this.screens[App.MENU] = new ui.menu.MenuScreen(this);
            this.screens[App.PONG] = new ui.pong.PongScreen(this);
            this.screens[App.FORTUNE] = new ui.fortune.FortuneScreen(this);
            this.screens[App.TETRIS] = new ui.tetris.TetrisScreen(this);

            if (has_debug)
            {
                this.htmlButtons = new ui.buttons.HTMLButtons(document.body, window.location.host);
            }

            this.curScreen = this.screens[App.LOGO];
            this.startTime = Date.now();
            window.requestAnimationFrame(() => this._doFrame());
            this.curScreen.show();
        }
    }

    getButtons(): ui.Buttons
    {
        return this.buttons;
    }

    showScreen(name: string): void
    {
        this.curScreen.hide();
        this.curScreen = this.screens[name];
        this.startTime = Date.now();
        this.curScreen.show();                           
    }

    private _doFrame()
    {
        window.requestAnimationFrame(() => this._doFrame());

        this.curScreen.updateFrame(Date.now() - this.startTime);
        this.buttons.frameCompleted();
    }
}

window.addEventListener('load', () =>
{
    var doc: any;
    doc = document;
    doc.fonts.load('10pt "Still Time"')
    new App();
});