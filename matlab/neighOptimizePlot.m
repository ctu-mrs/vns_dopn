clear all
close all

actualDistanceCheck = 7.07468
actual_neigh_ang = 1.5708
actualDistance = 7.07468
angles = [0 0.00314159 0.00628319 0.00942478 0.0125664 0.015708 0.0188496 0.0219911 0.0251327 0.0282743 0.0314159 0.0345575 0.0376991 0.0408407 0.0439823 0.0471239 0.0502655 0.0534071 0.0565487 0.0596903 0.0628319 0.0659734 0.069115 0.0722566 0.0753982 0.0785398 0.0816814 0.084823 0.0879646 0.0911062 0.0942478 0.0973894 0.100531 0.103673 0.106814 0.109956 0.113097 0.116239 0.119381 0.122522 0.125664 0.128805 0.131947 0.135088 0.13823 0.141372 0.144513 0.147655 0.150796 0.153938 0.15708 0.160221 0.163363 0.166504 0.169646 0.172788 0.175929 0.179071 0.182212 0.185354 0.188496 0.191637 0.194779 0.19792 0.201062 0.204204 0.207345 0.210487 0.213628 0.21677 0.219911 0.223053 0.226195 0.229336 0.232478 0.235619 0.238761 0.241903 0.245044 0.248186 0.251327 0.254469 0.257611 0.260752 0.263894 0.267035 0.270177 0.273319 0.27646 0.279602 0.282743 0.285885 0.289027 0.292168 0.29531 0.298451 0.301593 0.304734 0.307876 0.311018 0.314159 0.317301 0.320442 0.323584 0.326726 0.329867 0.333009 0.33615 0.339292 0.342434 0.345575 0.348717 0.351858 0.355 0.358142 0.361283 0.364425 0.367566 0.370708 0.37385 0.376991 0.380133 0.383274 0.386416 0.389557 0.392699 0.395841 0.398982 0.402124 0.405265 0.408407 0.411549 0.41469 0.417832 0.420973 0.424115 0.427257 0.430398 0.43354 0.436681 0.439823 0.442965 0.446106 0.449248 0.452389 0.455531 0.458673 0.461814 0.464956 0.468097 0.471239 0.47438 0.477522 0.480664 0.483805 0.486947 0.490088 0.49323 0.496372 0.499513 0.502655 0.505796 0.508938 0.51208 0.515221 0.518363 0.521504 0.524646 0.527788 0.530929 0.534071 0.537212 0.540354 0.543496 0.546637 0.549779 0.55292 0.556062 0.559203 0.562345 0.565487 0.568628 0.57177 0.574911 0.578053 0.581195 0.584336 0.587478 0.590619 0.593761 0.596903 0.600044 0.603186 0.606327 0.609469 0.612611 0.615752 0.618894 0.622035 0.625177 0.628319 0.63146 0.634602 0.637743 0.640885 0.644026 0.647168 0.65031 0.653451 0.656593 0.659734 0.662876 0.666018 0.669159 0.672301 0.675442 0.678584 0.681726 0.684867 0.688009 0.69115 0.694292 0.697434 0.700575 0.703717 0.706858 0.71 0.713142 0.716283 0.719425 0.722566 0.725708 0.728849 0.731991 0.735133 0.738274 0.741416 0.744557 0.747699 0.750841 0.753982 0.757124 0.760265 0.763407 0.766549 0.76969 0.772832 0.775973 0.779115 0.782257 0.785398 0.78854 0.791681 0.794823 0.797965 0.801106 0.804248 0.807389 0.810531 0.813672 0.816814 0.819956 0.823097 0.826239 0.82938 0.832522 0.835664 0.838805 0.841947 0.845088 0.84823 0.851372 0.854513 0.857655 0.860796 0.863938 0.86708 0.870221 0.873363 0.876504 0.879646 0.882788 0.885929 0.889071 0.892212 0.895354 0.898495 0.901637 0.904779 0.90792 0.911062 0.914203 0.917345 0.920487 0.923628 0.92677 0.929911 0.933053 0.936195 0.939336 0.942478 0.945619 0.948761 0.951903 0.955044 0.958186 0.961327 0.964469 0.967611 0.970752 0.973894 0.977035 0.980177 0.983319 0.98646 0.989602 0.992743 0.995885 0.999026 1.00217 1.00531 1.00845 1.01159 1.01473 1.01788 1.02102 1.02416 1.0273 1.03044 1.03358 1.03673 1.03987 1.04301 1.04615 1.04929 1.05243 1.05558 1.05872 1.06186 1.065 1.06814 1.07128 1.07442 1.07757 1.08071 1.08385 1.08699 1.09013 1.09327 1.09642 1.09956 1.1027 1.10584 1.10898 1.11212 1.11527 1.11841 1.12155 1.12469 1.12783 1.13097 1.13411 1.13726 1.1404 1.14354 1.14668 1.14982 1.15296 1.15611 1.15925 1.16239 1.16553 1.16867 1.17181 1.17496 1.1781 1.18124 1.18438 1.18752 1.19066 1.19381 1.19695 1.20009 1.20323 1.20637 1.20951 1.21265 1.2158 1.21894 1.22208 1.22522 1.22836 1.2315 1.23465 1.23779 1.24093 1.24407 1.24721 1.25035 1.2535 1.25664 1.25978 1.26292 1.26606 1.2692 1.27235 1.27549 1.27863 1.28177 1.28491 1.28805 1.29119 1.29434 1.29748 1.30062 1.30376 1.3069 1.31004 1.31319 1.31633 1.31947 1.32261 1.32575 1.32889 1.33204 1.33518 1.33832 1.34146 1.3446 1.34774 1.35088 1.35403 1.35717 1.36031 1.36345 1.36659 1.36973 1.37288 1.37602 1.37916 1.3823 1.38544 1.38858 1.39173 1.39487 1.39801 1.40115 1.40429 1.40743 1.41058 1.41372 1.41686 1.42 1.42314 1.42628 1.42942 1.43257 1.43571 1.43885 1.44199 1.44513 1.44827 1.45142 1.45456 1.4577 1.46084 1.46398 1.46712 1.47027 1.47341 1.47655 1.47969 1.48283 1.48597 1.48911 1.49226 1.4954 1.49854 1.50168 1.50482 1.50796 1.51111 1.51425 1.51739 1.52053 1.52367 1.52681 1.52996 1.5331 1.53624 1.53938 1.54252 1.54566 1.54881 1.55195 1.55509 1.55823 1.56137 1.56451 1.56765 1.5708 1.57394 1.57708 1.58022 1.58336 1.5865 1.58965 1.59279 1.59593 1.59907 1.60221 1.60535 1.6085 1.61164 1.61478 1.61792 1.62106 1.6242 1.62734 1.63049 1.63363 1.63677 1.63991 1.64305 1.64619 1.64934 1.65248 1.65562 1.65876 1.6619 1.66504 1.66819 1.67133 1.67447 1.67761 1.68075 1.68389 1.68704 1.69018 1.69332 1.69646 1.6996 1.70274 1.70588 1.70903 1.71217 1.71531 1.71845 1.72159 1.72473 1.72788 1.73102 1.73416 1.7373 1.74044 1.74358 1.74673 1.74987 1.75301 1.75615 1.75929 1.76243 1.76558 1.76872 1.77186 1.775 1.77814 1.78128 1.78442 1.78757 1.79071 1.79385 1.79699 1.80013 1.80327 1.80642 1.80956 1.8127 1.81584 1.81898 1.82212 1.82527 1.82841 1.83155 1.83469 1.83783 1.84097 1.84411 1.84726 1.8504 1.85354 1.85668 1.85982 1.86296 1.86611 1.86925 1.87239 1.87553 1.87867 1.88181 1.88496 1.8881 1.89124 1.89438 1.89752 1.90066 1.90381 1.90695 1.91009 1.91323 1.91637 1.91951 1.92265 1.9258 1.92894 1.93208 1.93522 1.93836 1.9415 1.94465 1.94779 1.95093 1.95407 1.95721 1.96035 1.9635 1.96664 1.96978 1.97292 1.97606 1.9792 1.98234 1.98549 1.98863 1.99177 1.99491 1.99805 2.00119 2.00434 2.00748 2.01062 2.01376 2.0169 2.02004 2.02319 2.02633 2.02947 2.03261 2.03575 2.03889 2.04204 2.04518 2.04832 2.05146 2.0546 2.05774 2.06088 2.06403 2.06717 2.07031 2.07345 2.07659 2.07973 2.08288 2.08602 2.08916 2.0923 2.09544 2.09858 2.10173 2.10487 2.10801 2.11115 2.11429 2.11743 2.12058 2.12372 2.12686 2.13 2.13314 2.13628 2.13942 2.14257 2.14571 2.14885 2.15199 2.15513 2.15827 2.16142 2.16456 2.1677 2.17084 2.17398 2.17712 2.18027 2.18341 2.18655 2.18969 2.19283 2.19597 2.19911 2.20226 2.2054 2.20854 2.21168 2.21482 2.21796 2.22111 2.22425 2.22739 2.23053 2.23367 2.23681 2.23996 2.2431 2.24624 2.24938 2.25252 2.25566 2.25881 2.26195 2.26509 2.26823 2.27137 2.27451 2.27765 2.2808 2.28394 2.28708 2.29022 2.29336 2.2965 2.29965 2.30279 2.30593 2.30907 2.31221 2.31535 2.3185 2.32164 2.32478 2.32792 2.33106 2.3342 2.33734 2.34049 2.34363 2.34677 2.34991 2.35305 2.35619 2.35934 2.36248 2.36562 2.36876 2.3719 2.37504 2.37819 2.38133 2.38447 2.38761 2.39075 2.39389 2.39704 2.40018 2.40332 2.40646 2.4096 2.41274 2.41588 2.41903 2.42217 2.42531 2.42845 2.43159 2.43473 2.43788 2.44102 2.44416 2.4473 2.45044 2.45358 2.45673 2.45987 2.46301 2.46615 2.46929 2.47243 2.47558 2.47872 2.48186 2.485 2.48814 2.49128 2.49442 2.49757 2.50071 2.50385 2.50699 2.51013 2.51327 2.51642 2.51956 2.5227 2.52584 2.52898 2.53212 2.53527 2.53841 2.54155 2.54469 2.54783 2.55097 2.55411 2.55726 2.5604 2.56354 2.56668 2.56982 2.57296 2.57611 2.57925 2.58239 2.58553 2.58867 2.59181 2.59496 2.5981 2.60124 2.60438 2.60752 2.61066 2.61381 2.61695 2.62009 2.62323 2.62637 2.62951 2.63265 2.6358 2.63894 2.64208 2.64522 2.64836 2.6515 2.65465 2.65779 2.66093 2.66407 2.66721 2.67035 2.6735 2.67664 2.67978 2.68292 2.68606 2.6892 2.69234 2.69549 2.69863 2.70177 2.70491 2.70805 2.71119 2.71434 2.71748 2.72062 2.72376 2.7269 2.73004 2.73319 2.73633 2.73947 2.74261 2.74575 2.74889 2.75204 2.75518 2.75832 2.76146 2.7646 2.76774 2.77088 2.77403 2.77717 2.78031 2.78345 2.78659 2.78973 2.79288 2.79602 2.79916 2.8023 2.80544 2.80858 2.81173 2.81487 2.81801 2.82115 2.82429 2.82743 2.83057 2.83372 2.83686 2.84 2.84314 2.84628 2.84942 2.85257 2.85571 2.85885 2.86199 2.86513 2.86827 2.87142 2.87456 2.8777 2.88084 2.88398 2.88712 2.89027 2.89341 2.89655 2.89969 2.90283 2.90597 2.90911 2.91226 2.9154 2.91854 2.92168 2.92482 2.92796 2.93111 2.93425 2.93739 2.94053 2.94367 2.94681 2.94996 2.9531 2.95624 2.95938 2.96252 2.96566 2.96881 2.97195 2.97509 2.97823 2.98137 2.98451 2.98765 2.9908 2.99394 2.99708 3.00022 3.00336 3.0065 3.00965 3.01279 3.01593 3.01907 3.02221 3.02535 3.0285 3.03164 3.03478 3.03792 3.04106 3.0442 3.04734 3.05049 3.05363 3.05677 3.05991 3.06305 3.06619 3.06934 3.07248 3.07562 3.07876 3.0819 3.08504 3.08819 3.09133 3.09447 3.09761 3.10075 3.10389 3.10704 3.11018 3.11332 3.11646 3.1196 3.12274 3.12588 3.12903 3.13217 3.13531 3.13845 3.14159 ];
distances = [11.994 11.9949 11.9958 11.9968 11.9977 11.9987 11.9996 12.0005 12.0014 12.0023 12.0033 12.0042 12.0051 12.006 12.0069 12.0078 12.0086 12.0095 12.0104 12.0113 12.0121 12.013 12.0139 12.0147 12.0156 12.0164 12.0173 12.0181 12.0189 12.0198 12.0206 12.0214 12.0222 12.023 12.0238 12.0246 12.0254 12.0262 12.027 12.0278 12.0285 12.0293 12.03 12.0308 12.0315 12.0323 12.033 12.0337 12.0345 12.0352 12.0359 12.0366 12.0372 12.0379 12.0386 12.0392 12.0399 12.0405 12.0411 12.0417 12.0423 12.0429 12.0435 12.044 12.0445 12.045 12.0455 12.0459 12.0463 12.0467 12.047 12.0472 12.0475 12.0476 12.0477 12.0477 12.0476 12.0476 12.0476 12.0476 12.0476 12.0476 12.0476 12.0476 12.0476 12.0476 12.0476 12.0476 12.0477 12.0453 12.0422 12.0391 12.0359 12.0328 12.0297 12.0266 12.0235 12.0203 12.0172 12.0141 12.0109 7.05153 7.05194 7.05235 7.05278 7.05321 7.05366 7.0541 7.05456 7.05501 7.05548 7.05594 7.05641 7.05688 7.05735 7.05782 7.05829 7.05877 7.05924 7.05971 7.06019 7.06066 7.06113 7.0616 7.06207 7.06254 7.06301 7.06347 7.06394 7.0644 7.06486 7.06532 7.06577 7.06623 7.06668 7.06713 7.06758 7.06802 7.06847 7.06891 7.06935 7.06978 7.07022 7.07065 7.07108 7.0715 7.07193 7.07235 7.07276 7.07318 7.07359 7.074 7.07441 7.07481 7.07521 7.07561 7.07601 7.0764 7.07679 7.07718 7.07756 7.07794 7.07832 7.07869 7.07907 7.07944 7.0798 7.08017 7.08053 7.08088 7.08124 7.08159 7.08194 7.08229 7.08263 7.08297 7.08331 7.08364 7.08397 7.0843 7.08462 7.08495 7.08527 7.08558 7.0859 7.08621 7.08651 7.08682 7.08712 7.08742 7.08771 7.08801 7.0883 7.08858 7.08887 7.08915 7.08943 7.0897 7.08997 7.09024 7.09051 7.09077 7.09103 7.09129 7.09154 7.09179 7.09204 7.09229 7.09253 7.09277 7.09301 7.09324 7.09347 7.0937 7.09393 7.09415 7.09437 7.09459 7.0948 7.09501 7.09522 7.09543 7.09563 7.09583 7.09603 7.09622 7.09641 7.0966 7.09679 7.09697 7.09715 7.09733 7.09751 7.09768 7.09785 7.09801 7.09818 7.09834 7.0985 7.09865 7.09881 7.09896 7.0991 7.09925 7.09939 7.09953 7.09967 7.0998 7.09993 7.10006 7.10019 7.10031 7.10043 7.10055 7.10067 7.10078 7.10089 7.101 7.1011 7.10121 7.10131 7.1014 7.1015 7.10159 7.10168 7.10177 7.10185 7.10194 7.10201 7.10209 7.10217 7.10224 7.10231 7.10238 7.10244 7.1025 7.10256 7.10262 7.10268 7.10273 7.10278 7.10283 7.10287 7.10292 7.10296 7.10299 7.10303 7.10306 7.1031 7.10312 7.10315 7.10318 7.1032 7.10322 7.10323 7.10325 7.10326 7.10327 7.10328 7.10329 7.10329 7.10329 7.10329 7.10329 7.10328 7.10328 7.10327 7.10325 7.10324 7.10322 7.10321 7.10319 7.10316 7.10314 7.10311 7.10308 7.10305 7.10302 7.10298 7.10295 7.10291 7.10287 7.10282 7.10278 7.10273 7.10268 7.10263 7.10258 7.10252 7.10247 7.10241 7.10235 7.10228 7.10222 7.10215 7.10208 7.10201 7.10194 7.10186 7.10179 7.10171 7.10163 7.10155 7.10147 7.10138 7.10129 7.1012 7.10111 7.10102 7.10093 7.10083 7.10073 7.10063 7.10053 7.10043 7.10032 7.10022 7.10011 7.1 7.09989 7.09978 7.09966 7.09954 7.09943 7.09931 7.09919 7.09906 7.09894 7.09881 7.09869 7.09856 7.09843 7.0983 7.09816 7.09803 7.09789 7.09775 7.09762 7.09748 7.09733 7.09719 7.09705 7.0969 7.09675 7.0966 7.09645 7.0963 7.09615 7.09599 7.09584 7.09568 7.09552 7.09536 7.0952 7.09504 7.09488 7.09471 7.09455 7.09438 7.09421 7.09404 7.09387 7.0937 7.09353 7.09336 7.09318 7.093 7.09283 7.09265 7.09247 7.09229 7.09211 7.09193 7.09174 7.09156 7.09138 7.09119 7.091 7.09082 7.09063 7.09044 7.09025 7.09006 7.08987 7.08967 7.08948 7.08929 7.08909 7.0889 7.0887 7.08851 7.08831 7.08811 7.08792 7.08772 7.08752 7.08732 7.08712 7.08692 7.08672 7.08652 7.08632 7.08611 7.08591 7.08571 7.08551 7.0853 7.0851 7.0849 7.08469 7.08449 7.08429 7.08408 7.08388 7.08367 7.08347 7.08326 7.08306 7.08286 7.08265 7.08245 7.08224 7.08204 7.08183 7.08163 7.08143 7.08122 7.08102 7.08082 7.08062 7.08041 7.08021 7.08001 7.07981 7.07961 7.07941 7.07921 7.07901 7.07882 7.07862 7.07842 7.07823 7.07803 7.07784 7.07764 7.07745 7.07726 7.07707 7.07688 7.07669 7.0765 7.07631 7.07612 7.07594 7.07576 7.07557 7.07539 7.07521 7.07503 7.07486 7.07468 7.0745 7.07433 7.07416 7.07399 7.07382 7.07366 7.07349 7.07333 7.07317 7.07301 7.07285 7.07269 7.07254 7.07239 7.07224 7.0721 7.07195 7.07181 7.07167 7.07153 7.0714 7.07127 7.07114 7.07102 7.07089 7.07078 7.07066 7.07055 7.07044 7.07034 7.07024 7.07014 7.07005 7.06996 7.06988 7.0698 7.06973 7.06967 7.06962 7.06957 10.7429 10.7412 10.7396 10.738 10.7364 10.7348 10.7332 10.7316 10.73 10.7285 10.7269 10.7254 10.7238 10.7223 10.7208 10.7193 10.7178 10.7163 10.7149 10.7134 10.712 10.7105 10.7091 10.7077 10.7063 10.7049 10.7035 10.7021 10.7007 10.6994 10.698 10.6967 10.6953 10.694 10.6927 10.6914 10.6901 10.6889 10.6876 10.6863 10.6851 10.6839 10.6826 10.6814 10.6802 10.679 10.6778 10.6767 10.6755 10.6744 10.6732 10.6721 10.671 10.6699 10.6688 10.6677 10.6666 10.6655 10.6645 10.6634 10.6624 10.6614 10.6604 10.6594 10.6584 10.6574 10.6564 10.6555 10.6545 10.6536 10.6526 10.6517 10.6508 10.6499 10.6491 10.6482 10.6473 10.6465 10.6456 10.6448 10.644 10.6432 10.6424 10.6416 10.6408 10.6401 10.6393 10.6386 10.6378 10.6371 10.6364 10.6357 10.635 10.6344 10.6337 10.6331 10.6324 10.6318 10.6312 10.6306 10.63 10.6294 10.6288 10.6282 10.6277 10.6271 10.6266 10.6261 10.6256 10.6251 10.6246 10.6241 10.6237 10.6232 10.6228 10.6224 10.6219 10.6215 10.6211 10.6208 10.6204 10.62 10.6197 10.6193 10.619 10.6187 10.6184 10.6181 10.6178 10.6175 10.6173 10.617 10.6168 10.6166 10.6163 10.6161 10.6159 10.6158 10.6156 10.6154 10.6153 10.6152 10.615 10.6149 10.6148 10.6147 10.6146 10.6146 10.6145 10.6145 10.6144 10.6144 10.6144 10.6144 10.6144 10.6144 10.6145 10.6145 10.6146 10.6146 10.6147 10.6148 10.6149 10.615 10.6151 10.6153 10.6154 10.6156 10.6158 10.6159 10.6161 10.6163 10.6165 10.6168 10.617 10.6173 10.6175 10.6178 10.6181 10.6184 10.6187 10.619 10.6193 10.6196 10.62 10.6204 10.6207 10.6211 10.6215 10.6219 10.6223 10.6228 10.6232 10.6236 10.6241 10.6246 10.6251 10.6256 10.6261 10.6266 10.6271 10.6276 10.6282 10.6288 10.6293 10.6299 10.6305 10.6311 10.6317 10.6324 10.633 10.6336 10.6343 10.635 10.6357 10.6364 10.6371 10.6378 10.6385 10.6392 10.64 10.6408 10.6415 10.6423 10.6431 10.6439 10.6447 10.6456 10.6464 10.6472 10.6481 10.649 10.6498 10.6507 10.6516 10.6526 10.6535 10.6544 10.6554 10.6563 10.6573 10.6583 10.6593 10.6603 10.6613 10.6623 10.6633 10.6644 10.6654 10.6665 10.6676 10.6686 10.6697 10.6708 10.672 10.6731 10.6742 10.6754 10.6765 10.6777 10.6789 10.6801 10.6813 10.6825 10.6837 10.685 10.6862 10.6875 10.6887 10.69 10.6913 10.6926 10.6939 10.6952 10.6965 10.6979 10.6992 10.7006 10.702 10.7033 10.7047 10.7061 10.7075 10.709 10.7104 10.7118 10.7133 10.7147 10.7162 10.7177 10.7192 10.7207 10.7222 10.7237 10.7252 10.7268 10.7283 10.7299 10.7315 10.7331 10.7347 10.7363 10.7379 10.7395 10.7411 10.7428 10.7444 10.7461 10.7477 10.7494 10.7511 10.7528 10.7545 10.7562 10.758 10.7597 10.7615 10.7632 10.765 10.7668 10.7686 10.7704 10.7722 10.774 10.7758 10.7776 10.7795 10.7813 10.7832 10.7851 10.787 10.7888 10.7907 10.7927 10.7946 10.7965 10.7984 10.8004 10.8024 10.8043 10.8063 10.8083 10.8103 10.8123 10.8143 10.8163 10.8183 10.8204 10.8224 10.8245 10.8265 10.8286 10.8307 10.8328 10.8349 10.837 10.8391 10.8413 10.8434 10.8455 10.8477 10.8499 10.852 10.8542 10.8564 10.8586 10.8608 10.863 10.8653 10.8675 10.8697 10.872 10.8742 10.8765 10.8788 10.8811 10.8834 10.8857 10.888 10.8903 10.8926 10.8949 10.8973 10.8996 10.902 10.9044 10.9067 10.9091 10.9115 10.9139 10.9163 10.9187 10.9211 10.9236 10.926 10.9285 10.9309 10.9334 10.9358 10.9383 10.9408 10.9433 10.9458 10.9483 10.9508 10.9534 10.9559 10.9584 10.961 10.9635 10.9661 10.9687 10.9712 10.9738 10.9764 10.979 10.9816 10.9842 10.9869 10.9895 10.9921 10.9948 10.9974 11.0001 11.0028 11.0054 11.0081 11.0108 11.0135 11.0162 11.0189 11.0216 11.0243 11.0271 11.0298 11.0325 11.0353 11.038 11.0408 11.0436 11.0464 11.0491 11.0519 11.0547 11.0575 11.0603 11.0632 11.066 11.0688 11.0716 11.0745 11.0773 11.0802 11.0831 11.0859 11.0888 11.0892 11.0885 11.0879 11.0872 ];
minDistance = 7.05153
minAngle = 0.317301
angles2 = [2.35619 0.785398 1.9635 1.1781 1.76715 1.37445 1.66897 1.47262 1.86532 1.47262 1.76715 1.5708 1.71806 1.61988 1.69351 1.64443 1.7426 1.64443 1.71806 1.66897 1.70579 1.68124 ];
distances2 = [10.6324 7.10019 10.6408 7.09789 10.7098 7.08702 7.07021 7.08067 10.6701 7.08067 10.7098 7.07468 10.7334 7.07215 7.06961 7.07109 10.7213 7.07109 10.7334 7.07021 10.7396 7.06987 ];
minDistance2 = 7.06961
minAngle2 = 1.69351










plot(angles,distances,'r-',minAngle, minDistance,'r*',angles2,distances2,'g.',minAngle2, minDistance2,'g*',actual_neigh_ang,actualDistance,'k*')

hold on

minAnglesArray=min(angles);
maxAnglesArray=max(angles);
minDistancesArray=min(distances);
maxDistancesArray=max(distances);
num_angles = 40;
step = (maxAnglesArray-minAnglesArray)/num_angles;
xValues =  [minAnglesArray:step:maxAnglesArray]
plot(xValues,((maxDistancesArray+minDistancesArray)/2) * ones(1,size(xValues,2)),'*')   