#include "noded.h"

void noded_sect_on_clear(
	ax_dict_node 		node _prepass
){
	c16 *name = node->key;
	noded_kvp *kvp = node->value;
	noded_kvp_unload(kvp);

	axfree(name);
	axfree(kvp);
}

// Resolve (read) range of the section
_free c16 *_noded_sect_resolve(
	_in noded_doc		*doc,
	_in seq_loc		*sect_loc
){
	if (sect_loc == nullptr){
		return nullptr;
	}

	axres res = AX_SUCC;

	// Skip initial line
	const c16 *sect_char = sect_loc->end;
	seq_loc next_loc = {0};
	res = seq_find(sect_char, NODED_SECT_FMT, &next_loc);
	if (res == AX_NOT_FND){
		next_loc.beg = doc->file->map.root + doc->file->map.size;
	}else axcheck_r(res, nullptr);

	// If next section/EOF has no offset between
	if (dif_c16(sect_char, next_loc.beg) == 0){
		return axmalloc(1);
	}

	/*
	 	Read range of the section
	*/
	u32 rng_len_n = 0;
	c16 *rng_buf = nullptr;
	res = read_range(
		sect_char,
		0, 
		dif_c16(sect_char, next_loc.beg),
		&rng_len_n, rng_buf
	);
	axcheck_r(res, nullptr);

	rng_buf = axmalloc(rng_len_n * sizeof(c16));

	res = read_range(
		sect_char,
		0, 
		dif_c16(sect_char, next_loc.beg),
		&rng_len_n, rng_buf
	);
	axcheck_r(res, nullptr, axfree(rng_buf));

	return rng_buf;
}
axres noded_sect_load(
	_in noded_doc		*doc,
	_in seq_loc		*sect_loc
){
	if (noded_doc_inv(doc)){
		return AX_INV_DATA;
	}

	axres res = AX_SUCC;

	noded_sect sect = {0};

	// Read sect_name value
	const c16 *name = index_as(sect_loc->seq_vars, c16*, u"sect_name", sizeof(u"sect_name"));
	axcheck_r((name == nullptr), AX_INV_DATA);

	// Read section range (until next section or EOF)
	c16 *rng_buf = _noded_sect_resolve(doc, sect_loc);
	axcheck_r((rng_buf == nullptr), AX_INV_DATA);
	u32 rng_len = _c16len(rng_buf);

	const c16 *rng_char = rng_buf;

	/*
	 	Write to buffer before processing lines 
	*/
	sect.name = _c16dup(name);

	/*
		Initialize kvp dictionary of the section
	*/
	u32 kvp_dict_max = 0; // Maximum count of nodes (if each line is a valid kvp node)
	line_count(rng_buf, &kvp_dict_max);
	res = ax_dict_init(kvp_dict_max + 1, &sect.kvp_dict);
	axcheck(res);

	sect.kvp_dict->overrides.on_clear = (ax_structure_override)noded_sect_on_clear;

	/*
		Process each line as kvp
	*/
	while(in_c16_s(rng_buf, rng_char, rng_len)){
		res = noded_kvp_load(&sect, rng_char);
		axcheck_g(res, error_jump);

		res = skip_line(rng_char, &rng_char);
		axcheck_b(res);
	}
	axcheck(res, sect.kvp_dict->delete(sect.kvp_dict), axfree(rng_buf));

	doc->sect_dict->add(
		doc->sect_dict,
		(void*)name,
		_c16len_b(name) + sizeof(c16),
		&sect,
		sizeof(noded_sect)
	);

	axfree(rng_buf);
	return AX_SUCC;

error_jump:
	axfree(rng_buf);

	return res;
}

axres noded_sect_unload(
	_in noded_sect 		*sect
){
	if(sect == nullptr){
		return AX_INV_ARG;
	}

	if (sect->kvp_dict != nullptr){
		sect->kvp_dict->delete(sect->kvp_dict);
	}
	axfree(sect->name);

	return AX_SUCC;
}

