#include <pebble.h>

GColor pride[] = {GColorRed, GColorOrange, GColorYellow, GColorGreen, GColorBlue, GColorPurple};
GColor trans[] = {GColorVividCerulean, GColorShockingPink, GColorWhite, GColorShockingPink, GColorVividCerulean};
GColor demigirl[] = {GColorDarkGray, GColorLightGray, GColorShockingPink, GColorWhite, GColorShockingPink, GColorLightGray, GColorDarkGray};
GColor lesbian[] = {GColorOrange, GColorChromeYellow, GColorWhite, GColorPurpureus, GColorJazzberryJam};
GColor asexual[] = {GColorBlack, GColorLightGray, GColorWhite, GColorPurple};
GColor blank[] = {GColorClear};

GColor *flag_colors[] = {pride, trans, demigirl, lesbian, asexual, blank};
int num_stripes[] = {6, 5, 7, 5, 4, 1};