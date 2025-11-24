module;

// ====================
// = Standard Library =
// ====================

#if defined(__cpp_lib_modules) && (__cpp_lib_modules >= 202207L)
import std;
#else
#include <memory>
#include <string>
#include <vector>
#endif

// =====================
// = Archive Support ==
// =====================
#include <minizip-ng/mz.h>
#include <minizip-ng/mz_strm.h>
#include <minizip-ng/mz_zip.h>
#include <minizip-ng/mz_zip_rw.h>

import neko.schema;
import neko.function;

export module neko.function.archive;

#define NEKO_FUNCTION_ENABLE_MODULE true

export {
    #include "archive.hpp"    
}

#include "../../../src/neko/function/archiveZip.cpp"