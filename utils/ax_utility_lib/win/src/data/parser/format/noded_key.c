#include "noded.h"

axres noded_load_kvp(
	_in noded_sect 		*sect,
	_in const c16 		*kvp_name,
	_out noded_kvp 		**kvp
){
	if (noded_sect_inv(sect)
	|| kvp_name == nullptr){
		return AX_INV_ARG;
	}
	if (kvp == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	u64 sect_range_len = 0;
	c16 *sect_range = nullptr;

	u64 kvp_fmt_len = 0;
	c16 *kvp_fmt = nullptr;

	res = read_range(
		sect->doc->file->map.root,
		sect->beg / sizeof(c16),
		sect->end / sizeof(c16),
		&sect_range_len,
		sect_range
	);
	axcheck(res);

	sect_range = axmalloc(sect_range_len * sizeof(c16));

	res = read_range(
		sect->doc->file->map.root,
		sect->beg / sizeof(c16),
		sect->end / sizeof(c16),
		&sect_range_len,
		sect_range
	);
	axcheck_g(res, cleanup);

	res = join_with(
		kvp_fmt, &kvp_fmt_len, 4, 
		L"\\", kvp_name, L" <{:}+{=}> ", L"\\"
	);
	axcheck_g(res, cleanup);
	kvp_fmt = axmalloc(kvp_fmt_len * sizeof(c16));
	res = join_with(
		kvp_fmt, &kvp_fmt_len, 4, 
		L"\\", kvp_name, L" <{:}+{=}> ", L"\\"
	);
	axcheck_g(res, cleanup);

	seq_loc loc = {0};
	res = seq_find(sect_range, kvp_fmt, &loc);
	axcheck_g(res, cleanup);

	io_str(loc.beg);
	io_str(loc.end);

cleanup:
	axfree(kvp_fmt);
	axfree(sect_range);
	axcheck(res);

	return AX_SUCC;
}

