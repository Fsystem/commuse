#ifndef __CommMath_H
#define __CommMath_H

/* 常量定义 */
extern const double INF ;
extern const double EP ;
extern const int MAXV ;
extern const double PI ;
/* 基本几何结构 */
struct POINT_
{
	double x;
	double y;
	POINT_(double a=0, double b=0) { x=a; y=b;}
};
struct LINESEG
{
	POINT_ s;
	POINT_ e;
	LINESEG(POINT_ a, POINT_ b) { s=a; e=b;}
	LINESEG() { }
};
// 直线的解析方程 a*x+b*y+c=0 为统一表示,约定 a>= 0
struct LINE
{
	double a;
	double b;
	double c;
	LINE(double d1=1, double d2=-1, double d3=0) {a=d1; b=d2; c=d3;}
};
//线段树
struct LINETREE
{
};
//浮点误差的处理
int dblcmp(double d);
//////////////////////////////////////////////////////////////////////////
//<一>点的基本运算
// 返回两点之间欧氏距离
double dist(POINT_ p1,POINT_ p2);
// 判断两个点是否重合
bool equal_point(POINT_ p1,POINT_ p2);
/*(sp-op)*(ep-op)的叉积
r=multiply(sp,ep,op),得到(sp-op)*(ep-op)的叉积
r>0:sp在矢量op ep的顺时针方向;
r=0：op sp ep三点共线;
r<0: sp在矢量op ep的逆时针方向 */
double multiply(POINT_ sp,POINT_ ep,POINT_ op);
double amultiply(POINT_ sp,POINT_ ep,POINT_ op);
/*矢量(p1-op)和(p2-op)的点积
r=dotmultiply(p1,p2,op),得到矢量(p1-op)和(p2-op)的点积如果两个矢量都非零矢量
r < 0: 两矢量夹角为锐角;
r = 0：两矢量夹角为直角;
r > 0: 两矢量夹角为钝角 */
double dotmultiply(POINT_ p1,POINT_ p2,POINT_ p0);
/* 判断点p是否在线段l上
条件：(p在线段l所在的直线上)&& (点p在以线段l为对角线的矩形内) */
bool online(LINESEG l,POINT_ p);
// 返回点p以点o为圆心逆时针旋转alpha(单位：弧度)后所在的位置
POINT_ rotate(POINT_ o,double alpha,POINT_ p);
/* 返回顶角在o点,起始边为os,终止边为oe的夹角(单位：弧度)
角度小于pi,返回正值
角度大于pi,返回负值
可以用于求线段之间的夹角 */
double angle(POINT_ o,POINT_ s,POINT_ e);
//////////////////////////////////////////////////////////////////////////
//<二>线段及直线的基本运算
/* 判断点C在线段AB所在的直线l上垂足P的与线段AB的关系
本函数是根据下面的公式写的,P是点C到线段AB所在直线的垂足
AC dot AB
r = ----------------------
||AB||^2
(Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
= ----------------------------------------------------
L^2
r has the following meaning:
r=0 P = A
r=1 P = B
r<0 P is on the backward extension of AB
r>1 P is on the forward extension of AB
0<r<1 P is interior to AB
*/
double relation(POINT_ c,LINESEG l);
// 求点C到线段AB所在直线的垂足 P
POINT_ perpendicular(POINT_ p,LINESEG l);
/* 求点p到线段l的最短距离
返回线段上距该点最近的点np 注意：np是线段l上到点p最近的点,不一定是垂足 */
double ptolinesegdist(POINT_ p,LINESEG l,POINT_ &np);
// 求点p到线段l所在直线的距离
//请注意本函数与上个函数的区别
double ptoldist(POINT_ p,LINESEG l);
/* 计算点到折线集的最近距离,并返回最近点.
注意：调用的是ptolineseg()函数 */
double ptopointset(int vcount, POINT_ pointset[], POINT_ p, POINT_ &q);
/* 判断圆是否在多边形内*/
bool CircleInsidePolygon(int vcount,POINT_ center,double radius,POINT_ polygon[]);
/* 返回两个矢量l1和l2的夹角的余弦 (-1 ~ 1)
注意：如果想从余弦求夹角的话,注意反余弦函数的值域是从 0到pi */
double cosine(LINESEG l1,LINESEG l2);
// 返回线段l1与l2之间的夹角
//单位：弧度 范围(-pi,pi)
double lsangle(LINESEG l1,LINESEG l2);
//判断线段u和v相交(包括相交在端点处)
bool intersect(LINESEG u,LINESEG v);
// 判断线段u和v相交（不包括双方的端点）
bool intersect_A(LINESEG u,LINESEG v);
// 判断线段v所在直线与线段u相交

