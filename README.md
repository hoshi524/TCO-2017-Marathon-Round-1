# TCO-2017-Marathon-Round-1

## link

https://community.topcoder.com/longcontest/?module=ViewProblemStatement&rd=16903&compid=55119
https://community.topcoder.com/longcontest/?module=ViewStandings&rd=16903

## memo

近傍の選択について改善したい？

## algorithm

- ランダムに頂点を設定
- 近傍定義
    - 1点のみ動かす方が良いか、多点が良いか
    - 遷移先は、上下か、斜めも含めるか
- 重心を中央に直す

## memo

0.9  = 0.770
0.8  = 0.772
0.75 = 0.774
0.7  = 0.777
0.65 = 0.771
0.6  = 0.764

50 0.7 = 781
50 0.6 = 780
50 0.5 = 781
50 0.4 = 781
50 0.3 = 781

90 0.5 = 781
70 0.5 = 781
50 0.5 = 781
40 0.5 = 780
30 0.5 = 780
20 0.5 = 779
10 0.5 = 779
 0 0.5 = 775

1.0 = 781
1.5 = 779