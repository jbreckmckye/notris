/**
 * ############################################################################
 * #       NOTRIS - THE FAMILIAR BUT LEGALLY DISTINCT BLOCK PUZZLE GAME       #
 * ############################################################################
 */

#include <psxgpu.h>
#include <stdio.h>

#include "./gfx/gfx.h"
#include "defs.h"

int main(int argc, char** argv) {
  gfx_init();

  // Code for displaying debug font
  FntLoad(960, 0);
  int printStream = FntOpen(20, 20, 300, 220, 0, 80);
  int framecount = 0;

  TIM_IMAGE tim;
  gfx_loadFontTexture(&tim);

  while (1) {
    gfx_showFontTexture(&tim);

    gfx_drawFontString(200, 200, "HELLO", 0);

    framecount++;
    framecount = framecount % 60;
    // Write into debug text printstream & flush into drawing environment
    FntPrint(printStream, "Hello! FRAME %d !", framecount);
    FntFlush(printStream);

    gfx_endFrame();
  }

  return 0;
}
