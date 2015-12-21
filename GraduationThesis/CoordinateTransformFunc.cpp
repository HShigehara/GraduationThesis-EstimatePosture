/*!
* @file CoordinateTransformFunc.cpp
* @brief CoordinateTransformクラスの実装ファイル
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
* @brief int CoordinateTransform::ct2dto3d()関数.2Dの座標データを距離データと合わせて3Dデータに変換する
* @param pushpt Mouse()関数でクリックした静止画像の座標(x,y)
* @return 0 or -1.0なら取得した店の距離データが0以外であったということ.-1なら取得した点の距離データが0であったということ.-1なら点を再取得する
*/

int CoordinateTransform::ct2Dto3D(cv::Point pushpt, int points)
{
	EstimatePosture ep; //インスタンス生成

	ofstream ofs("data/RealPoints.dat"); //3次元座標に変換した時の(x,y,z)

	ofstream ofs_ave("data/ave.csv");
	double sum_x = 0;
	double sum_y = 0;
	double sum_z = 0;
	double ave_x = 0;
	double ave_y = 0;
	double ave_z = 0;

	pt[cnt_getcoordinate] = pushpt; //クリック時に取得した点の座標をコピーする

	onedim[cnt_getcoordinate] = pt[cnt_getcoordinate].x + 640 * pt[cnt_getcoordinate].y - 640; //2次元で表されている点を1次元に変換する
	cout << "pt[" << cnt_getcoordinate << "] => " << pt[cnt_getcoordinate] << " , onedim => " << onedim[cnt_getcoordinate] << " , dist[" << onedim[cnt_getcoordinate] - 1 << "] => " << dist[onedim[cnt_getcoordinate]] << endl;

	//距離データが0だった場合座標取得をやり直す
	if (dist[onedim[cnt_getcoordinate]] == 0){
		cout << "距離データが0だったのでもう一度周辺をクリックしてください" << endl;
		return -1;
	}

	WorldCoordinate = NuiTransformDepthImageToSkeleton((long)pt[cnt_getcoordinate].x, (long)pt[cnt_getcoordinate].y, dist[onedim[cnt_getcoordinate]] << 3, NUI_IMAGE_RESOLUTION_640x480);

	real_x[cnt_getcoordinate] = WorldCoordinate.x*1000.0;
	real_y[cnt_getcoordinate] = WorldCoordinate.y*1000.0;
	real_z[cnt_getcoordinate] = WorldCoordinate.z*1000.0;

	cout << "real_x[" << cnt_getcoordinate << "] => " << real_x[cnt_getcoordinate] << "[mm] real_y[" << cnt_getcoordinate << "] => " << real_y[cnt_getcoordinate] << "[mm] real_z[" << cnt_getcoordinate << "] => " << real_z[cnt_getcoordinate] << "[mm]" << endl;
	cout << "==========================================================================" << endl;

	//0~8までの点が揃ったらそれぞれの3次元座標をファイルへ出力し最小二乗法を行う
	if (points == MAX_POINTS - 1){
		for (int i = 0; i < MAX_POINTS; i++){
			sum_x = sum_x + real_x[i];
			sum_y = sum_y + real_y[i];
			sum_z = sum_z + real_z[i];

			ofs << real_x[i] << " " << real_y[i] << " " << real_z[i] << endl;
		}

		ave_x = sum_x / MAX_POINTS;
		ave_y = sum_y / MAX_POINTS;
		ave_z = sum_z / MAX_POINTS;
		ofs_ave << ave_x << "," << ave_y << "," << ave_z << endl;

		//処理時間を計測する
		f2 = 1000.0 / cv::getTickFrequency();
		time2 = cv::getTickCount();
		time2th = 0;

		ep.LeastSqureMethod(real_x, real_y, real_z); //最小二乗法の要素計算

		time2th = (cv::getTickCount() - time2)*f2;

		cout << MAX_POINTS << "点目をクリックしてから回転角度を求めるまでの時間" << time2th << "[ms]" << endl;

		sumtime = time1th + time2th - time3th;

		cout << "処理にかかった時間(gnuplotの処理時間を除く)は" << sumtime << "[ms] でした" << "\n" << endl;

		cout << "============== メニュー ==============" << endl;
		cout << "もう一度計測しますか??" << endl;
		cout << "画像上でキーを入力してください" << endl;
		cout << "1 : Yes" << endl;
		cout << "2 : No" << endl;
		cout << "======================================" << "\n" << endl;
	}

	cnt_getcoordinate++; //取得した点の数が9つになるまで座標を取得し続ける

	return 0;
}