/*!
* @file EstimatePostureFunc.cpp
* @brief EstimatePosture�N���X�̎����t�@�C��
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
* @brief void EstimatePosture::EstimatePosture().EstimatePosture�N���X�̃R���X�g���N�^
*/
EstimatePosture::EstimatePosture()
{
	//�ŏ����@�ɕK�v�ȗv�f�̏�����
	Szz = 0, Sxz = 0, Syz = 0, Sz = 0, Sxx = 0, Sxy = 0, Sx = 0, Syy = 0, Sy = 0;

	//�ŏ����@�ɂ���ċ��߂��W��a,b,c�̏�����
	a = 0, b = 0, c = 0;
}

/*!
* @brief void EstimatePosture::LeastSqureMethod()�֐�.�ŏ����@�̗v�f���v�Z����
* @param pushpt Mouse()�֐��ŃN���b�N�����Î~�摜�̍��W(x,y)
* @return 0 or -1.0�Ȃ�擾�����X�̋����f�[�^��0�ȊO�ł������Ƃ�������.-1�Ȃ�擾�����_�̋����f�[�^��0�ł������Ƃ�������.-1�Ȃ�_���Ď擾����
*/
void EstimatePosture::LeastSqureMethod(double real_x[MAX_POINTS - 1], double real_y[MAX_POINTS - 1], double real_z[MAX_POINTS - 1])
{
	Plot pl; //Plot�N���X�̃C���X�^���X�𐶐�

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

	pl.PlotData(a, b, c); //�f�[�^���v���b�g����.�K�v�Ȃ��ꍇ�̓R�����g�A�E�g����

	time3th = (cv::getTickCount() - time3)*f3;

	cout << "\n";
	cout << "gnuplot����Ƃ��s���Ă��鎞��" << time3th << "[ms]" << endl;

	CalcYawRollPitch(a, b, c);
}

/*!
* @brief void EstimatePosture::CalcYawRollPitch()�֐�.��]�p�x���v�Z����
* @param a �ŏ����@�ɂ���ċ��߂����ʂ̎�aX+bY+c=Z�̌W��
* @param b �ŏ����@�ɂ���ċ��߂����ʂ̎�aX+bY+c=Z�̌W��
* @param c �ŏ����@�ɂ���ċ��߂����ʂ̎�aX+bY+c=Z�̌W��
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
	cout << "====== �p���v������ ======" << endl;
	cout << "Yaw : " << yaw_deg << "[deg]" << endl;
	cout << "Roll : " << roll_deg << "[deg]" << endl;
	cout << "Pitch : " << pitch_deg << "[deg]" << endl;
	cout << "==========================" << "\n" << endl;
}