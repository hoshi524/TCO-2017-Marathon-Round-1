# TCO-2017-Marathon-Round-1

## link

https://community.topcoder.com/longcontest/?module=ViewProblemStatement&rd=16903&compid=55119
https://community.topcoder.com/longcontest/?module=ViewStandings&rd=16903

## memo

焼きなましっぽい問題だけど、

- 貪欲に近傍を遷移できる気がする
- スコアに影響する辺が最小辺と最大辺のみでスコアが変わる近傍を少ない

とか適してない部分はある。

近傍に関しては遷移に偏りを持たせれば良いか。
スコアに関しては複合的な評価にするとかあるけど、もっとシンプルに良い評価はないものか。

scoreの算出法を勘違いしてた。
勘違いしたやつだと、score上限が0.55くらいが上限のはずだけど、Psyho氏が0.75以上のscoreを出していて勘違いに気づいた。
結局、評価値をどうやって設計するかが重要か。

## algorithm

- ランダムに頂点を設定
- 近傍定義
    - 1点のみ動かす方が良いか、多点が良いか
    - 遷移先は、上下か、斜めも含めるか
- 重心を中央に直す