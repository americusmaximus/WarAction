/*
Copyright (c) 2024 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "BinArchive.hxx"
#include "FontAsset.hxx"

#define MAX_BINARY_ARCHIVE_COUNT    16
#define MAX_BINARY_CHUNK_COUNT      8
#define MAX_BINARY_FILE_COUNT       4096

typedef struct AssetStateModuleContainer
{
    BOOL                IsActive;                           // 0x100460a8

    BINFILECONTENT      Files[MAX_BINARY_FILE_COUNT];       // 0x1005ae70
    struct
    {
        U32             Count;                              // 0x1005ae68
        BINFILECHUNK    Values[MAX_BINARY_CHUNK_COUNT];     // 0x1007ae70
    } Chunks;
    BINARCHIVE          Archives[MAX_BINARY_ARCHIVE_COUNT]; // 0x1007aef0

    struct
    {
        BinAsset MsgBox;                                    // 0x100590e8

        BINASSET Msg1CBT1;                                  // 0x10059320
        BINASSET MenuG;                                     // 0x10059330
        BINASSET Msg1CBT0;                                  // 0x10059340
        BINASSET MainRES;                                   // 0x10059350
        BINASSET Single0SC;                                 // 0x10059360
        BINASSET Single0BT;                                 // 0x10059370

        BINASSET MN2Texts;                                  // 0x100594d0
        BINASSET Single0BK;                                 // 0x100594e0
        BINASSET RatingSC;                                  // 0x100594f0

        BINASSET RatingBK;                                  // 0x10059504
        BINASSET RatingBT;                                  // 0x10059514
        BINASSET Single5BT;                                 // 0x10059524
        BINASSET Single5BK;                                 // 0x10059534
        BINASSET MainBK;                                    // 0x10059544
        BINASSET MainBT;                                    // 0x10059554
        BINASSET Single0Dif;                                // 0x10059564
        BINASSET Single4SC;                                 // 0x10059574
        BINASSET Single4BT;                                 // 0x10059584
        BINASSET Single4BK;                                 // 0x10059598
        BINASSET ZVGold;                                    // 0x100595a8
        BINASSET Single3SC;                                 // 0x100595b8
        BINASSET ZVM;                                       // 0x100595c8
        BINASSET Single3BT;                                 // 0x100595d8
        BINASSET Zvezdy;                                    // 0x100595e8
        BINASSET Msg2BK;                                    // 0x100595f8
        BINASSET Single3BK;                                 // 0x10059608
        BINASSET Single2BT;                                 // 0x10059618
        BINASSET Msg0;                                      // 0x10059628
        BINASSET BriefBT;                                   // 0x10059638
        BINASSET BriefBK;                                   // 0x10059648
        BINASSET Msg1BK;                                    // 0x10059658
        BINASSET Single2BK;                                 // 0x10059668

        BINASSET Multi4ParmOP;                              // 0x10059678
        BINASSET JMulti2BK;                                 // 0x10059688
        BINASSET Multi4ParmNM;                              // 0x10059698
        BINASSET JMulti2BT;                                 // 0x100596a8
        BINASSET JMulti1BT;                                 // 0x100596b8
        BINASSET Multi4ROP;                                 // 0x100596c8

        BINASSET Multi4ParmBK;                              // 0x100596dc
        BINASSET SinOld2BK;                                 // 0x100596ec
        BINASSET GreetingsBK;                               // 0x100596fc

        BINASSET Multi4TOP;                                 // 0x10059938
        BINASSET SinOld2BT;                                 // 0x10059948
        BINASSET Multi4NOP;                                 // 0x10059958
        BINASSET ScrollBar;                                 // 0x10059968
        BINASSET JMulti1BK;                                 // 0x10059978
        BINASSET MainOP;                                    // 0x10059988
        BINASSET MStatBT;                                   // 0x10059998
        BINASSET MStatBK;                                   // 0x100599a8
        BINASSET Single5TX;                                 // 0x100599b8
        BINASSET Multi4SP;                                  // 0x100599c8

        BINASSET Multi4LOP;                                 // 0x100599d8
        BINASSET KartaBK;                                   // 0x100599e8
        BINASSET Multi4SC;                                  // 0x100599f8
        BINASSET Multi3OP;                                  // 0x10059a08
        BINASSET Multi4BT;                                  // 0x10059a18

        BINASSET Multi4CO;                                  // 0x10059a28
        BINASSET Multi4BK;                                  // 0x10059a38
        BINASSET Multi3SC;                                  // 0x10059a48
        BINASSET DialSC;                                    // 0x10059a58
        BINASSET Msg2BT21;                                  // 0x10059a68
        BINASSET Msg2BT20;                                  // 0x10059a78
        BINASSET Msg2BT11;                                  // 0x10059a88
        BINASSET Msg2BT10;                                  // 0x10059a98
        BINASSET Multi3BT;                                  // 0x10059aa8
        BINASSET Multi3BK;                                  // 0x10059ab8
        BINASSET Multi2SC;                                  // 0x10059ac8

        BINASSET DialBK;                                    // 0x10059aec
        BINASSET Msg1BT0;                                   // 0x10059afc
        BINASSET DialBT;                                    // 0x10059b0c
        BINASSET Rombik;                                    // 0x10059b1c
        BINASSET Credits;                                   // 0x10059b2c
        BINASSET Msg1BT1;                                   // 0x10059b3c
        BINASSET Multi2BT;                                  // 0x10059b4c
        BINASSET Multi2BK;                                  // 0x10059b5c
        BINASSET JMulti2SC;                                 // 0x10059b6c

        BINASSET Multi1BK;                                  // 0x10059b8c
        BINASSET Multi1BT;                                  // 0x10059b9c
    } Assets;

    struct
    {
        FONTASSET Main;                                     // 0x100590f8
        FONTASSET Comic;                                    // 0x10059710
    } Fonts;
} ASSETSTATEMODULECONTAINER, * ASSETSTATEMODULECONTAINERPTR;

EXTERN ASSETSTATEMODULECONTAINER AssetsState;

LPCSTR AcquireAssetMessage(CONST U32 indx);

LPVOID AcquireBinFileChunk(CONST BFH indx, CONST U32 chunk);
LPVOID InitializeBinFileChunk(CONST BFH indx, CONST U32 chunk, CONST U32 size);
U32 AcquireBinFileChunkSize(CONST BFH indx, CONST U32 offset);

U32 AcquireAssetContent(LPCSTR name, LPVOID* content, CONST U32 size);

VOID DisposeAssets(VOID);
VOID DisposeBinFileChunks(VOID);
VOID ReleaseAssets(VOID);
VOID ReleaseBinFileChunks(VOID);
VOID SelectAssetsStateIsActive(CONST BOOL active);