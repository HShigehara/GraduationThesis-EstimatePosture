/*!
* @file KinectFunc.cpp
* @brief Kinect�N���X�̊֐��̎����t�@�C��
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
 * @brief Kinect::~Kinect()�֐�.�f�X�g���N�^
 * @param 
 */
Kinect::~Kinect()
{
	/* �I������ */
	if (kinect != 0){
		kinect->NuiShutdown(); //Kinect�̏I������
		kinect->Release(); //�C���X�^���X�̉��
	}
}

/*!
 * @brief void Kinect::createInstance()�֐�.Kinect�ڑ��̂��߂̃C���X�^���X�𐶐�
 * @param 
 * @return 
 */
void Kinect::createInstance()
{
	/* �ڑ�����Ă���Kinect�̐����擾���� */
	int count = 0; //Kinect�̐���������
	ERROR_CHECK(::NuiGetSensorCount(&count));
	if (count == 0){
		throw runtime_error("Kinect��ڑ����Ă�������");
	}

	/* �ŏ���Kinect�̃C���X�^���X���쐬���� */
	ERROR_CHECK(::NuiCreateSensorByIndex(0, &kinect));

	/* Kinect�̏�Ԃ��擾���� */
	HRESULT status = kinect->NuiStatus(); //COM�C���^�t�F�[�X�֘A�̖߂�l�Ƃ��ė��p����邱�Ƃ�������������S_OK�ȂǃG���[�̌����܂ł킩��
	if (status != S_OK){
		throw runtime_error("Kinect�����p�\�ł͂���܂���");
	}
}

/*!
 * @brief void Kinect::initialize()�֐�.Kinect�̏�����
 * @param 
 * @return 
 */
void Kinect::initialize()
{
	createInstance(); //createInstance()���Ăяo��

	/* Kinect�̐ݒ������������ */
	ERROR_CHECK(kinect->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH)); //RGB-D�J�������������߂̃t���O�ǉ�

	/* RGB�J�����̏����� */
	ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, CAMERA_RESOLUTION, 0, 2, 0, &imageStreamHandle));

	/* �����J�����̏����� */
	ERROR_CHECK(kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, CAMERA_RESOLUTION, 0, 2, 0, &depthStreamHandle));

	/* Near���[�h.�g�p���Ȃ��ꍇ�̓R�����g�A�E�g���� */
	ERROR_CHECK(kinect->NuiImageStreamSetImageFrameFlags(depthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE));

	/* �t���[���X�V�̃C�x���g�n���h�����쐬���� */
	streamEvent = ::CreateEvent(0, TRUE, FALSE, 0);
	ERROR_CHECK(kinect->NuiSetFrameEndEvent(streamEvent, 0));

	/* �w�肵���𑜓x�̉�ʃT�C�Y���擾���� */
	::NuiImageResolutionToSize(CAMERA_RESOLUTION, width, height);
}

/*!
* @brief void Kinect::drawRGBImage()�֐�.RGB�f�[�^���擾����
* @param image Kinect���擾�����摜
* @return
*/
void Kinect::drawRGBImage(cv::Mat& image)
{
	/* RGB�J�����̃t���[���f�[�^���擾���� */
	NUI_IMAGE_FRAME imageFrame = { 0 }; //���ׂẴt���[����0�ŏ�����
	ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(imageStreamHandle, 0, &imageFrame)); //�V�����t���[�����擾����

	/* �t���[���̉摜�f�[�^���擾���� */
	NUI_LOCKED_RECT colorData;
	imageFrame.pFrameTexture->LockRect(0, &colorData, 0, 0);

	/* �t���[���̉摜�f�[�^���擾���� */
	image = cv::Mat(height, width, CV_8UC4, colorData.pBits);

	/* �t���[���f�[�^��������� */
	ERROR_CHECK(kinect->NuiImageStreamReleaseFrame(imageStreamHandle, &imageFrame));
}

