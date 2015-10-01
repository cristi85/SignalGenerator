/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "types.h"
#include "config.h"
#include "stm32f0xx_it.h"
#include "delay.h"
#include "errors.h"
#include "config.h"
#include "timeout.h"
#include "hd44780.h"
#include "string.h"

#define VREFINT_CAL   (u32)0x1FFFF7BA

#define UART_CMD_1  (u8)0x01
#define UART_CMD_2  (u8)0x10
#define UART_CMD_3  (u8)0x02

const u16 Sinus12bit65[65] = {
                              1800, 1974, 2146, 2315, 2479, 2637, 2786, 2927, 3057,
                              3176, 3281, 3373, 3450, 3512 , 3558, 3587, 3599, 3595,
                              3574, 3537, 3483, 3414, 3329, 3230, 3118, 2994, 2858,
                              2713, 2559, 2397, 2231, 2060, 1887, 1713, 1540, 1369,
                              1203, 1041, 887, 742, 606, 482, 370, 271, 186, 117, 63, 26,
                              5, 1, 13, 42, 88, 150, 227, 319, 424, 543, 673, 814, 963,
                              1121, 1285, 1454, 1626
                             };
const u16 Sinus12bit130[130] = {
                                1800, 1887, 1974, 2060, 2146, 2231, 2315, 2397, 2479,
                                2559, 2637, 2713, 2786, 2858, 2927, 2994, 3057, 3118,
                                3176, 3230, 3281, 3329, 3373, 3414, 3450, 3483, 3512,
                                3537, 3558, 3574, 3587, 3595, 3599, 3599, 3595, 3587,
                                3574, 3558, 3537, 3512, 3483, 3450, 3414, 3373, 3329,
                                3281, 3230, 3176, 3118, 3057, 2994, 2927, 2858, 2786,
                                2713, 2637, 2559, 2479, 2397, 2315, 2231, 2146, 2060,
                                1974, 1887, 1800, 1713, 1626, 1540, 1454, 1369, 1285,
                                1203, 1121, 1041, 963, 887, 814, 742, 673, 606, 543, 482,
                                424, 370, 319, 271, 227, 186, 150, 117, 88, 63, 42, 26, 13,
                                5, 1, 1, 5, 13, 26, 42, 63, 88, 117, 150, 186, 227, 271, 319,
                                370, 424, 482, 543, 606, 673, 742, 814, 887, 963, 1041,
                                1121, 1203, 1285, 1369, 1454, 1540, 1626, 1713
                               };
const u16 SawTooth12bit65[65] = {
                                0, 63, 126, 190, 253, 317, 380, 444, 507, 571, 634,
                                698, 761, 825, 888, 952, 1015, 1079, 1142, 1206, 1269,
                                1333, 1396, 1460, 1523, 1587, 1650, 1714, 1777, 1841,
                                1904, 1968, 2031, 2095, 2158, 2222, 2285, 2349, 2412,
                                2476, 2539, 2603, 2666, 2730, 2793, 2856, 2920, 2983,
                                3047, 3110, 3174, 3237, 3301, 3364, 3428, 3491, 3555,
                                3618, 3682, 3745, 3809, 3872, 3936, 3999, 4063
                               };
const u16 SawTooth12bit130[130] = {
                                0, 31, 63, 95, 126, 158, 190, 222, 253, 285, 317, 349,
                                380, 412, 444, 476, 507, 539, 571, 603, 634, 666, 698,
                                730, 761, 793, 825, 857, 888, 920, 952, 984, 1015, 1047,
                                1079, 1111, 1142, 1174, 1206, 1238, 1269, 1301, 1333,
                                1365, 1396, 1428, 1460, 1491, 1523, 1555, 1587, 1618,
                                1650, 1682, 1714, 1745, 1777, 1809, 1841, 1872, 1904,
                                1936, 1968, 1999, 2031, 2063, 2095, 2126, 2158, 2190,
                                2222, 2253, 2285, 2317, 2349, 2380, 2412, 2444, 2476,
                                2507, 2539, 2571, 2603, 2634, 2666, 2698, 2730, 2761,
                                2793, 2825, 2856, 2888, 2920, 2952, 2983, 3015, 3047,
                                3079, 3110, 3142, 3174, 3206, 3237, 3269, 3301, 3333,
                                3364, 3396, 3428, 3460, 3491, 3523, 3555, 3587, 3618,
                                3650, 3682, 3714, 3745, 3777, 3809, 3841, 3872, 3904,
                                3936, 3968, 3999, 4031, 4063, 4095
                               };
const u16 Triang12bit65[65] = {
                              0, 120, 240, 360, 480, 600, 720, 840, 960, 1080, 1200, 
                              1320, 1440, 1560, 1680, 1800, 1920, 2040, 2160, 2280, 2400,
                              2520,  2640,  2760,  2880, 3000, 3120, 3240, 3360, 
                              3480, 3600, 3720, 3840, 3720, 3600, 3480, 3360, 3240, 3120, 
                              3000, 2880, 2760, 2640, 2520, 2400, 2280, 2160, 2040, 1920, 
                              1800, 1680, 1560, 1440, 1320, 1200, 1080, 960, 840, 720, 600,
                              480, 360, 240, 120, 60
                             };
const u16 Triang12bit130[130] = {
                              0, 60, 120, 180, 240, 300, 360, 420, 480, 540, 600, 660,
                              720, 780, 840, 900, 960, 1020, 1080, 1140, 1200, 1260,
                              1320, 1380, 1440, 1500, 1560, 1620, 1680, 1740, 1800,
                              1860, 1920, 1980, 2040, 2100, 2160, 2220, 2280, 2340,
                              2400, 2460, 2520, 2580, 2640, 2700, 2760, 2820, 2880,
                              2940, 3000, 3060, 3120, 3180, 3240, 3300, 3360, 3420,
                              3480, 3540, 3600, 3660, 3720, 3780, 3840, 3900, 3840,
                              3780, 3720, 3660, 3600, 3540, 3480, 3420, 3360, 3300,
                              3240, 3180, 3120, 3060, 3000, 2940, 2880, 2820, 2760,
                              2700, 2640, 2580, 2520, 2460, 2400, 2340, 2280, 2220,
                              2160, 2100, 2040, 1980, 1920, 1860, 1800, 1740, 1680,
                              1620, 1560, 1500, 1440, 1380, 1320, 1260, 1200, 1140,
                              1080, 1020, 960, 900, 840, 780, 720, 660, 600, 540, 480,
                              420, 360, 300, 240, 180, 120, 60
                             };
const u16 SinCard12bit65[65] = {
                               1032, 1056, 1019, 931, 825, 741, 715, 762, 868, 996, 1096, 1125, 
                               1065, 931, 771, 645, 610, 690, 868, 1084, 1257, 1307, 1194, 
                               931, 597, 312, 207, 381, 868, 1613, 2485, 3302, 3883, 4095, 
                               3883, 3302, 2485, 1613, 868, 381, 207, 312, 597, 931, 1194, 
                               1307, 1257, 1084, 868, 690, 610, 645, 771, 931, 1065, 1125,
                               1096, 996, 868, 762, 715, 741, 825, 931, 1019
                              };
