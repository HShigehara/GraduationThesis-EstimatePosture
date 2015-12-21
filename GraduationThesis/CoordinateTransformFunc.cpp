/*!
* @file CoordinateTransformFunc.cpp
* @brief CoordinateTransform�N���X�̎����t�@�C��
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
* @brief int CoordinateTransform::ct2dto3d()�֐�.2D�̍��W�f�[�^�������f�[�^�ƍ��킹��3D�f�[�^�ɕϊ�����
* @param pushpt Mouse()�֐��ŃN���b�N�����Î~�摜�̍��W(x,y)
* @return 0 or -1.0�Ȃ�擾�����X�̋����f�[�^��0�ȊO�ł������Ƃ�������.-1�Ȃ�擾�����_�̋����f�[�^��0�ł������Ƃ�������.-1�Ȃ�_���Ď擾����
*/

int CoordinateTransform::ct2Dto3D(cv::Point pushpt, int points)
{
	EstimatePosture ep; //�C���X�^���X����

	ofstream ofs("data/RealPoints.dat"); //3�������W�ɕϊ���������(x,y,z)

	ofstream ofs_ave("data/ave.csv");
	double sum_x = 0;
	double sum_y = 0;
	double sum_z = 0;
	double ave_x = 0;
	double ave_y = 0;
	double ave_z = 0;

	pt[cnt_getcoordinate] = pushpt; //�N���b�N���Ɏ擾�����_�̍��W���R�s�[����

	onedim[cnt_getcoordinate] = pt[cnt_getcoordinate].x + 640 * pt[cnt_getcoordinate].y - 640; //2�����ŕ\����Ă���_��1�����ɕϊ�����
	cout << "pt[" << cnt_getcoordinate << "] => " << pt[cnt_getcoordinate] << " , onedim => " << onedim[cnt_getcoordinate] << " , dist[" << onedim[cnt_getcoordinate] - 1 << "] => " << dist[onedim[cnt_getcoordinate]] << endl;

	//�����f�[�^��0�������ꍇ���W�擾����蒼��
	if (dist[onedim[cnt_getcoordinate]] == 0){
		cout << "�����f�[�^��0�������̂ł�����x���ӂ��N���b�N���Ă�������" << endl;
		return -1;
	}

	WorldCoordinate = NuiTransformDepthImageToSkeleton((long)pt[cnt_getcoordinate].x, (long)pt[cnt_getcoordinate].y, dist[onedim[cnt_getcoordinate]] << 3, NUI_IMAGE_RESOLUTION_640x480);

	real_x[cnt_getcoordinate] = WorldCoordinate.x*1000.0;
	real_y[cnt_getcoordinate] = WorldCoordinate.y*1000.0;
	real_z[cnt_getcoordinate] = WorldCoordinate.z*1000.0;

	cout << "real_x[" << cnt_getcoordinate << "] => " << real_x[cnt_getcoordinate] << "[mm] real_y[" << cnt_getcoordinate << "] => " << real_y[cnt_getcoordinate] << "[mm] real_z[" << cnt_getcoordinate << "] => " << real_z[cnt_getcoordinate] << "[mm]" << endl;
	cout << "==========================================================================" << endl;

	//0~8�܂ł̓_���������炻�ꂼ���3�������W���t�@�C���֏o�͂��ŏ����@���s��
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

		//�������Ԃ��v������
		f2 = 1000.0 / cv::getTickFrequency();
		time2 = cv::getTickCount();
		time2th = 0;

		ep.LeastSqureMethod(real_x, real_y, real_z); //�ŏ����@�̗v�f�v�Z

		time2th = (cv::getTickCount() - time2)*f2;

		cout << MAX_POINTS << "�_�ڂ��N���b�N���Ă����]�p�x�����߂�܂ł̎���" << time2th << "[ms]" << endl;

		sumtime = time1th + time2th - time3th;

		cout << "�����ɂ�����������(gnuplot�̏������Ԃ�����)��" << sumtime << "[ms] �ł���" << "\n" << endl;

		cout << "============== ���j���[ ==============" << endl;
		cout << "������x�v�����܂���??" << endl;
		cout << "�摜��ŃL�[����͂��Ă�������" << endl;
		cout << "1 : Yes" << endl;
		cout << "2 : No" << endl;
		cout << "======================================" << "\n" << endl;
	}

	cnt_getcoordinate++; //�擾�����_�̐���9�ɂȂ�܂ō��W���擾��������

	return 0;
}