#ifndef FOREACH_PREPROCESSOR_H
#define FOREACH_PREPROCESSOR_H


#define PP_NARG(...) \
		 PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
		 PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
		  _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
		 _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
		 _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
		 _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
		 _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
		 _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
		 _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
		 63,62,61,60,                   \
		 59,58,57,56,55,54,53,52,51,50, \
		 49,48,47,46,45,44,43,42,41,40, \
		 39,38,37,36,35,34,33,32,31,30, \
		 29,28,27,26,25,24,23,22,21,20, \
		 19,18,17,16,15,14,13,12,11,10, \
		 9,8,7,6,5,4,3,2,1,0

#define FOREACH_1(macro, val) macro(val)
#define FOREACH_2(macro, val, ...) macro(val), FOREACH_1(macro, ##__VA_ARGS__)
#define FOREACH_3(macro, val, ...) macro(val), FOREACH_2(macro, ##__VA_ARGS__)
#define FOREACH_4(macro, val, ...) macro(val), FOREACH_3(macro, ##__VA_ARGS__)
#define FOREACH_5(macro, val, ...) macro(val), FOREACH_4(macro, ##__VA_ARGS__)
#define FOREACH_6(macro, val, ...) macro(val), FOREACH_5(macro, ##__VA_ARGS__)
#define FOREACH_7(macro, val, ...) macro(val), FOREACH_6(macro, ##__VA_ARGS__)
#define FOREACH_8(macro, val, ...) macro(val), FOREACH_7(macro, ##__VA_ARGS__)
#define FOREACH_9(macro, val, ...) macro(val), FOREACH_8(macro, ##__VA_ARGS__)
#define FOREACH_10(macro, val, ...) macro(val), FOREACH_9(macro, ##__VA_ARGS__)
#define FOREACH_11(macro, val, ...) macro(val), FOREACH_10(macro, ##__VA_ARGS__)
#define FOREACH_12(macro, val, ...) macro(val), FOREACH_11(macro, ##__VA_ARGS__)
#define FOREACH_13(macro, val, ...) macro(val), FOREACH_12(macro, ##__VA_ARGS__)
#define FOREACH_14(macro, val, ...) macro(val), FOREACH_13(macro, ##__VA_ARGS__)
#define FOREACH_15(macro, val, ...) macro(val), FOREACH_14(macro, ##__VA_ARGS__)
#define FOREACH_16(macro, val, ...) macro(val), FOREACH_15(macro, ##__VA_ARGS__)
#define FOREACH_17(macro, val, ...) macro(val), FOREACH_16(macro, ##__VA_ARGS__)
#define FOREACH_18(macro, val, ...) macro(val), FOREACH_17(macro, ##__VA_ARGS__)
#define FOREACH_19(macro, val, ...) macro(val), FOREACH_18(macro, ##__VA_ARGS__)
#define FOREACH_20(macro, val, ...) macro(val), FOREACH_19(macro, ##__VA_ARGS__)
#define FOREACH_21(macro, val, ...) macro(val), FOREACH_20(macro, ##__VA_ARGS__)
#define FOREACH_22(macro, val, ...) macro(val), FOREACH_21(macro, ##__VA_ARGS__)
#define FOREACH_23(macro, val, ...) macro(val), FOREACH_22(macro, ##__VA_ARGS__)
#define FOREACH_24(macro, val, ...) macro(val), FOREACH_23(macro, ##__VA_ARGS__)
#define FOREACH_25(macro, val, ...) macro(val), FOREACH_24(macro, ##__VA_ARGS__)
#define FOREACH_26(macro, val, ...) macro(val), FOREACH_25(macro, ##__VA_ARGS__)
#define FOREACH_27(macro, val, ...) macro(val), FOREACH_26(macro, ##__VA_ARGS__)
#define FOREACH_28(macro, val, ...) macro(val), FOREACH_27(macro, ##__VA_ARGS__)
#define FOREACH_29(macro, val, ...) macro(val), FOREACH_28(macro, ##__VA_ARGS__)
#define FOREACH_30(macro, val, ...) macro(val), FOREACH_29(macro, ##__VA_ARGS__)
#define FOREACH_31(macro, val, ...) macro(val), FOREACH_30(macro, ##__VA_ARGS__)
#define FOREACH_32(macro, val, ...) macro(val), FOREACH_31(macro, ##__VA_ARGS__)
#define FOREACH_33(macro, val, ...) macro(val), FOREACH_32(macro, ##__VA_ARGS__)
#define FOREACH_34(macro, val, ...) macro(val), FOREACH_33(macro, ##__VA_ARGS__)
#define FOREACH_35(macro, val, ...) macro(val), FOREACH_34(macro, ##__VA_ARGS__)
#define FOREACH_36(macro, val, ...) macro(val), FOREACH_35(macro, ##__VA_ARGS__)
#define FOREACH_37(macro, val, ...) macro(val), FOREACH_36(macro, ##__VA_ARGS__)
#define FOREACH_38(macro, val, ...) macro(val), FOREACH_37(macro, ##__VA_ARGS__)
#define FOREACH_39(macro, val, ...) macro(val), FOREACH_38(macro, ##__VA_ARGS__)
#define FOREACH_40(macro, val, ...) macro(val), FOREACH_39(macro, ##__VA_ARGS__)
#define FOREACH_41(macro, val, ...) macro(val), FOREACH_40(macro, ##__VA_ARGS__)
#define FOREACH_42(macro, val, ...) macro(val), FOREACH_41(macro, ##__VA_ARGS__)
#define FOREACH_43(macro, val, ...) macro(val), FOREACH_42(macro, ##__VA_ARGS__)
#define FOREACH_44(macro, val, ...) macro(val), FOREACH_43(macro, ##__VA_ARGS__)
#define FOREACH_45(macro, val, ...) macro(val), FOREACH_44(macro, ##__VA_ARGS__)
#define FOREACH_46(macro, val, ...) macro(val), FOREACH_45(macro, ##__VA_ARGS__)
#define FOREACH_47(macro, val, ...) macro(val), FOREACH_46(macro, ##__VA_ARGS__)
#define FOREACH_48(macro, val, ...) macro(val), FOREACH_47(macro, ##__VA_ARGS__)
#define FOREACH_49(macro, val, ...) macro(val), FOREACH_48(macro, ##__VA_ARGS__)
#define FOREACH_50(macro, val, ...) macro(val), FOREACH_49(macro, ##__VA_ARGS__)
#define FOREACH_51(macro, val, ...) macro(val), FOREACH_50(macro, ##__VA_ARGS__)
#define FOREACH_52(macro, val, ...) macro(val), FOREACH_51(macro, ##__VA_ARGS__)
#define FOREACH_53(macro, val, ...) macro(val), FOREACH_52(macro, ##__VA_ARGS__)
#define FOREACH_54(macro, val, ...) macro(val), FOREACH_53(macro, ##__VA_ARGS__)
#define FOREACH_55(macro, val, ...) macro(val), FOREACH_54(macro, ##__VA_ARGS__)
#define FOREACH_56(macro, val, ...) macro(val), FOREACH_55(macro, ##__VA_ARGS__)
#define FOREACH_57(macro, val, ...) macro(val), FOREACH_56(macro, ##__VA_ARGS__)
#define FOREACH_58(macro, val, ...) macro(val), FOREACH_57(macro, ##__VA_ARGS__)
#define FOREACH_59(macro, val, ...) macro(val), FOREACH_58(macro, ##__VA_ARGS__)
#define FOREACH_60(macro, val, ...) macro(val), FOREACH_59(macro, ##__VA_ARGS__)
#define FOREACH_61(macro, val, ...) macro(val), FOREACH_60(macro, ##__VA_ARGS__)
#define FOREACH_62(macro, val, ...) macro(val), FOREACH_61(macro, ##__VA_ARGS__)
#define FOREACH_63(macro, val, ...) macro(val), FOREACH_62(macro, ##__VA_ARGS__)


#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define FOREACH_(N, macro, ...) CONCATENATE(FOREACH_, N)(macro, __VA_ARGS__)
#define FOREACH(macro, ...) FOREACH_(PP_NARG(__VA_ARGS__), macro, __VA_ARGS__)

#endif // FOREACH_PREPROCESSOR_H