const u16 SinCard12bit130[130] = {892, 886, 864, 828, 780, 726, 670, 617, 574, 545, 532,
                               539, 565, 608, 664, 729, 797, 860, 913, 950, 966, 959,
                               928, 876, 807, 726, 641, 560, 492, 444, 421, 428, 466,
                               532, 622, 729, 843, 954, 1050, 1120, 1156, 1152, 1104,
                               1013, 884, 726, 551, 376, 216, 90, 13, 0, 61, 204, 428,
                               729, 1097, 1517, 1968, 2428, 2872, 3276, 3618, 3877,
                               4039, 4094, 4039, 3877, 3618, 3276, 2872, 2428, 1968,
                               1517, 1097, 729, 428, 204, 61, 0, 13, 90, 216, 376, 551,
                               726, 884, 1013, 1104, 1152, 1156, 1120, 1050, 954, 843,
                               729, 622, 532, 466, 428, 421, 444, 492, 560, 641, 726,
                               807, 876, 928, 959, 966, 950, 913, 860, 797, 729, 664,
                               608, 565, 539, 532, 545, 574, 617, 670, 726, 780, 828,
                               864, 886
                              };


const u32 TMR_PSC_ARR_tab130_1HzTo5Hz[5][2] = {/* PSC and ARR corresponding to frequencies from 1Hz to 5Hz */
                                               {9, 36923}/* 1Hz */, {4, 36923}/* 2Hz */, {1, 61538}/* 3Hz */, {2, 30769}/* 4Hz */, {1, 36923}/* 5Hz */
};
const u16 TMR_ARR_tab130_6HzTo628Hz[623] = {/* ARR corresponding to frequencies from 6Hz to 628Hz */
                                            61538/* 6Hz */, 52747/* 7Hz */, 46153/* 8Hz */, \
                                            41025/* 9Hz */, 36923/* 10Hz */, 33566/* 11Hz */, 30769/* 12Hz */, 28402/* 13Hz */, 26373/* 14Hz */, 24615/* 15Hz */, 23076/* 16Hz */, \
                                            21719/* 17Hz */, 20512/* 18Hz */, 19433/* 19Hz */, 18461/* 20Hz */, 17582/* 21Hz */, 16783/* 22Hz */, 16053/* 23Hz */, 15384/* 24Hz */, \
                                            14769/* 25Hz */, 14201/* 26Hz */, 13675/* 27Hz */, 13186/* 28Hz */, 12732/* 29Hz */, 12307/* 30Hz */, 11910/* 31Hz */, 11538/* 32Hz */, \
                                            11188/* 33Hz */, 10859/* 34Hz */, 10549/* 35Hz */, 10256/* 36Hz */, 9979/* 37Hz */, 9716/* 38Hz */, 9467/* 39Hz */, 9230/* 40Hz */, \
                                            9005/* 41Hz */, 8791/* 42Hz */, 8586/* 43Hz */, 8391/* 44Hz */, 8205/* 45Hz */, 8026/* 46Hz */, 7855/* 47Hz */, 7692/* 48Hz */, \
                                            7535/* 49Hz */, 7384/* 50Hz */, 7239/* 51Hz */, 7100/* 52Hz */, 6966/* 53Hz */, 6837/* 54Hz */, 6713/* 55Hz */, 6593/* 56Hz */, \
                                            6477/* 57Hz */, 6366/* 58Hz */, 6258/* 59Hz */, 6153/* 60Hz */, 6052/* 61Hz */, 5955/* 62Hz */, 5860/* 63Hz */, 5769/* 64Hz */, \
                                            5680/* 65Hz */, 5594/* 66Hz */, 5510/* 67Hz */, 5429/* 68Hz */, 5351/* 69Hz */, 5274/* 70Hz */, 5200/* 71Hz */, 5128/* 72Hz */, \
                                            5057/* 73Hz */, 4989/* 74Hz */, 4923/* 75Hz */, 4858/* 76Hz */, 4795/* 77Hz */, 4733/* 78Hz */, 4673/* 79Hz */, 4615/* 80Hz */, \
                                            4558/* 81Hz */, 4502/* 82Hz */, 4448/* 83Hz */, 4395/* 84Hz */, 4343/* 85Hz */, 4293/* 86Hz */, 4244/* 87Hz */, 4195/* 88Hz */, \
                                            4148/* 89Hz */, 4102/* 90Hz */, 4057/* 91Hz */, 4013/* 92Hz */, 3970/* 93Hz */, 3927/* 94Hz */, 3886/* 95Hz */, 3846/* 96Hz */, \
                                            3806/* 97Hz */, 3767/* 98Hz */, 3729/* 99Hz */, 3692/* 100Hz */, 3655/* 101Hz */, 3619/* 102Hz */, 3584/* 103Hz */, 3550/* 104Hz */, \
                                            3516/* 105Hz */, 3483/* 106Hz */, 3450/* 107Hz */, 3418/* 108Hz */, 3387/* 109Hz */, 3356/* 110Hz */, 3326/* 111Hz */, 3296/* 112Hz */, \
                                            3267/* 113Hz */, 3238/* 114Hz */, 3210/* 115Hz */, 3183/* 116Hz */, 3155/* 117Hz */, 3129/* 118Hz */, 3102/* 119Hz */, 3076/* 120Hz */, \
                                            3051/* 121Hz */, 3026/* 122Hz */, 3001/* 123Hz */, 2977/* 124Hz */, 2953/* 125Hz */, 2930/* 126Hz */, 2907/* 127Hz */, 2884/* 128Hz */, \
                                            2862/* 129Hz */, 2840/* 130Hz */, 2818/* 131Hz */, 2797/* 132Hz */, 2776/* 133Hz */, 2755/* 134Hz */, 2735/* 135Hz */, 2714/* 136Hz */, \
                                            2695/* 137Hz */, 2675/* 138Hz */, 2656/* 139Hz */, 2637/* 140Hz */, 2618/* 141Hz */, 2600/* 142Hz */, 2582/* 143Hz */, 2564/* 144Hz */, \
                                            2546/* 145Hz */, 2528/* 146Hz */, 2511/* 147Hz */, 2494/* 148Hz */, 2478/* 149Hz */, 2461/* 150Hz */, 2445/* 151Hz */, 2429/* 152Hz */, \
                                            2413/* 153Hz */, 2397/* 154Hz */, 2382/* 155Hz */, 2366/* 156Hz */, 2351/* 157Hz */, 2336/* 158Hz */, 2322/* 159Hz */, 2307/* 160Hz */, \
                                            2293/* 161Hz */, 2279/* 162Hz */, 2265/* 163Hz */, 2251/* 164Hz */, 2237/* 165Hz */, 2224/* 166Hz */, 2210/* 167Hz */, 2197/* 168Hz */, \
                                            2184/* 169Hz */, 2171/* 170Hz */, 2159/* 171Hz */, 2146/* 172Hz */, 2134/* 173Hz */, 2122/* 174Hz */, 2109/* 175Hz */, 2097/* 176Hz */, \
                                            2086/* 177Hz */, 2074/* 178Hz */, 2062/* 179Hz */, 2051/* 180Hz */, 2039/* 181Hz */, 2028/* 182Hz */, 2017/* 183Hz */, 2006/* 184Hz */, \
                                            1995/* 185Hz */, 1985/* 186Hz */, 1974/* 187Hz */, 1963/* 188Hz */, 1953/* 189Hz */, 1943/* 190Hz */, 1933/* 191Hz */, 1923/* 192Hz */, \
                                            1913/* 193Hz */, 1903/* 194Hz */, 1893/* 195Hz */, 1883/* 196Hz */, 1874/* 197Hz */, 1864/* 198Hz */, 1855/* 199Hz */, 1846/* 200Hz */, \
                                            1836/* 201Hz */, 1827/* 202Hz */, 1818/* 203Hz */, 1809/* 204Hz */, 1801/* 205Hz */, 1792/* 206Hz */, 1783/* 207Hz */, 1775/* 208Hz */, \
                                            1766/* 209Hz */, 1758/* 210Hz */, 1749/* 211Hz */, 1741/* 212Hz */, 1733/* 213Hz */, 1725/* 214Hz */, 1717/* 215Hz */, 1709/* 216Hz */, \
                                            1701/* 217Hz */, 1693/* 218Hz */, 1685/* 219Hz */, 1678/* 220Hz */, 1670/* 221Hz */, 1663/* 222Hz */, 1655/* 223Hz */, 1648/* 224Hz */, \
                                            1641/* 225Hz */, 1633/* 226Hz */, 1626/* 227Hz */, 1619/* 228Hz */, 1612/* 229Hz */, 1605/* 230Hz */, 1598/* 231Hz */, 1591/* 232Hz */, \
                                            1584/* 233Hz */, 1577/* 234Hz */, 1571/* 235Hz */, 1564/* 236Hz */, 1557/* 237Hz */, 1551/* 238Hz */, 1544/* 239Hz */, 1538/* 240Hz */, \
                                            1532/* 241Hz */, 1525/* 242Hz */, 1519/* 243Hz */, 1513/* 244Hz */, 1507/* 245Hz */, 1500/* 246Hz */, 1494/* 247Hz */, 1488/* 248Hz */, \
                                            1482/* 249Hz */, 1476/* 250Hz */, 1471/* 251Hz */, 1465/* 252Hz */, 1459/* 253Hz */, 1453/* 254Hz */, 1447/* 255Hz */, 1442/* 256Hz */, \
                                            1436/* 257Hz */, 1431/* 258Hz */, 1425/* 259Hz */, 1420/* 260Hz */, 1414/* 261Hz */, 1409/* 262Hz */, 1403/* 263Hz */, 1398/* 264Hz */, \
                                            1393/* 265Hz */, 1388/* 266Hz */, 1382/* 267Hz */, 1377/* 268Hz */, 1372/* 269Hz */, 1367/* 270Hz */, 1362/* 271Hz */, 1357/* 272Hz */, \
                                            1352/* 273Hz */, 1347/* 274Hz */, 1342/* 275Hz */, 1337/* 276Hz */, 1332/* 277Hz */, 1328/* 278Hz */, 1323/* 279Hz */, 1318/* 280Hz */, \
                                            1313/* 281Hz */, 1309/* 282Hz */, 1304/* 283Hz */, 1300/* 284Hz */, 1295/* 285Hz */, 1291/* 286Hz */, 1286/* 287Hz */, 1282/* 288Hz */, \
                                            1277/* 289Hz */, 1273/* 290Hz */, 1268/* 291Hz */, 1264/* 292Hz */, 1260/* 293Hz */, 1255/* 294Hz */, 1251/* 295Hz */, 1247/* 296Hz */, \
                                            1243/* 297Hz */, 1239/* 298Hz */, 1234/* 299Hz */, 1230/* 300Hz */, 1226/* 301Hz */, 1222/* 302Hz */, 1218/* 303Hz */, 1214/* 304Hz */, \
                                            1210/* 305Hz */, 1206/* 306Hz */, 1202/* 307Hz */, 1198/* 308Hz */, 1194/* 309Hz */, 1191/* 310Hz */, 1187/* 311Hz */, 1183/* 312Hz */, \
                                            1179/* 313Hz */, 1175/* 314Hz */, 1172/* 315Hz */, 1168/* 316Hz */, 1164/* 317Hz */, 1161/* 318Hz */, 1157/* 319Hz */, 1153/* 320Hz */, \
                                            1150/* 321Hz */, 1146/* 322Hz */, 1143/* 323Hz */, 1139/* 324Hz */, 1136/* 325Hz */, 1132/* 326Hz */, 1129/* 327Hz */, 1125/* 328Hz */, \
                                            1122/* 329Hz */, 1118/* 330Hz */, 1115/* 331Hz */, 1112/* 332Hz */, 1108/* 333Hz */, 1105/* 334Hz */, 1102/* 335Hz */, 1098/* 336Hz */, \
                                            1095/* 337Hz */, 1092/* 338Hz */, 1089/* 339Hz */, 1085/* 340Hz */, 1082/* 341Hz */, 1079/* 342Hz */, 1076/* 343Hz */, 1073/* 344Hz */, \
                                            1070/* 345Hz */, 1067/* 346Hz */, 1064/* 347Hz */, 1061/* 348Hz */, 1057/* 349Hz */, 1054/* 350Hz */, 1051/* 351Hz */, 1048/* 352Hz */, \
                                            1045/* 353Hz */, 1043/* 354Hz */, 1040/* 355Hz */, 1037/* 356Hz */, 1034/* 357Hz */, 1031/* 358Hz */, 1028/* 359Hz */, 1025/* 360Hz */, \
                                            1022/* 361Hz */, 1019/* 362Hz */, 1017/* 363Hz */, 1014/* 364Hz */, 1011/* 365Hz */, 1008/* 366Hz */, 1006/* 367Hz */, 1003/* 368Hz */, \
                                            1000/* 369Hz */, 997/* 370Hz */, 995/* 371Hz */, 992/* 372Hz */, 989/* 373Hz */, 987/* 374Hz */, 984/* 375Hz */, 981/* 376Hz */, \
                                            979/* 377Hz */, 976/* 378Hz */, 974/* 379Hz */, 971/* 380Hz */, 969/* 381Hz */, 966/* 382Hz */, 964/* 383Hz */, 961/* 384Hz */, \
                                            959/* 385Hz */, 956/* 386Hz */, 954/* 387Hz */, 951/* 388Hz */, 949/* 389Hz */, 946/* 390Hz */, 944/* 391Hz */, 941/* 392Hz */, \
                                            939/* 393Hz */, 937/* 394Hz */, 934/* 395Hz */, 932/* 396Hz */, 930/* 397Hz */, 927/* 398Hz */, 925/* 399Hz */, 923/* 400Hz */, \
                                            920/* 401Hz */, 918/* 402Hz */, 916/* 403Hz */, 913/* 404Hz */, 911/* 405Hz */, 909/* 406Hz */, 907/* 407Hz */, 904/* 408Hz */, \
                                            902/* 409Hz */, 900/* 410Hz */, 898/* 411Hz */, 896/* 412Hz */, 894/* 413Hz */, 891/* 414Hz */, 889/* 415Hz */, 887/* 416Hz */, \
                                            885/* 417Hz */, 883/* 418Hz */, 881/* 419Hz */, 879/* 420Hz */, 877/* 421Hz */, 874/* 422Hz */, 872/* 423Hz */, 870/* 424Hz */, \
                                            868/* 425Hz */, 866/* 426Hz */, 864/* 427Hz */, 862/* 428Hz */, 860/* 429Hz */, 858/* 430Hz */, 856/* 431Hz */, 854/* 432Hz */, \
                                            852/* 433Hz */, 850/* 434Hz */, 848/* 435Hz */, 846/* 436Hz */, 844/* 437Hz */, 842/* 438Hz */, 841/* 439Hz */, 839/* 440Hz */, \
                                            837/* 441Hz */, 835/* 442Hz */, 833/* 443Hz */, 831/* 444Hz */, 829/* 445Hz */, 827/* 446Hz */, 826/* 447Hz */, 824/* 448Hz */, \
                                            822/* 449Hz */, 820/* 450Hz */, 818/* 451Hz */, 816/* 452Hz */, 815/* 453Hz */, 813/* 454Hz */, 811/* 455Hz */, 809/* 456Hz */, \
                                            807/* 457Hz */, 806/* 458Hz */, 804/* 459Hz */, 802/* 460Hz */, 800/* 461Hz */, 799/* 462Hz */, 797/* 463Hz */, 795/* 464Hz */, \
                                            794/* 465Hz */, 792/* 466Hz */, 790/* 467Hz */, 788/* 468Hz */, 787/* 469Hz */, 785/* 470Hz */, 783/* 471Hz */, 782/* 472Hz */, \
                                            780/* 473Hz */, 778/* 474Hz */, 777/* 475Hz */, 775/* 476Hz */, 774/* 477Hz */, 772/* 478Hz */, 770/* 479Hz */, 769/* 480Hz */, \
                                            767/* 481Hz */, 766/* 482Hz */, 764/* 483Hz */, 762/* 484Hz */, 761/* 485Hz */, 759/* 486Hz */, 758/* 487Hz */, 756/* 488Hz */, \
                                            755/* 489Hz */, 753/* 490Hz */, 751/* 491Hz */, 750/* 492Hz */, 748/* 493Hz */, 747/* 494Hz */, 745/* 495Hz */, 744/* 496Hz */, \
                                            742/* 497Hz */, 741/* 498Hz */, 739/* 499Hz */, 738/* 500Hz */, 736/* 501Hz */, 735/* 502Hz */, 734/* 503Hz */, 732/* 504Hz */, \
                                            731/* 505Hz */, 729/* 506Hz */, 728/* 507Hz */, 726/* 508Hz */, 725/* 509Hz */, 723/* 510Hz */, 722/* 511Hz */, 721/* 512Hz */, \
                                            719/* 513Hz */, 718/* 514Hz */, 716/* 515Hz */, 715/* 516Hz */, 714/* 517Hz */, 712/* 518Hz */, 711/* 519Hz */, 710/* 520Hz */, \
                                            708/* 521Hz */, 707/* 522Hz */, 705/* 523Hz */, 704/* 524Hz */, 703/* 525Hz */, 701/* 526Hz */, 700/* 527Hz */, 699/* 528Hz */, \
                                            697/* 529Hz */, 696/* 530Hz */, 695/* 531Hz */, 694/* 532Hz */, 692/* 533Hz */, 691/* 534Hz */, 690/* 535Hz */, 688/* 536Hz */, \
                                            687/* 537Hz */, 686/* 538Hz */, 685/* 539Hz */, 683/* 540Hz */, 682/* 541Hz */, 681/* 542Hz */, 679/* 543Hz */, 678/* 544Hz */, \
                                            677/* 545Hz */, 676/* 546Hz */, 675/* 547Hz */, 673/* 548Hz */, 672/* 549Hz */, 671/* 550Hz */, 670/* 551Hz */, 668/* 552Hz */, \
                                            667/* 553Hz */, 666/* 554Hz */, 665/* 555Hz */, 664/* 556Hz */, 662/* 557Hz */, 661/* 558Hz */, 660/* 559Hz */, 659/* 560Hz */, \
                                            658/* 561Hz */, 656/* 562Hz */, 655/* 563Hz */, 654/* 564Hz */, 653/* 565Hz */, 652/* 566Hz */, 651/* 567Hz */, 650/* 568Hz */, \
                                            648/* 569Hz */, 647/* 570Hz */, 646/* 571Hz */, 645/* 572Hz */, 644/* 573Hz */, 643/* 574Hz */, 642/* 575Hz */, 641/* 576Hz */, \
                                            639/* 577Hz */, 638/* 578Hz */, 637/* 579Hz */, 636/* 580Hz */, 635/* 581Hz */, 634/* 582Hz */, 633/* 583Hz */, 632/* 584Hz */, \
                                            631/* 585Hz */, 630/* 586Hz */, 629/* 587Hz */, 627/* 588Hz */, 626/* 589Hz */, 625/* 590Hz */, 624/* 591Hz */, 623/* 592Hz */, \
                                            622/* 593Hz */, 621/* 594Hz */, 620/* 595Hz */, 619/* 596Hz */, 618/* 597Hz */, 617/* 598Hz */, 616/* 599Hz */, 615/* 600Hz */, \
                                            614/* 601Hz */, 613/* 602Hz */, 612/* 603Hz */, 611/* 604Hz */, 610/* 605Hz */, 609/* 606Hz */, 608/* 607Hz */, 607/* 608Hz */, \
                                            606/* 609Hz */, 605/* 610Hz */, 604/* 611Hz */, 603/* 612Hz */, 602/* 613Hz */, 601/* 614Hz */, 600/* 615Hz */, 599/* 616Hz */, \
                                            598/* 617Hz */, 597/* 618Hz */, 596/* 619Hz */, 595/* 620Hz */, 594/* 621Hz */, 593/* 622Hz */, 592/* 623Hz */, 591/* 624Hz */, \
                                            590/* 625Hz */, 589/* 626Hz */, 588/* 627Hz */, 587/* 628Hz */
};
                              
