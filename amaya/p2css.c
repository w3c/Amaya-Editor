/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * p2css.c : file used to analyse a PSchema and translate it to CSS or
 *           modify it on the fly. Complementary to api/genericdriver.c.
 *
 * Author: I. Vatton
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#include "AHTURLTools_f.h"
#include "HTMLimage_f.h"
#include "HTMLstyle_f.h"
#include "html2thot_f.h"
#include "p2css_f.h"

