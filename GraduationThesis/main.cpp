/*!
* @file main.cpp
* @brief �N���X���g�����̃t�@�C��
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/* �O���[�o���ϐ� */
/* �����f�[�^ */
unsigned short dist[307200]; //!<�����f�[�^���i�[����z��
/* �}�E�X����֘A */
cv::Point pushpt[MAX_POINTS]; //!<�擾�����_�̍��W
int points; //!<�擾�����_�̐�
cv::Point pt[MAX_POINTS]; //!<�擾�������W�̃R�s�[
int cnt_getcoordinate; //!<���W���擾�����񐔂��J�E���g
int onedim[MAX_POINTS]; //!<���ꂼ��̓V���ꎟ���ɒ���
double real_x[MAX_POINTS]; //!<3D��ԍ��W��X
double real_y[MAX_POINTS]; //!<3D��ԍ��W��Y
double real_z[MAX_POINTS]; //!<3D��ԍ��W��Z

double f1, f2, f3;
int64 time1, time2, time3;
double time1th; //!<"p"�L�[����͂��Ă���\������܂ł̎���
double time2th; //!<9�_�ڂ��N���b�N���Ă����]�p�x�����߂�܂ł̎���
double time3th; //!<gnuplot����Ƃ��s���Ă��鎞��
double sumtime; //!<�����ɂ����������Ԃ̍��v

cv::Mat internal_cameraparam;
cv::Mat distortion_coefficient;

void Mouse();

int main()
{
	try{
		Kinect kinect; //Kinect�N���X�̃C���X�^���Xkinect�𐶐�

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