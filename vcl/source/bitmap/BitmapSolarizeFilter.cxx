/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include <vcl/bitmap.hxx>
#include <vcl/bitmapex.hxx>
#include <vcl/bitmapaccess.hxx>
#include <vcl/BitmapSolarizeFilter.hxx>

#include <bitmapwriteaccess.hxx>

BitmapEx BitmapSolarizeFilter::execute(BitmapEx const& rBitmapEx)
{
    Bitmap aBitmap(rBitmapEx.GetBitmap());
    bool bRet = false;
    BitmapScopedWriteAccess pWriteAcc(aBitmap);

    if (pWriteAcc)
    {
        if (pWriteAcc->HasPalette())
        {
            const BitmapPalette& rPal = pWriteAcc->GetPalette();

            for (sal_uInt16 i = 0, nCount = rPal.GetEntryCount(); i < nCount; i++)
            {
                if (rPal[i].GetLuminance() >= mcSolarGreyThreshold)
                {
                    BitmapColor aCol(rPal[i]);
                    pWriteAcc->SetPaletteColor(i, aCol.Invert());
                }
            }
        }
        else
        {
            BitmapColor aCol;
            const long nWidth = pWriteAcc->Width();
            const long nHeight = pWriteAcc->Height();

            for (long nY = 0; nY < nHeight; nY++)
            {
                Scanline pScanline = pWriteAcc->GetScanline(nY);
                for (long nX = 0; nX < nWidth; nX++)
                {
                    aCol = pWriteAcc->GetPixelFromData(pScanline, nX);

                    if (aCol.GetLuminance() >= mcSolarGreyThreshold)
                        pWriteAcc->SetPixelOnData(pScanline, nX, aCol.Invert());
                }
            }
        }

        pWriteAcc.reset();
        bRet = true;
    }

    if (bRet)
        return BitmapEx(rBitmapEx);

    return BitmapEx();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
