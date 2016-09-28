/* Genxml macros: */

struct i965_address {
   drm_intel_bo *bo;
   uint32_t offset;
   uint32_t read_domains;
   uint32_t write_domain;
};

static inline uint64_t
_i965_combine_address(struct brw_context *brw, void *location,
                      const struct i965_address address, uint32_t delta)
{
   if (address.bo == NULL) {
      return address.offset + delta;
   } else {
      return intel_batchbuffer_reloc64(brw, address.bo,
                                       address.offset,
                                       address.read_domains,
                                       address.write_domain,
                                       delta);
   }
}

#define __gen_address_type struct i965_address
#define __gen_user_data struct brw_context
#define __gen_combine_address _i965_combine_address

#define __i965_cmd_length(cmd) cmd ## _length
#define __i965_cmd_length_bias(cmd) cmd ## _length_bias
#define __i965_cmd_header(cmd) cmd ## _header
#define __i965_cmd_pack(cmd) cmd ## _pack
//#define __i965_reg_num(reg) reg ## _num


#define I965_BATCH_EMIT(cmd, name)                                      \
   for (struct cmd name = { __i965_cmd_header(cmd) },                   \
           *_dst =                                                      \
           intel_batchbuffer_begin(brw,                                 \
                                   __i965_cmd_length(cmd),              \
                                   RENDER_RING);                        \
        __builtin_expect(_dst != NULL, 1);                              \
        ({ __i965_cmd_pack(cmd)(brw, _dst, &name);                      \
           brw->batch.map_next += __i965_cmd_length(cmd);               \
           intel_batchbuffer_advance(brw);                              \
           _dst = NULL;                                                 \
        }))
