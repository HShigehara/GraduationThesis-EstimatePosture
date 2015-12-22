/*!
 * @file EstimatePosture.h
 * @brief �v���W�F�N�g�̃w�b�_�t�@�C��
 * @date 2014/03/26
 * @author H.Shigehara
 */

/* Windows API��Nui API�̃C���N���[�h */
#include<Windows.h>
#include<NuiApi.h>

/* C++�̃C���N���[�h */
#include<iostream> //�W�����o��
using namespace std;
#include<fstream> //�t�@�C�����o��

/* C�̃C���N���[�h */
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<direct.h> //�t�H���_�̍쐬

/* OpenCV�֌W�̃C���N���[�h */
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>

/* ��` */
#define MAX_POINTS 17 //!< �}�E�X�N���b�N���Ɏ擾����_�̐�
#define PI 3.1415926535897932384626433832795 //!<�~������

/* �O���[�o���ϐ� */
/* �����f�[�^ */
extern unsigned short dist[307200]; //!<�����f�[�^���i�[����z��
/* �}�E�X����֘A */
extern cv::Point pushpt[MAX_POINTS]; //!<�擾�����_�̍��W
extern int points; //!<�擾�����_�̐�
extern cv::Point pt[MAX_POINTS]; //!<�擾�������W�̃R�s�[
extern int cnt_getcoordinate; //!<���W���擾�����񐔂��J�E���g
extern int onedim[MAX_POINTS]; //!<���ꂼ��̓V���ꎟ���ɒ���
extern double real_x[MAX_POINTS]; //!<3D��ԍ��W��X
extern double real_y[MAX_POINTS]; //!<3D��ԍ��W��Y
extern double real_z[MAX_POINTS]; //!<3D��ԍ��W��Z

extern double f1, f2, f3;
extern int64 time1, time2, time3;
extern double time1th; //!<"p"�L�[����͂��Ă���\������܂ł̎���
extern double time2th; //!<9�_�ڂ��N���b�N���Ă����]�p�x�����߂�܂ł̎���
extern double time3th; //!<gnuplot����Ƃ��s���Ă��鎞��
extern double sumtime; //!<�����ɂ����������Ԃ̍��v

extern cv::Mat internal_cameraparam;
extern cv::Mat distortion_coefficient;

extern void Mouse(int event,int x,int y,int flags,void *param);

/* Class��` */
/*!
 * @class Kinect
 * @brief Kinect����
 */
class Kinect
{
private:
	INuiSensor* kinect; //!<INuiSensor�^��kinect::kinect�𑀍삷�邽�߂̃C���^�[�t�F�[�X
	HANDLE imageStreamHandle; //!<RGB�J�����̃X�g���[���f�[�^���������߂̃n���h��
	HANDLE depthStreamHandle; //!<Depth�J�����̃X�g���[���f�[�^���������߂̃n���h��
	HANDLE streamEvent; //!<RGB-D�J�����̃t���[���X�V�C�x���g��҂��߂̃C�x���g�n���h��

	DWORD width; //!<��
	DWORD height; //!<����
	void createInstance(); //!<�C���X�^���X�̐���

public:
	~Kinect(); //!<�f�X�g���N�^
	void initialize(); //!<Kinect�̏�����
	void run(); //!<Kinect�ɂ�鏈��
	void drawRGBImage(cv::Mat& image); //!<RGB�摜�̕`��
	void drawDepthImage(cv::Mat& image); //!<Depth�摜�̕`��
	void GetDepthData(cv::Mat& image); //!<Depth�f�[�^�̎擾

	void loadInternalCameraParameter(const string camera_param); //!<�L�����u���[�V�����p�̃J�����p�����[�^���擾����
	cv::Mat getUndistortionImage(cv::Mat &image);
};

/*!
 * @class CoordinateTransform
 * @brief 2D��3D�ւ̍��W�ϊ�
 */
class CoordinateTransform
{
private:
	Vector4 WorldCoordinate; //!<3�������W��(x,y,z)

public:
	int ct2Dto3D(cv::Point pushpt, int points); //!<3D��ԍ��W�֕ϊ�
};

/*!
 * @class EstimatePosture
 * @brief �ŏ����@�ɂ���Ċp�x���v�Z����N���X
 */
class EstimatePosture
{
private:
	double Szz,Sxz,Syz,Sz,Sxx,Sxy,Sx,Syy,Sy; //!<�ŏ����@�ɕK�v�ȗv�f

	cv::Mat m1; //!<�ŏ����@�̈�ڂ̍s��
	cv::Mat m2; //!<�ŏ����@�̓�ڂ̍s��
	cv::Mat A; //!<���߂��W��a,b,c�̍s��

	double yaw_rad; //!<Y������̉�]�̊p�x[rad]
	double roll_rad; //!<Z������̉�]�̊p�x[rad]
	double pitch_rad; //!<X������̉�]�̊p�x[rad]
	double yaw_deg; //!<Y������̉�]�̊p�x[deg]
	double roll_deg; //!<Z������̉�]�̊p�x[deg]
	double pitch_deg; //!<X������̉�]�̊p�x[deg]

public:
	double a, b, c; //!<�ŏ����@�ŋ��߂����ʂ̌W��a,b,c

	EstimatePosture(); //!<EstimatePosture�N���X�̃R���X�g���N�^
	void LeastSqureMethod(double real_x[MAX_POINTS - 1], double real_y[MAX_POINTS - 1], double real_z[MAX_POINTS - 1]); //!<�ŏ����@
	void CalcYawRollPitch(double a, double b, double c); //!<��]�p�x�̌v�Z
};

/*!
* @class Plot
* @brief gnuplot�ɂ��摜��\���A�ۑ�����
*/
class Plot
{
private:
	FILE *gnuplot; //!<gnuplot�ւ̃p�C�v

public:
	void PlotData(double a,double b,double c); //!<�擾�����f�[�^���v���b�g����֐�
};

/*! @brief �G���[�`�F�b�N */
#define ERROR_CHECK(ret) \
if (ret != S_OK){ \
	stringstream ss; \
	ss << "faild " #ret " " << hex << ret << endl; \
	throw runtime_error(ss.str().c_str()); \
}

/*! @brief Kinect�̉𑜓x */
const NUI_IMAGE_RESOLUTION CAMERA_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;