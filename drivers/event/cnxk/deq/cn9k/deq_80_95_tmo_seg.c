/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2022 Marvell.
 */

#include "cn9k_worker.h"
#include "cnxk_eventdev.h"
#include "cnxk_worker.h"

#if !defined(CNXK_DIS_TMPLT_FUNC)

#define R(name, flags) SSO_DEQ_TMO_SEG(cn9k_sso_hws_deq_tmo_seg_##name, flags)

NIX_RX_FASTPATH_MODES_80_95
#undef R

#endif
