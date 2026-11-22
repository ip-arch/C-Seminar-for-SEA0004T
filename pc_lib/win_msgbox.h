// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>


#ifndef WIN_MSG_BOX_H
#define WIN_MSG_BOX_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

	/* UTF-8 文字列を メッセージボックスに出力する */
	void message_box(const char* title, const char* text);

#ifdef __cplusplus
}
#endif

#endif /* WIN_MSG_H */