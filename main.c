#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define LeftShift (1 << 24) /*shift left by 24, so multiply by 2^24*/

int ValidSquares = 136, SpecialMove = 128, PenaltyValue = 8e3, NewEvaluation = 0, EnPassantSquare = 0, StartingSquare, PositionsEvaluated, MoveCount, J, Z, MovingSide = 16, *p, c[9]; /* M=0x88  10001000   */ // better get moving side from fen

char L, StartingBestSquare, TargetBestSquare, CastlingRights = 0, //this is for later micromax implementations
                                              PieceValues[] = {0, 2, 2, 7, -1, 8, 12, 23}, /* relative piece values    */
    PieceMovements[] = {-16, -15, -17, 0, 1, 16, 0, 1, 16, 15, 17, 0, 14, 18, 31, 33, 0,   /* step-vector lists */
                        7, -1, 11, 6, 8, 3, 6,                                             /* 1st dir. in PieceMovements[] per piece. Which piece has which moves*/
                        6, 3, 5, 7, 4, 5, 3, 6},                                           /* initial piece setup      */
    Board[129] = {0},                                                                      /* board: half of 16x8+dummy*/
    TranslationTable[1035];                                                                /* hash translation table for immediate computations  */

int FENToPiece(char fen_char)
{
    switch (fen_char)
    {
    case 'p':
        return 2 + 16; // Black pawn, add 8 for black
    case 'r':
        return PieceMovements[24] + 16; // Black rook
    case 'n':
        return PieceMovements[25] + 16; // Black knight
    case 'b':
        return PieceMovements[26] + 16; // Black bishop
    case 'q':
        return PieceMovements[27] + 16; // Black queen
    case 'k':
        return PieceMovements[28] + 16; // Black king

    case 'P':
        return 1 + 8; // White pawn, add 16 for white
    case 'R':
        return PieceMovements[24] + 8; // White rook
    case 'N':
        return PieceMovements[25] + 8; // White knight
    case 'B':
        return PieceMovements[26] + 8; // White bishop
    case 'Q':
        return PieceMovements[27] + 8; // White queen
    case 'K':
        return PieceMovements[28] + 8; // White king
    }
}
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
void ParseFEN(const char *fen)
{
    int sq = 0; // Start at A8 (top-left corner, index 0)

    // Parse piece placement
    while (*fen)
    {
        if (*fen >= '1' && *fen <= '8')
        {
            sq += (*fen - '0'); // Skip empty squares
        }
        else if (*fen == '/')
        {
            sq += 8; // Move to the next rank
        }
        else if (*fen == ' ')
        {
            break; // End of board description
        }
        else
        {
            Board[sq++] = FENToPiece(*fen); // Map FEN char to piece
        }
        fen++;
    }

    fen++; // Skip space
    MovingSide = (*fen == 'w') ? 8 : 16;

    // Parse castling rights
    fen += 2; // Move to castling section

    while (*fen && *fen != ' ')
    {
        switch (*fen)
        {
        case 'K':
            CastlingRights |= 1;
            break; // 0001
        case 'Q':
            CastlingRights |= 2;
            break; // 0010
        case 'k':
            CastlingRights |= 4;
            break; // 0100
        case 'q':
            CastlingRights |= 8;
            break; // 1000
        }
        fen++;
    }

    // Parse en passant square
    fen++;
    if (*fen != '-')
    {
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');
        EnPassantSquare = (rank * 16) + file;
    }
    else
    {
        EnPassantSquare = -1;
    }
}

int main()
{
    const char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b kq - 1 15";

    // Call the FEN parser
    ParseFEN(fen);

    // Check the board state
    for (int i = 0; i < 128; i++)
    {
        if (!(i & 0x88))
        { // Skip squares outside the valid board
            printf("Board[%d] = %d\n", i, Board[i]);
        }
    }

    // Print other parsed details
    printf("MovingSide: %d\n", MovingSide);
    printf("CastlingFlags: %d\n", CastlingRights);
    printf("EnPassantSquare: %d\n", EnPassantSquare);

    return 0;
}
