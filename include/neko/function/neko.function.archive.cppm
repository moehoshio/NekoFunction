module;

// ====================
// = Standard Library =
// ====================
#include <memory>
#include <string>
#include <vector>
#include <cstring>
#include <filesystem>

// =====================
// = Archive Support ==
// =====================
#include <minizip-ng/mz.h>
#include <minizip-ng/mz_strm.h>
#include <minizip-ng/mz_zip.h>
#include <minizip-ng/mz_zip_rw.h>

export module neko.function.archive;

import neko.schema;
import neko.function;

#define NEKO_FUNCTION_ENABLE_MODULE true

export {
    #include "archive.hpp"    
}

#include "../../../src/neko/function/archiveZip.cpp"