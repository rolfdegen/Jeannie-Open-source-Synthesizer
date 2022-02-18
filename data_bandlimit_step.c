#include <stdint.h>

// band limited step function, 17kHz limit for 44.1kSPS.
// step goes from -BASE_AMPLITUDE to +BASE_AMPLITUDE nominally
// [ only store the first half of the table due to its odd symmetry ]
// the extra first entry allows for the interpolation code not to overrun on reflected table,
// the last extra entry allows for interpolation not to overrun when non-reflected

const int16_t step_table [258] =  //   size is N/2+2  where  N = SUPPORT * SCALE * 2
{
 -24576,
 -24551, -24550, -24549, -24548, -24546, -24544, -24542, -24540, -24538, -24537, -24537, -24537, -24539, -24543, -24547, -24553,
 -24560, -24569, -24579, -24590, -24602, -24614, -24627, -24639, -24651, -24663, -24673, -24682, -24689, -24693, -24695, -24694,
 -24691, -24684, -24674, -24661, -24646, -24627, -24607, -24585, -24561, -24536, -24511, -24486, -24463, -24441, -24421, -24403,
 -24390, -24380, -24374, -24373, -24377, -24386, -24400, -24418, -24441, -24469, -24499, -24533, -24569, -24607, -24645, -24683,
 -24720, -24755, -24786, -24814, -24836, -24854, -24865, -24870, -24867, -24858, -24842, -24819, -24789, -24753, -24712, -24666,
 -24617, -24566, -24513, -24460, -24408, -24359, -24314, -24274, -24239, -24212, -24193, -24182, -24181, -24189, -24206, -24233,
 -24268, -24312, -24364, -24422, -24485, -24552, -24621, -24691, -24760, -24826, -24889, -24945, -24994, -25034, -25064, -25084,
 -25091, -25087, -25070, -25041, -25000, -24948, -24886, -24815, -24736, -24652, -24564, -24473, -24383, -24296, -24213, -24136,
 -24068, -24011, -23965, -23934, -23916, -23914, -23928, -23958, -24003, -24064, -24137, -24224, -24320, -24426, -24537, -24653,
 -24769, -24884, -24994, -25097, -25190, -25271, -25337, -25387, -25418, -25430, -25422, -25394, -25345, -25276, -25188, -25084,
 -24964, -24832, -24691, -24542, -24391, -24240, -24092, -23953, -23824, -23710, -23613, -23537, -23484, -23456, -23454, -23478,
 -23531, -23610, -23714, -23843, -23993, -24162, -24347, -24542, -24745, -24950, -25153, -25348, -25531, -25697, -25841, -25960,
 -26048, -26104, -26125, -26108, -26054, -25961, -25831, -25665, -25466, -25238, -24984, -24710, -24422, -24125, -23828, -23535,
 -23256, -22998, -22766, -22570, -22414, -22304, -22246, -22244, -22300, -22417, -22594, -22831, -23126, -23475, -23872, -24311,
 -24785, -25283, -25795, -26311, -26816, -27300, -27747, -28143, -28476, -28731, -28894, -28953, -28896, -28711, -28390, -27923,
 -27305, -26530, -25596, -24501, -23246, -21836, -20274, -18569, -16729, -14766, -12691, -10521,  -8269,  -5953,  -3591,  -1200,
  1200
};