const u16 TMR_ARR_tab65_629HzTo876Hz[248] = {/* ARR corresponding to frequencies from 629Hz to 876Hz */
                                             1174/* 629Hz */, 1172/* 630Hz */, 1170/* 631Hz */, 1168/* 632Hz */, \
                                             1166/* 633Hz */, 1164/* 634Hz */, 1162/* 635Hz */, 1161/* 636Hz */, 1159/* 637Hz */, 1157/* 638Hz */, 1155/* 639Hz */, 1153/* 640Hz */, \
                                             1152/* 641Hz */, 1150/* 642Hz */, 1148/* 643Hz */, 1146/* 644Hz */, 1144/* 645Hz */, 1143/* 646Hz */, 1141/* 647Hz */, 1139/* 648Hz */, \
                                             1137/* 649Hz */, 1136/* 650Hz */, 1134/* 651Hz */, 1132/* 652Hz */, 1130/* 653Hz */, 1129/* 654Hz */, 1127/* 655Hz */, 1125/* 656Hz */, \
                                             1123/* 657Hz */, 1122/* 658Hz */, 1120/* 659Hz */, 1118/* 660Hz */, 1117/* 661Hz */, 1115/* 662Hz */, 1113/* 663Hz */, 1112/* 664Hz */, \
                                             1110/* 665Hz */, 1108/* 666Hz */, 1107/* 667Hz */, 1105/* 668Hz */, 1103/* 669Hz */, 1102/* 670Hz */, 1100/* 671Hz */, 1098/* 672Hz */, \
                                             1097/* 673Hz */, 1095/* 674Hz */, 1094/* 675Hz */, 1092/* 676Hz */, 1090/* 677Hz */, 1089/* 678Hz */, 1087/* 679Hz */, 1085/* 680Hz */, \
                                             1084/* 681Hz */, 1082/* 682Hz */, 1081/* 683Hz */, 1079/* 684Hz */, 1078/* 685Hz */, 1076/* 686Hz */, 1074/* 687Hz */, 1073/* 688Hz */, \
                                             1071/* 689Hz */, 1070/* 690Hz */, 1068/* 691Hz */, 1067/* 692Hz */, 1065/* 693Hz */, 1064/* 694Hz */, 1062/* 695Hz */, 1061/* 696Hz */, \
                                             1059/* 697Hz */, 1057/* 698Hz */, 1056/* 699Hz */, 1054/* 700Hz */, 1053/* 701Hz */, 1051/* 702Hz */, 1050/* 703Hz */, 1048/* 704Hz */, \
                                             1047/* 705Hz */, 1045/* 706Hz */, 1044/* 707Hz */, 1043/* 708Hz */, 1041/* 709Hz */, 1040/* 710Hz */, 1038/* 711Hz */, 1037/* 712Hz */, \
                                             1035/* 713Hz */, 1034/* 714Hz */, 1032/* 715Hz */, 1031/* 716Hz */, 1029/* 717Hz */, 1028/* 718Hz */, 1027/* 719Hz */, 1025/* 720Hz */, \
                                             1024/* 721Hz */, 1022/* 722Hz */, 1021/* 723Hz */, 1019/* 724Hz */, 1018/* 725Hz */, 1017/* 726Hz */, 1015/* 727Hz */, 1014/* 728Hz */, \
                                             1012/* 729Hz */, 1011/* 730Hz */, 1010/* 731Hz */, 1008/* 732Hz */, 1007/* 733Hz */, 1006/* 734Hz */, 1004/* 735Hz */, 1003/* 736Hz */, \
                                             1001/* 737Hz */, 1000/* 738Hz */, 999/* 739Hz */, 997/* 740Hz */, 996/* 741Hz */, 995/* 742Hz */, 993/* 743Hz */, 992/* 744Hz */, \
                                             991/* 745Hz */, 989/* 746Hz */, 988/* 747Hz */, 987/* 748Hz */, 985/* 749Hz */, 984/* 750Hz */, 983/* 751Hz */, 981/* 752Hz */, \
                                             980/* 753Hz */, 979/* 754Hz */, 978/* 755Hz */, 976/* 756Hz */, 975/* 757Hz */, 974/* 758Hz */, 972/* 759Hz */, 971/* 760Hz */, \
                                             970/* 761Hz */, 969/* 762Hz */, 967/* 763Hz */, 966/* 764Hz */, 965/* 765Hz */, 964/* 766Hz */, 962/* 767Hz */, 961/* 768Hz */, \
                                             960/* 769Hz */, 959/* 770Hz */, 957/* 771Hz */, 956/* 772Hz */, 955/* 773Hz */, 954/* 774Hz */, 952/* 775Hz */, 951/* 776Hz */, \
                                             950/* 777Hz */, 949/* 778Hz */, 947/* 779Hz */, 946/* 780Hz */, 945/* 781Hz */, 944/* 782Hz */, 943/* 783Hz */, 941/* 784Hz */, \
                                             940/* 785Hz */, 939/* 786Hz */, 938/* 787Hz */, 937/* 788Hz */, 935/* 789Hz */, 934/* 790Hz */, 933/* 791Hz */, 932/* 792Hz */, \
                                             931/* 793Hz */, 930/* 794Hz */, 928/* 795Hz */, 927/* 796Hz */, 926/* 797Hz */, 925/* 798Hz */, 924/* 799Hz */, 923/* 800Hz */, \
                                             921/* 801Hz */, 920/* 802Hz */, 919/* 803Hz */, 918/* 804Hz */, 917/* 805Hz */, 916/* 806Hz */, 915/* 807Hz */, 913/* 808Hz */, \
                                             912/* 809Hz */, 911/* 810Hz */, 910/* 811Hz */, 909/* 812Hz */, 908/* 813Hz */, 907/* 814Hz */, 906/* 815Hz */, 904/* 816Hz */, \
                                             903/* 817Hz */, 902/* 818Hz */, 901/* 819Hz */, 900/* 820Hz */, 899/* 821Hz */, 898/* 822Hz */, 897/* 823Hz */, 896/* 824Hz */, \
                                             895/* 825Hz */, 894/* 826Hz */, 892/* 827Hz */, 891/* 828Hz */, 890/* 829Hz */, 889/* 830Hz */, 888/* 831Hz */, 887/* 832Hz */, \
                                             886/* 833Hz */, 885/* 834Hz */, 884/* 835Hz */, 883/* 836Hz */, 882/* 837Hz */, 881/* 838Hz */, 880/* 839Hz */, 879/* 840Hz */, \
                                             878/* 841Hz */, 877/* 842Hz */, 875/* 843Hz */, 874/* 844Hz */, 873/* 845Hz */, 872/* 846Hz */, 871/* 847Hz */, 870/* 848Hz */, \
                                             869/* 849Hz */, 868/* 850Hz */, 867/* 851Hz */, 866/* 852Hz */, 865/* 853Hz */, 864/* 854Hz */, 863/* 855Hz */, 862/* 856Hz */, \
                                             861/* 857Hz */, 860/* 858Hz */, 859/* 859Hz */, 858/* 860Hz */, 857/* 861Hz */, 856/* 862Hz */, 855/* 863Hz */, 854/* 864Hz */, \
                                             853/* 865Hz */, 852/* 866Hz */, 851/* 867Hz */, 850/* 868Hz */, 849/* 869Hz */, 848/* 870Hz */, 847/* 871Hz */, 846/* 872Hz */, \
                                             845/* 873Hz */, 844/* 874Hz */, 843/* 875Hz */, 842/* 876Hz */
};

