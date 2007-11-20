#ifndef ARCHIVES_H_
#define ARCHIVES_H_

#include "thot_sys.h"


/**
 * Crate a ZIP archive.
 * \param srcpath Path of source (file or directory).
 * \param dstfile Path of the destination file.
 */
ThotBool TtaCreateZipArchive(const char* srcpath, const char* dstfile);

#endif /*ARCHIVES_H_*/
