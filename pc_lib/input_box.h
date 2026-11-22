// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>


#ifndef INPUT_BOX_H
#define INPUT_BOX_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

	/* UTF-8 のプロンプトを表示し、入力された文字列を UTF-8 で buf に格納する */
	void input_box(const char* prompt, char* buf, size_t buf_size);

#ifdef __cplusplus
}
#endif

#endif /* INPUT_BOX_H */