/*!
* @brief void Kinect::drawDepthImage()�֐�.�����f�[�^���擾����
* @param image Kinect���擾�����摜
* @return
*/
void Kinect::drawDepthImage(cv::Mat& image)
{
	/* �����摜����.�����摜���ق����ꍇ�̂ݗ��p���� */
	//image = cv::Mat(height,width,CV_8UC1,cv::Scalar(0));

	/* �����J�����̃t���[���f�[�^���擾���� */
	NUI_IMAGE_FRAME depthFrame = { 0 }; //���ׂẴt���[����0�ŏ�����
	ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame));
	
	/* �����f�[�^���擾���� */
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
* @brief void Kinect::GetDepthData()�֐�.depthdata��dist[]�֓ǂݍ���.(x,y,z)�̃f�[�^�����
* @param image Kinect���擾�����摜
* @return
*/
void Kinect::GetDepthData(cv::Mat& image)
{
	FILE *fp1, *fp2; //�t�@�C���|�C���^
	errno_t error1, error2; //errno_t�^�̕ϐ�

	if (error1 = fopen_s(&fp1, "data/depthdata.txt", "w") != 0){
		throw runtime_error("depthdata�t�@�C�����J���܂���");
	}

	if (error2 = fopen_s(&fp2, "data/xyz.txt", "w") != 0){
		throw runtime_error("xyz.txt���J���܂���");
	}

	/* �����J�����̃t���[���f�[�^���擾���� */
	NUI_IMAGE_FRAME depthFrame = { 0 }; //���ׂẴt���[����0�ŏ�����
	ERROR_CHECK(kinect->NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame));

	/* �����f�[�^���擾���� */
	NUI_LOCKED_RECT depthData = { 0 };
	depthFrame.pFrameTexture->LockRect(0, &depthData, 0, 0);

	USHORT* depth = (USHORT*)depthData.pBits;

	for (unsigned int i = 0; i < (depthData.size / sizeof(USHORT)); ++i){
		USHORT distance = ::NuiDepthPixelToDepth(depth[i]);

		fprintf(fp1, "%d\n", distance);
		dist[i] = distance; //�����f�[�^��ϐ�dist[i]�Ɋi�[
	}

	/* (x,y,z)�̑g����� */
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
* @brief void Kinect::run()�֐�.Kinect�̏��������s����
* @param
* @return
*/
void Kinect::run()
{
	cv::Mat image; //Kinect���擾�����摜

	cout << "============== ���j���[ ==============" << endl;
	cout << "1�F���ʂ��Ă��镽�ʂ��v������" << endl;
	cout << "2�F�v�����I������" << endl;
	cout << "(�v���̓r���ŏI�����邱�Ƃ��ł��܂�)" << endl;
	cout << "======================================" << endl << endl;

	//���C�����[�v
	while (1){
		/* �f�[�^�̍X�V��҂� */
		DWORD ret = ::WaitForSingleObject(streamEvent, INFINITE);
		::ResetEvent(streamEvent); //���̃C�x���g�ɔ����ă��Z�b�g

		drawRGBImage(image); //RGB�f�[�^�̎擾

		drawDepthImage(image); //Depth�f�[�^�̎擾

		loadInternalCameraParameter("cameraParam.xml");

		image = getUndistortionImage(image);

		cv::namedWindow("���摜", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO); //���摜���\�������E�C���h�E���`
		cv::imshow("���摜", image); //���摜��\��

		int key = cv::waitKey(1); //���摜��\����������

		if (key == '2'){
			cout << "�I�����܂�" << endl;
			break;
		}
		else if (key == '1'){
			sumtime = 0; //�����ɂ����鍇�v���Ԃ̏�����
			f1 = 1000.0 / cv::getTickFrequency();
			time1 = cv::getTickCount();
			time1th = 0;

			points = 0; //�擾�����_�̐���������
			cnt_getcoordinate = 0; //���W���擾�����񐔂�������

			cv::namedWindow("�Î~�摜", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO); //�Î~�摜���\�������E�C���h�E���`

			cout << MAX_POINTS << "��N���b�N���Ă�������" << endl;

			cv::setMouseCallback("�Î~�摜", Mouse); //�}�E�X�R�[���o�b�N�֐��̐ݒ�

			cv::imwrite("plot_img/rgbimage.bmp", image); //�擾�����Î~�摜��ۑ�
			cv::imshow("�Î~�摜", image); //�擾�����Î~�摜��\��

			time1th = (cv::getTickCount() - time1)*f1; //�����ɂ����������Ԃ��v��
			cout << "\"1\"�L�[����͂��Ă���\������܂ł̎���" << time1th << "[ms]" << endl << endl;

			cout << "============================ �擾�����_�̍��W ============================" << endl;

			GetDepthData(image); //�����f�[�^���擾
		}
	}
}