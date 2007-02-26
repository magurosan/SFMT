#define POS1	431
#define SL1	9
#define SL2	3
#define SR1	4
#define SR2	16
#define MSK1	UINT64_C(0x7effffffffd775ff)
#define MSK2	UINT64_C(0xffffffb7ff75bfff)
#define MSK32_1	0x7effffffU
#define MSK32_2	0xffd775ffU
#define MSK32_3	0xffffffb7U
#define MSK32_4	0xff75bfffU
#define PCV1	UINT64_C(0x000cc40542000001)
#define PCV2	UINT64_C(0x000e712ea08fa206)
#define IDSTR	"dSFMT-86243:431-9-3-4-16:7effffffffd775ff-ffffffb7ff75bfff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 1
#define SL1_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xfffffe00U,0xffffffffU,0xfffffe00U)
#define SL2_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define ALTI_SR1 4
#define SR1_MSK \
(vector unsigned int)(0x0effffffU,0xffd775ffU,0x0fffffb7U,0xff75bfffU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)