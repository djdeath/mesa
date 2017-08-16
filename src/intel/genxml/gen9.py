import gen

cs = gen.GenCS('SKL')

cs.enum('3D_Prim_Topo_Type',
    {
        'POINTLIST': 1,
        'LINELIST': 2,
        'LINESTRIP': 3,
        'TRILIST': 4,
        'TRISTRIP': 5,
        'TRIFAN': 6,
        'QUADLIST': 7,
        'QUADSTRIP': 8,
        'LINELIST_ADJ': 9,
        'LINESTRIP_ADJ': 10,
        'TRILIST_ADJ': 11,
        'TRISTRIP_ADJ': 12,
        'TRISTRIP_REVERSE': 13,
        'POLYGON': 14,
        'RECTLIST': 15,
        'LINELOOP': 16,
        'POINTLIST _BF': 17,
        'LINESTRIP_CONT': 18,
        'LINESTRIP_BF': 19,
        'LINESTRIP_CONT_BF': 20,
        'TRIFAN_NOSTIPPLE': 22,
        'PATCHLIST_1': 32,
        'PATCHLIST_2': 33,
        'PATCHLIST_3': 34,
        'PATCHLIST_4': 35,
        'PATCHLIST_5': 36,
        'PATCHLIST_6': 37,
        'PATCHLIST_7': 38,
        'PATCHLIST_8': 39,
        'PATCHLIST_9': 40,
        'PATCHLIST_10': 41,
        'PATCHLIST_11': 42,
        'PATCHLIST_12': 43,
        'PATCHLIST_13': 44,
        'PATCHLIST_14': 45,
        'PATCHLIST_15': 46,
        'PATCHLIST_16': 47,
        'PATCHLIST_17': 48,
        'PATCHLIST_18': 49,
        'PATCHLIST_19': 50,
        'PATCHLIST_20': 51,
        'PATCHLIST_21': 52,
        'PATCHLIST_22': 53,
        'PATCHLIST_23': 54,
        'PATCHLIST_24': 55,
        'PATCHLIST_25': 56,
        'PATCHLIST_26': 57,
        'PATCHLIST_27': 58,
        'PATCHLIST_28': 59,
        'PATCHLIST_29': 60,
        'PATCHLIST_30': 61,
        'PATCHLIST_31': 62,
        'PATCHLIST_32': 63,
    })
cs.enum('3D_Vertex_Component_Control',
    {
        'NOSTORE': 0,
        'STORE_SRC': 1,
        'STORE_0': 2,
        'STORE_1_FP': 3,
        'STORE_1_INT': 4,
        'STORE_PID': 7,
    })
cs.enum('COMPONENT_ENABLES',
    {
        'NONE': 0,
        'X': 1,
        'Y': 2,
        'XY': 3,
        'Z': 4,
        'XZ': 5,
        'YZ': 6,
        'XYZ': 7,
        'W': 8,
        'XW': 9,
        'YW': 10,
        'XYW': 11,
        'ZW': 12,
        'XZW': 13,
        'YZW': 14,
        'XYZW': 15,
    })
cs.enum('Attribute_Component_Format',
    {
        'disabled': 0,
        '.xy': 1,
        '.xyz': 2,
        '.xyzw': 3,
    })
cs.enum('WRAP_SHORTEST_ENABLE',
    {
        'X': 1,
        'Y': 2,
        'XY': 3,
        'Z': 4,
        'XZ': 5,
        'YZ': 6,
        'XYZ': 7,
        'W': 8,
        'XW': 9,
        'YW': 10,
        'XYW': 11,
        'ZW': 12,
        'XZW': 13,
        'YZW': 14,
        'XYZW': 15,
    })
cs.enum('3D_Stencil_Operation',
    {
        'KEEP': 0,
        'ZERO': 1,
        'REPLACE': 2,
        'INCRSAT': 3,
        'DECRSAT': 4,
        'INCR': 5,
        'DECR': 6,
        'INVERT': 7,
    })
cs.enum('3D_Color_Buffer_Blend_Factor',
    {
        'ONE': 1,
        'SRC_COLOR': 2,
        'SRC_ALPHA': 3,
        'DST_ALPHA': 4,
        'DST_COLOR': 5,
        'SRC_ALPHA_SATURATE': 6,
        'CONST_COLOR': 7,
        'CONST_ALPHA': 8,
        'SRC1_COLOR': 9,
        'SRC1_ALPHA': 10,
        'ZERO': 17,
        'INV_SRC_COLOR': 18,
        'INV_SRC_ALPHA': 19,
        'INV_DST_ALPHA': 20,
        'INV_DST_COLOR': 21,
        'INV_CONST_COLOR': 23,
        'INV_CONST_ALPHA': 24,
        'INV_SRC1_COLOR': 25,
        'INV_SRC1_ALPHA': 26,
    })
cs.enum('3D_Color_Buffer_Blend_Function',
    {
        'ADD': 0,
        'SUBTRACT': 1,
        'REVERSE_SUBTRACT': 2,
        'MIN': 3,
        'MAX': 4,
    })
cs.enum('3D_Compare_Function',
    {
        'ALWAYS': 0,
        'NEVER': 1,
        'LESS': 2,
        'EQUAL': 3,
        'LEQUAL': 4,
        'GREATER': 5,
        'NOTEQUAL': 6,
        'GEQUAL': 7,
    })
cs.enum('3D_Logic_Op_Function',
    {
        'CLEAR': 0,
        'NOR': 1,
        'AND_INVERTED': 2,
        'COPY_INVERTED': 3,
        'AND_REVERSE': 4,
        'INVERT': 5,
        'XOR': 6,
        'NAND': 7,
        'AND': 8,
        'EQUIV': 9,
        'NOOP': 10,
        'OR_INVERTED': 11,
        'COPY': 12,
        'OR_REVERSE': 13,
        'OR': 14,
        'SET': 15,
    })
cs.enum('SURFACE_FORMAT',
    {
        'R32G32B32A32_FLOAT': 0,
        'R32G32B32A32_SINT': 1,
        'R32G32B32A32_UINT': 2,
        'R32G32B32A32_UNORM': 3,
        'R32G32B32A32_SNORM': 4,
        'R64G64_FLOAT': 5,
        'R32G32B32X32_FLOAT': 6,
        'R32G32B32A32_SSCALED': 7,
        'R32G32B32A32_USCALED': 8,
        'R32G32B32A32_SFIXED': 32,
        'R64G64_PASSTHRU': 33,
        'R32G32B32_FLOAT': 64,
        'R32G32B32_SINT': 65,
        'R32G32B32_UINT': 66,
        'R32G32B32_UNORM': 67,
        'R32G32B32_SNORM': 68,
        'R32G32B32_SSCALED': 69,
        'R32G32B32_USCALED': 70,
        'R32G32B32_SFIXED': 80,
        'R16G16B16A16_UNORM': 128,
        'R16G16B16A16_SNORM': 129,
        'R16G16B16A16_SINT': 130,
        'R16G16B16A16_UINT': 131,
        'R16G16B16A16_FLOAT': 132,
        'R32G32_FLOAT': 133,
        'R32G32_SINT': 134,
        'R32G32_UINT': 135,
        'R32_FLOAT_X8X24_TYPELESS': 136,
        'X32_TYPELESS_G8X24_UINT': 137,
        'L32A32_FLOAT': 138,
        'R32G32_UNORM': 139,
        'R32G32_SNORM': 140,
        'R64_FLOAT': 141,
        'R16G16B16X16_UNORM': 142,
        'R16G16B16X16_FLOAT': 143,
        'A32X32_FLOAT': 144,
        'L32X32_FLOAT': 145,
        'I32X32_FLOAT': 146,
        'R16G16B16A16_SSCALED': 147,
        'R16G16B16A16_USCALED': 148,
        'R32G32_SSCALED': 149,
        'R32G32_USCALED': 150,
        'R32G32_SFIXED': 160,
        'R64_PASSTHRU': 161,
        'B8G8R8A8_UNORM': 192,
        'B8G8R8A8_UNORM_SRGB': 193,
        'R10G10B10A2_UNORM': 194,
        'R10G10B10A2_UNORM_SRGB': 195,
        'R10G10B10A2_UINT': 196,
        'R10G10B10_SNORM_A2_UNORM': 197,
        'R8G8B8A8_UNORM': 199,
        'R8G8B8A8_UNORM_SRGB': 200,
        'R8G8B8A8_SNORM': 201,
        'R8G8B8A8_SINT': 202,
        'R8G8B8A8_UINT': 203,
        'R16G16_UNORM': 204,
        'R16G16_SNORM': 205,
        'R16G16_SINT': 206,
        'R16G16_UINT': 207,
        'R16G16_FLOAT': 208,
        'B10G10R10A2_UNORM': 209,
        'B10G10R10A2_UNORM_SRGB': 210,
        'R11G11B10_FLOAT': 211,
        'R32_SINT': 214,
        'R32_UINT': 215,
        'R32_FLOAT': 216,
        'R24_UNORM_X8_TYPELESS': 217,
        'X24_TYPELESS_G8_UINT': 218,
        'L32_UNORM': 221,
        'A32_UNORM': 222,
        'L16A16_UNORM': 223,
        'I24X8_UNORM': 224,
        'L24X8_UNORM': 225,
        'A24X8_UNORM': 226,
        'I32_FLOAT': 227,
        'L32_FLOAT': 228,
        'A32_FLOAT': 229,
        'X8B8_UNORM_G8R8_SNORM': 230,
        'A8X8_UNORM_G8R8_SNORM': 231,
        'B8X8_UNORM_G8R8_SNORM': 232,
        'B8G8R8X8_UNORM': 233,
        'B8G8R8X8_UNORM_SRGB': 234,
        'R8G8B8X8_UNORM': 235,
        'R8G8B8X8_UNORM_SRGB': 236,
        'R9G9B9E5_SHAREDEXP': 237,
        'B10G10R10X2_UNORM': 238,
        'L16A16_FLOAT': 240,
        'R32_UNORM': 241,
        'R32_SNORM': 242,
        'R10G10B10X2_USCALED': 243,
        'R8G8B8A8_SSCALED': 244,
        'R8G8B8A8_USCALED': 245,
        'R16G16_SSCALED': 246,
        'R16G16_USCALED': 247,
        'R32_SSCALED': 248,
        'R32_USCALED': 249,
        'B5G6R5_UNORM': 256,
        'B5G6R5_UNORM_SRGB': 257,
        'B5G5R5A1_UNORM': 258,
        'B5G5R5A1_UNORM_SRGB': 259,
        'B4G4R4A4_UNORM': 260,
        'B4G4R4A4_UNORM_SRGB': 261,
        'R8G8_UNORM': 262,
        'R8G8_SNORM': 263,
        'R8G8_SINT': 264,
        'R8G8_UINT': 265,
        'R16_UNORM': 266,
        'R16_SNORM': 267,
        'R16_SINT': 268,
        'R16_UINT': 269,
        'R16_FLOAT': 270,
        'A8P8_UNORM_PALETTE0': 271,
        'A8P8_UNORM_PALETTE1': 272,
        'I16_UNORM': 273,
        'L16_UNORM': 274,
        'A16_UNORM': 275,
        'L8A8_UNORM': 276,
        'I16_FLOAT': 277,
        'L16_FLOAT': 278,
        'A16_FLOAT': 279,
        'L8A8_UNORM_SRGB': 280,
        'R5G5_SNORM_B6_UNORM': 281,
        'B5G5R5X1_UNORM': 282,
        'B5G5R5X1_UNORM_SRGB': 283,
        'R8G8_SSCALED': 284,
        'R8G8_USCALED': 285,
        'R16_SSCALED': 286,
        'R16_USCALED': 287,
        'P8A8_UNORM_PALETTE0': 290,
        'P8A8_UNORM_PALETTE1': 291,
        'A1B5G5R5_UNORM': 292,
        'A4B4G4R4_UNORM': 293,
        'L8A8_UINT': 294,
        'L8A8_SINT': 295,
        'R8_UNORM': 320,
        'R8_SNORM': 321,
        'R8_SINT': 322,
        'R8_UINT': 323,
        'A8_UNORM': 324,
        'I8_UNORM': 325,
        'L8_UNORM': 326,
        'P4A4_UNORM_PALETTE0': 327,
        'A4P4_UNORM_PALETTE0': 328,
        'R8_SSCALED': 329,
        'R8_USCALED': 330,
        'P8_UNORM_PALETTE0': 331,
        'L8_UNORM_SRGB': 332,
        'P8_UNORM_PALETTE1': 333,
        'P4A4_UNORM_PALETTE1': 334,
        'A4P4_UNORM_PALETTE1': 335,
        'Y8_UNORM': 336,
        'L8_UINT': 338,
        'L8_SINT': 339,
        'I8_UINT': 340,
        'I8_SINT': 341,
        'DXT1_RGB_SRGB': 384,
        'R1_UNORM': 385,
        'YCRCB_NORMAL': 386,
        'YCRCB_SWAPUVY': 387,
        'P2_UNORM_PALETTE0': 388,
        'P2_UNORM_PALETTE1': 389,
        'BC1_UNORM': 390,
        'BC2_UNORM': 391,
        'BC3_UNORM': 392,
        'BC4_UNORM': 393,
        'BC5_UNORM': 394,
        'BC1_UNORM_SRGB': 395,
        'BC2_UNORM_SRGB': 396,
        'BC3_UNORM_SRGB': 397,
        'MONO8': 398,
        'YCRCB_SWAPUV': 399,
        'YCRCB_SWAPY': 400,
        'DXT1_RGB': 401,
        'FXT1': 402,
        'R8G8B8_UNORM': 403,
        'R8G8B8_SNORM': 404,
        'R8G8B8_SSCALED': 405,
        'R8G8B8_USCALED': 406,
        'R64G64B64A64_FLOAT': 407,
        'R64G64B64_FLOAT': 408,
        'BC4_SNORM': 409,
        'BC5_SNORM': 410,
        'R16G16B16_FLOAT': 411,
        'R16G16B16_UNORM': 412,
        'R16G16B16_SNORM': 413,
        'R16G16B16_SSCALED': 414,
        'R16G16B16_USCALED': 415,
        'BC6H_SF16': 417,
        'BC7_UNORM': 418,
        'BC7_UNORM_SRGB': 419,
        'BC6H_UF16': 420,
        'PLANAR_420_8': 421,
        'R8G8B8_UNORM_SRGB': 424,
        'ETC1_RGB8': 425,
        'ETC2_RGB8': 426,
        'EAC_R11': 427,
        'EAC_RG11': 428,
        'EAC_SIGNED_R11': 429,
        'EAC_SIGNED_RG11': 430,
        'ETC2_SRGB8': 431,
        'R16G16B16_UINT': 432,
        'R16G16B16_SINT': 433,
        'R32_SFIXED': 434,
        'R10G10B10A2_SNORM': 435,
        'R10G10B10A2_USCALED': 436,
        'R10G10B10A2_SSCALED': 437,
        'R10G10B10A2_SINT': 438,
        'B10G10R10A2_SNORM': 439,
        'B10G10R10A2_USCALED': 440,
        'B10G10R10A2_SSCALED': 441,
        'B10G10R10A2_UINT': 442,
        'B10G10R10A2_SINT': 443,
        'R64G64B64A64_PASSTHRU': 444,
        'R64G64B64_PASSTHRU': 445,
        'ETC2_RGB8_PTA': 448,
        'ETC2_SRGB8_PTA': 449,
        'ETC2_EAC_RGBA8': 450,
        'ETC2_EAC_SRGB8_A8': 451,
        'R8G8B8_UINT': 456,
        'R8G8B8_SINT': 457,
        'RAW': 511,
    })
