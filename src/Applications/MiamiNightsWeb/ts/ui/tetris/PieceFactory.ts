/// <reference path="Piece.ts" />

namespace ui.tetris
{
    export class PieceFactory
    {
        //private next: number;

        constructor()
        {
            //this.next = 0;
        }

        public newPiece(): Piece
        {
            return new Piece(Math.floor(Math.random() * Piece.NUM_PIECES));
            //var result = new Piece(this.next);
            //this.next = (this.next + 1) % Piece.NUM_PIECES;
            //return result;
        }
    }
}