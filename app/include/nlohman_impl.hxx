#pragma once

// https://developercommunity.visualstudio.com/t/C20-Modules---ICE-when-importing-nlohm/10024182
// OUTDATED POSSIBLY CHECK WITH PRAGMA:
// https://github.com/nlohmann/json/issues/3970
// https://github.com/nlohmann/json/pull/3975
// https://github.com/nlohmann/json/issues/3974
#ifndef __NLOHMANN_JSON_IMPL_H_INCLUDED__
#define __NLOHMANN_JSON_IMPL_H_INCLUDED__

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#endif