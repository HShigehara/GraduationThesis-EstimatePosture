/*!
* @file main.cpp
* @brief クラスを使う側のファイル
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/* グローバル変数 */
/* 距離データ */
unsigned short dist[307200]; //!<距離データを格納する配列
/* マウス操作関連 */
cv::Point pushpt[MAX_POINTS]; //!<取得した点の座標
int points; //!<取得した点の数
cv::Point pt[MAX_POINTS]; //!<取得した座標のコピー
int cnt_getcoordinate; //!<座標を取得した回数をカウント
int onedim[MAX_POINTS]; //!<それぞれの天を一次元に直す
double real_x[MAX_POINTS]; //!<3D空間座標のX
double real_y[MAX_POINTS]; //!<3D空間座標のY
double real_z[MAX_POINTS]; //!<3D空間座標のZ

double f1, f2, f3;
int64 time1, time2, time3;
double time1th; //!<"p"キーを入力してから表示するまでの時間
double time2th; //!<9点目をクリックしてから回転角度を求めるまでの時間
double time3th; //!<gnuplotが作業を行っている時間
double sumtime; //!<処理にかかった時間の合計

cv::Mat internal_cameraparam;
cv::Mat distortion_coefficient;

void Mouse();

int main()
{
	try{
		Kinect kinect; //Kinectクラスのインスタンスkinectを生成

		kinect.initialize();

		_mkdir("data");
		//_mkdir("plot_img");

		kinect.run();

		exit(0);
	}
	catch (exception& ex){
		cout << ex.what() << endl;
	}
	return 0;
}