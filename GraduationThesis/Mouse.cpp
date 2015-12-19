/*!
* @file cMouseFunc.cpp
* @brief cMouseクラスの関数の実装ファイル
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
* @brief cMouse::Mouse()関数.コンストラクタ
* @param
*/
void Mouse(int event, int x, int y, int flags, void *param)
{
	CoordinateTransform ct; //CoordinateTransformクラスのSインスタンスの生成

	int check = 0; //距離データが0でないかチェックするフラグ.check == 0なら距離データが0以外を表す.

	if (event == CV_EVENT_LBUTTONDOWN){
		if (points < MAX_POINTS){
			pushpt[points] = cv::Point(x, y);
			check = ct.ct2Dto3D(pushpt[points], points);
			if (check == 0){
				points++;
			}
		}
	}
}