/*
 	Iterator data structure passed when creating converting
	noded_sect into string.
*/
struct noded_sect_c16_iter_data{
	_in noded_sect 			*sect;
	_in const struct noded_kvp_temp	*temp;
	_out u32 			buf_len_n;
	_out _free c16 			*buf;
};
iter_code noded_sect_c16_iter(
	_in ax_dict_iter_stack		stack _prepass
){
	noded_kvp *kvp = (noded_kvp*)stack->node->value;
	struct noded_sect_c16_iter_data *prov = (struct noded_sect_c16_iter_data*)stack->data;
	if (prov == nullptr){
		return ITER_FAIL;
	}

	axres res = AX_SUCC;
	c16 *res_buf = nullptr;
	u32 res_len_n = 0;

	/*
	 	Convert node value to kvp line
	*/
	c16 *kvp_buf = nullptr;
	u32 kvp_len_n = 0;
	res = noded_kvp_c16(kvp, prov->temp, &kvp_len_n, kvp_buf);
	axcheck_g(res, cleanup);

	kvp_buf = axmalloc(kvp_len_n * sizeof(c16));

	res = noded_kvp_c16(kvp, prov->temp, &kvp_len_n, kvp_buf);
	axcheck_g(res, cleanup);

	/*
	 	Set buffer into the provided data
	*/
	if (prov->buf == nullptr){
		res_buf = kvp_buf;
		res_len_n = kvp_len_n;
	}else{
		/*
		 	Concat if provided data buffer is allocated
		*/
		res = c16_cat(prov->buf, kvp_buf, &res_len_n, res_buf);
		axcheck_g(res, cleanup);

		res_buf = axmalloc(res_len_n * sizeof(c16));

		res = c16_cat(prov->buf, kvp_buf, &res_len_n, res_buf);
		axcheck_g(res, cleanup);
	}

cleanup:
	// If res_buf is not the kvp line (prov->buf == nullptr)
	if (res_buf != kvp_buf){
		axfree(kvp_buf);
	}

	axfree(prov->buf);
	prov->buf_len_n = 0;
	if (res_buf == nullptr){
		return ITER_FAIL;
	}

	prov->buf = res_buf;
	prov->buf_len_n = res_len_n;

	return ITER_NONE;
}
axres noded_sect_c16(
	_in noded_sect			*sect,
	_in const struct noded_kvp_temp	*temp,
	_in_out u32			*size,
	_in_out _eval c16		*buf // Evaluate by using (size * sizeof(c16))
){
	if (noded_sect_inv(sect)){
		return AX_INV_DATA;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}

	axres res = AX_SUCC;

	// Count length of the section
	u32 sect_len_n = _c16len(u"[") 
		+ _c16len(sect->name)
		+ _c16len(u"]:\n") + 1;
	
	/*
	 	Iterate over the KVP dictionary and load all lines into buffer.
	*/
	struct noded_sect_c16_iter_data data = (struct noded_sect_c16_iter_data){
		.buf_len_n = 0,
		.buf = nullptr,
		.sect = sect,
		.temp = temp
	};
	res = sect->kvp_dict->iter(
		sect->kvp_dict,
		(ax_iter_act)noded_sect_c16_iter,
		&data,
		nullptr
	);
	axcheck_r(res, AX_INV_DATA);

	// Increment the buffer length by the count of characters gathered
	sect_len_n += data.buf_len_n - 1;
	if (ret_size){
		// Free since we do not want the data on size return
		axfree(data.buf);

		*size = sect_len_n;
		return AX_SUCC;
	}

	axcheck(_ax_buf_err(*size, sect_len_n));

	/*
	 	Create a section section label line
	*/
	c16 *sect_buf = axmalloc(sect_len_n);
	u32 t = sect_len_n - data.buf_len_n + 1; // Length of the label line (null-terminated)

	res = join_with(sect_buf, &t, 3, u"[", sect->name, u"]:\n");
	axcheck(res, axfree(sect_buf), axfree(data.buf));

	// Copy data gathered from the iter into the buffer
	memcpy(buf, sect_buf, (t - 1) * sizeof(c16));
	memcpy(buf + (t - 1), data.buf, (data.buf_len_n - 1) * sizeof(c16));

	// Cleanup
	axfree(sect_buf);
	axfree(data.buf);

	return AX_SUCC;
}