//////////////////////////////////////////////////////////////////////////
//方法：判断线段u是否跨立线段v
bool intersect_l(LINESEG u,LINESEG v);
// 根据已知两点坐标,求过这两点的直线解析方程： a*x+b*y+c = 0 (a >= 0)
LINE makeline(POINT_ p1,POINT_ p2);
// 根据直线解析方程返回直线的斜率k,水平线返回 0,竖直线返回 1e200
double slope(LINE l);
// 返回直线的倾斜角alpha ( 0 - pi)
// 注意：atan()返回的是 -PI/2 ~ PI/2
double alpha(LINE l);
// 求点p关于直线l的对称点
POINT_ symmetry(LINE l,POINT_ p);
// 如果两条直线 l1(a1*x+b1*y+c1 = 0), l2(a2*x+b2*y+c2 = 0)相交,返回true,且返回交点p
bool lineintersect(LINE l1,LINE l2,POINT_ &p) ;// 是 L1,L2
// 如果线段l1和l2相交,返回true且交点由(inter)返回,否则返回false
bool intersection(LINESEG l1,LINESEG l2,POINT_ &inter);

//////////////////////////////////////////////////////////////////////////
//<三> 多边形常用算法模块
//如果无特别说明,输入多边形顶点要求按逆时针排列
// 返回多边形面积(signed);
// 输入顶点按逆时针排列时,返回正值;否则返回负值
double area_of_polygon(int vcount,POINT_ polygon[]);
// 判断顶点是否按逆时针排列
// 如果输入顶点按逆时针排列,返回true
bool isconterclock(int vcount,POINT_ polygon[]);
/*射线法判断点q与多边形polygon的位置关系
要求polygon为简单多边形,顶点时针排列
如果点在多边形内： 返回0
如果点在多边形边上：返回1
如果点在多边形外： 返回2 */
int insidepolygon(POINT_ q,int vcount,POINT_ Polygon[]);
//判断点q在凸多边形polygon内
// 点q是凸多边形polygon内[包括边上]时,返回true
// 注意：多边形polygon一定要是凸多边形
bool InsideConvexPolygon(int vcount,POINT_ polygon[],POINT_ q);
/*寻找凸包的graham 扫描法
PointSet为输入的点集;
ch为输出的凸包上的点集,按照逆时针方向排列;
n为PointSet中的点的数目
len为输出的凸包上的点的个数 */
void Graham_scan(POINT_ PointSet[],POINT_ ch[],int n,int &len);
// 卷包裹法求点集凸壳,参数说明同graham算法
void ConvexClosure(POINT_ PointSet[],POINT_ ch[],int n,int &len);
// 求凸多边形的重心,要求输入多边形按逆时针排序
POINT_ gravitycenter(int vcount,POINT_ polygon[]);
/*所谓凸多边形的直径,即凸多边形任两个顶点的最大距离。下面的算法
仅耗时O(n),是一个优秀的算法。 输入必须是一个凸多边形,且顶点
必须按顺序（顺时针、逆时针均可）依次输入。若输入不是凸多边形
而是一般点集,则要先求其凸包。 就是先求出所有跖对,然后求出每
个跖对的距离,取最大者。点数要多于5个*/
void Diameter(POINT_ ch[],int n,double &dia);


#endif //__CommMath_H