cs.enum('Shader Channel Select',
    {
        'ZERO': 0,
        'ONE': 1,
        'RED': 4,
        'GREEN': 5,
        'BLUE': 6,
        'ALPHA': 7,
    })
cs.enum('Texture Coordinate Mode',
    {
        'WRAP': 0,
        'MIRROR': 1,
        'CLAMP': 2,
        'CUBE': 3,
        'CLAMP_BORDER': 4,
        'MIRROR_ONCE': 5,
        'HALF_BORDER': 6,
    })

cs.struct(name='3DSTATE_CONSTANT_BODY', length=10, bias=0) \
    .add(gen.FixedGroup(start=0, count=4, size=16) \
        .add(gen.Field(name='Read Length', start=0, end=15, gen_type=gen.UInteger(16)))) \
    .add(gen.FixedGroup(start=64, count=4, size=64) \
        .add(gen.Field(name='Buffer', start=5, end=63, gen_type=gen.Address())))
cs.struct(name='BINDING_TABLE_EDIT_ENTRY', length=1, bias=0) \
    .add(gen.Field(name='Surface State Pointer', start=0, end=15, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Binding Table Index', start=16, end=23, gen_type=gen.UInteger(8)))
cs.struct(name='GATHER_CONSTANT_ENTRY', length=1, bias=0) \
    .add(gen.Field(name='Binding Table Index Offset', start=0, end=3, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Channel Mask', start=4, end=7, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Constant Buffer Offset', start=8, end=15, gen_type=gen.OffsetFrom()))
cs.struct(name='MEMORY_OBJECT_CONTROL_STATE', length=1, bias=0) \
    .add(gen.Field(name='Index to MOCS Tables', start=1, end=6, gen_type=gen.UInteger(6)))
cs.struct(name='VERTEX_BUFFER_STATE', length=4, bias=0) \
    .add(gen.Field(name='Buffer Pitch', start=0, end=11, gen_type=gen.UInteger(12))) \
    .add(gen.Field(name='Null Vertex Buffer', start=13, end=13, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Address Modify Enable', start=14, end=14, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Memory Object Control State', start=16, end=22, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Vertex Buffer MOCS', start=16, end=22, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Vertex Buffer Index', start=26, end=31, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Buffer Starting Address', start=32, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Buffer Size', start=96, end=127, gen_type=gen.UInteger(32)))
cs.struct(name='VERTEX_ELEMENT_STATE', length=2, bias=0) \
    .add(gen.Field(name='Source Element Offset', start=0, end=11, gen_type=gen.UInteger(12))) \
    .add(gen.Field(name='Edge Flag Enable', start=15, end=15, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Source Element Format', start=16, end=24, gen_type=cs.type('SURFACE_FORMAT'))) \
    .add(gen.Field(name='Valid', start=25, end=25, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Vertex Buffer Index', start=26, end=31, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Component 3 Control', start=48, end=50, gen_type=cs.type('3D_Vertex_Component_Control'))) \
    .add(gen.Field(name='Component 2 Control', start=52, end=54, gen_type=cs.type('3D_Vertex_Component_Control'))) \
    .add(gen.Field(name='Component 1 Control', start=56, end=58, gen_type=cs.type('3D_Vertex_Component_Control'))) \
    .add(gen.Field(name='Component 0 Control', start=60, end=62, gen_type=cs.type('3D_Vertex_Component_Control')))
cs.struct(name='SO_DECL', length=1, bias=0) \
    .add(gen.Field(name='Component Mask', default=0, start=0, end=3, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Register Index', start=4, end=9, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Hole Flag', start=11, end=11, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Output Buffer Slot', start=12, end=13, gen_type=gen.UInteger(2)))
cs.struct(name='SO_DECL_ENTRY', length=2, bias=0) \
    .add(gen.Field(name='Stream 0 Decl', start=0, end=15, gen_type=cs.type('SO_DECL'))) \
    .add(gen.Field(name='Stream 1 Decl', start=16, end=31, gen_type=cs.type('SO_DECL'))) \
    .add(gen.Field(name='Stream 2 Decl', start=32, end=47, gen_type=cs.type('SO_DECL'))) \
    .add(gen.Field(name='Stream 3 Decl', start=48, end=63, gen_type=cs.type('SO_DECL')))
cs.struct(name='SF_OUTPUT_ATTRIBUTE_DETAIL', length=1, bias=0) \
    .add(gen.Field(name='Source Attribute', start=0, end=4, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Swizzle Select', start=6, end=7, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Constant Source', start=9, end=10, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Swizzle Control Mode', start=11, end=11, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Component Override X', start=12, end=12, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Component Override Y', start=13, end=13, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Component Override Z', start=14, end=14, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Component Override W', start=15, end=15, gen_type=gen.Boolean()))
cs.struct(name='SCISSOR_RECT', length=2, bias=0) \
    .add(gen.Field(name='Scissor Rectangle X Min', start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Scissor Rectangle Y Min', start=16, end=31, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Scissor Rectangle X Max', start=32, end=47, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Scissor Rectangle Y Max', start=48, end=63, gen_type=gen.UInteger(16)))
cs.struct(name='SF_CLIP_VIEWPORT', length=16, bias=0) \
    .add(gen.Field(name='Viewport Matrix Element m00', start=0, end=31, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Viewport Matrix Element m11', start=32, end=63, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Viewport Matrix Element m22', start=64, end=95, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Viewport Matrix Element m30', start=96, end=127, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Viewport Matrix Element m31', start=128, end=159, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Viewport Matrix Element m32', start=160, end=191, gen_type=gen.Float(32))) \
    .add(gen.Field(name='X Min Clip Guardband', start=256, end=287, gen_type=gen.Float(32))) \
    .add(gen.Field(name='X Max Clip Guardband', start=288, end=319, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Y Min Clip Guardband', start=320, end=351, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Y Max Clip Guardband', start=352, end=383, gen_type=gen.Float(32))) \
    .add(gen.Field(name='X Min ViewPort', start=384, end=415, gen_type=gen.Float(32))) \
    .add(gen.Field(name='X Max ViewPort', start=416, end=447, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Y Min ViewPort', start=448, end=479, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Y Max ViewPort', start=480, end=511, gen_type=gen.Float(32)))
cs.struct(name='BLEND_STATE_ENTRY', length=2, bias=0) \
    .add(gen.Field(name='Write Disable Blue', start=0, end=0, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Write Disable Green', start=1, end=1, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Write Disable Red', start=2, end=2, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Write Disable Alpha', start=3, end=3, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Alpha Blend Function', start=5, end=7, gen_type=cs.type('3D_Color_Buffer_Blend_Function'))) \
    .add(gen.Field(name='Destination Alpha Blend Factor', start=8, end=12, gen_type=cs.type('3D_Color_Buffer_Blend_Factor'))) \
    .add(gen.Field(name='Source Alpha Blend Factor', start=13, end=17, gen_type=cs.type('3D_Color_Buffer_Blend_Factor'))) \
    .add(gen.Field(name='Color Blend Function', start=18, end=20, gen_type=cs.type('3D_Color_Buffer_Blend_Function'))) \
    .add(gen.Field(name='Destination Blend Factor', start=21, end=25, gen_type=cs.type('3D_Color_Buffer_Blend_Factor'))) \
    .add(gen.Field(name='Source Blend Factor', start=26, end=30, gen_type=cs.type('3D_Color_Buffer_Blend_Factor'))) \
    .add(gen.Field(name='Color Buffer Blend Enable', start=31, end=31, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Post-Blend Color Clamp Enable', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pre-Blend Color Clamp Enable', start=33, end=33, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Color Clamp Range', start=34, end=35, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Pre-Blend Source Only Clamp Enable', start=36, end=36, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Logic Op Function', start=59, end=62, gen_type=cs.type('3D_Logic_Op_Function'))) \
    .add(gen.Field(name='Logic Op Enable', start=63, end=63, gen_type=gen.Boolean()))
cs.struct(name='BLEND_STATE', length=1, bias=0) \
    .add(gen.Field(name='Y Dither Offset', start=19, end=20, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='X Dither Offset', start=21, end=22, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Color Dither Enable', start=23, end=23, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Alpha Test Function', start=24, end=26, gen_type=cs.type('3D_Compare_Function'))) \
    .add(gen.Field(name='Alpha Test Enable', start=27, end=27, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Alpha To Coverage Dither Enable', start=28, end=28, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Alpha To One Enable', start=29, end=29, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Independent Alpha Blend Enable', start=30, end=30, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Alpha To Coverage Enable', start=31, end=31, gen_type=gen.Boolean())) \
    .add(gen.VariableGroup(start=32, size=64) \
        .add(gen.Field(name='Entry', start=0, end=63, gen_type=cs.type('BLEND_STATE_ENTRY'))))
cs.struct(name='CC_VIEWPORT', length=2, bias=0) \
    .add(gen.Field(name='Minimum Depth', start=0, end=31, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Maximum Depth', start=32, end=63, gen_type=gen.Float(32)))
cs.struct(name='COLOR_CALC_STATE', length=6, bias=0) \
    .add(gen.Field(name='Alpha Test Format', start=0, end=0, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Round Disable Function Disable', start=15, end=15, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Alpha Reference Value As UNORM8', start=32, end=63, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Alpha Reference Value As FLOAT32', start=32, end=63, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Blend Constant Color Red', start=64, end=95, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Blend Constant Color Green', start=96, end=127, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Blend Constant Color Blue', start=128, end=159, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Blend Constant Color Alpha', start=160, end=191, gen_type=gen.Float(32)))
cs.struct(name='EXECUTION_UNIT_EXTENDED_MESSAGE_DESCRIPTOR', length=1, bias=0) \
    .add(gen.Field(name='Target Function ID', start=0, end=3, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='End Of Thread', start=5, end=5, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Extended Message Length', start=6, end=9, gen_type=gen.UInteger(4)))
cs.struct(name='INTERFACE_DESCRIPTOR_DATA', length=8, bias=0) \
    .add(gen.Field(name='Kernel Start Pointer', start=6, end=47, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Software Exception Enable', start=71, end=71, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Mask Stack Exception Enable', start=75, end=75, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Illegal Opcode Exception Enable', start=77, end=77, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Floating Point Mode', start=80, end=80, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Thread Priority', start=81, end=81, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Single Program Flow', start=82, end=82, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Denorm Mode', start=83, end=83, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Sampler Count', start=98, end=100, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Sampler State Pointer', start=101, end=127, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Binding Table Entry Count', start=128, end=132, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Binding Table Pointer', start=133, end=143, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Constant URB Entry Read Offset', start=160, end=175, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Constant URB Entry Read Length', start=176, end=191, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Number of Threads in GPGPU Thread Group', start=192, end=201, gen_type=gen.UInteger(10))) \
    .add(gen.Field(name='Global Barrier Enable', start=207, end=207, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Shared Local Memory Size', start=208, end=212, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Barrier Enable', start=213, end=213, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Rounding Mode', start=214, end=215, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Cross-Thread Constant Data Read Length', start=224, end=231, gen_type=gen.UInteger(8)))
cs.struct(name='ROUNDINGPRECISIONTABLE_3_BITS', length=1, bias=0) \
    .add(gen.Field(name='Rounding Precision', start=0, end=2, gen_type=gen.UInteger(3)))
cs.struct(name='PALETTE_ENTRY', length=1, bias=0) \
    .add(gen.Field(name='Blue', start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Green', start=8, end=15, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Red', start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Alpha', start=24, end=31, gen_type=gen.UInteger(8)))
cs.struct(name='BINDING_TABLE_STATE', length=1, bias=0) \
    .add(gen.Field(name='Surface State Pointer', start=6, end=31, gen_type=gen.OffsetFrom()))
cs.struct(name='RENDER_SURFACE_STATE', length=16, bias=0) \
    .add(gen.Field(name='Cube Face Enable - Positive Z', start=0, end=0, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Cube Face Enable - Negative Z', start=1, end=1, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Cube Face Enable - Positive Y', start=2, end=2, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Cube Face Enable - Negative Y', start=3, end=3, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Cube Face Enable - Positive X', start=4, end=4, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Cube Face Enable - Negative X', start=5, end=5, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Media Boundary Pixel Mode', start=6, end=7, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Render Cache Read Write Mode', start=8, end=8, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Sampler L2 Bypass Mode Disable', start=9, end=9, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Vertical Line Stride Offset', start=10, end=10, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Vertical Line Stride', start=11, end=11, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Tile Mode', start=12, end=13, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Surface Horizontal Alignment', start=14, end=15, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Surface Vertical Alignment', start=16, end=17, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Surface Format', start=18, end=27, gen_type=cs.type('SURFACE_FORMAT'))) \
    .add(gen.Field(name='Surface Array', start=28, end=28, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Surface Type', start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Surface QPitch', start=32, end=46, gen_type=gen.UInteger(15))) \
    .add(gen.Field(name='Base Mip Level', start=51, end=55, gen_type=gen.UFixed(4, 1))) \
    .add(gen.Field(name='Memory Object Control State', start=56, end=62, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='MOCS', start=56, end=62, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Width', start=64, end=77, gen_type=gen.UInteger(14))) \
    .add(gen.Field(name='Height', start=80, end=93, gen_type=gen.UInteger(14))) \
    .add(gen.Field(name='Surface Pitch', start=96, end=113, gen_type=gen.UInteger(18))) \
    .add(gen.Field(name='Depth', start=117, end=127, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Multisample Position Palette Index', start=128, end=130, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Number of Multisamples', start=131, end=133, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Multisampled Surface Storage Format', start=134, end=134, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Render Target View Extent', start=135, end=145, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Minimum Array Element', start=146, end=156, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Render Target And Sample Unorm Rotation', start=157, end=158, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='MIP Count / LOD', start=160, end=163, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Surface Min LOD', start=164, end=167, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Mip Tail Start LOD', start=168, end=171, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Coherency Type', start=174, end=174, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Tiled Resource Mode', start=178, end=179, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='EWA Disable For Cube', start=180, end=180, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Y Offset', start=181, end=183, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='X Offset', start=185, end=191, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Auxiliary Surface Mode', start=192, end=194, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Y Offset for U or UV Plane', start=192, end=205, gen_type=gen.UInteger(14))) \
    .add(gen.Field(name='Auxiliary Surface Pitch', start=195, end=203, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Auxiliary Surface QPitch', start=208, end=222, gen_type=gen.UInteger(15))) \
    .add(gen.Field(name='X Offset for U or UV Plane', start=208, end=221, gen_type=gen.UInteger(14))) \
    .add(gen.Field(name='Separate UV Plane Enable', start=223, end=223, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Resource Min LOD', start=224, end=235, gen_type=gen.UFixed(4, 8))) \
    .add(gen.Field(name='Shader Channel Select Alpha', start=240, end=242, gen_type=cs.type('Shader Channel Select'))) \
    .add(gen.Field(name='Shader Channel Select Blue', start=243, end=245, gen_type=cs.type('Shader Channel Select'))) \
    .add(gen.Field(name='Shader Channel Select Green', start=246, end=248, gen_type=cs.type('Shader Channel Select'))) \
    .add(gen.Field(name='Shader Channel Select Red', start=249, end=251, gen_type=cs.type('Shader Channel Select'))) \
    .add(gen.Field(name='Memory Compression Enable', start=254, end=254, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Memory Compression Mode', start=255, end=255, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Surface Base Address', start=256, end=319, gen_type=gen.Address())) \
    .add(gen.Field(name='Quilt Width', start=320, end=324, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Quilt Height', start=325, end=329, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Auxiliary Surface Base Address', start=332, end=383, gen_type=gen.Address())) \
    .add(gen.Field(name='Auxiliary Table Index for Media Compressed Surface', start=341, end=351, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Y Offset for V Plane', start=352, end=365, gen_type=gen.UInteger(14))) \
    .add(gen.Field(name='X Offset for V Plane', start=368, end=381, gen_type=gen.UInteger(14))) \
    .add(gen.Field(name='Hierarchical Depth Clear Value', start=384, end=415, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Red Clear Color', start=384, end=415, gen_type=gen.Integer(32))) \
    .add(gen.Field(name='Green Clear Color', start=416, end=447, gen_type=gen.Integer(32))) \
    .add(gen.Field(name='Blue Clear Color', start=448, end=479, gen_type=gen.Integer(32))) \
    .add(gen.Field(name='Alpha Clear Color', start=480, end=511, gen_type=gen.Integer(32)))
cs.struct(name='FILTER_COEFFICIENT', length=1, bias=0) \
    .add(gen.Field(name='Filter Coefficient', start=0, end=7, gen_type=gen.SFixed(1, 6)))
cs.struct(name='SAMPLER_BORDER_COLOR_STATE', length=4, bias=0) \
    .add(gen.Field(name='Border Color Float Red', start=0, end=31, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Border Color 32bit Red', start=0, end=31, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Border Color Float Green', start=32, end=63, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Border Color 32bit Green', start=32, end=63, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Border Color Float Blue', start=64, end=95, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Border Color 32bit Blue', start=64, end=95, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Border Color Float Alpha', start=96, end=127, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Border Color 32bit Alpha', start=96, end=127, gen_type=gen.UInteger(32)))
cs.struct(name='SAMPLER_STATE', length=4, bias=0) \
    .add(gen.Field(name='Anisotropic Algorithm', start=0, end=0, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Texture LOD Bias', start=1, end=13, gen_type=gen.SFixed(4, 8))) \
    .add(gen.Field(name='Min Mode Filter', start=14, end=16, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Mag Mode Filter', start=17, end=19, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Mip Mode Filter', start=20, end=21, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Coarse LOD Quality Mode', start=22, end=26, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='LOD PreClamp Mode', start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Texture Border Color Mode', start=29, end=29, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Sampler Disable', start=31, end=31, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Cube Surface Control Mode', start=32, end=32, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Shadow Function', start=33, end=35, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='ChromaKey Mode', start=36, end=36, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='ChromaKey Index', start=37, end=38, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='ChromaKey Enable', start=39, end=39, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Max LOD', start=40, end=51, gen_type=gen.UFixed(4, 8))) \
    .add(gen.Field(name='Min LOD', start=52, end=63, gen_type=gen.UFixed(4, 8))) \
    .add(gen.Field(name='LOD Clamp Magnification Mode', start=64, end=64, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Border Color Pointer', start=70, end=87, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='TCZ Address Control Mode', start=96, end=98, gen_type=cs.type('Texture Coordinate Mode'))) \
    .add(gen.Field(name='TCY Address Control Mode', start=99, end=101, gen_type=cs.type('Texture Coordinate Mode'))) \
    .add(gen.Field(name='TCX Address Control Mode', start=102, end=104, gen_type=cs.type('Texture Coordinate Mode'))) \
    .add(gen.Field(name='Reduction Type Enable', start=105, end=105, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Non-normalized Coordinate Enable', start=106, end=106, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Trilinear Filter Quality', start=107, end=108, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='R Address Min Filter Rounding Enable', start=109, end=109, gen_type=gen.Boolean())) \
    .add(gen.Field(name='R Address Mag Filter Rounding Enable', start=110, end=110, gen_type=gen.Boolean())) \
    .add(gen.Field(name='V Address Min Filter Rounding Enable', start=111, end=111, gen_type=gen.Boolean())) \
    .add(gen.Field(name='V Address Mag Filter Rounding Enable', start=112, end=112, gen_type=gen.Boolean())) \
    .add(gen.Field(name='U Address Min Filter Rounding Enable', start=113, end=113, gen_type=gen.Boolean())) \
    .add(gen.Field(name='U Address Mag Filter Rounding Enable', start=114, end=114, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Maximum Anisotropy', start=115, end=117, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Reduction Type', start=118, end=119, gen_type=gen.UInteger(2)))
cs.struct(name='SAMPLER_STATE_8X8_AVS_COEFFICIENTS', length=8, bias=0) \
    .add(gen.Field(name='Table 0X Filter Coefficient[n,0]', start=0, end=7, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0Y Filter Coefficient[n,0]', start=8, end=15, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0X Filter Coefficient[n,1]', start=16, end=23, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0Y Filter Coefficient[n,1]', start=24, end=31, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0X Filter Coefficient[n,2]', start=32, end=39, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0Y Filter Coefficient[n,2]', start=40, end=47, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0X Filter Coefficient[n,3]', start=48, end=55, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0Y Filter Coefficient[n,3]', start=56, end=63, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0X Filter Coefficient[n,4]', start=64, end=71, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0Y Filter Coefficient[n,4]', start=72, end=79, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0X Filter Coefficient[n,5]', start=80, end=87, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0Y Filter Coefficient[n,5]', start=88, end=95, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0X Filter Coefficient[n,6]', start=96, end=103, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0Y Filter Coefficient[n,6]', start=104, end=111, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0X Filter Coefficient[n,7]', start=112, end=119, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 0Y Filter Coefficient[n,7]', start=120, end=127, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 1X Filter Coefficient[n,2]', start=144, end=151, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 1X Filter Coefficient[n,3]', start=152, end=159, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 1X Filter Coefficient[n,4]', start=160, end=167, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 1X Filter Coefficient[n,5]', start=168, end=175, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 1Y Filter Coefficient[n,2]', start=208, end=215, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 1Y Filter Coefficient[n,3]', start=216, end=223, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 1Y Filter Coefficient[n,4]', start=224, end=231, gen_type=gen.SFixed(1, 6))) \
    .add(gen.Field(name='Table 1Y Filter Coefficient[n,5]', start=232, end=239, gen_type=gen.SFixed(1, 6)))
cs.struct(name='MI_MATH_ALU_INSTRUCTION', length=1, bias=0) \
    .add(gen.Field(name='Operand 2', start=0, end=9, gen_type=gen.UInteger(10))) \
    .add(gen.Field(name='Operand 1', start=10, end=19, gen_type=gen.UInteger(10))) \
    .add(gen.Field(name='ALU Opcode', start=20, end=31, gen_type=gen.UInteger(12)))

cs.instruction(name='3DPRIMITIVE', length=7, bias=2) \
    .add(gen.Field(name='DWord Length', default=5, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Predicate Enable', start=8, end=8, gen_type=gen.Boolean())) \
    .add(gen.Field(name='UAV Coherency Required', start=9, end=9, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Indirect Parameter Enable', start=10, end=10, gen_type=gen.Boolean())) \
    .add(gen.Field(name='3D Command Sub Opcode', default=0, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=3, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Primitive Topology Type', start=32, end=37, gen_type=cs.type('3D_Prim_Topo_Type'))) \
    .add(gen.Field(name='Vertex Access Type', start=40, end=40, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='End Offset Enable', start=41, end=41, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Vertex Count Per Instance', start=64, end=95, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Start Vertex Location', start=96, end=127, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Instance Count', start=128, end=159, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Start Instance Location', start=160, end=191, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Base Vertex Location', start=192, end=223, gen_type=gen.Integer(32)))
cs.instruction(name='3DSTATE_AA_LINE_PARAMETERS', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=10, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='AA Coverage Slope', start=32, end=39, gen_type=gen.UFixed(0, 8))) \
    .add(gen.Field(name='AA Point Coverage Slope', start=40, end=47, gen_type=gen.UFixed(0, 8))) \
    .add(gen.Field(name='AA Coverage Bias', start=48, end=55, gen_type=gen.UFixed(0, 8))) \
    .add(gen.Field(name='AA Point Coverage Bias', start=56, end=63, gen_type=gen.UFixed(0, 8))) \
    .add(gen.Field(name='AA Coverage EndCap Slope', start=64, end=71, gen_type=gen.UFixed(0, 8))) \
    .add(gen.Field(name='AA Point Coverage EndCap Slope', start=72, end=79, gen_type=gen.UFixed(0, 8))) \
    .add(gen.Field(name='AA Coverage EndCap Bias', start=80, end=87, gen_type=gen.UFixed(0, 8))) \
    .add(gen.Field(name='AA Point Coverage EndCap Bias', start=88, end=95, gen_type=gen.UFixed(0, 8)))
cs.instruction(name='3DSTATE_BINDING_TABLE_EDIT_DS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=8, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=70, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Binding Table Edit Target', start=32, end=33, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Binding Table Block Clear', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.VariableGroup(start=64, size=32) \
        .add(gen.Field(name='Entry [n]', start=0, end=31, gen_type=cs.type('BINDING_TABLE_EDIT_ENTRY'))))
cs.instruction(name='3DSTATE_BINDING_TABLE_EDIT_GS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=8, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=68, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Binding Table Edit Target', start=32, end=33, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Binding Table Block Clear', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.VariableGroup(start=64, size=32) \
        .add(gen.Field(name='Entry [n]', start=0, end=31, gen_type=cs.type('BINDING_TABLE_EDIT_ENTRY'))))
cs.instruction(name='3DSTATE_BINDING_TABLE_EDIT_HS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=8, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=69, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Binding Table Edit Target', start=32, end=33, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Binding Table Block Clear', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.VariableGroup(start=64, size=32) \
        .add(gen.Field(name='Entry [n]', start=0, end=31, gen_type=cs.type('BINDING_TABLE_EDIT_ENTRY'))))
cs.instruction(name='3DSTATE_BINDING_TABLE_EDIT_PS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=8, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=71, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Binding Table Edit Target', start=32, end=33, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Binding Table Block Clear', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.VariableGroup(start=64, size=32) \
        .add(gen.Field(name='Entry [n]', start=0, end=31, gen_type=cs.type('BINDING_TABLE_EDIT_ENTRY'))))
cs.instruction(name='3DSTATE_BINDING_TABLE_EDIT_VS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=8, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=67, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Binding Table Edit Target', start=32, end=33, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Binding Table Block Clear', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.VariableGroup(start=64, size=32) \
        .add(gen.Field(name='Entry [n]', start=0, end=31, gen_type=cs.type('BINDING_TABLE_EDIT_ENTRY'))))
cs.instruction(name='3DSTATE_BINDING_TABLE_POINTERS_DS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=40, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to DS Binding Table', start=37, end=47, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_BINDING_TABLE_POINTERS_GS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=41, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to GS Binding Table', start=37, end=47, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_BINDING_TABLE_POINTERS_HS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=39, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to HS Binding Table', start=37, end=47, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_BINDING_TABLE_POINTERS_PS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=42, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to PS Binding Table', start=37, end=47, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_BINDING_TABLE_POINTERS_VS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=38, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to VS Binding Table', start=37, end=47, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_BINDING_TABLE_POOL_ALLOC', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=25, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Surface Object Control State', start=32, end=38, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Binding Table Pool Enable', start=43, end=43, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Binding Table Pool Base Address', start=44, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Binding Table Pool Buffer Size', start=108, end=127, gen_type=gen.UInteger(20)))
cs.instruction(name='3DSTATE_BLEND_STATE_POINTERS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=36, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Blend State Pointer Valid', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Blend State Pointer', start=38, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_CC_STATE_POINTERS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=14, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Color Calc State Pointer Valid', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Color Calc State Pointer', start=38, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_CHROMA_KEY', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=4, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='ChromaKey Table Index', start=62, end=63, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='ChromaKey Low Value', start=64, end=95, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='ChromaKey High Value', start=96, end=127, gen_type=gen.UInteger(32)))
cs.instruction(name='3DSTATE_CLEAR_PARAMS', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=4, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Depth Clear Value', start=32, end=63, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Depth Clear Value Valid', start=64, end=64, gen_type=gen.Boolean()))
cs.instruction(name='3DSTATE_CLIP', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=18, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='User Clip Distance Cull Test Enable Bitmask', start=32, end=39, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Statistics Enable', start=42, end=42, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Force Clip Mode', start=48, end=48, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Force User Clip Distance Clip Test Enable Bitmask', start=49, end=49, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Early Cull Enable', start=50, end=50, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Vertex Sub Pixel Precision Select', start=51, end=51, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Force User Clip Distance Cull Test Enable Bitmask', start=52, end=52, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Triangle Fan Provoking Vertex Select', start=64, end=65, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Line Strip/List Provoking Vertex Select', start=66, end=67, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Triangle Strip/List Provoking Vertex Select', start=68, end=69, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Non-Perspective Barycentric Enable', start=72, end=72, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Perspective Divide Disable', start=73, end=73, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Clip Mode', start=77, end=79, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='User Clip Distance Clip Test Enable Bitmask', start=80, end=87, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Guardband Clip Test Enable', start=90, end=90, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Viewport XY Clip Test Enable', start=92, end=92, gen_type=gen.Boolean())) \
    .add(gen.Field(name='API Mode', start=94, end=94, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Clip Enable', start=95, end=95, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Maximum VP Index', start=96, end=99, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Force Zero RTA Index Enable', start=101, end=101, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Maximum Point Width', start=102, end=112, gen_type=gen.UFixed(8, 3))) \
    .add(gen.Field(name='Minimum Point Width', start=113, end=123, gen_type=gen.UFixed(8, 3)))
cs.instruction(name='3DSTATE_CONSTANT_DS', length=11, bias=2) \
    .add(gen.Field(name='DWord Length', default=9, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Constant Buffer Object Control State', start=8, end=14, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=26, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Body', start=32, end=351, gen_type=cs.type('3DSTATE_CONSTANT_BODY')))
cs.instruction(name='3DSTATE_CONSTANT_GS', length=11, bias=2) \
    .add(gen.Field(name='DWord Length', default=9, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Constant Buffer Object Control State', start=8, end=14, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=22, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Body', start=32, end=351, gen_type=cs.type('3DSTATE_CONSTANT_BODY')))
cs.instruction(name='3DSTATE_CONSTANT_HS', length=11, bias=2) \
    .add(gen.Field(name='DWord Length', default=9, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Constant Buffer Object Control State', start=8, end=14, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=25, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Body', start=32, end=351, gen_type=cs.type('3DSTATE_CONSTANT_BODY')))
cs.instruction(name='3DSTATE_CONSTANT_PS', length=11, bias=2) \
    .add(gen.Field(name='DWord Length', default=9, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Constant Buffer Object Control State', start=8, end=14, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=23, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Body', start=32, end=351, gen_type=cs.type('3DSTATE_CONSTANT_BODY')))
cs.instruction(name='3DSTATE_CONSTANT_VS', length=11, bias=2) \
    .add(gen.Field(name='DWord Length', default=9, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Constant Buffer Object Control State', start=8, end=14, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=21, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Body', start=32, end=351, gen_type=cs.type('3DSTATE_CONSTANT_BODY')))
cs.instruction(name='3DSTATE_DEPTH_BUFFER', length=8, bias=2) \
    .add(gen.Field(name='DWord Length', default=6, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=5, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Surface Pitch', start=32, end=49, gen_type=gen.UInteger(18))) \
    .add(gen.Field(name='Surface Format', start=50, end=52, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Hierarchical Depth Buffer Enable', start=54, end=54, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Stencil Write Enable', start=59, end=59, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Depth Write Enable', start=60, end=60, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Surface Type', start=61, end=63, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Surface Base Address', start=64, end=127, gen_type=gen.Address())) \
    .add(gen.Field(name='LOD', start=128, end=131, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Width', start=132, end=145, gen_type=gen.UInteger(14))) \
    .add(gen.Field(name='Height', start=146, end=159, gen_type=gen.UInteger(14))) \
    .add(gen.Field(name='Depth Buffer Object Control State', start=160, end=166, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Depth Buffer MOCS', start=160, end=166, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Minimum Array Element', start=170, end=180, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Depth', start=181, end=191, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Mip Tail Start LOD', start=218, end=221, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Tiled Resource Mode', start=222, end=223, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Surface QPitch', start=224, end=238, gen_type=gen.UInteger(15))) \
    .add(gen.Field(name='Render Target View Extent', start=245, end=255, gen_type=gen.UInteger(11)))
cs.instruction(name='3DSTATE_DRAWING_RECTANGLE', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Core Mode Select', start=14, end=15, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=0, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Clipped Drawing Rectangle X Min', start=32, end=47, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Clipped Drawing Rectangle Y Min', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Clipped Drawing Rectangle X Max', start=64, end=79, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Clipped Drawing Rectangle Y Max', start=80, end=95, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Drawing Rectangle Origin X', start=96, end=111, gen_type=gen.Integer(16))) \
    .add(gen.Field(name='Drawing Rectangle Origin Y', start=112, end=127, gen_type=gen.Integer(16)))
cs.instruction(name='3DSTATE_DS', length=11, bias=2) \
    .add(gen.Field(name='DWord Length', default=9, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=29, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Kernel Start Pointer', start=38, end=95, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Software Exception Enable', start=103, end=103, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Illegal Opcode Exception Enable', start=109, end=109, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Accesses UAV', start=110, end=110, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Floating Point Mode', start=112, end=112, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Thread Dispatch Priority', start=113, end=113, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Binding Table Entry Count', start=114, end=121, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Sampler Count', start=123, end=125, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Vector Mask Enable', start=126, end=126, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Per-Thread Scratch Space', start=128, end=131, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Scratch Space Base Pointer', start=138, end=191, gen_type=gen.Address())) \
    .add(gen.Field(name='Patch URB Entry Read Offset', start=196, end=201, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Patch URB Entry Read Length', start=203, end=209, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Dispatch GRF Start Register For URB Data', start=212, end=216, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Enable', start=224, end=224, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Cache Disable', start=225, end=225, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Compute W Coordinate Enable', start=226, end=226, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Dispatch Mode', start=227, end=228, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Statistics Enable', start=234, end=234, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Maximum Number of Threads', start=245, end=253, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='User Clip Distance Cull Test Enable Bitmask', start=256, end=263, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='User Clip Distance Clip Test Enable Bitmask', start=264, end=271, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Vertex URB Entry Output Length', start=272, end=276, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Vertex URB Entry Output Read Offset', start=277, end=282, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='DUAL_PATCH Kernel Start Pointer', start=294, end=351, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_GATHER_CONSTANT_DS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=55, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Update Gather Table Only', start=33, end=33, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Binding Table Block', start=44, end=47, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Constant Buffer Valid', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='On-Die Table', start=67, end=67, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Dx9 Generate Stall', start=69, end=69, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Gather Buffer Offset', start=70, end=86, gen_type=gen.OffsetFrom())) \
    .add(gen.VariableGroup(start=96, size=32) \
        .add(gen.Field(name='Entry_0', start=0, end=15, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))) \
        .add(gen.Field(name='Entry_1', start=16, end=31, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))))
cs.instruction(name='3DSTATE_GATHER_CONSTANT_GS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=53, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Update Gather Table Only', start=33, end=33, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Binding Table Block', start=44, end=47, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Constant Buffer Valid', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='On-Die Table', start=67, end=67, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Dx9 Generate Stall', start=69, end=69, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Gather Buffer Offset', start=70, end=86, gen_type=gen.OffsetFrom())) \
    .add(gen.VariableGroup(start=96, size=32) \
        .add(gen.Field(name='Entry_0', start=0, end=15, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))) \
        .add(gen.Field(name='Entry_1', start=16, end=31, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))))
cs.instruction(name='3DSTATE_GATHER_CONSTANT_HS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=54, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Update Gather Table Only', start=33, end=33, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Binding Table Block', start=44, end=47, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Constant Buffer Valid', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='On-Die Table', start=67, end=67, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Dx9 Generate Stall', start=69, end=69, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Gather Buffer Offset', start=70, end=86, gen_type=gen.OffsetFrom())) \
    .add(gen.VariableGroup(start=96, size=32) \
        .add(gen.Field(name='Entry_0', start=0, end=15, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))) \
        .add(gen.Field(name='Entry_1', start=16, end=31, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))))
cs.instruction(name='3DSTATE_GATHER_CONSTANT_PS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=56, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='DX9 On-Die Register Read Enable', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Update Gather Table Only', start=33, end=33, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Binding Table Block', start=44, end=47, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Constant Buffer Valid', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='On-Die Table', start=67, end=67, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Dx9 Enable', start=68, end=68, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Constant Buffer Dx9 Generate Stall', start=69, end=69, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Gather Buffer Offset', start=70, end=86, gen_type=gen.OffsetFrom())) \
    .add(gen.VariableGroup(start=96, size=32) \
        .add(gen.Field(name='Entry_0', start=0, end=15, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))) \
        .add(gen.Field(name='Entry_1', start=16, end=31, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))))
cs.instruction(name='3DSTATE_GATHER_CONSTANT_VS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=52, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='DX9 On-Die Register Read Enable', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Update Gather Table Only', start=33, end=33, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Binding Table Block', start=44, end=47, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Constant Buffer Valid', start=48, end=63, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='On-Die Table', start=67, end=67, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Constant Buffer Dx9 Enable', start=68, end=68, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Constant Buffer Dx9 Generate Stall', start=69, end=69, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Gather Buffer Offset', start=70, end=86, gen_type=gen.OffsetFrom())) \
    .add(gen.VariableGroup(start=96, size=32) \
        .add(gen.Field(name='Entry_0', start=0, end=15, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))) \
        .add(gen.Field(name='Entry_1', start=16, end=31, gen_type=cs.type('GATHER_CONSTANT_ENTRY'))))
cs.instruction(name='3DSTATE_GATHER_POOL_ALLOC', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=26, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Memory Object Control State', start=32, end=38, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Gather Pool Enable', start=43, end=43, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Gather Pool Base Address', start=44, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Gather Pool Buffer Size', start=108, end=127, gen_type=gen.UInteger(20)))
cs.instruction(name='3DSTATE_GS', length=10, bias=2) \
    .add(gen.Field(name='DWord Length', default=8, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=17, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Kernel Start Pointer', start=38, end=95, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Expected Vertex Count', start=96, end=101, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Software Exception Enable', start=103, end=103, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Mask Stack Exception Enable', start=107, end=107, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Accesses UAV', start=108, end=108, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Illegal Opcode Exception Enable', start=109, end=109, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Floating Point Mode', start=112, end=112, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Thread Dispatch Priority', start=113, end=113, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Binding Table Entry Count', start=114, end=121, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Sampler Count', start=123, end=125, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Vector Mask Enable', start=126, end=126, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Single Program Flow', start=127, end=127, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Per-Thread Scratch Space', start=128, end=131, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Scratch Space Base Pointer', start=138, end=191, gen_type=gen.Address())) \
    .add(gen.Field(name='Dispatch GRF Start Register For URB Data', start=192, end=195, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex URB Entry Read Offset', start=196, end=201, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Include Vertex Handles', start=202, end=202, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Vertex URB Entry Read Length', start=203, end=208, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Output Topology', start=209, end=214, gen_type=cs.type('3D_Prim_Topo_Type'))) \
    .add(gen.Field(name='Output Vertex Size', start=215, end=220, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Dispatch GRF Start Register For URB Data [5:4]', start=221, end=222, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Enable', start=224, end=224, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Discard Adjacency', start=225, end=225, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Reorder Mode', start=226, end=226, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Hint', start=227, end=227, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Include Primitive ID', start=228, end=228, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Invocations Increment Value', start=229, end=233, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Statistics Enable', start=234, end=234, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Dispatch Mode', start=235, end=236, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Default Stream Id', start=237, end=238, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Instance Control', start=239, end=243, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Control Data Header Size', start=244, end=247, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Maximum Number of Threads', start=256, end=264, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Static Output Vertex Count', start=272, end=282, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Static Output', start=286, end=286, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Control Data Format', start=287, end=287, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='User Clip Distance Cull Test Enable Bitmask', start=288, end=295, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='User Clip Distance Clip Test Enable Bitmask', start=296, end=303, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Vertex URB Entry Output Length', start=304, end=308, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Vertex URB Entry Output Read Offset', start=309, end=314, gen_type=gen.UInteger(6)))
cs.instruction(name='3DSTATE_HIER_DEPTH_BUFFER', length=5, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=7, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Surface Pitch', start=32, end=48, gen_type=gen.UInteger(17))) \
    .add(gen.Field(name='Hierarchical Depth Buffer Object Control State', start=57, end=63, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Hierarchical Depth Buffer MOCS', start=57, end=63, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Surface Base Address', start=64, end=127, gen_type=gen.Address())) \
    .add(gen.Field(name='Surface QPitch', start=128, end=142, gen_type=gen.UInteger(15)))
cs.instruction(name='3DSTATE_HS', length=9, bias=2) \
    .add(gen.Field(name='DWord Length', default=7, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=27, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Software Exception Enable', start=44, end=44, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Illegal Opcode Exception Enable', start=45, end=45, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Floating Point Mode', start=48, end=48, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Thread Dispatch Priority', start=49, end=49, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Binding Table Entry Count', start=50, end=57, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Sampler Count', start=59, end=61, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Instance Count', start=64, end=67, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Maximum Number of Threads', start=72, end=80, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Statistics Enable', start=93, end=93, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Enable', start=95, end=95, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Kernel Start Pointer', start=102, end=159, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Per-Thread Scratch Space', start=160, end=163, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Scratch Space Base Pointer', start=170, end=223, gen_type=gen.Address())) \
    .add(gen.Field(name='Include Primitive ID', start=224, end=224, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Vertex URB Entry Read Offset', start=228, end=233, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Vertex URB Entry Read Length', start=235, end=240, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Dispatch Mode', start=241, end=242, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Dispatch GRF Start Register For URB Data', start=243, end=247, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Include Vertex Handles', start=248, end=248, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Accesses UAV', start=249, end=249, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Vector Mask Enable', start=250, end=250, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Single Program Flow', start=251, end=251, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Dispatch GRF Start Register For URB Data [5]', start=252, end=252, gen_type=gen.UInteger(1)))
cs.instruction(name='3DSTATE_INDEX_BUFFER', length=5, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=10, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Memory Object Control State', start=32, end=38, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='IndexBufferMOCS', start=32, end=38, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Index Format', start=40, end=41, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Buffer Starting Address', start=64, end=127, gen_type=gen.Address())) \
    .add(gen.Field(name='Buffer Size', start=128, end=159, gen_type=gen.UInteger(32)))
cs.instruction(name='3DSTATE_LINE_STIPPLE', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=8, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Line Stipple Pattern', start=32, end=47, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Current Stipple Index', start=48, end=51, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Current Repeat Counter', start=53, end=61, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Modify Enable (Current Repeat Counter, Current Stipple Index)', start=63, end=63, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Line Stipple Repeat Count', start=64, end=72, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Line Stipple Inverse Repeat Count', start=79, end=95, gen_type=gen.UFixed(1, 16)))
cs.instruction(name='3DSTATE_MONOFILTER_SIZE', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=17, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Monochrome Filter Height', start=32, end=34, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Monochrome Filter Width', start=35, end=37, gen_type=gen.UInteger(3)))
cs.instruction(name='3DSTATE_MULTISAMPLE', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=13, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Number of Multisamples', start=33, end=35, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pixel Location', start=36, end=36, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Pixel Position Offset Enable', start=37, end=37, gen_type=gen.Boolean()))
cs.instruction(name='3DSTATE_POLY_STIPPLE_OFFSET', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=6, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Polygon Stipple Y Offset', start=32, end=36, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Polygon Stipple X Offset', start=40, end=44, gen_type=gen.UInteger(5)))
cs.instruction(name='3DSTATE_POLY_STIPPLE_PATTERN', length=33, bias=2) \
    .add(gen.Field(name='DWord Length', default=31, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=7, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.FixedGroup(start=32, count=32, size=32) \
        .add(gen.Field(name='Pattern Row', start=0, end=31, gen_type=gen.UInteger(32))))
cs.instruction(name='3DSTATE_PS', length=12, bias=2) \
    .add(gen.Field(name='DWord Length', default=10, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=32, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Kernel Start Pointer 0', start=38, end=95, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Software Exception Enable', start=103, end=103, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Mask Stack Exception Enable', start=107, end=107, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Illegal Opcode Exception Enable', start=109, end=109, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Rounding Mode', start=110, end=111, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Floating Point Mode', start=112, end=112, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Thread Dispatch Priority', start=113, end=113, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Binding Table Entry Count', start=114, end=121, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Single Precision Denormal Mode', start=122, end=122, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Sampler Count', start=123, end=125, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Vector Mask Enable', start=126, end=126, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Single Program Flow', start=127, end=127, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Per Thread Scratch Space', start=128, end=131, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Scratch Space Base Pointer', start=138, end=191, gen_type=gen.Address())) \
    .add(gen.Field(name='8 Pixel Dispatch Enable', start=192, end=192, gen_type=gen.Boolean())) \
    .add(gen.Field(name='16 Pixel Dispatch Enable', start=193, end=193, gen_type=gen.Boolean())) \
    .add(gen.Field(name='32 Pixel Dispatch Enable', start=194, end=194, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Position XY Offset Select', start=195, end=196, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Render Target Resolve Type', start=198, end=199, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Render Target Fast Clear Enable', start=200, end=200, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Push Constant Enable', start=203, end=203, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Maximum Number of Threads Per PSD', start=215, end=223, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Dispatch GRF Start Register For Constant/Setup Data 2', start=224, end=230, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Dispatch GRF Start Register For Constant/Setup Data 1', start=232, end=238, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Dispatch GRF Start Register For Constant/Setup Data 0', start=240, end=246, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Kernel Start Pointer 1', start=262, end=319, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Kernel Start Pointer 2', start=326, end=383, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_PS_BLEND', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=77, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Independent Alpha Blend Enable', start=39, end=39, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Alpha Test Enable', start=40, end=40, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Destination Blend Factor', start=41, end=45, gen_type=cs.type('3D_Color_Buffer_Blend_Factor'))) \
    .add(gen.Field(name='Source Blend Factor', start=46, end=50, gen_type=cs.type('3D_Color_Buffer_Blend_Factor'))) \
    .add(gen.Field(name='Destination Alpha Blend Factor', start=51, end=55, gen_type=cs.type('3D_Color_Buffer_Blend_Factor'))) \
    .add(gen.Field(name='Source Alpha Blend Factor', start=56, end=60, gen_type=cs.type('3D_Color_Buffer_Blend_Factor'))) \
    .add(gen.Field(name='Color Buffer Blend Enable', start=61, end=61, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Has Writeable RT', start=62, end=62, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Alpha To Coverage Enable', start=63, end=63, gen_type=gen.Boolean()))
cs.instruction(name='3DSTATE_PS_EXTRA', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=79, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Input Coverage Mask State', start=32, end=33, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Pixel Shader Has UAV', start=34, end=34, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Pulls Bary', start=35, end=35, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Computes Stencil', start=37, end=37, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Is Per Sample', start=38, end=38, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Disables Alpha To Coverage', start=39, end=39, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Attribute Enable', start=40, end=40, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Uses Source W', start=55, end=55, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Uses Source Depth', start=56, end=56, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Force Computed Depth', start=57, end=57, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Computed Depth Mode', start=58, end=59, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Pixel Shader Kills Pixel', start=60, end=60, gen_type=gen.Boolean())) \
    .add(gen.Field(name='oMask Present to Render Target', start=61, end=61, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Does not write to RT', start=62, end=62, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Shader Valid', start=63, end=63, gen_type=gen.Boolean()))
cs.instruction(name='3DSTATE_PUSH_CONSTANT_ALLOC_DS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=20, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Buffer Size', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Constant Buffer Offset', start=48, end=52, gen_type=gen.UInteger(5)))
cs.instruction(name='3DSTATE_PUSH_CONSTANT_ALLOC_GS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=21, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Buffer Size', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Constant Buffer Offset', start=48, end=52, gen_type=gen.UInteger(5)))
cs.instruction(name='3DSTATE_PUSH_CONSTANT_ALLOC_HS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=19, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Buffer Size', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Constant Buffer Offset', start=48, end=52, gen_type=gen.UInteger(5)))
cs.instruction(name='3DSTATE_PUSH_CONSTANT_ALLOC_PS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=22, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Buffer Size', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Constant Buffer Offset', start=48, end=52, gen_type=gen.UInteger(5)))
cs.instruction(name='3DSTATE_PUSH_CONSTANT_ALLOC_VS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=18, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Constant Buffer Size', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Constant Buffer Offset', start=48, end=52, gen_type=gen.UInteger(5)))
cs.instruction(name='3DSTATE_RASTER', length=5, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=80, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Viewport Z Near Clip Test Enable', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Scissor Rectangle Enable', start=33, end=33, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Antialiasing Enable', start=34, end=34, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Back Face Fill Mode', start=35, end=36, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Front Face Fill Mode', start=37, end=38, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Global Depth Offset Enable Point', start=39, end=39, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Global Depth Offset Enable Wireframe', start=40, end=40, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Global Depth Offset Enable Solid', start=41, end=41, gen_type=gen.Boolean())) \
    .add(gen.Field(name='DX Multisample Rasterization Mode', start=42, end=43, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='DX Multisample Rasterization Enable', start=44, end=44, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Smooth Point Enable', start=45, end=45, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Force Multisampling', start=46, end=46, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Cull Mode', start=48, end=49, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Forced Sample Count', start=50, end=52, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Front Winding', start=53, end=53, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='API Mode', start=54, end=55, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Conservative Rasterization Enable', start=56, end=56, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Viewport Z Far Clip Test Enable', start=58, end=58, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Global Depth Offset Constant', start=64, end=95, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Global Depth Offset Scale', start=96, end=127, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Global Depth Offset Clamp', start=128, end=159, gen_type=gen.Float(32)))
cs.instruction(name='3DSTATE_RS_CONSTANT_POINTER', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=84, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Operation Load or Store', start=44, end=44, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Shader Select', start=60, end=62, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Global Constant Buffer Address', start=70, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Global Constant Buffer Address High', start=96, end=127, gen_type=gen.Address()))
cs.instruction(name='3DSTATE_SAMPLER_PALETTE_LOAD0', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=2, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.VariableGroup(start=32, size=32) \
        .add(gen.Field(name='Entry', start=0, end=31, gen_type=cs.type('PALETTE_ENTRY'))))
cs.instruction(name='3DSTATE_SAMPLER_PALETTE_LOAD1', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=12, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.VariableGroup(start=32, size=32) \
        .add(gen.Field(name='Palette Blue[0:N-1]', start=0, end=7, gen_type=gen.UInteger(8))) \
        .add(gen.Field(name='Palette Green[0:N-1]', start=8, end=15, gen_type=gen.UInteger(8))) \
        .add(gen.Field(name='Palette Red[0:N-1]', start=16, end=23, gen_type=gen.UInteger(8))) \
        .add(gen.Field(name='Palette Alpha[0:N-1]', start=24, end=31, gen_type=gen.UInteger(8))))
cs.instruction(name='3DSTATE_SAMPLER_STATE_POINTERS_DS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=45, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to DS Sampler State', start=37, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_SAMPLER_STATE_POINTERS_GS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=46, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to GS Sampler State', start=37, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_SAMPLER_STATE_POINTERS_HS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=44, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to HS Sampler State', start=37, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_SAMPLER_STATE_POINTERS_PS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=47, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to PS Sampler State', start=37, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_SAMPLER_STATE_POINTERS_VS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=43, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pointer to VS Sampler State', start=37, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_SAMPLE_MASK', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=24, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Sample Mask', start=32, end=47, gen_type=gen.UInteger(16)))
cs.instruction(name='3DSTATE_SAMPLE_PATTERN', length=9, bias=2) \
    .add(gen.Field(name='DWord Length', default=7, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=28, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='16x Sample0 Y Offset', start=32, end=35, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample0 X Offset', start=36, end=39, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample1 Y Offset', start=40, end=43, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample1 X Offset', start=44, end=47, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample2 Y Offset', start=48, end=51, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample2 X Offset', start=52, end=55, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample3 Y Offset', start=56, end=59, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample3 X Offset', start=60, end=63, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample4 Y Offset', start=64, end=67, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample4 X Offset', start=68, end=71, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample5 Y Offset', start=72, end=75, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample5 X Offset', start=76, end=79, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample6 Y Offset', start=80, end=83, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample6 X Offset', start=84, end=87, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample7 Y Offset', start=88, end=91, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample7 X Offset', start=92, end=95, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample8 Y Offset', start=96, end=99, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample8 X Offset', start=100, end=103, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample9 Y Offset', start=104, end=107, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample9 X Offset', start=108, end=111, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample10 Y Offset', start=112, end=115, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample10 X Offset', start=116, end=119, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample11 Y Offset', start=120, end=123, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample11 X Offset', start=124, end=127, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample12 Y Offset', start=128, end=131, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample12 X Offset', start=132, end=135, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample13 Y Offset', start=136, end=139, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample13 X Offset', start=140, end=143, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample14 Y Offset', start=144, end=147, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample14 X Offset', start=148, end=151, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample15 Y Offset', start=152, end=155, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='16x Sample15 X Offset', start=156, end=159, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample4 Y Offset', start=160, end=163, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample4 X Offset', start=164, end=167, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample5 Y Offset', start=168, end=171, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample5 X Offset', start=172, end=175, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample6 Y Offset', start=176, end=179, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample6 X Offset', start=180, end=183, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample7 Y Offset', start=184, end=187, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample7 X Offset', start=188, end=191, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample0 Y Offset', start=192, end=195, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample0 X Offset', start=196, end=199, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample1 Y Offset', start=200, end=203, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample1 X Offset', start=204, end=207, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample2 Y Offset', start=208, end=211, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample2 X Offset', start=212, end=215, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample3 Y Offset', start=216, end=219, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='8x Sample3 X Offset', start=220, end=223, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='4x Sample0 Y Offset', start=224, end=227, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='4x Sample0 X Offset', start=228, end=231, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='4x Sample1 Y Offset', start=232, end=235, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='4x Sample1 X Offset', start=236, end=239, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='4x Sample2 Y Offset', start=240, end=243, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='4x Sample2 X Offset', start=244, end=247, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='4x Sample3 Y Offset', start=248, end=251, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='4x Sample3 X Offset', start=252, end=255, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='2x Sample0 Y Offset', start=256, end=259, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='2x Sample0 X Offset', start=260, end=263, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='2x Sample1 Y Offset', start=264, end=267, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='2x Sample1 X Offset', start=268, end=271, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='1x Sample0 Y Offset', start=272, end=275, gen_type=gen.UFixed(0, 4))) \
    .add(gen.Field(name='1x Sample0 X Offset', start=276, end=279, gen_type=gen.UFixed(0, 4)))
cs.instruction(name='3DSTATE_SBE', length=6, bias=2) \
    .add(gen.Field(name='DWord Length', default=4, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=31, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Primitive ID Override Attribute Select', start=32, end=36, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Vertex URB Entry Read Offset', start=37, end=42, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Vertex URB Entry Read Length', start=43, end=47, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Primitive ID Override Component X', start=48, end=48, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Primitive ID Override Component Y', start=49, end=49, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Primitive ID Override Component Z', start=50, end=50, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Primitive ID Override Component W', start=51, end=51, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Point Sprite Texture Coordinate Origin', start=52, end=52, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Attribute Swizzle Enable', start=53, end=53, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Number of SF Output Attributes', start=54, end=59, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Force Vertex URB Entry Read Offset', start=60, end=60, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Force Vertex URB Entry Read Length', start=61, end=61, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Point Sprite Texture Coordinate Enable', start=64, end=95, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Constant Interpolation Enable', start=96, end=127, gen_type=gen.UInteger(32))) \
    .add(gen.FixedGroup(start=128, count=32, size=2) \
        .add(gen.Field(name='Attribute Active Component Format', start=0, end=1, gen_type=gen.UInteger(2))))
cs.instruction(name='3DSTATE_SBE_SWIZ', length=11, bias=2) \
    .add(gen.Field(name='DWord Length', default=9, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=81, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.FixedGroup(start=32, count=16, size=16) \
        .add(gen.Field(name='Attribute', start=0, end=15, gen_type=cs.type('SF_OUTPUT_ATTRIBUTE_DETAIL')))) \
    .add(gen.FixedGroup(start=288, count=16, size=4) \
        .add(gen.Field(name='Attribute Wrap Shortest Enables', start=0, end=3, gen_type=gen.UInteger(4))))
cs.instruction(name='3DSTATE_SCISSOR_STATE_POINTERS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=15, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Scissor Rect Pointer', start=37, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_SF', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=19, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Viewport Transform Enable', start=33, end=33, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Statistics Enable', start=42, end=42, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Legacy Global Depth Bias Enable', start=43, end=43, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Line Width', start=44, end=61, gen_type=gen.UFixed(11, 7))) \
    .add(gen.Field(name='Line End Cap Antialiasing Region Width', start=80, end=81, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Point Width', start=96, end=106, gen_type=gen.UFixed(8, 3))) \
    .add(gen.Field(name='Point Width Source', start=107, end=107, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Vertex Sub Pixel Precision Select', start=108, end=108, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Smooth Point Enable', start=109, end=109, gen_type=gen.Boolean())) \
    .add(gen.Field(name='AA Line Distance Mode', start=110, end=110, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Triangle Fan Provoking Vertex Select', start=121, end=122, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Line Strip/List Provoking Vertex Select', start=123, end=124, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Triangle Strip/List Provoking Vertex Select', start=125, end=126, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Last Pixel Enable', start=127, end=127, gen_type=gen.Boolean()))
cs.instruction(name='3DSTATE_SO_BUFFER', length=8, bias=2) \
    .add(gen.Field(name='DWord Length', default=6, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=24, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Stream Output Buffer Offset Address Enable', start=52, end=52, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Stream Offset Write Enable', start=53, end=53, gen_type=gen.Boolean())) \
    .add(gen.Field(name='SO Buffer Object Control State', start=54, end=60, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='SO Buffer MOCS', start=54, end=60, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='SO Buffer Index', start=61, end=62, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='SO Buffer Enable', start=63, end=63, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Surface Base Address', start=66, end=111, gen_type=gen.Address())) \
    .add(gen.Field(name='Surface Size', start=128, end=157, gen_type=gen.UInteger(30))) \
    .add(gen.Field(name='Stream Output Buffer Offset Address', start=162, end=207, gen_type=gen.Address())) \
    .add(gen.Field(name='Stream Offset', start=224, end=255, gen_type=gen.UInteger(32)))
cs.instruction(name='3DSTATE_SO_DECL_LIST', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', start=0, end=8, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=23, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Stream to Buffer Selects [0]', start=32, end=35, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Stream to Buffer Selects [1]', start=36, end=39, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Stream to Buffer Selects [2]', start=40, end=43, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Stream to Buffer Selects [3]', start=44, end=47, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Num Entries [0]', start=64, end=71, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Num Entries [1]', start=72, end=79, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Num Entries [2]', start=80, end=87, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Num Entries [3]', start=88, end=95, gen_type=gen.UInteger(8))) \
    .add(gen.VariableGroup(start=96, size=64) \
        .add(gen.Field(name='Entry', start=0, end=63, gen_type=cs.type('SO_DECL_ENTRY'))))
cs.instruction(name='3DSTATE_STENCIL_BUFFER', length=5, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=6, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Surface Pitch', start=32, end=48, gen_type=gen.UInteger(17))) \
    .add(gen.Field(name='Stencil Buffer Object Control State', start=54, end=60, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Stencil Buffer MOCS', start=54, end=60, gen_type=gen.UInteger(7))) \
    .add(gen.Field(name='Stencil Buffer Enable', start=63, end=63, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Surface Base Address', start=64, end=127, gen_type=gen.Address())) \
    .add(gen.Field(name='Surface QPitch', start=128, end=142, gen_type=gen.UInteger(15)))
cs.instruction(name='3DSTATE_STREAMOUT', length=5, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=30, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Force Rendering', start=55, end=56, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='SO Statistics Enable', start=57, end=57, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Reorder Mode', start=58, end=58, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Render Stream Select', start=59, end=60, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Rendering Disable', start=62, end=62, gen_type=gen.Boolean())) \
    .add(gen.Field(name='SO Function Enable', start=63, end=63, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Stream 0 Vertex Read Length', start=64, end=68, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Stream 0 Vertex Read Offset', start=69, end=69, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Stream 1 Vertex Read Length', start=72, end=76, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Stream 1 Vertex Read Offset', start=77, end=77, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Stream 2 Vertex Read Length', start=80, end=84, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Stream 2 Vertex Read Offset', start=85, end=85, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Stream 3 Vertex Read Length', start=88, end=92, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Stream 3 Vertex Read Offset', start=93, end=93, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Buffer 0 Surface Pitch', start=96, end=107, gen_type=gen.UInteger(12))) \
    .add(gen.Field(name='Buffer 1 Surface Pitch', start=112, end=123, gen_type=gen.UInteger(12))) \
    .add(gen.Field(name='Buffer 2 Surface Pitch', start=128, end=139, gen_type=gen.UInteger(12))) \
    .add(gen.Field(name='Buffer 3 Surface Pitch', start=144, end=155, gen_type=gen.UInteger(12)))
cs.instruction(name='3DSTATE_TE', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=28, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='TE Enable', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='TE Mode', start=33, end=34, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='TE Domain', start=36, end=37, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Output Topology', start=40, end=41, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Partitioning', start=44, end=45, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Maximum Tessellation Factor Odd', start=64, end=95, gen_type=gen.Float(32))) \
    .add(gen.Field(name='Maximum Tessellation Factor Not Odd', start=96, end=127, gen_type=gen.Float(32)))
cs.instruction(name='3DSTATE_URB_CLEAR', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=29, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='URB Address', start=32, end=46, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='URB Clear Length', start=48, end=61, gen_type=gen.UInteger(14)))
cs.instruction(name='3DSTATE_URB_DS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=50, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='DS Number of URB Entries', start=32, end=47, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='DS URB Entry Allocation Size', start=48, end=56, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='DS URB Starting Address', start=57, end=63, gen_type=gen.UInteger(7)))
cs.instruction(name='3DSTATE_URB_GS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=51, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='GS Number of URB Entries', start=32, end=47, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='GS URB Entry Allocation Size', start=48, end=56, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='GS URB Starting Address', start=57, end=63, gen_type=gen.UInteger(7)))
cs.instruction(name='3DSTATE_URB_HS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=49, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='HS Number of URB Entries', start=32, end=47, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='HS URB Entry Allocation Size', start=48, end=56, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='HS URB Starting Address', start=57, end=63, gen_type=gen.UInteger(7)))
cs.instruction(name='3DSTATE_URB_VS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=48, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='VS Number of URB Entries', start=32, end=47, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='VS URB Entry Allocation Size', start=48, end=56, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='VS URB Starting Address', start=57, end=63, gen_type=gen.UInteger(7)))
cs.instruction(name='3DSTATE_VERTEX_BUFFERS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=8, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.VariableGroup(start=32, size=128) \
        .add(gen.Field(name='Vertex Buffer State', start=0, end=127, gen_type=cs.type('VERTEX_BUFFER_STATE'))))
cs.instruction(name='3DSTATE_VERTEX_ELEMENTS', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=9, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.VariableGroup(start=32, size=64) \
        .add(gen.Field(name='Element', start=0, end=63, gen_type=cs.type('VERTEX_ELEMENT_STATE'))))
cs.instruction(name='3DSTATE_VF', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Indexed Draw Cut Index Enable', start=8, end=8, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Component Packing Enable', start=9, end=9, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Sequential Draw Cut Index Enable', start=10, end=10, gen_type=gen.Boolean())) \
    .add(gen.Field(name='3D Command Sub Opcode', default=12, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Cut Index', start=32, end=63, gen_type=gen.UInteger(32)))
cs.instruction(name='3DSTATE_VF_COMPONENT_PACKING', length=5, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=85, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Vertex Element 00 Enables', start=32, end=35, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 01 Enables', start=36, end=39, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 02 Enables', start=40, end=43, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 03 Enables', start=44, end=47, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 04 Enables', start=48, end=51, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 05 Enables', start=52, end=55, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 06 Enables', start=56, end=59, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 07 Enables', start=60, end=63, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 08 Enables', start=64, end=67, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 09 Enables', start=68, end=71, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 10 Enables', start=72, end=75, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 11 Enables', start=76, end=79, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 12 Enables', start=80, end=83, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 13 Enables', start=84, end=87, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 14 Enables', start=88, end=91, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 15 Enables', start=92, end=95, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 16 Enables', start=96, end=99, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 17 Enables', start=100, end=103, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 18 Enables', start=104, end=107, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 19 Enables', start=108, end=111, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 20 Enables', start=112, end=115, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 21 Enables', start=116, end=119, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 22 Enables', start=120, end=123, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 23 Enables', start=124, end=127, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 24 Enables', start=128, end=131, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 25 Enables', start=132, end=135, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 26 Enables', start=136, end=139, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 27 Enables', start=140, end=143, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 28 Enables', start=144, end=147, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 29 Enables', start=148, end=151, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 30 Enables', start=152, end=155, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Vertex Element 31 Enables', start=156, end=159, gen_type=gen.UInteger(4)))
cs.instruction(name='3DSTATE_VF_INSTANCING', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=73, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Vertex Element Index', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Instancing Enable', start=40, end=40, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Instance Data Step Rate', start=64, end=95, gen_type=gen.UInteger(32)))
cs.instruction(name='3DSTATE_VF_SGVS', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=74, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='VertexID Element Offset', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='VertexID Component Number', start=45, end=46, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='VertexID Enable', start=47, end=47, gen_type=gen.Boolean())) \
    .add(gen.Field(name='InstanceID Element Offset', start=48, end=53, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='InstanceID Component Number', start=61, end=62, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='InstanceID Enable', start=63, end=63, gen_type=gen.Boolean()))
cs.instruction(name='3DSTATE_VF_STATISTICS', length=1, bias=1) \
    .add(gen.Field(name='Statistics Enable', start=0, end=0, gen_type=gen.Boolean())) \
    .add(gen.Field(name='3D Command Sub Opcode', default=11, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=1, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='3DSTATE_VF_TOPOLOGY', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=75, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Primitive Topology Type', start=32, end=37, gen_type=cs.type('3D_Prim_Topo_Type')))
cs.instruction(name='3DSTATE_VIEWPORT_STATE_POINTERS_CC', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=35, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='CC Viewport Pointer', start=37, end=63, gen_type=gen.OffsetFrom()))
cs.instruction(name='3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=33, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='SF Clip Viewport Pointer', start=38, end=63, gen_type=gen.OffsetFrom(offset='Dynamic State Base Address', gen_type=cs.type('SF_CLIP_VIEWPORT'))))
cs.instruction(name='3DSTATE_VS', length=9, bias=2) \
    .add(gen.Field(name='DWord Length', default=7, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=16, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Kernel Start Pointer', start=38, end=95, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Software Exception Enable', start=103, end=103, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Accesses UAV', start=108, end=108, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Illegal Opcode Exception Enable', start=109, end=109, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Floating Point Mode', start=112, end=112, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Thread Dispatch Priority', start=113, end=113, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Binding Table Entry Count', start=114, end=121, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Sampler Count', start=123, end=125, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Vector Mask Enable', start=126, end=126, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Single Vertex Dispatch', start=127, end=127, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Per-Thread Scratch Space', start=128, end=131, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Scratch Space Base Pointer', start=138, end=191, gen_type=gen.Address())) \
    .add(gen.Field(name='Vertex URB Entry Read Offset', start=196, end=201, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Vertex URB Entry Read Length', start=203, end=208, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Dispatch GRF Start Register For URB Data', start=212, end=216, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Enable', start=224, end=224, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Vertex Cache Disable', start=225, end=225, gen_type=gen.Boolean())) \
    .add(gen.Field(name='SIMD8 Dispatch Enable', start=226, end=226, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Statistics Enable', start=234, end=234, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Maximum Number of Threads', start=247, end=255, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='User Clip Distance Cull Test Enable Bitmask', start=256, end=263, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='User Clip Distance Clip Test Enable Bitmask', start=264, end=271, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Vertex URB Entry Output Length', start=272, end=276, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Vertex URB Entry Output Read Offset', start=277, end=282, gen_type=gen.UInteger(6)))
cs.instruction(name='3DSTATE_WM', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=20, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Force Kill Pixel Enable', start=32, end=33, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Point Rasterization Rule', start=34, end=34, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Line Stipple Enable', start=35, end=35, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Polygon Stipple Enable', start=36, end=36, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Line Antialiasing Region Width', start=38, end=39, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Line End Cap Antialiasing Region Width', start=40, end=41, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Barycentric Interpolation Mode', start=43, end=48, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Position ZW Interpolation Mode', start=49, end=50, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Force Thread Dispatch Enable', start=51, end=52, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Early Depth/Stencil Control', start=53, end=54, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Legacy Diamond Line Rasterization', start=58, end=58, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Legacy Hierarchical Depth Buffer Resolve Enable', start=59, end=59, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Legacy Depth Buffer Resolve Enable', start=60, end=60, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Legacy Depth Buffer Clear Enable', start=62, end=62, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Statistics Enable', start=63, end=63, gen_type=gen.Boolean()))
cs.instruction(name='3DSTATE_WM_CHROMAKEY', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=76, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='ChromaKey Kill Enable', start=63, end=63, gen_type=gen.Boolean()))
cs.instruction(name='3DSTATE_WM_DEPTH_STENCIL', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=78, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Depth Buffer Write Enable', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Depth Test Enable', start=33, end=33, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Stencil Buffer Write Enable', start=34, end=34, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Stencil Test Enable', start=35, end=35, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Double Sided Stencil Enable', start=36, end=36, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Depth Test Function', start=37, end=39, gen_type=cs.type('3D_Compare_Function'))) \
    .add(gen.Field(name='Stencil Test Function', start=40, end=42, gen_type=cs.type('3D_Compare_Function'))) \
    .add(gen.Field(name='Backface Stencil Pass Depth Pass Op', start=43, end=45, gen_type=cs.type('3D_Stencil_Operation'))) \
    .add(gen.Field(name='Backface Stencil Pass Depth Fail Op', start=46, end=48, gen_type=cs.type('3D_Stencil_Operation'))) \
    .add(gen.Field(name='Backface Stencil Fail Op', start=49, end=51, gen_type=cs.type('3D_Stencil_Operation'))) \
    .add(gen.Field(name='Backface Stencil Test Function', start=52, end=54, gen_type=cs.type('3D_Compare_Function'))) \
    .add(gen.Field(name='Stencil Pass Depth Pass Op', start=55, end=57, gen_type=cs.type('3D_Stencil_Operation'))) \
    .add(gen.Field(name='Stencil Pass Depth Fail Op', start=58, end=60, gen_type=cs.type('3D_Stencil_Operation'))) \
    .add(gen.Field(name='Stencil Fail Op', start=61, end=63, gen_type=cs.type('3D_Stencil_Operation'))) \
    .add(gen.Field(name='Backface Stencil Write Mask', start=64, end=71, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Backface Stencil Test Mask', start=72, end=79, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Stencil Write Mask', start=80, end=87, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Stencil Test Mask', start=88, end=95, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Backface Stencil Reference Value', start=96, end=103, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Stencil Reference Value', start=104, end=111, gen_type=gen.UInteger(8)))
cs.instruction(name='3DSTATE_WM_HZ_OP', length=5, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=82, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Number of Multisamples', start=45, end=47, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Stencil Clear Value', start=48, end=55, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Full Surface Depth and Stencil Clear', start=57, end=57, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pixel Position Offset Enable', start=58, end=58, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Hierarchical Depth Buffer Resolve Enable', start=59, end=59, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Depth Buffer Resolve Enable', start=60, end=60, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Scissor Rectangle Enable', start=61, end=61, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Depth Buffer Clear Enable', start=62, end=62, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Stencil Buffer Clear Enable', start=63, end=63, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Clear Rectangle X Min', start=64, end=79, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Clear Rectangle Y Min', start=80, end=95, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Clear Rectangle X Max', start=96, end=111, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Clear Rectangle Y Max', start=112, end=127, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Sample Mask', start=128, end=143, gen_type=gen.UInteger(16)))
cs.instruction(name='GPGPU_CSR_BASE_ADDRESS', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=4, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=0, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='GPGPU CSR Base Address', start=44, end=95, gen_type=gen.Address()))
cs.instruction(name='GPGPU_WALKER', length=15, bias=2) \
    .add(gen.Field(name='DWord Length', default=13, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Predicate Enable', start=8, end=8, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Indirect Parameter Enable', start=10, end=10, gen_type=gen.Boolean())) \
    .add(gen.Field(name='SubOpcode', default=5, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Interface Descriptor Offset', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Indirect Data Length', start=64, end=80, gen_type=gen.UInteger(17))) \
    .add(gen.Field(name='Indirect Data Start Address', start=102, end=127, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Thread Width Counter Maximum', start=128, end=133, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Thread Height Counter Maximum', start=136, end=141, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Thread Depth Counter Maximum', start=144, end=149, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='SIMD Size', start=158, end=159, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Thread Group ID Starting X', start=160, end=191, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Thread Group ID X Dimension', start=224, end=255, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Thread Group ID Starting Y', start=256, end=287, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Thread Group ID Y Dimension', start=320, end=351, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Thread Group ID Starting/Resume Z', start=352, end=383, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Thread Group ID Z Dimension', start=384, end=415, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Right Execution Mask', start=416, end=447, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Bottom Execution Mask', start=448, end=479, gen_type=gen.UInteger(32)))
cs.instruction(name='MEDIA_CURBE_LOAD', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='SubOpcode', default=1, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='CURBE Total Data Length', start=64, end=80, gen_type=gen.UInteger(17))) \
    .add(gen.Field(name='CURBE Data Start Address', start=96, end=127, gen_type=gen.UInteger(32)))
cs.instruction(name='MEDIA_INTERFACE_DESCRIPTOR_LOAD', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='SubOpcode', default=2, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Interface Descriptor Total Length', start=64, end=80, gen_type=gen.UInteger(17))) \
    .add(gen.Field(name='Interface Descriptor Data Start Address', start=96, end=127, gen_type=gen.OffsetFrom()))
cs.instruction(name='MEDIA_OBJECT', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=4, start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Media Command Sub-Opcode', default=0, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Media Command Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Interface Descriptor Offset', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Indirect Data Length', start=64, end=80, gen_type=gen.UInteger(17))) \
    .add(gen.Field(name='SubSlice Destination Select', start=81, end=82, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Slice Destination Select', start=83, end=84, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Use Scoreboard', start=85, end=85, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Force Destination', start=86, end=86, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Thread Synchronization', start=88, end=88, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Slice Destination Select MSBs', start=89, end=90, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Children Present', start=95, end=95, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Indirect Data Start Address', start=96, end=127, gen_type=gen.Address())) \
    .add(gen.Field(name='Scoreboard X', start=128, end=136, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Scoredboard Y', start=144, end=152, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Scoreboard Mask', start=160, end=167, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Scoreboard Color', start=176, end=179, gen_type=gen.UInteger(4))) \
    .add(gen.VariableGroup(start=192, size=32) \
        .add(gen.Field(name='Inline Data', start=0, end=31, gen_type=gen.UInteger(32))))
cs.instruction(name='MEDIA_OBJECT_GRPID', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=5, start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Media Command Sub-Opcode', default=6, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Media Command Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Interface Descriptor Offset', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Indirect Data Length', start=64, end=80, gen_type=gen.UInteger(17))) \
    .add(gen.Field(name='SubSlice Destination Select', start=81, end=82, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Slice Destination Select', start=83, end=84, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Use Scoreboard', start=85, end=85, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Force Destination', start=86, end=86, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='End of Thread Group', start=87, end=87, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Slice Destination Select MSB', start=88, end=88, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Indirect Data Start Address', start=96, end=127, gen_type=gen.Address())) \
    .add(gen.Field(name='Scoreboard X', start=128, end=136, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Scoreboard Y', start=144, end=152, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='Scoreboard Mask', start=160, end=167, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Scoreboard Color', start=176, end=179, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='GroupID', start=192, end=223, gen_type=gen.UInteger(32))) \
    .add(gen.VariableGroup(start=224, size=32) \
        .add(gen.Field(name='Inline Data', start=0, end=31, gen_type=gen.UInteger(32))))
cs.instruction(name='MEDIA_OBJECT_PRT', length=16, bias=2) \
    .add(gen.Field(name='DWord Length', default=14, start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='SubOpcode', default=2, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Interface Descriptor Offset', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='PRT_FenceType', start=86, end=86, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='PRT_Fence Needed', start=87, end=87, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Children Present', start=95, end=95, gen_type=gen.Boolean())) \
    .add(gen.FixedGroup(start=128, count=12, size=32) \
        .add(gen.Field(name='Inline Data', start=0, end=31, gen_type=gen.UInteger(32))))
cs.instruction(name='MEDIA_OBJECT_WALKER', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=15, start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='SubOpcode', default=3, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Interface Descriptor Offset', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Indirect Data Length', start=64, end=80, gen_type=gen.UInteger(17))) \
    .add(gen.Field(name='Use Scoreboard', start=85, end=85, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Masked Dispatch', start=86, end=87, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Thread Synchronization', start=88, end=88, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Indirect Data Start Address', start=96, end=127, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Scoreboard Mask', start=160, end=167, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Group ID Loop Select', start=168, end=191, gen_type=gen.UInteger(24))) \
    .add(gen.Field(name='Mid-Loop Unit X', start=200, end=201, gen_type=gen.Integer(2))) \
    .add(gen.Field(name='Local Mid-Loop Unit Y', start=204, end=205, gen_type=gen.Integer(2))) \
    .add(gen.Field(name='Middle Loop Extra Steps', start=208, end=212, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Color Count Minus One', start=216, end=219, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Local Loop Exec Count', start=224, end=235, gen_type=gen.UInteger(12))) \
    .add(gen.Field(name='Global Loop Exec Count', start=240, end=251, gen_type=gen.UInteger(12))) \
    .add(gen.Field(name='Block Resolution X', start=256, end=266, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Block Resolution Y', start=272, end=282, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Local Start X', start=288, end=298, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Local Start Y', start=304, end=314, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Local Outer Loop Stride X', start=352, end=363, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Local Outer Loop Stride Y', start=368, end=379, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Local Inner Loop Unit X', start=384, end=395, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Local Inner Loop Unit Y', start=400, end=411, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Global Resolution X', start=416, end=426, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Global Resolution Y', start=432, end=442, gen_type=gen.UInteger(11))) \
    .add(gen.Field(name='Global Start X', start=448, end=459, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Global Start Y', start=464, end=475, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Global Outer Loop Stride X', start=480, end=491, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Global Outer Loop Stride Y', start=496, end=507, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Global Inner Loop Unit X', start=512, end=523, gen_type=gen.Integer(12))) \
    .add(gen.Field(name='Global Inner Loop Unit Y', start=528, end=539, gen_type=gen.Integer(12))) \
    .add(gen.VariableGroup(start=544, size=32) \
        .add(gen.Field(name='Inline Data', start=0, end=31, gen_type=gen.UInteger(32))))
cs.instruction(name='MEDIA_STATE_FLUSH', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='SubOpcode', default=4, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Interface Descriptor Offset', start=32, end=37, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Watermark Required', start=38, end=38, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Flush to GO', start=39, end=39, gen_type=gen.Boolean()))
cs.instruction(name='MEDIA_VFE_STATE', length=9, bias=2) \
    .add(gen.Field(name='DWord Length', default=7, start=0, end=15, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='SubOpcode', default=0, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Media Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Pipeline', default=2, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Per Thread Scratch Space', start=32, end=35, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Stack Size', start=36, end=39, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='Scratch Space Base Pointer', start=42, end=79, gen_type=gen.Address())) \
    .add(gen.Field(name='Reset Gateway Timer', start=103, end=103, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Number of URB Entries', start=104, end=111, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Maximum Number of Threads', start=112, end=127, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Slice Disable', start=128, end=129, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='CURBE Allocation Size', start=160, end=175, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='URB Entry Allocation Size', start=176, end=191, gen_type=gen.UInteger(16))) \
    .add(gen.Field(name='Scoreboard Mask', start=192, end=199, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Scoreboard Type', start=222, end=222, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Scoreboard Enable', start=223, end=223, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Scoreboard 0 Delta X', start=224, end=227, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 0 Delta Y', start=228, end=231, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 1 Delta X', start=232, end=235, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 1 Delta Y', start=236, end=239, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 2 Delta X', start=240, end=243, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 2 Delta Y', start=244, end=247, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 3 Delta X', start=248, end=251, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 3 Delta Y', start=252, end=255, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 4 Delta X', start=256, end=259, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 4 Delta Y', start=260, end=263, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 5 Delta X', start=264, end=267, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 5 Delta Y', start=268, end=271, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 6 Delta X', start=272, end=275, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 6 Delta Y', start=276, end=279, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 7 Delta X', start=280, end=283, gen_type=gen.Integer(4))) \
    .add(gen.Field(name='Scoreboard 7 Delta Y', start=284, end=287, gen_type=gen.Integer(4)))
cs.instruction(name='MI_ARB_CHECK', length=1, bias=1) \
    .add(gen.Field(name='MI Command Opcode', default=5, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_ATOMIC', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='ATOMIC OPCODE', start=8, end=15, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Return Data Control', start=16, end=16, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='CS STALL', start=17, end=17, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Inline Data', start=18, end=18, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Data Size', start=19, end=20, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Post-Sync Operation', start=21, end=21, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Memory Type', start=22, end=22, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='MI Command Opcode', default=47, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Memory Address', start=34, end=79, gen_type=gen.Address())) \
    .add(gen.Field(name='Operand1 Data Dword 0', start=96, end=127, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Operand2 Data Dword 0', start=128, end=159, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Operand1 Data Dword 1', start=160, end=191, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Operand2 Data Dword 1', start=192, end=223, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Operand1 Data Dword 2', start=224, end=255, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Operand2 Data Dword 2', start=256, end=287, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Operand1 Data Dword 3', start=288, end=319, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Operand2 Data Dword 3', start=320, end=351, gen_type=gen.UInteger(32)))
cs.instruction(name='MI_BATCH_BUFFER_END', length=1, bias=1) \
    .add(gen.Field(name='MI Command Opcode', default=10, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_BATCH_BUFFER_START', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Address Space Indicator', start=8, end=8, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Resource Streamer Enable', start=10, end=10, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Predication Enable', start=15, end=15, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Add Offset Enable', start=16, end=16, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Second Level Batch Buffer', start=22, end=22, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='MI Command Opcode', default=49, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Batch Buffer Start Address', start=34, end=95, gen_type=gen.Address(cs.decode_instructions_until)))
cs.instruction(name='MI_CLFLUSH', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=9, gen_type=gen.UInteger(10))) \
    .add(gen.Field(name='Use Global GTT', start=22, end=22, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=39, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Starting Cacheline Offset', start=38, end=43, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Page Base Address', start=44, end=79, gen_type=gen.Address())) \
    .add(gen.VariableGroup(start=96, size=32) \
        .add(gen.Field(name='DW Representing a Half Cache Line', start=0, end=31, gen_type=gen.UInteger(32))))
cs.instruction(name='MI_CONDITIONAL_BATCH_BUFFER_END', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Compare Mask Mode', start=19, end=19, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Compare Semaphore', default=0, start=21, end=21, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Use Global GTT', start=22, end=22, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=54, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Compare Data Dword', start=32, end=63, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Compare Address', start=67, end=127, gen_type=gen.Address()))
cs.instruction(name='MI_COPY_MEM_MEM', length=5, bias=2) \
    .add(gen.Field(name='DWord Length', default=3, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Use Global GTT Destination', start=21, end=21, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Use Global GTT Source', start=22, end=22, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=46, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Destination Memory Address', start=34, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Source Memory Address', start=98, end=159, gen_type=gen.Address()))
cs.instruction(name='MI_DISPLAY_FLIP', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Display Plane Select', start=8, end=12, gen_type=gen.UInteger(5))) \
    .add(gen.Field(name='Async Flip Indicator', start=22, end=22, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=20, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Tile Parameter', start=32, end=34, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Buffer Pitch', start=38, end=47, gen_type=gen.UInteger(10))) \
    .add(gen.Field(name='Stereoscopic 3D Mode', start=63, end=63, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Flip Type', start=64, end=65, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Display Buffer Base Address', start=76, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Left Eye Display Buffer Base Address', start=108, end=127, gen_type=gen.Address()))
cs.instruction(name='MI_FORCE_WAKEUP', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='MI Command Opcode', default=29, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Force Media Awake', start=32, end=32, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Force Render Awake', start=33, end=33, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Mask Bits', start=48, end=63, gen_type=gen.UInteger(16)))
cs.instruction(name='MI_LOAD_REGISTER_IMM', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Byte Write Disables', start=8, end=11, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='MI Command Opcode', default=34, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Register Offset', start=34, end=54, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Data DWord', start=64, end=95, gen_type=gen.UInteger(32)))
cs.instruction(name='MI_LOAD_REGISTER_MEM', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Async Mode Enable', start=21, end=21, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Use Global GTT', start=22, end=22, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=41, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Register Address', start=34, end=54, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Memory Address', start=66, end=127, gen_type=gen.Address()))
cs.instruction(name='MI_LOAD_REGISTER_REG', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='MI Command Opcode', default=42, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Source Register Address', start=34, end=54, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Destination Register Address', start=66, end=86, gen_type=gen.OffsetFrom()))
cs.instruction(name='MI_LOAD_SCAN_LINES_EXCL', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=5, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Display (Plane) Select', start=19, end=21, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='MI Command Opcode', default=19, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='End Scan Line Number', start=32, end=44, gen_type=gen.UInteger(13))) \
    .add(gen.Field(name='Start Scan Line Number', start=48, end=60, gen_type=gen.UInteger(13)))
cs.instruction(name='MI_LOAD_SCAN_LINES_INCL', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=5, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Scan Line Event Done Forward', start=17, end=18, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display (Plane) Select', start=19, end=21, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='MI Command Opcode', default=18, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='End Scan Line Number', start=32, end=44, gen_type=gen.UInteger(13))) \
    .add(gen.Field(name='Start Scan Line Number', start=48, end=60, gen_type=gen.UInteger(13)))
cs.instruction(name='MI_LOAD_URB_MEM', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='MI Command Opcode', default=44, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='URB Address', start=34, end=46, gen_type=gen.UInteger(13))) \
    .add(gen.Field(name='Memory Address', start=70, end=127, gen_type=gen.Address()))
cs.instruction(name='MI_MATH', length=0, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='MI Command Opcode', default=26, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.VariableGroup(start=32, size=32) \
        .add(gen.Field(name='Instruction', start=0, end=31, gen_type=cs.type('MI_MATH_ALU_INSTRUCTION'))))
cs.instruction(name='MI_NOOP', length=1, bias=1) \
    .add(gen.Field(name='Identification Number', start=0, end=21, gen_type=gen.UInteger(22))) \
    .add(gen.Field(name='Identification Number Register Write Enable', start=22, end=22, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=0, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_PREDICATE', length=1, bias=1) \
    .add(gen.Field(name='Compare Operation', start=0, end=1, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Combine Operation', start=3, end=4, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Load Operation', start=6, end=7, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='MI Command Opcode', default=12, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_REPORT_HEAD', length=1, bias=1) \
    .add(gen.Field(name='MI Command Opcode', default=7, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_REPORT_PERF_COUNT', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=5, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='MI Command Opcode', default=40, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Use Global GTT', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Core Mode Enable', start=36, end=36, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Memory Address', start=38, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Report ID', start=96, end=127, gen_type=gen.UInteger(32)))
cs.instruction(name='MI_RS_CONTEXT', length=1, bias=1) \
    .add(gen.Field(name='Resource Streamer Save', start=0, end=0, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='MI Command Opcode', default=15, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_RS_CONTROL', length=1, bias=1) \
    .add(gen.Field(name='Resource Streamer Control', start=0, end=0, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='MI Command Opcode', default=6, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_RS_STORE_DATA_IMM', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='MI Command Opcode', default=43, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Core Mode Enable', start=32, end=32, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Destination Address', start=34, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Data DWord 0', start=96, end=127, gen_type=gen.UInteger(32)))
cs.instruction(name='MI_SEMAPHORE_SIGNAL', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Target Engine Select', start=15, end=17, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Post-Sync Operation', start=21, end=21, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=27, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Target Context ID', start=32, end=63, gen_type=gen.UInteger(32)))
cs.instruction(name='MI_SEMAPHORE_WAIT', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Compare Operation', start=12, end=14, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Wait Mode', start=15, end=15, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Register Poll Mode', start=16, end=16, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Memory Type', start=22, end=22, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='MI Command Opcode', default=28, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Semaphore Data Dword', start=32, end=63, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Semaphore Address', start=66, end=127, gen_type=gen.Address()))
cs.instruction(name='MI_SET_CONTEXT', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='MI Command Opcode', default=24, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Restore Inhibit', start=32, end=32, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Force Restore', start=33, end=33, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Resource Streamer State Restore Enable', start=34, end=34, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Resource Streamer State Save Enable', start=35, end=35, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Core Mode Enable', start=36, end=36, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Reserved, Must be 1', start=40, end=40, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Logical Context Address', start=44, end=63, gen_type=gen.Address()))
cs.instruction(name='MI_SET_PREDICATE', length=1, bias=1) \
    .add(gen.Field(name='PREDICATE ENABLE', start=0, end=3, gen_type=gen.UInteger(4))) \
    .add(gen.Field(name='MI Command Opcode', default=1, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_STORE_DATA_IMM', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=9, gen_type=gen.UInteger(10))) \
    .add(gen.Field(name='Store Qword', start=21, end=21, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Use Global GTT', start=22, end=22, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=32, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Core Mode Enable', start=32, end=32, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Address', start=34, end=79, gen_type=gen.Address())) \
    .add(gen.Field(name='Immediate Data', start=96, end=159, gen_type=gen.UInteger(64)))
cs.instruction(name='MI_STORE_DATA_INDEX', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Use Per-Process Hardware Status Page', start=21, end=21, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='MI Command Opcode', default=33, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Offset', start=34, end=43, gen_type=gen.UInteger(10))) \
    .add(gen.Field(name='Data DWord 0', start=64, end=95, gen_type=gen.UInteger(32))) \
    .add(gen.Field(name='Data DWord 1', start=96, end=127, gen_type=gen.UInteger(32)))
cs.instruction(name='MI_STORE_REGISTER_MEM', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='Predicate Enable', start=21, end=21, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Use Global GTT', start=22, end=22, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=36, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Register Address', start=34, end=54, gen_type=gen.OffsetFrom())) \
    .add(gen.Field(name='Memory Address', start=66, end=127, gen_type=gen.Address()))
cs.instruction(name='MI_STORE_URB_MEM', length=4, bias=2) \
    .add(gen.Field(name='DWord Length', default=2, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='MI Command Opcode', default=45, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='URB Address', start=34, end=46, gen_type=gen.UInteger(13))) \
    .add(gen.Field(name='Memory Address', start=70, end=127, gen_type=gen.Address()))
cs.instruction(name='MI_SUSPEND_FLUSH', length=1, bias=1) \
    .add(gen.Field(name='Suspend Flush', start=0, end=0, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=11, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_TOPOLOGY_FILTER', length=1, bias=1) \
    .add(gen.Field(name='Topology Filter Value', start=0, end=5, gen_type=cs.type('3D_Prim_Topo_Type'))) \
    .add(gen.Field(name='MI Command Opcode', default=13, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_URB_ATOMIC_ALLOC', length=1, bias=1) \
    .add(gen.Field(name='URB Atomic Storage Size', start=0, end=8, gen_type=gen.UInteger(9))) \
    .add(gen.Field(name='URB Atomic Storage Offset', start=12, end=19, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='MI Command Opcode', default=9, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_USER_INTERRUPT', length=1, bias=1) \
    .add(gen.Field(name='MI Command Opcode', default=2, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='MI_WAIT_FOR_EVENT', length=1, bias=1) \
    .add(gen.Field(name='Display Plnae 1 A Scan Line Wait Enable', start=0, end=0, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 1 Flip Pending Wait Enable', start=1, end=1, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 4 Flip Pending Wait Enable', start=2, end=2, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 1 A Vertical Blank Wait Enable', start=3, end=3, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 7 Flip Pending Wait Enable', start=6, end=6, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 8 Flip Pending Wait Enable', start=7, end=7, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 1 B Scan Line Wait Enable', start=8, end=8, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 2 Flip Pending Wait Enable', start=9, end=9, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 5 Flip Pending Wait Enable', start=10, end=10, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 1 B Vertical Blank Wait Enable', start=11, end=11, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 1 C Scan Line Wait Enable', start=14, end=14, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 3 Flip Pending Wait Enable', start=15, end=15, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 9 Flip Pending Wait Enable', start=16, end=16, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 10 Flip Pending Wait Enable', start=17, end=17, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 11 Flip Pending Wait Enable', start=18, end=18, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 12 Flip Pending Wait Enable', start=19, end=19, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 6 Flip Pending Wait Enable', start=20, end=20, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Display Plane 1 C Vertical Blank Wait Enable', start=21, end=21, gen_type=gen.Boolean())) \
    .add(gen.Field(name='MI Command Opcode', default=3, start=23, end=28, gen_type=gen.UInteger(6))) \
    .add(gen.Field(name='Command Type', default=0, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='PIPELINE_SELECT', length=1, bias=1) \
    .add(gen.Field(name='Pipeline Selection', start=0, end=1, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Media Sampler DOP Clock Gate Enable', start=4, end=4, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Force Media Awake', start=5, end=5, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Mask Bits', start=8, end=15, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=4, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=1, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3)))
cs.instruction(name='PIPE_CONTROL', length=6, bias=2) \
    .add(gen.Field(name='DWord Length', default=4, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=0, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=2, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=3, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Depth Cache Flush Enable', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Stall At Pixel Scoreboard', start=33, end=33, gen_type=gen.Boolean())) \
    .add(gen.Field(name='State Cache Invalidation Enable', start=34, end=34, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Constant Cache Invalidation Enable', start=35, end=35, gen_type=gen.Boolean())) \
    .add(gen.Field(name='VF Cache Invalidation Enable', start=36, end=36, gen_type=gen.Boolean())) \
    .add(gen.Field(name='DC Flush Enable', start=37, end=37, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Pipe Control Flush Enable', start=39, end=39, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Notify Enable', start=40, end=40, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Indirect State Pointers Disable', start=41, end=41, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Texture Cache Invalidation Enable', start=42, end=42, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Instruction Cache Invalidate Enable', start=43, end=43, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Render Target Cache Flush Enable', start=44, end=44, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Depth Stall Enable', start=45, end=45, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Post Sync Operation', start=46, end=47, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Generic Media State Clear', start=48, end=48, gen_type=gen.Boolean())) \
    .add(gen.Field(name='TLB Invalidate', start=50, end=50, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Global Snapshot Count Reset', start=51, end=51, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Command Streamer Stall Enable', start=52, end=52, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Store Data Index', start=53, end=53, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='LRI Post Sync Operation', start=55, end=55, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Destination Address Type', start=56, end=56, gen_type=gen.UInteger(1))) \
    .add(gen.Field(name='Flush LLC', start=58, end=58, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Address', start=66, end=111, gen_type=gen.Address())) \
    .add(gen.Field(name='Immediate Data', start=128, end=191, gen_type=gen.UInteger(64)))
cs.instruction(name='STATE_BASE_ADDRESS', length=19, bias=2) \
    .add(gen.Field(name='DWord Length', default=17, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=1, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=0, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='General State Base Address Modify Enable', start=32, end=32, gen_type=gen.Boolean())) \
    .add(gen.Field(name='General State Memory Object Control State', start=36, end=42, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='General State Base Address', start=44, end=95, gen_type=gen.Address())) \
    .add(gen.Field(name='Stateless Data Port Access Memory Object Control State', start=112, end=118, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Surface State Base Address Modify Enable', start=128, end=128, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Surface State Memory Object Control State', start=132, end=138, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Surface State Base Address', start=140, end=191, gen_type=gen.Address())) \
    .add(gen.Field(name='Dynamic State Base Address Modify Enable', start=192, end=192, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Dynamic State Memory Object Control State', start=196, end=202, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Dynamic State Base Address', start=204, end=255, gen_type=gen.Address())) \
    .add(gen.Field(name='Indirect Object Base Address Modify Enable', start=256, end=256, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Indirect Object Memory Object Control State', start=260, end=266, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Indirect Object Base Address', start=268, end=319, gen_type=gen.Address())) \
    .add(gen.Field(name='Instruction Base Address Modify Enable', start=320, end=320, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Instruction Memory Object Control State', start=324, end=330, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Instruction Base Address', start=332, end=383, gen_type=gen.Address())) \
    .add(gen.Field(name='General State Buffer Size Modify Enable', start=384, end=384, gen_type=gen.Boolean())) \
    .add(gen.Field(name='General State Buffer Size', start=396, end=415, gen_type=gen.UInteger(20))) \
    .add(gen.Field(name='Dynamic State Buffer Size Modify Enable', start=416, end=416, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Dynamic State Buffer Size', start=428, end=447, gen_type=gen.UInteger(20))) \
    .add(gen.Field(name='Indirect Object Buffer Size Modify Enable', start=448, end=448, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Indirect Object Buffer Size', start=460, end=479, gen_type=gen.UInteger(20))) \
    .add(gen.Field(name='Instruction Buffer size Modify Enable', start=480, end=480, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Instruction Buffer Size', start=492, end=511, gen_type=gen.UInteger(20))) \
    .add(gen.Field(name='Bindless Surface State Base Address Modify Enable', start=512, end=512, gen_type=gen.Boolean())) \
    .add(gen.Field(name='Bindless Surface State Memory Object Control State', start=516, end=522, gen_type=cs.type('MEMORY_OBJECT_CONTROL_STATE'))) \
    .add(gen.Field(name='Bindless Surface State Base Address', start=524, end=575, gen_type=gen.Address())) \
    .add(gen.Field(name='Bindless Surface State Size', start=588, end=607, gen_type=gen.UInteger(20)))
cs.instruction(name='STATE_PREFETCH', length=2, bias=2) \
    .add(gen.Field(name='DWord Length', default=0, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=3, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=0, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=0, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Prefetch Count', start=32, end=34, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Prefetch Pointer', start=38, end=63, gen_type=gen.Address()))
cs.instruction(name='STATE_SIP', length=3, bias=2) \
    .add(gen.Field(name='DWord Length', default=1, start=0, end=7, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Sub Opcode', default=2, start=16, end=23, gen_type=gen.UInteger(8))) \
    .add(gen.Field(name='3D Command Opcode', default=1, start=24, end=26, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='Command SubType', default=0, start=27, end=28, gen_type=gen.UInteger(2))) \
    .add(gen.Field(name='Command Type', default=3, start=29, end=31, gen_type=gen.UInteger(3))) \
    .add(gen.Field(name='System Instruction Pointer', start=36, end=95, gen_type=gen.OffsetFrom()))
