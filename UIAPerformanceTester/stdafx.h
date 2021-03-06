#pragma once

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <locale>
#include <codecvt>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <memory>
#include <atlbase.h>
#include <atlcomcli.h>
#include <atlsafe.h>
#include <windows.h>
#include <UIAutomationClient.h>
#include <Shobjidl.h>
#include <IsolatedAppLauncher.h>
#define INLINE_TEST_METHOD_MARKUP
#include <WexTestClass.h>
