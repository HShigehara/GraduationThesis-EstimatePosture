/*!
* @file cMouseFunc.cpp
* @brief cMouse�N���X�̊֐��̎����t�@�C��
* @date 2014/03/26
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
* @brief cMouse::Mouse()�֐�.�R���X�g���N�^
* @param
*/
void Mouse(int event, int x, int y, int flags, void *param)
{
	CoordinateTransform ct; //CoordinateTransform�N���X��S�C���X�^���X�̐���

	int check = 0; //�����f�[�^��0�łȂ����`�F�b�N����t���O.check == 0�Ȃ狗���f�[�^��0�ȊO��\��.

	if (event == CV_EVENT_LBUTTONDOWN){
		if (points < MAX_POINTS){
			pushpt[points] = cv::Point(x, y);
			check = ct.ct2Dto3D(pushpt[points], points);
			if (check == 0){
				points++;
			}
		}
	}
}