/*====================================================================*
 -  Copyright (C) 2001 Leptonica.  All rights reserved.
 -
 -  Redistribution and use in source and binary forms, with or without
 -  modification, are permitted provided that the following conditions
 -  are met:
 -  1. Redistributions of source code must retain the above copyright
 -     notice, this list of conditions and the following disclaimer.
 -  2. Redistributions in binary form must reproduce the above
 -     copyright notice, this list of conditions and the following
 -     disclaimer in the documentation and/or other materials
 -     provided with the distribution.
 -
 -  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 -  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 -  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 -  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ANY
 -  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 -  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 -  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 -  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 -  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 -  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 -  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *====================================================================*/

/*
 * adaptmap_reg.c
 *
 *   Regression test demonstrating adaptive mappings in both gray and color
 */

#include "allheaders.h"

   /* Location of image region in wet-day.jpg */
static const l_int32  XS = 151;
static const l_int32  YS = 225;
static const l_int32  WS = 913;
static const l_int32  HS = 1285;

static const l_int32  SIZE_X = 10;
static const l_int32  SIZE_Y = 30;
static const l_int32  BINTHRESH = 50;
static const l_int32  MINCOUNT = 30;

static const l_int32  BGVAL = 200;
static const l_int32  SMOOTH_X = 2;
static const l_int32  SMOOTH_Y = 1;

