/// <reference path="Piece.ts" />

namespace ui.tetris
{
    export class PieceFactory
    {
        //private next: number;
        private pieces_img: HTMLImageElement;

        constructor(pieces_img: HTMLImageElement)
        {
            //this.next = 0;
            this.pieces_img = pieces_img;
        }

        public newPiece(): Piece
        {
            return new Piece(this.pieces_img, Math.floor(Math.random() * Piece.NUM_PIECES));
            //var result = new Piece(this.next);
            //this.next = (this.next + 1) % Piece.NUM_PIECES;
            //return result;
        }
    }
}