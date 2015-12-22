/*!
 * @file EstimatePosture.h
 * @brief プロジェクトのヘッダファイル
 * @date 2014/03/26
 * @author H.Shigehara
 */

/* Windows APIとNui APIのインクルード */
#include<Windows.h>
#include<NuiApi.h>

/* C++のインクルード */
#include<iostream> //標準入出力
using namespace std;
#include<fstream> //ファイル入出力

/* Cのインクルード */
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<direct.h> //フォルダの作成

/* OpenCV関係のインクルード */
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>

/* 定義 */
#define MAX_POINTS 17 //!< マウスクリック時に取得する点の数
#define PI 3.1415926535897932384626433832795 //!<円周率π

/* グローバル変数 */
/* 距離データ */
extern unsigned short dist[307200]; //!<距離データを格納する配列
/* マウス操作関連 */
extern cv::Point pushpt[MAX_POINTS]; //!<取得した点の座標
extern int points; //!<取得した点の数
extern cv::Point pt[MAX_POINTS]; //!<取得した座標のコピー
extern int cnt_getcoordinate; //!<座標を取得した回数をカウント
extern int onedim[MAX_POINTS]; //!<それぞれの天を一次元に直す
extern double real_x[MAX_POINTS]; //!<3D空間座標のX
extern double real_y[MAX_POINTS]; //!<3D空間座標のY
extern double real_z[MAX_POINTS]; //!<3D空間座標のZ

extern double f1, f2, f3;
extern int64 time1, time2, time3;
extern double time1th; //!<"p"キーを入力してから表示するまでの時間
extern double time2th; //!<9点目をクリックしてから回転角度を求めるまでの時間
extern double time3th; //!<gnuplotが作業を行っている時間
extern double sumtime; //!<処理にかかった時間の合計

extern cv::Mat internal_cameraparam;
extern cv::Mat distortion_coefficient;

extern void Mouse(int event,int x,int y,int flags,void *param);

/* Class定義 */
/*!
 * @class Kinect
 * @brief Kinect操作
 */
class Kinect
{
private:
	INuiSensor* kinect; //!<INuiSensor型のkinect::kinectを操作するためのインターフェース
	HANDLE imageStreamHandle; //!<RGBカメラのストリームデータを扱うためのハンドル
	HANDLE depthStreamHandle; //!<Depthカメラのストリームデータを扱うためのハンドル
	HANDLE streamEvent; //!<RGB-Dカメラのフレーム更新イベントを待つためのイベントハンドル

	DWORD width; //!<幅
	DWORD height; //!<高さ
	void createInstance(); //!<インスタンスの生成

public:
	~Kinect(); //!<デストラクタ
	void initialize(); //!<Kinectの初期化
	void run(); //!<Kinectによる処理
	void drawRGBImage(cv::Mat& image); //!<RGB画像の描画
	void drawDepthImage(cv::Mat& image); //!<Depth画像の描画
	void GetDepthData(cv::Mat& image); //!<Depthデータの取得

	void loadInternalCameraParameter(const string camera_param); //!<キャリブレーション用のカメラパラメータを取得する
	cv::Mat getUndistortionImage(cv::Mat &image);
};

/*!
 * @class CoordinateTransform
 * @brief 2D→3Dへの座標変換
 */
class CoordinateTransform
{
private:
	Vector4 WorldCoordinate; //!<3次元座標の(x,y,z)

public:
	int ct2Dto3D(cv::Point pushpt, int points); //!<3D空間座標へ変換
};

/*!
 * @class EstimatePosture
 * @brief 最小二乗法によって角度を計算するクラス
 */
class EstimatePosture
{
private:
	double Szz,Sxz,Syz,Sz,Sxx,Sxy,Sx,Syy,Sy; //!<最小二乗法に必要な要素

	cv::Mat m1; //!<最小二乗法の一つ目の行列
	cv::Mat m2; //!<最小二乗法の二つ目の行列
	cv::Mat A; //!<求めた係数a,b,cの行列

	double yaw_rad; //!<Y軸周りの回転の角度[rad]
	double roll_rad; //!<Z軸周りの回転の角度[rad]
	double pitch_rad; //!<X軸周りの回転の角度[rad]
	double yaw_deg; //!<Y軸周りの回転の角度[deg]
	double roll_deg; //!<Z軸周りの回転の角度[deg]
	double pitch_deg; //!<X軸周りの回転の角度[deg]

public:
	double a, b, c; //!<最小二乗法で求めた平面の係数a,b,c

	EstimatePosture(); //!<EstimatePostureクラスのコンストラクタ
	void LeastSqureMethod(double real_x[MAX_POINTS - 1], double real_y[MAX_POINTS - 1], double real_z[MAX_POINTS - 1]); //!<最小二乗法
	void CalcYawRollPitch(double a, double b, double c); //!<回転角度の計算
};

/*!
* @class Plot
* @brief gnuplotによる画像を表示、保存する
*/
class Plot
{
private:
	FILE *gnuplot; //!<gnuplotへのパイプ

public:
	void PlotData(double a,double b,double c); //!<取得したデータをプロットする関数
};

/*! @brief エラーチェック */
#define ERROR_CHECK(ret) \
if (ret != S_OK){ \
	stringstream ss; \
	ss << "faild " #ret " " << hex << ret << endl; \
	throw runtime_error(ss.str().c_str()); \
}

/*! @brief Kinectの解像度 */
const NUI_IMAGE_RESOLUTION CAMERA_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;