int main(int    argc,
         char **argv)
{
l_int32       w, h, d;
PIX          *pixs, *pixg, *pixim, *pixgm, *pixmi, *pix1, *pix2;
PIX          *pixmr, *pixmg, *pixmb, *pixmri, *pixmgi, *pixmbi;
PIXA         *pixa;
L_REGPARAMS  *rp;

    if (regTestSetup(argc, argv, &rp))
        return 1;

    lept_mkdir("lept/adapt");  // REMOVE?

    pixs = pixRead("wet-day.jpg");
    pixa = pixaCreate(0);
    pixg = pixConvertRGBToGray(pixs, 0.33, 0.34, 0.33);
    pixaAddPix(pixa, pixs, L_INSERT);
    pixaAddPix(pixa, pixg, L_INSERT);
    pixGetDimensions(pixs, &w, &h, NULL);

        /* Process in grayscale */
    startTimer();
    pixim = pixCreate(w, h, 1);
    pixRasterop(pixim, XS, YS, WS, HS, PIX_SET, NULL, 0, 0);
    pixGetBackgroundGrayMap(pixg, pixim, SIZE_X, SIZE_Y,
                            BINTHRESH, MINCOUNT, &pixgm);
    fprintf(stderr, "Time for gray adaptmap gen: %7.3f\n", stopTimer());
    regTestWritePixAndCheck(rp, pixgm, IFF_PNG);  /* 0 */
    pixaAddPix(pixa, pixgm, L_INSERT);

    startTimer();
    pixmi = pixGetInvBackgroundMap(pixgm, BGVAL, SMOOTH_X, SMOOTH_Y);
    fprintf(stderr, "Time for gray inv map generation: %7.3f\n", stopTimer());
    regTestWritePixAndCheck(rp, pixmi, IFF_PNG);  /* 1 */
    pixaAddPix(pixa, pixmi, L_INSERT);

    startTimer();
    pix1 = pixApplyInvBackgroundGrayMap(pixg, pixmi, SIZE_X, SIZE_Y);
    fprintf(stderr, "Time to apply gray inv map: %7.3f\n", stopTimer());
    regTestWritePixAndCheck(rp, pix1, IFF_JFIF_JPEG);  /* 2 */
    pixaAddPix(pixa, pix1, L_INSERT);

    pix2 = pixGammaTRCMasked(NULL, pix1, pixim, 1.0, 0, 190);
    pixInvert(pixim, pixim);
    pixGammaTRCMasked(pix2, pix2, pixim, 1.0, 60, 190);
    regTestWritePixAndCheck(rp, pix2, IFF_JFIF_JPEG);  /* 3 */
    pixaAddPix(pixa, pix2, L_INSERT);
    pixDestroy(&pixim);

        /* Process in color */
    startTimer();
    pixim = pixCreate(w, h, 1);
    pixRasterop(pixim, XS, YS, WS, HS, PIX_SET, NULL, 0, 0);
    pixGetBackgroundRGBMap(pixs, pixim, NULL, SIZE_X, SIZE_Y,
                           BINTHRESH, MINCOUNT,
                           &pixmr, &pixmg, &pixmb);
    fprintf(stderr, "Time for color adaptmap gen: %7.3f\n", stopTimer());
    regTestWritePixAndCheck(rp, pixmr, IFF_PNG);  /* 4 */
    regTestWritePixAndCheck(rp, pixmg, IFF_PNG);  /* 5 */
    regTestWritePixAndCheck(rp, pixmb, IFF_PNG);  /* 6 */
    pixaAddPix(pixa, pixmr, L_INSERT);
    pixaAddPix(pixa, pixmg, L_INSERT);
    pixaAddPix(pixa, pixmb, L_INSERT);

    startTimer();
    pixmri = pixGetInvBackgroundMap(pixmr, BGVAL, SMOOTH_X, SMOOTH_Y);
    pixmgi = pixGetInvBackgroundMap(pixmg, BGVAL, SMOOTH_X, SMOOTH_Y);
    pixmbi = pixGetInvBackgroundMap(pixmb, BGVAL, SMOOTH_X, SMOOTH_Y);
    fprintf(stderr, "Time for color inv map generation: %7.3f\n", stopTimer());
    regTestWritePixAndCheck(rp, pixmri, IFF_PNG);  /* 7 */
    regTestWritePixAndCheck(rp, pixmgi, IFF_PNG);  /* 8 */
    regTestWritePixAndCheck(rp, pixmbi, IFF_PNG);  /* 9 */
    pixaAddPix(pixa, pixmri, L_INSERT);
    pixaAddPix(pixa, pixmgi, L_INSERT);
    pixaAddPix(pixa, pixmbi, L_INSERT);

    startTimer();
    pix1 = pixApplyInvBackgroundRGBMap(pixs, pixmri, pixmgi, pixmbi,
                                       SIZE_X, SIZE_Y);
    fprintf(stderr, "Time to apply color inv maps: %7.3f\n", stopTimer());
    regTestWritePixAndCheck(rp, pix1, IFF_JFIF_JPEG);  /* 10 */
    pixaAddPix(pixa, pix1, L_INSERT);

    pix2 = pixGammaTRCMasked(NULL, pix1, pixim, 1.0, 0, 190);
    pixInvert(pixim, pixim);
    pixGammaTRCMasked(pix2, pix2, pixim, 1.0, 60, 190);
    regTestWritePixAndCheck(rp, pix2, IFF_JFIF_JPEG);  /* 11 */
    pixaAddPix(pixa, pix2, L_INSERT);
    pixDestroy(&pixim);

        /* Process at higher level in color */
    startTimer();
    pixim = pixCreate(w, h, 1);
    pixRasterop(pixim, XS, YS, WS, HS, PIX_SET, NULL, 0, 0);
    pix1 = pixBackgroundNorm(pixs, pixim, NULL, 5, 10, BINTHRESH, 20,
                             BGVAL, SMOOTH_X, SMOOTH_Y);
    fprintf(stderr, "Time for bg normalization: %7.3f\n", stopTimer());
    regTestWritePixAndCheck(rp, pix1, IFF_JFIF_JPEG);  /* 12 */
    pixaAddPix(pixa, pix1, L_INSERT);

    pix2 = pixGammaTRCMasked(NULL, pix1, pixim, 1.0, 0, 190);
    pixInvert(pixim, pixim);
    pixGammaTRCMasked(pix2, pix2, pixim, 1.0, 60, 190);
    regTestWritePixAndCheck(rp, pix2, IFF_JFIF_JPEG);  /* 13 */
    pixaAddPix(pixa, pix2, L_INSERT);
    pixDestroy(&pixim);

        /* Display results */
    pix1 = pixaDisplayTiledAndScaled(pixa, 32, 400, 4, 0, 20, 2);
    pixWrite("/tmp/junk.jpg", pix1, IFF_JFIF_JPEG);
    pixDisplayWithTitle(pix1, 100, 0, NULL, rp->display);
    pixDestroy(&pix1);
    pixaDestroy(&pixa);

    return regTestCleanup(rp);
}

