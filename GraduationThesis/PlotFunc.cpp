/*!
* @file PlotFunc.cpp
* @brief PlotFuncクラスの実装ファイル
* @date 2014/04/01
* @author H.Shigehara
*/

#include"EstimatePosture.h"

/*!
 * @brief void Plot::PlotData(double a,double b,double c)関数.取得したデータをプロットする
 * @param a 最小二乗法によって求めた平面の式aX+bY+c=Zの係数
 * @param b 最小二乗法によって求めた平面の式aX+bY+c=Zの係数
 * @param c 最小二乗法によって求めた平面の式aX+bY+c=Zの係数
 * @return
 */
void Plot::PlotData(double a, double b, double c)
{
	gnuplot = _popen("gnuplot", "w");
	if (gnuplot == NULL){
		cout << "gnuplotが開けません" << endl;
	}

	//a,b,cの値を送信
	fprintf_s(gnuplot, "a = %f\n", a);
	fprintf_s(gnuplot, "b = %f\n", b);
	fprintf_s(gnuplot, "c = %f\n", c);

	//それぞれの軸ラベルを付ける
	fputs("set xlabel \"X-axis\"\n", gnuplot);
	fputs("set ylabel \"Y-axis\"\n", gnuplot);
	fputs("set zlabel \"Z-axis\"\n", gnuplot);

	//クリックした3D座標と最小二乗法によって求めた平面をプロットする
	fputs("splot \"data/RealPoints.dat\",a*x+b*y+c\n", gnuplot);

	//デフォルトで表示された画像を保存する
	fputs("set title \"Default\"\n", gnuplot);
	fputs("set term jpeg\n", gnuplot);
	fputs("set output \"data/default.jpg\"\n", gnuplot);
	fputs("rep\n", gnuplot);

	//X-Y平面でプロット.Z軸とZ軸のラベルを消す
	fputs("unset zlabel\n", gnuplot);
	fputs("unset ztics\n", gnuplot);
	fputs("rep\n", gnuplot);

	fputs("set view 0,0,1,1\n", gnuplot);
	fputs("set title \"X-Y Plane\"\n", gnuplot);
	fputs("set term jpeg\n", gnuplot);
	fputs("set output \"data/X-Y_Plane.jpg\"\n", gnuplot);
	fputs("rep\n", gnuplot);

	//X-Z平面でプロット.Z軸とZ軸のラベル表示し、Y軸とY軸のラベルを消す
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

	//X-Y平面でプロット.Y軸とY軸のラベルを表示し、X軸とX軸のラベルを消す
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

	//ここまでの処理をgnuplotへ流し閉じる
	fflush(gnuplot);
	_pclose(gnuplot);
}