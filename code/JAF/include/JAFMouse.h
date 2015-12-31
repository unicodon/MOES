//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>

namespace JAF
{
	/// マウス状態
	struct MouseState {
		static const uint32_t kMouseButtonLeft = 0x1U << 0;
		static const uint32_t kMouseButtonRight = 0x1U << 1;

		// マウスの位置。メインウィンドウに対しての相対値として返す。
		// マウスポインタがウィンドウ上にある場合は[0, 1]の範囲になる
		float x; ///< マウスX位置
		float y; ///< マウスY位置

		/// 現在押されているマウスボタンを、kMouseButton* のビット和で表す
		uint32_t buttons;

		/// MouseStateの他のメンバが有効かどうか。
		/// マウス状態が取得できない場合は isValid が false になる
		bool isValid;
	};

	/// 現在のマウス状態を取得する
	MouseState getMouseState();
}