#define POS1	1
#define SL1	29
#define SL2	1
#define SR1	3
#define SR2	16
#define MSK1	UINT64_C(0x3ff7fffffff5fbff)
#define MSK2	UINT64_C(0xffff7fffffefffff)
#define MSK32_1	0x3ff7ffffU
#define MSK32_2	0xfff5fbffU
#define MSK32_3	0xffff7fffU
#define MSK32_4	0xffefffffU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x0004940000000000)
#define IDSTR	"dSFMT-607:1-29-1-3-16:3ff7fffffff5fbff-ffff7fffffefffff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 5
#define SL1_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xe0000000U,0xffffffffU,0xe0000000U)
#define SL2_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define ALTI_SR1 3
#define SR1_MSK \
(vector unsigned int)(0x1ff7ffffU,0xfff5fbffU,0x1fff7fffU,0xffefffffU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)