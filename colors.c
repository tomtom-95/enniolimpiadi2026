#ifndef COLORS_C
#define COLORS_C

#include "clay.h"

Clay_Color COLOR_WHITE     = { 255, 255, 255, 255};
Clay_Color COLOR_OFF_WHITE = { 245, 245, 245, 255};

Clay_Color COLOR_BLACK     = { 0, 0, 0, 255};
Clay_Color COLOR_RED       = { 255, 0, 0, 255};
Clay_Color COLOR_GREEN     = { 0, 255, 0, 255};
Clay_Color COLOR_BLUE      = { 0, 0, 255, 255};

Clay_Color stringColor            = { 0, 0, 0, 255};
Clay_Color stringColorOnClick     = { 135, 206, 235, 255 };
Clay_Color contentBackgroundColor = { 90, 90, 90, 255 };
Clay_Color headerBackgroundColor  = { 255, 255, 255, 255 };


Clay_Color headerButtonColor      = { 255, 255, 255, 255};
Clay_Color headerButtonHoverColor = { 240, 240, 240, 255 };
Clay_Color headerButtonStringClickColor = { 0, 0, 255, 255 };

Clay_Color dashboardWindowColor  = { 255, 255, 255, 255};

Clay_Color eventElementColor      = { 255, 255, 255, 255};
Clay_Color eventElementHoverColor = { 250, 250, 250, 255};

Clay_Color matchBorderColor       = { 200, 200, 200, 255};
Clay_Color matchVsColor           = { 150, 150, 150, 255};

Clay_Color goBackButtonColor      = { 230, 240, 250, 255};
Clay_Color goBackButtonHoverColor = { 200, 220, 245, 255};
Clay_Color goBackTextColor        = { 60, 100, 160, 255};
Clay_Color tournamentTitleColor   = { 40, 40, 60, 255};

Clay_Color textInputBackgroundColor = { 250, 250, 250, 255};
Clay_Color textInputBorderColor     = { 200, 200, 200, 255};
Clay_Color addButtonColor           = { 76, 175, 80, 255};
Clay_Color addButtonHoverColor      = { 56, 142, 60, 255};
Clay_Color addButtonTextColor       = { 255, 255, 255, 255};
Clay_Color playerRowColor           = { 255, 255, 255, 255};
Clay_Color playerRowHoverColor      = { 245, 245, 245, 255};

#endif