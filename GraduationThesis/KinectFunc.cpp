/*!
* @file KinectFunc.cpp
* @brief Kinectクラスの関数の実装ファイル
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
 * @brief Kinect::~Kinect()関数.デストラクタ
 * @param 
 */
Kinect::~Kinect()
{
	/* 終了処理 */
	if (kinect != 0){
		kinect->NuiShutdown(); //Kinectの終了処理
		kinect->Release(); //インスタンスの解放
	}
}

/*!
 * @brief void Kinect::createInstance()関数.Kinect接続のためのインスタンスを生成
 * @param 
 * @return 
 */
void Kinect::createInstance()
{
	/* 接続されているKinectの数を取得する */
	int count = 0; //Kinectの数を初期化
	ERROR_CHECK(::NuiGetSensorCount(&count));
	if (count == 0){
		throw runtime_error("Kinectを接続してください");
	}

	/* 最初のKinectのインスタンスを作成する */
	ERROR_CHECK(::NuiCreateSensorByIndex(0, &kinect));

	/* Kinectの状態を取得する */
	HRESULT status = kinect->NuiStatus(); //COMインタフェース関連の戻り値として利用されることが多く成功だとS_OKなどエラーの原因までわかる
	if (status != S_OK){
		throw runtime_error("Kinectが利用可能ではありません");
	}
}

/*!
 * @brief void Kinect::initialize()関数.Kinectの初期化
 * @param 
 * @return 
 */
void Kinect::initialize()
{
	createInstance(); //createInstance()を呼び出す

	/* Kinectの設定を初期化する */
	ERROR_CHECK(kinect->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH)); //RGB-Dカメラを扱うためのフラグ追加

	/* RGBカメラの初期化 */
	ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, CAMERA_RESOLUTION, 0, 2, 0, &imageStreamHandle));

	/* 距離カメラの初期化 */
	ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, CAMERA_RESOLUTION, 0, 2, 0, &depthStreamHandle));

	/* Nearモード.使用しない場合はコメントアウトする */
	ERROR_CHECK(kinect->NuiImageStreamSetImageFrameFlags(depthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE));

	/* フレーム更新のイベントハンドルを作成する */
	streamEvent = ::CreateEvent(0, TRUE, FALSE, 0);
	ERROR_CHECK(kinect->NuiSetFrameEndEvent(streamEvent, 0));

	/* 指定した解像度の画面サイズを取得する */
	::NuiImageResolutionToSize(CAMERA_RESOLUTION, width, height);
}

/*!
* @brief void Kinect::drawRGBImage()関数.RGBデータを取得する
* @param image Kinectが取得した画像
* @return
*/
void Kinect::drawRGBImage(cv::Mat& image)
{
	/* RGBカメラのフレームデータを取得する */
	NUI_IMAGE_FRAME imageFrame = { 0 }; //すべてのフレームを0で初期化
	ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(imageStreamHandle, 0, &imageFrame)); //新しいフレームを取得する

	/* フレームの画像データを取得する */
	NUI_LOCKED_RECT colorData;
	imageFrame.pFrameTexture->LockRect(0, &colorData, 0, 0);

	/* フレームの画像データを取得する */
	image = cv::Mat(height, width, CV_8UC4, colorData.pBits);

	/* フレームデータを解放する */
	ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(imageStreamHandle, &imageFrame));
}

/*!
* @brief void Kinect::drawDepthImage()関数.距離データを取得する
* @param image Kinectが取得した画像
* @return
*/
void Kinect::drawDepthImage(cv::Mat& image)
{
	/* 距離画像準備.距離画像がほしい場合のみ利用する */
	//image = cv::Mat(height,width,CV_8UC1,cv::Scalar(0));

	/* 距離カメラのフレームデータを取得する */
	NUI_IMAGE_FRAME depthFrame = { 0 }; //すべてのフレームを0で初期化
	ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame));
	
	/* 距離データを取得する */
	NUI_LOCKED_RECT depthData = { 0 };
	depthFrame.pFrameTexture->LockRect(0, &depthData, 0, 0);

	USHORT* depth = (USHORT*)depthData.pBits;

	for (unsigned int i = 0; i < (depthData.size / sizeof(USHORT)); ++i){
		USHORT distance = ::NuiDepthPixelToDepth(depth[i]);

		LONG depthX = i%width;
		LONG depthY = i / width;

		LONG colorX = depthX;
		LONG colorY = depthY;

		//image.at<UCHAR>(colorY,colorX) = distance/8192.0*255.0;

		kinect->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(CAMERA_RESOLUTION, CAMERA_RESOLUTION, 0, depthX, depthY, depth[i], &colorX, &colorY);
	}
	ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(depthStreamHandle, &depthFrame));
}

