/// <reference path="Piece.ts" />

namespace ui.tetris
{
    export class PieceFactory
    {
        //private next: number;
        private pieces_img: HTMLImageElement;
        private safe: boolean;

        constructor(pieces_img: HTMLImageElement, safe: boolean)
        {
            //this.next = 0;
            this.pieces_img = pieces_img;
            this.safe = safe;
        }

        public newPiece(): Piece
        {
            var num = Math.floor(Math.random() * Piece.NUM_PIECES);

            //var num = this.next;
            //this.next = (this.next + 1) % Piece.NUM_PIECES;

            return new Piece(this.pieces_img, this.safe, num);
        }
    }
}