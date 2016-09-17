/**
* @file stdafx.h
*/
#pragma once

// windows.hは、互換性のために、多くの古いバージョンのヘッダや、滅多に使用されないヘッダまでインクルードする.
// マクロWIN32_LEAN_AND_MEANを定義すると、windows.hにこのようなヘッダをインクルードしないように指示することができる.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"