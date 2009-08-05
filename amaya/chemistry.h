#ifndef PROTO_TOKENIDS_CHEMISTRY_H_
#define PROTO_TOKENIDS_CHEMISTRY_H_

#undef THOT_EXPORT
#define THOT_EXPORT extern

#include "amaya.h"
#include "fetchXMLname_f.h"
#include "chemistry.tab.hpp"

#define TKN_TERMINATION 0
#define TKN_UNINITIALIZED 1
#define TKN_DUMMY 2

#define TKN_ATOM 3
#define TKN_OXIDATION_NUMBER 4
#define TKN_INTEGER 5
#define TKN_QUADRUPLE_BOND 6
#define TKN_TRIPLE_BOND 7
#define TKN_DOUBLE_BOND 8
#define TKN_SIMPLE_BOND 9
#define TKN_GENERIC_BOND 10
#define TKN_DOT 11

#endif /* PROTO_TOKENIDS_CHEMISTRY_ */