typedef enum {
          Screen_Sin       = 0,
          Screen_Sawtooth  = 1,
          Screen_Triang    = 2,
          Screen_Sinc      = 3,
          Screen_PWM       = 4
} Screen_t;

/* Calibration Data */
const struct CalibData CAL = 
  {
    100,    // LCD Backlight Brightness
    16      //number of samples when averaging ADC measured data
  };
#define STM_Clk_Src_HSI  (u8)0
#define STM_Clk_Src_HSE  (u8)1
extern u8 STM_Clk_Src;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

const u16* ptr_VREFINT_CAL = (u16*)VREFINT_CAL;
u16 VrefINT_CAL; 

static char lcd_row1[17], lcd_row2[17];
#define LCD_CLEAR_ROW "                "

Screen_t Current_Screen = Screen_Sin;
u16 ADC_Conv_Tab_Avg[ADC_Scan_Channels];
/* Private function prototypes -----------------------------------------------*/
void TASK_RFCommand(void);
void TASK_UARTCommands(void);
/* Private functions ---------------------------------------------------------*/
typedef enum
{
  Polarity_Positive = (u8)0x01,
  Polarity_Negative = (u8)0x02
}Pwm_Polarity_t;
u16 analog_sig_freq = 120;
u16 analog_sig_freq_old = 0;
u16 pwm_period = 1000;
u16 pwm_sig_pulse = 500;
u16 pwm_duty = 500;  // 50% duty cycle
u16 pwm_duty_old = pwm_duty;
u32 pwm_freq = 0;
u16 pwm_timebase = 48;  // 48Mhz/48=1us timebase
u16 pwm_timebase_old = pwm_timebase;
_Bool lcd_needs_update = FALSE;
Pwm_Polarity_t pwm_polarity = Polarity_Positive;
typedef enum
{
  Period_Selected     = (u8)0x01,
  Duty_Selected     = (u8)0x02,
  Timebase_Selected = (u8)0x03,
  Polarity_Selected = (u8)0x04
}Freq_Duty_t;
Freq_Duty_t freq_duty_selection = Period_Selected;

TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
TIM_OCInitTypeDef TIM_OCInitStruct;

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  VrefINT_CAL = *ptr_VREFINT_CAL;
  Config();
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = pwm_timebase - 1;           // Prescaler=48 (47+1), This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = pwm_period;                    // This parameter must be a number between 0x0000 and 0xFFFF, fclk=1M, 1000000->T=1s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                  // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
  
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStruct.TIM_Pulse = pwm_sig_pulse;                        // Duty cycle (compared to TIM_Period)
  if(pwm_polarity == Polarity_Positive) 
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  else if(pwm_polarity == Polarity_Negative) 
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low
  TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  TIM_OC4Init(TIM2, &TIM_OCInitStruct);
  TIM_Cmd(TIM2, DISABLE);
  TIM_CtrlPWMOutputs(TIM2, ENABLE);
  
  pwm_freq = (48000000 / pwm_timebase) / pwm_period;
  
  Errors_Init();
  
  SystemCoreClockUpdate();
  
  LCD_Initialize();
  LCD_Clear();
  LCD_WriteString("     SINUS      ");
  TIM_Cmd(TIM6, ENABLE);
  
  while (1)
  {
    /* ============== PRESS BTN FREQ INC ================= */
    if(BTN_FREQINC_DEB_STATE == BTN_PRESSED && BTN_FREQINC_DELAY_FLAG)
    {
      char strtmp[11];
      BTN_FREQINC_DELAY_FLAG = FALSE;
      if(analog_sig_freq < 876)
      {
        analog_sig_freq++;
        if(analog_sig_freq_old == 628 && analog_sig_freq == 629)
        {
          DMA_Cmd(DMA1_Channel3, DISABLE);
          switch(Current_Screen)
          {
            case Screen_Sin:
            {
              DMA1_Channel3->CMAR = (u32)(&Sinus12bit65);
              break;
            }
            case Screen_Sawtooth:
            {
              DMA1_Channel3->CMAR = (u32)(&SawTooth12bit65);
              break;
            }
            case Screen_Triang:
            {
              DMA1_Channel3->CMAR = (u32)(&Triang12bit65);
              break;
            }
            case Screen_Sinc:
            {
              DMA1_Channel3->CMAR = (u32)(&SinCard12bit65);
              break;
            }
            default:
            {
              break;
            }
          }
          DMA1_Channel3->CNDTR = 65;
          DMA_Cmd(DMA1_Channel3, ENABLE);
        }
        if(analog_sig_freq <= 5)
        {
          /* 1Hz - 5Hz */
          TIM6->PSC = TMR_PSC_ARR_tab130_1HzTo5Hz[analog_sig_freq-1][0];
          TIM6->ARR = TMR_PSC_ARR_tab130_1HzTo5Hz[analog_sig_freq-1][1];
        }
        else if(analog_sig_freq <= 628)
        {/* 6Hz - 628Hz */
          TIM6->PSC = 0;
          TIM6->ARR = TMR_ARR_tab130_6HzTo628Hz[analog_sig_freq-6];
        }
        else
        {/* 629Hz - 876Hz */
          TIM6->PSC = 0;
          TIM6->ARR = TMR_ARR_tab65_629HzTo876Hz[analog_sig_freq-629];
        }
        analog_sig_freq_old = analog_sig_freq;
        
        string_copy(lcd_row2, "  f=");
        string_append(lcd_row2, string_U32ToStr(analog_sig_freq, strtmp));
        string_append(lcd_row2, "Hz");
        LCD_WriteString(LCD_CLEAR_ROW);
        LCD_Move_Cursor(2, 0);
        LCD_WriteString(lcd_row2);
      }
      else if(analog_sig_freq == 876)
      {
        TIM6->PSC = 0;
        if(TIM6->ARR > 37) TIM6->ARR--;  /* 37 corresponds to 19958Hz */
        string_copy(lcd_row2, "  f=");
        string_append(lcd_row2, string_U32ToStr(738462/TIM6->ARR, strtmp));
        string_append(lcd_row2, "Hz");
        LCD_WriteString(LCD_CLEAR_ROW);
        LCD_Move_Cursor(2, 0);
        LCD_WriteString(lcd_row2);
      }
      if(Current_Screen == Screen_PWM)
      {
        switch(freq_duty_selection)
        {
          case Period_Selected:
          {
            if(BTN_FREQINC_press_timer < BTN_DELAY_1000ms)  
            {
              if(pwm_period < 65535) pwm_period++;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_1000ms && BTN_FREQINC_press_timer < BTN_DELAY_2500ms)
            {
              if(pwm_period < 65525) pwm_period += 10;
              else pwm_period = 65535;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_2500ms && BTN_FREQINC_press_timer < BTN_DELAY_5000ms)
            {
              if(pwm_period < 65435) pwm_period += 100;
              else pwm_period = 65535;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_5000ms)
            {
              if(pwm_period < 65035) pwm_period += 500;
              else pwm_period = 65535;
            }
            if(pwm_period != pwm_period_old)
            {
              pwm_sig_pulse = (pwm_duty * pwm_period) / 1000;
              TIM_TimeBaseInitStruct.TIM_Period = pwm_period;  // This parameter must be a number between 0x0000 and 0xFFFF, fclk=10k, 10000->T=1s
              TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
              TIM_OCInitStruct.TIM_Pulse = pwm_sig_pulse;          // Duty cycle (compared to TIM_Period)
              TIM_OC4Init(TIM2, &TIM_OCInitStruct);
              pwm_freq = (48000000 / pwm_timebase) / pwm_period;
              lcd_needs_update = TRUE;
            }
            pwm_period_old = pwm_period;
            
            break;
          }
          case Duty_Selected:
          {
            if(BTN_FREQINC_press_timer < BTN_DELAY_1000ms)  
            {
              if(pwm_duty < 998) pwm_duty++;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_1000ms && BTN_FREQINC_press_timer < BTN_DELAY_2500ms)
            {
              if(pwm_duty < 988) pwm_duty += 10;
              else pwm_duty = 998;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_2500ms && BTN_FREQINC_press_timer < BTN_DELAY_5000ms)
            {
              if(pwm_duty < 898) pwm_duty += 100;
              else pwm_duty = 998;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_5000ms)
            {
              if(pwm_duty < 798) pwm_duty += 200;
              else pwm_duty = 998;
            }
            if(pwm_duty != pwm_duty_old)
            {
              pwm_sig_pulse = (pwm_duty * pwm_period) / 1000;
              TIM_OCInitStruct.TIM_Pulse = pwm_sig_pulse;          // Duty cycle (compared to TIM_Period)
              TIM_OC4Init(TIM2, &TIM_OCInitStruct);
              lcd_needs_update = TRUE;
            }
            pwm_duty_old = pwm_duty;
            break;
          }
          case Timebase_Selected:
          {
            if(BTN_FREQINC_press_timer < BTN_DELAY_1000ms)  
            {
              if(pwm_timebase < 65534) pwm_timebase++;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_1000ms && BTN_FREQINC_press_timer < BTN_DELAY_2500ms)
            {
              if(pwm_timebase < 65524) pwm_timebase += 10;
              else pwm_timebase = 65534;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_2500ms && BTN_FREQINC_press_timer < BTN_DELAY_5000ms)
            {
              if(pwm_timebase < 65434) pwm_timebase += 100;
              else pwm_timebase = 65534;
            }
            else if(BTN_FREQINC_press_timer >= BTN_DELAY_5000ms)
            {
              if(pwm_timebase < 65334) pwm_timebase += 200;
              else pwm_timebase = 65534;
            }
            if(pwm_timebase != pwm_timebase_old)
            {
              TIM_TimeBaseInitStruct.TIM_Prescaler = pwm_timebase; 
              TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
              pwm_freq = (48000000 / pwm_timebase) / pwm_period;
              lcd_needs_update = TRUE;
            }
            pwm_timebase_old = pwm_timebase;
            break;
          }
          case Polarity_Selected:
          {
            if(pwm_polarity == Polarity_Positive)
            {
            
            }
            else if(pwm_polarity == Polarity_Negative)
            {
            
            }
          }
          default: break;
        }
        if(lcd_needs_update)
        {
          lcd_needs_update = FALSE;
          LCD_Clear();
          //string_copy(lcd_row1, "f=");
          string_copy(lcd_row1, string_U32ToStr(pwm_freq, strtmp));
          string_append(lcd_row1, "Hz");
          LCD_WriteString(lcd_row1);
          string_copy(lcd_row2, "  duty=");
          string_append(lcd_row2, string_U32ToStr(pwm_duty/10, strtmp));
          string_append(lcd_row2, ".");
          string_append(lcd_row2, string_U32ToStr(pwm_duty%10, strtmp));
          string_append(lcd_row2, "%");
          LCD_Move_Cursor(2, 0);
          LCD_WriteString(lcd_row2);
        }
      }
    }
    /* ================ END PRESS BTN FREQ INC ================== */
    
    /* ============== PRESS BTN FREQ DEC ================= */
    if(BTN_FREQDEC_DEB_STATE == BTN_PRESSED && BTN_FREQDEC_DELAY_FLAG)
    {
      char strtmp[11];
      BTN_FREQDEC_DELAY_FLAG = FALSE;
      if(analog_sig_freq <= 875)  /* generated frequency < 876Hz */
      {
        if(analog_sig_freq > 1)
        {
          analog_sig_freq--;
          if(analog_sig_freq_old == 629 && analog_sig_freq == 628)
          {
            DMA_Cmd(DMA1_Channel3, DISABLE);
            switch(Current_Screen)
            {
              case Screen_Sin:
              {
                DMA1_Channel3->CMAR = (u32)(&Sinus12bit130);
                break;
              }
              case Screen_Sawtooth:
              {
                DMA1_Channel3->CMAR = (u32)(&SawTooth12bit130);
                break;
              }
              case Screen_Triang:
              {
                DMA1_Channel3->CMAR = (u32)(&Triang12bit130);
                break;
              }
              case Screen_Sinc:
              {
                DMA1_Channel3->CMAR = (u32)(&SinCard12bit130);
                break;
              }
              default:
              {
                break;
              }
            }
            DMA1_Channel3->CNDTR = 130;
            DMA_Cmd(DMA1_Channel3, ENABLE);
          }
          if(analog_sig_freq <= 5)
          {
            /* 1Hz - 5Hz */
            TIM6->PSC = TMR_PSC_ARR_tab130_1HzTo5Hz[analog_sig_freq-1][0];
            TIM6->ARR = TMR_PSC_ARR_tab130_1HzTo5Hz[analog_sig_freq-1][1];
          }
          else if(analog_sig_freq <= 628)
          {/* 6Hz - 628Hz */
            TIM6->PSC = 0;
            TIM6->ARR = TMR_ARR_tab130_6HzTo628Hz[analog_sig_freq-6];
          }
          else
          {/* 629Hz - 876Hz */
            TIM6->ARR = TMR_ARR_tab65_629HzTo876Hz[analog_sig_freq-629];
          }
          analog_sig_freq_old = analog_sig_freq;
          if(Current_Screen != Screen_PWM)
          {
            string_copy(lcd_row2, "  f=");
            string_append(lcd_row2, string_U32ToStr(analog_sig_freq, strtmp));
            string_append(lcd_row2, "Hz");
            LCD_WriteString(LCD_CLEAR_ROW);
            LCD_Move_Cursor(2, 0);
            LCD_WriteString(lcd_row2);
          }
        }
      }
      else if(analog_sig_freq == 876)
      {
        TIM6->PSC = 0;
        TIM6->ARR++;
        if(TIM6->ARR == 843) analog_sig_freq = 875;  /* 843 corresponds to 875Hz */
        if(Current_Screen != Screen_PWM)
        {
          string_copy(lcd_row2, "  f=");
          string_append(lcd_row2, string_U32ToStr(738462/TIM6->ARR, strtmp));
          string_append(lcd_row2, "Hz");
          LCD_WriteString(LCD_CLEAR_ROW);
          LCD_Move_Cursor(2, 0);
          LCD_WriteString(lcd_row2);
        }
      }
      if(Current_Screen == Screen_PWM)
      {
        switch(freq_duty_selection)
        {
          case Period_Selected:
          {
            if(BTN_FREQDEC_press_timer < BTN_DELAY_1000ms)  
            {
              if(pwm_period > 2) pwm_period--;
            }
            else if(BTN_FREQDEC_press_timer >= BTN_DELAY_1000ms && BTN_FREQDEC_press_timer < BTN_DELAY_2500ms)
            {
              if(pwm_period > 12) pwm_period -= 10;
              else pwm_period = 2;
            }
            else if(BTN_FREQDEC_press_timer >= BTN_DELAY_2500ms && BTN_FREQDEC_press_timer < BTN_DELAY_5000ms)
            {
              if(pwm_period > 102) pwm_period -= 100;
              else pwm_period = 2;
            }
            else if(BTN_FREQDEC_press_timer >= BTN_DELAY_5000ms)
            {
              if(pwm_period > 502) pwm_period -= 500;
              else pwm_period = 2;
            }
            pwm_sig_pulse = (pwm_duty * pwm_period) / 1000;
            TIM_TimeBaseInitStruct.TIM_Period = pwm_period;  // This parameter must be a number between 0x0000 and 0xFFFF, fclk=10k, 10000->T=1s
            TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
            TIM_OCInitStruct.TIM_Pulse = pwm_sig_pulse;          // Duty cycle (compared to TIM_Period)
            TIM_OC4Init(TIM2, &TIM_OCInitStruct);
            break;
          }
          case Duty_Selected:
          {
            if(BTN_FREQDEC_press_timer < BTN_DELAY_1000ms)  
            {
              if(pwm_duty > 1) pwm_duty--;
            }
            else if(BTN_FREQDEC_press_timer >= BTN_DELAY_1000ms && BTN_FREQDEC_press_timer < BTN_DELAY_2500ms)
            {
              if(pwm_duty > 11) pwm_duty -= 10;
              else pwm_duty = 1;
            }
            else if(BTN_FREQDEC_press_timer >= BTN_DELAY_2500ms && BTN_FREQDEC_press_timer < BTN_DELAY_5000ms)
            {
              if(pwm_duty > 101) pwm_duty -= 100;
              else pwm_duty = 1;
            }
            else if(BTN_FREQDEC_press_timer >= BTN_DELAY_5000ms)
            {
              if(pwm_duty > 201) pwm_duty -= 200;
              else pwm_duty = 1;
            }
            
            pwm_sig_pulse = (pwm_duty * pwm_period) / 1000;
            TIM_OCInitStruct.TIM_Pulse = pwm_sig_pulse;          // Duty cycle (compared to TIM_Period)
            TIM_OC4Init(TIM2, &TIM_OCInitStruct);
            break;
          }
          default: break;
        }
        
        LCD_Clear();
        string_copy(lcd_row1, "  P=");
        string_append(lcd_row1, string_U32ToStr(pwm_period, strtmp));
        string_append(lcd_row1, "us");
        LCD_WriteString(lcd_row1);
        string_copy(lcd_row2, "  tON=");
        string_append(lcd_row2, string_U32ToStr(pwm_sig_pulse, strtmp));
        string_append(lcd_row2, "us");
        LCD_Move_Cursor(2, 0);
        LCD_WriteString(lcd_row2);
      }
    }
    /* ================ END PRESS BTN FREQ DEC ================== */
    
    /* ============== PRESS BTN FREQ DUTY ================= */
    if(BTN_FREQDUTY_DEB_STATE == BTN_PRESSED && BTN_FREQDUTY_DELAY_FLAG)
    {
      BTN_FREQDUTY_DELAY_FLAG = FALSE;
      switch(freq_duty_selection)
      {
        case Period_Selected:
        {
          freq_duty_selection = Duty_Selected;
          break;
        }
        case Duty_Selected:
        {
          freq_duty_selection = Timebase_Selected;
          break;
        }
        case Timebase_Selected:
        {
          freq_duty_selection = Polarity_Selected;
          break;
        }
        case Polarity_Selected:
        {
          freq_duty_selection = Period_Selected;
          break;
        }
        default: break;
      }
    }
    
    /* ============== PRESS BTN CHG WAVE ================= */
    if(BTN_CHGWAVE_DEB_STATE == BTN_PRESSED && BTN_CHGWAVE_DELAY_FLAG)
    {
      BTN_CHGWAVE_DELAY_FLAG = FALSE;
      Current_Screen++;
      if(Current_Screen == Screen_PWM + 1) Current_Screen = Screen_Sin;
      DMA_Cmd(DMA1_Channel3, DISABLE);
      switch(Current_Screen)
      {
        case Screen_Sin:
        {
          LCD_Clear();
          LCD_WriteString("     SINUS      ");
          TIM_Cmd(TIM6, ENABLE);
          if(analog_sig_freq <= 628)
          {
            DMA1_Channel3->CMAR = (u32)(&Sinus12bit130);
            DMA1_Channel3->CNDTR = 130;
          }
          else
          {
            DMA1_Channel3->CMAR = (u32)(&Sinus12bit65);
            DMA1_Channel3->CNDTR = 65;
          }
          break;
        }
        case Screen_Sawtooth:
        {
          LCD_Clear();
          LCD_WriteString("    SAWTOOTH    ");
          TIM_Cmd(TIM6, ENABLE);
          TIM_Cmd(TIM2, DISABLE);
          if(analog_sig_freq <= 628)
          {
            DMA1_Channel3->CMAR = (u32)(&SawTooth12bit130);
            DMA1_Channel3->CNDTR = 130;
          }
          else
          {
            DMA1_Channel3->CMAR = (u32)(&SawTooth12bit65);
            DMA1_Channel3->CNDTR = 65;
          }
          break;
        }
        case Screen_Triang:
        {
          LCD_Clear();
          LCD_WriteString("    TRIANGLE    ");
          TIM_Cmd(TIM6, ENABLE);
          TIM_Cmd(TIM2, DISABLE);
          if(analog_sig_freq <= 628)
          {
            DMA1_Channel3->CMAR = (u32)(&Triang12bit130);
            DMA1_Channel3->CNDTR = 130;
          }
          else
          {
            DMA1_Channel3->CMAR = (u32)(&Triang12bit65);
            DMA1_Channel3->CNDTR = 65;
          }
          break;
        }
        case Screen_Sinc:
        {
          LCD_Clear();
          LCD_WriteString("      SINC      ");
          TIM_Cmd(TIM6, ENABLE);
          TIM_Cmd(TIM2, DISABLE);
          if(analog_sig_freq <= 628)
          {
            DMA1_Channel3->CMAR = (u32)(&SinCard12bit130);
            DMA1_Channel3->CNDTR = 130;
          }
          else
          {
            DMA1_Channel3->CMAR = (u32)(&SinCard12bit65);
            DMA1_Channel3->CNDTR = 65;
          }
          break;
        }
        case Screen_PWM:
        {
          LCD_Clear();
          LCD_WriteString("      PWM      ");
          // stop dma, start pwm generation
          TIM_Cmd(TIM6, DISABLE);
          TIM_Cmd(TIM2, ENABLE);
          break;
        }
        default:
        {
          break;
        }
      }
      DMA_Cmd(DMA1_Channel3, ENABLE);
    }
    /* ================ END PRESS BTN CHG WAVE ================== */
    
    // ================== TASK cyclic 1000ms ===================
    if(FLAG_1000ms)
    {
      FLAG_1000ms = FALSE;
      
    }
    // ================== END TASK cyclic 1000ms ===================
    
    // ================== TASK cyclic 500ms ===================
    if(FLAG_500ms)
    {
      FLAG_500ms = FALSE;
    }
    // ================== END TASK cyclic 500ms ===================
    
    // ================== TASK cyclic 250ms ===================
    if(FLAG_250ms)
    {
      FLAG_250ms = FALSE;
    }
    // ================== END TASK cyclic 250ms ===================
    
    // ============= UART COMMAND RECEIVED ==============
    if(FLAG_UART_cmd_rcv)
    {
      switch(UART_CMD.CMD)
      {
        case UART_CMD_1:
        {
          //acknowledge CMD1
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
          USART_SendData(USART1, 0x50);
          break;
        }
        case UART_CMD_2:
        {
          //acknowledge CMD2
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
          USART_SendData(USART1, 0x60);
          break;
        }
        case UART_CMD_3:
        {
          //acknowledge CMD3
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
          USART_SendData(USART1, 0x70);
          break;
        }
        default:
        {
          //acknowledge command not recognized
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x8F);
          break;
        }
      }
      FLAG_UART_cmd_rcv = FALSE;
    }
    // ============= END UART COMMAND RECEIVED ==============
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
