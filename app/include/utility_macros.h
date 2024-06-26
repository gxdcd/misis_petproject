#pragma once

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x

#define __LINE_STRING__ STRINGIFY(__LINE__)

#define __SOURCE_LOC__ __FILE__ ":" STRINGIFY(__LINE__)