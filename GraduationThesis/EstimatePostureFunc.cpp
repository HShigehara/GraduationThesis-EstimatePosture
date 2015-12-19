/*!
* @file EstimatePostureFunc.cpp
* @brief EstimatePostureクラスの実装ファイル
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
* @brief void EstimatePosture::EstimatePosture().EstimatePostureクラスのコンストラクタ
*/
EstimatePosture::EstimatePosture()
{
	//最小二乗法に必要な要素の初期化
	Szz = 0, Sxz = 0, Syz = 0, Sz = 0, Sxx = 0, Sxy = 0, Sx = 0, Syy = 0, Sy = 0;

	//最小二乗法によって求めた係数a,b,cの初期化
	a = 0, b = 0, c = 0;
}

/*!
* @brief void EstimatePosture::LeastSqureMethod()関数.最小二乗法の要素を計算する
* @param pushpt Mouse()関数でクリックした静止画像の座標(x,y)
* @return 0 or -1.0なら取得した店の距離データが0以外であったということ.-1なら取得した点の距離データが0であったということ.-1なら点を再取得する
*/
void EstimatePosture::LeastSqureMethod(double real_x[MAX_POINTS - 1], double real_y[MAX_POINTS - 1], double real_z[MAX_POINTS - 1])
{
	Plot pl; //Plotクラスのインスタンスを生成

	ofstream ofs("data/abc.dat");

	for (int i = 0; i < MAX_POINTS; i++){
		Szz = Szz + real_z[i] * real_z[i];
		Sxz = Sxz + real_x[i] * real_z[i];
		Syz = Syz + real_y[i] * real_z[i];
		Sz = Sz + real_z[i];
		Sxx = Sxx + real_x[i] * real_x[i];
		Sxy = Sxy + real_x[i] * real_y[i];
		Sx = Sx + real_x[i];
		Syy = Syy + real_y[i] * real_y[i];
		Sy = Sy + real_y[i];
	}

	m1 = (cv::Mat_<double>(3, 3) << Sxx, Sxy, Sx, Sxy, Syy, Sy, Sx, Sy, MAX_POINTS);
	m2 = (cv::Mat_<double>(3, 1) << Sxz, Syz, Sz);
	A = m1.inv()*m2;

	a = A.at<double>(0, 0);
	b = A.at<double>(0, 1);
	c = A.at<double>(0, 2);

	ofs << "a = " << a << "\n" << "b = " << b << "\n" << "c = " << c << endl;
	
	f3 = 1000.0 / cv::getTickFrequency();
	time3 = cv::getTickCount();
	time3th = 0;

	pl.PlotData(a, b, c); //データをプロットする.必要ない場合はコメントアウトする

	time3th = (cv::getTickCount() - time3)*f3;

	cout << "\n";
	cout << "gnuplotが作業を行っている時間" << time3th << "[ms]" << endl;

	CalcYawRollPitch(a, b, c);
}

/*!
* @brief void EstimatePosture::CalcYawRollPitch()関数.回転角度を計算する
* @param a 最小二乗法によって求めた平面の式aX+bY+c=Zの係数
* @param b 最小二乗法によって求めた平面の式aX+bY+c=Zの係数
* @param c 最小二乗法によって求めた平面の式aX+bY+c=Zの係数
* @return
*/
void EstimatePosture::CalcYawRollPitch(double a, double b, double c)
{
	char input = ' ';
	
	yaw_rad = -atan(a);
	yaw_deg = yaw_rad / PI*180.0;

	roll_rad = atan2(-a, b);
	roll_deg = roll_rad / PI*180.0;

	pitch_rad = atan2(1, b);
	pitch_deg = pitch_rad / PI*180.0;

	cout << "\n";
	cout << "====== 姿勢計測結果 ======" << endl;
	cout << "Yaw : " << yaw_deg << "[deg]" << endl;
	cout << "Roll : " << roll_deg << "[deg]" << endl;
	cout << "Pitch : " << pitch_deg << "[deg]" << endl;
	cout << "==========================" << "\n" << endl;
}