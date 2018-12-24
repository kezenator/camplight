namespace ui.menu
{
    export class MenuEntry
    {
        show: boolean;
        screen: string;

        hue: number;
        imageUri: string;
        x: number;
        y: number;
        selected_x: number;
        selected_y: number;
        help_y: number;
        buttons: ButtonEntry[];

        imageElement: HTMLImageElement;

        constructor(_screen: string)
        {
            this.show = false;
            this.screen = _screen;
            this.buttons = [null, null, null, null, null, null, null, null];

            var marginx = 100;
            var marginy = 100;

            var width = 505;
            var height = 390;

            if (_screen == App.FORTUNE)
            {
                this.hue = 0;
                this.imageUri = 'res/imgs/menu_fortune.png';
                this.x = marginx + (0 * (width + marginx));
                this.y = marginy + (0 * (height + marginy));
                this.selected_x = marginx + (1 * (width + marginx));
                this.selected_y = marginy + (1 * (height + marginy));
                this.help_y = 0;

                this.buttons[0] = new ButtonEntry('#7F7F7F', 'ROLL', 'AGAIN');
                this.buttons[1] = new ButtonEntry('#7F7F7F', 'ROLL', 'AGAIN');
                this.buttons[2] = new ButtonEntry('#7F7F7F', 'ROLL', 'AGAIN');
                this.buttons[3] = new ButtonEntry('#7F7F7F', 'ROLL', 'AGAIN');
                this.buttons[4] = new ButtonEntry('#7F7F7F', 'ROLL', 'AGAIN');
                this.buttons[5] = new ButtonEntry('#7F7F7F', 'ROLL', 'AGAIN');
                this.buttons[6] = new ButtonEntry('#FF0000',   'MENU', '');
                this.buttons[7] = new ButtonEntry('#FFFFFF', 'ROLL', 'AGAIN');
            }
            else if (_screen == App.PONG)
            {
                this.hue = 30;
                this.imageUri = 'res/imgs/menu_pong.png';
                this.x = marginx + (1 * (width + marginx));
                this.y = marginy + (0 * (height + marginy));
                this.selected_x = marginx + (1 * (width + marginx));
                this.selected_y = marginy + (1 * (height + marginy));
                this.help_y = 0;

                this.buttons[0] = new ButtonEntry('red', 'P1', 'UP');
                this.buttons[1] = new ButtonEntry('red', 'P1', 'DOWN');
                this.buttons[2] = new ButtonEntry('black', '', '');
                this.buttons[3] = new ButtonEntry('black', '', '');
                this.buttons[4] = new ButtonEntry('yellow', 'P2', 'UP');
                this.buttons[5] = new ButtonEntry('yellow', 'P2', 'DOWN');
                this.buttons[6] = new ButtonEntry('red', 'MENU', '');
                this.buttons[7] = new ButtonEntry('black', '', '');
            }
            else if (_screen == App.SUPER_MARIO_WORLD)
            {
                this.hue = 120;
                this.imageUri = 'res/imgs/menu_super_mario_world.png';
                this.x = marginx + (2 * (width + marginx));
                this.y = marginy + (0 * (height + marginy));
                this.selected_x = marginx + (1 * (width + marginx));
                this.selected_y = marginy + (1 * (height + marginy));
                this.help_y = 0;

                this.buttons[0] = new ButtonEntry('#000000', '', '');
                this.buttons[1] = new ButtonEntry('#0000FF', 'JUMP', 'SELECT');
                this.buttons[2] = new ButtonEntry('#00FFFF', 'CROUCH', '');
                this.buttons[3] = new ButtonEntry('#7F00FF', 'LEFT', '');
                this.buttons[4] = new ButtonEntry('#7F00FF', 'RIGHT', '');
                this.buttons[5] = new ButtonEntry('#000000', '', '');
                this.buttons[6] = new ButtonEntry('#FF0000', 'MENU', '');
                this.buttons[7] = new ButtonEntry('#00FF00', 'START', 'PAUSE');
            }
            else if (_screen == App.TETRIS)
            {
                this.hue = 180;
                this.imageUri = 'res/imgs/menu_tetris.png';
                this.x = marginx + (0 * (width + marginx));
                this.y = marginy + (1 * (height + marginy));
                this.selected_x = marginx + (1 * (width + marginx));
                this.selected_y = marginy + (0 * (height + marginy));
                this.help_y = 540;

                this.buttons[0] = new ButtonEntry('#7F00FF', 'SPIN', 'DROP');
                this.buttons[1] = new ButtonEntry('#0000FF', 'P1', 'LEFT');
                this.buttons[2] = new ButtonEntry('#0000FF', 'P1', 'RIGHT');
                this.buttons[3] = new ButtonEntry('#FF7F00', 'SPIN', 'DROP');
                this.buttons[4] = new ButtonEntry('#FF0000', 'P2', 'LEFT');
                this.buttons[5] = new ButtonEntry('#FF0000', 'P2', 'RIGHT');
                this.buttons[6] = new ButtonEntry('#FF0000', 'MENU', '');
                this.buttons[7] = new ButtonEntry('#000000', '', '');
            }
            else if (_screen == App.SONIC)
            {
                this.hue = 240;
                this.imageUri = 'res/imgs/menu_sonic.png';
                this.x = marginx + (1 * (width + marginx));
                this.y = marginy + (1 * (height + marginy));
                this.selected_x = marginx + (1 * (width + marginx));
                this.selected_y = marginy + (0 * (height + marginy));
                this.help_y = 540;

                this.buttons[0] = new ButtonEntry('#000000', '', '');
                this.buttons[1] = new ButtonEntry('#0000FF', 'JUMP', 'SPIN');
                this.buttons[2] = new ButtonEntry('#00FFFF', 'CROUCH', '');
                this.buttons[3] = new ButtonEntry('#7F00FF', 'LEFT', '');
                this.buttons[4] = new ButtonEntry('#7F00FF', 'RIGHT', '');
                this.buttons[5] = new ButtonEntry('#000000', '', '');
                this.buttons[6] = new ButtonEntry('#FF0000', 'MENU', '');
                this.buttons[7] = new ButtonEntry('#00FF00', 'START', 'PAUSE');
            }
            else // App.ALTERED_BEAST
            {
                this.hue = 300;
                this.imageUri = 'res/imgs/menu_altered_beast.png';
                this.x = marginx + (2 * (width + marginx));
                this.y = marginy + (1 * (height + marginy));
                this.selected_x = marginx + (1 * (width + marginx));
                this.selected_y = marginy + (0 * (height + marginy));
                this.help_y = 540;

                this.buttons[0] = new ButtonEntry('#000000', '', '');
                this.buttons[1] = new ButtonEntry('#0000FF', 'JUMP', 'SPIN');
                this.buttons[2] = new ButtonEntry('#00FFFF', 'CROUCH', '');
                this.buttons[3] = new ButtonEntry('#7F00FF', 'LEFT', '');
                this.buttons[4] = new ButtonEntry('#7F00FF', 'RIGHT', '');
                this.buttons[5] = new ButtonEntry('#000000', '', '');
                this.buttons[6] = new ButtonEntry('#FF0000', 'MENU', '');
                this.buttons[7] = new ButtonEntry('#00FF00', 'START', 'PAUSE');
            }

            this.imageElement = document.createElement('img');
            this.imageElement.src = this.imageUri;
        }
    };
}