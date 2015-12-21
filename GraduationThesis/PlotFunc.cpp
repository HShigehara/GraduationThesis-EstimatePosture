/*!
* @file PlotFunc.cpp
* @brief PlotFunc�N���X�̎����t�@�C��
* @date 2014/04/01
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
 * @brief void Plot::PlotData(double a,double b,double c)�֐�.�擾�����f�[�^���v���b�g����
 * @param a �ŏ����@�ɂ���ċ��߂����ʂ̎�aX+bY+c=Z�̌W��
 * @param b �ŏ����@�ɂ���ċ��߂����ʂ̎�aX+bY+c=Z�̌W��
 * @param c �ŏ����@�ɂ���ċ��߂����ʂ̎�aX+bY+c=Z�̌W��
 * @return
 */
void Plot::PlotData(double a, double b, double c)
{
	gnuplot = _popen("gnuplot", "w");
	if (gnuplot == NULL){
		cout << "gnuplot���J���܂���" << endl;
	}

	//a,b,c�̒l�𑗐M
	fprintf_s(gnuplot, "a = %f\n", a);
	fprintf_s(gnuplot, "b = %f\n", b);
	fprintf_s(gnuplot, "c = %f\n", c);

	//���ꂼ��̎����x����t����
	fputs("set xlabel \"X-axis\"\n", gnuplot);
	fputs("set ylabel \"Y-axis\"\n", gnuplot);
	fputs("set zlabel \"Z-axis\"\n", gnuplot);

	//�N���b�N����3D���W�ƍŏ����@�ɂ���ċ��߂����ʂ��v���b�g����
	fputs("splot \"data/RealPoints.dat\",a*x+b*y+c\n", gnuplot);

	//�f�t�H���g�ŕ\�����ꂽ�摜��ۑ�����
	fputs("set title \"Default\"\n", gnuplot);
	fputs("set term jpeg\n", gnuplot);
	fputs("set output \"data/default.jpg\"\n", gnuplot);
	fputs("rep\n", gnuplot);

	//X-Y���ʂŃv���b�g.Z����Z���̃��x��������
	fputs("unset zlabel\n", gnuplot);
	fputs("unset ztics\n", gnuplot);
	fputs("rep\n", gnuplot);

	fputs("set view 0,0,1,1\n", gnuplot);
	fputs("set title \"X-Y Plane\"\n", gnuplot);
	fputs("set term jpeg\n", gnuplot);
	fputs("set output \"data/X-Y_Plane.jpg\"\n", gnuplot);
	fputs("rep\n", gnuplot);

	//X-Z���ʂŃv���b�g.Z����Z���̃��x���\�����AY����Y���̃��x��������
	fputs("set zlabel \"Z-axis\"\n", gnuplot);
	fputs("set ztics\n", gnuplot);
	fputs("unset ylabel\n", gnuplot);
	fputs("unset ytics\n", gnuplot);
	fputs("rep\n", gnuplot);

	fputs("set view 90,0,1,1\n", gnuplot);
	fputs("set title \"X-Z Plane\"\n", gnuplot);
	fputs("set term jpeg\n", gnuplot);
	fputs("set output \"data/X-Z_Plane.jpg\"\n", gnuplot);
	fputs("rep\n", gnuplot);

	//X-Y���ʂŃv���b�g.Y����Y���̃��x����\�����AX����X���̃��x��������
	fputs("set ylabel \"Y-axis\"\n", gnuplot);
	fputs("unset ytics\n", gnuplot);
	fputs("unset xlabel\n", gnuplot);
	fputs("unset xtics\n", gnuplot);
	fputs("rep\n", gnuplot);

	fputs("set view 0,0,1,1\n", gnuplot);
	fputs("set title \"Y-Z Plane\"\n", gnuplot);
	fputs("set term jpeg\n", gnuplot);
	fputs("set output \"data/Y-Z_Plane.jpg\"\n", gnuplot);
	fputs("rep\n", gnuplot);

	//�����܂ł̏�����gnuplot�֗�������
	fflush(gnuplot);
	_pclose(gnuplot);
}