/*!
* @brief void Kinect::GetDepthData()関数.depthdataをdist[]へ読み込む.(x,y,z)のデータを作る
* @param image Kinectが取得した画像
* @return
*/
void Kinect::GetDepthData(cv::Mat& image)
{
	FILE *fp1, *fp2; //ファイルポインタ
	errno_t error1, error2; //errno_t型の変数

	if (error1 = fopen_s(&fp1, "data/depthdata.txt", "w") != 0){
		throw runtime_error("depthdataファイルが開けません");
	}

	if (error2 = fopen_s(&fp2, "data/xyz.txt", "w") != 0){
		throw runtime_error("xyz.txtか開けません");
	}

	/* 距離カメラのフレームデータを取得する */
	NUI_IMAGE_FRAME depthFrame = { 0 }; //すべてのフレームを0で初期化
	ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame));

	/* 距離データを取得する */
	NUI_LOCKED_RECT depthData = { 0 };
	depthFrame.pFrameTexture->LockRect(0, &depthData, 0, 0);

	USHORT* depth = (USHORT*)depthData.pBits;

	for (unsigned int i = 0; i < (depthData.size / sizeof(USHORT)); ++i){
		USHORT distance = ::NuiDepthPixelToDepth(depth[i]);

		fprintf(fp1, "%d\n", distance);
		dist[i] = distance; //距離データを変数dist[i]に格納
	}

	/* (x,y,z)の組を作る */
	int d = 0;
	for (int y = 0; y < 480; y++){
		for (int x = 0; x < 640; x++){
			fprintf(fp2, "%d %d %d\n",x, y, dist[d]);
			d++;
		}
	}

	fclose(fp1);
	fclose(fp2);

	ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(depthStreamHandle, &depthFrame));
}

void Kinect::loadInternalCameraParameter(const string camera_param)
{
	cv::FileStorage fs(camera_param, cv::FileStorage::READ);
	fs["camera_matrix"] >> internal_cameraparam;
	fs["distortion_coefficient"] >> distortion_coefficient;

	return;
}

cv::Mat Kinect::getUndistortionImage(cv::Mat &image)
{
	cv::Mat undistortion_img;
	cv::undistort(image, undistortion_img, internal_cameraparam, distortion_coefficient, cv::Mat());

	return undistortion_img;
}

/*!
* @brief void Kinect::run()関数.Kinectの処理を実行する
* @param
* @return
*/
void Kinect::run()
{
	cv::Mat image; //Kinectが取得した画像

	cout << "============== メニュー ==============" << endl;
	cout << "1：今写っている平面を計測する" << endl;
	cout << "2：計測を終了する" << endl;
	cout << "(計測の途中で終了することもできます)" << endl;
	cout << "======================================" << endl << endl;

	//メインループ
	while (1){
		/* データの更新を待つ */
		DWORD ret = ::WaitForSingleObject(streamEvent, INFINITE);
		::ResetEvent(streamEvent); //次のイベントに備えてリセット

		drawRGBImage(image); //RGBデータの取得

		drawDepthImage(image); //Depthデータの取得

		loadInternalCameraParameter("cameraParam.xml");

		image = getUndistortionImage(image);

		cv::namedWindow("動画像", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO); //動画像が表示されるウインドウを定義
		cv::imshow("動画像", image); //動画像を表示

		int key = cv::waitKey(1); //動画像を表示し続ける

		if (key == '2'){
			cout << "終了します" << endl;
			break;
		}
		else if (key == '1'){
			sumtime = 0; //処理にかかる合計時間の初期化
			f1 = 1000.0 / cv::getTickFrequency();
			time1 = cv::getTickCount();
			time1th = 0;

			points = 0; //取得した点の数を初期化
			cnt_getcoordinate = 0; //座標を取得した回数を初期化

			cv::namedWindow("静止画像", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO); //静止画像が表示されるウインドウを定義

			cout << MAX_POINTS << "回クリックしてください" << endl;

			cv::setMouseCallback("静止画像", Mouse); //マウスコールバック関数の設定

			cv::imwrite("plot_img/rgbimage.bmp", image); //取得した静止画像を保存
			cv::imshow("静止画像", image); //取得した静止画像を表示

			time1th = (cv::getTickCount() - time1)*f1; //処理にかかった時間を計測
			cout << "\"1\"キーを入力してから表示するまでの時間" << time1th << "[ms]" << endl << endl;

			cout << "============================ 取得した点の座標 ============================" << endl;

			GetDepthData(image); //距離データを取得
		}
	}
}