#ifndef __CommMath_H
#define __CommMath_H

/* �������� */
extern const double INF ;
extern const double EP ;
extern const int MAXV ;
extern const double PI ;
/* �������νṹ */
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
// ֱ�ߵĽ������� a*x+b*y+c=0 Ϊͳһ��ʾ,Լ�� a>= 0
struct LINE
{
	double a;
	double b;
	double c;
	LINE(double d1=1, double d2=-1, double d3=0) {a=d1; b=d2; c=d3;}
};
//�߶���
struct LINETREE
{
};
//�������Ĵ���
int dblcmp(double d);
//////////////////////////////////////////////////////////////////////////
//<һ>��Ļ�������
// ��������֮��ŷ�Ͼ���
double dist(POINT_ p1,POINT_ p2);
// �ж��������Ƿ��غ�
bool equal_point(POINT_ p1,POINT_ p2);
/*(sp-op)*(ep-op)�Ĳ��
r=multiply(sp,ep,op),�õ�(sp-op)*(ep-op)�Ĳ��
r>0:sp��ʸ��op ep��˳ʱ�뷽��;
r=0��op sp ep���㹲��;
r<0: sp��ʸ��op ep����ʱ�뷽�� */
double multiply(POINT_ sp,POINT_ ep,POINT_ op);
double amultiply(POINT_ sp,POINT_ ep,POINT_ op);
/*ʸ��(p1-op)��(p2-op)�ĵ��
r=dotmultiply(p1,p2,op),�õ�ʸ��(p1-op)��(p2-op)�ĵ���������ʸ��������ʸ��
r < 0: ��ʸ���н�Ϊ���;
r = 0����ʸ���н�Ϊֱ��;
r > 0: ��ʸ���н�Ϊ�۽� */
double dotmultiply(POINT_ p1,POINT_ p2,POINT_ p0);
/* �жϵ�p�Ƿ����߶�l��
������(p���߶�l���ڵ�ֱ����)&& (��p�����߶�lΪ�Խ��ߵľ�����) */
bool online(LINESEG l,POINT_ p);
// ���ص�p�Ե�oΪԲ����ʱ����תalpha(��λ������)�����ڵ�λ��
POINT_ rotate(POINT_ o,double alpha,POINT_ p);
/* ���ض�����o��,��ʼ��Ϊos,��ֹ��Ϊoe�ļн�(��λ������)
�Ƕ�С��pi,������ֵ
�Ƕȴ���pi,���ظ�ֵ
�����������߶�֮��ļн� */
double angle(POINT_ o,POINT_ s,POINT_ e);
//////////////////////////////////////////////////////////////////////////
//<��>�߶μ�ֱ�ߵĻ�������
/* �жϵ�C���߶�AB���ڵ�ֱ��l�ϴ���P�����߶�AB�Ĺ�ϵ
�������Ǹ�������Ĺ�ʽд��,P�ǵ�C���߶�AB����ֱ�ߵĴ���
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
// ���C���߶�AB����ֱ�ߵĴ��� P
POINT_ perpendicular(POINT_ p,LINESEG l);
/* ���p���߶�l����̾���
�����߶��Ͼ�õ�����ĵ�np ע�⣺np���߶�l�ϵ���p����ĵ�,��һ���Ǵ��� */
double ptolinesegdist(POINT_ p,LINESEG l,POINT_ &np);
// ���p���߶�l����ֱ�ߵľ���
//��ע�Ȿ�������ϸ�����������
double ptoldist(POINT_ p,LINESEG l);
/* ����㵽���߼����������,�����������.
ע�⣺���õ���ptolineseg()���� */
double ptopointset(int vcount, POINT_ pointset[], POINT_ p, POINT_ &q);
/* �ж�Բ�Ƿ��ڶ������*/
bool CircleInsidePolygon(int vcount,POINT_ center,double radius,POINT_ polygon[]);
/* ��������ʸ��l1��l2�ļнǵ����� (-1 ~ 1)
ע�⣺������������нǵĻ�,ע�ⷴ���Һ�����ֵ���Ǵ� 0��pi */
double cosine(LINESEG l1,LINESEG l2);
// �����߶�l1��l2֮��ļн�
//��λ������ ��Χ(-pi,pi)
double lsangle(LINESEG l1,LINESEG l2);
//�ж��߶�u��v�ཻ(�����ཻ�ڶ˵㴦)
bool intersect(LINESEG u,LINESEG v);
// �ж��߶�u��v�ཻ��������˫���Ķ˵㣩
bool intersect_A(LINESEG u,LINESEG v);
// �ж��߶�v����ֱ�����߶�u�ཻ

//////////////////////////////////////////////////////////////////////////
//�������ж��߶�u�Ƿ�����߶�v
bool intersect_l(LINESEG u,LINESEG v);
// ������֪��������,����������ֱ�߽������̣� a*x+b*y+c = 0 (a >= 0)
LINE makeline(POINT_ p1,POINT_ p2);
// ����ֱ�߽������̷���ֱ�ߵ�б��k,ˮƽ�߷��� 0,��ֱ�߷��� 1e200
double slope(LINE l);
// ����ֱ�ߵ���б��alpha ( 0 - pi)
// ע�⣺atan()���ص��� -PI/2 ~ PI/2
double alpha(LINE l);
// ���p����ֱ��l�ĶԳƵ�
POINT_ symmetry(LINE l,POINT_ p);
// �������ֱ�� l1(a1*x+b1*y+c1 = 0), l2(a2*x+b2*y+c2 = 0)�ཻ,����true,�ҷ��ؽ���p
bool lineintersect(LINE l1,LINE l2,POINT_ &p) ;// �� L1,L2
// ����߶�l1��l2�ཻ,����true�ҽ�����(inter)����,���򷵻�false
bool intersection(LINESEG l1,LINESEG l2,POINT_ &inter);

//////////////////////////////////////////////////////////////////////////
//<��> ����γ����㷨ģ��
//������ر�˵��,�������ζ���Ҫ����ʱ������
// ���ض�������(signed);
// ���붥�㰴��ʱ������ʱ,������ֵ;���򷵻ظ�ֵ
double area_of_polygon(int vcount,POINT_ polygon[]);
// �ж϶����Ƿ���ʱ������
// ������붥�㰴��ʱ������,����true
bool isconterclock(int vcount,POINT_ polygon[]);
/*���߷��жϵ�q������polygon��λ�ù�ϵ
Ҫ��polygonΪ�򵥶����,����ʱ������
������ڶ�����ڣ� ����0
������ڶ���α��ϣ�����1
������ڶ�����⣺ ����2 */
int insidepolygon(POINT_ q,int vcount,POINT_ Polygon[]);
//�жϵ�q��͹�����polygon��
// ��q��͹�����polygon��[��������]ʱ,����true
// ע�⣺�����polygonһ��Ҫ��͹�����
bool InsideConvexPolygon(int vcount,POINT_ polygon[],POINT_ q);
/*Ѱ��͹����graham ɨ�跨
PointSetΪ����ĵ㼯;
chΪ�����͹���ϵĵ㼯,������ʱ�뷽������;
nΪPointSet�еĵ����Ŀ
lenΪ�����͹���ϵĵ�ĸ��� */
void Graham_scan(POINT_ PointSet[],POINT_ ch[],int n,int &len);
// ���������㼯͹��,����˵��ͬgraham�㷨
void ConvexClosure(POINT_ PointSet[],POINT_ ch[],int n,int &len);
// ��͹����ε�����,Ҫ���������ΰ���ʱ������
POINT_ gravitycenter(int vcount,POINT_ polygon[]);
/*��ν͹����ε�ֱ��,��͹���������������������롣������㷨
����ʱO(n),��һ��������㷨�� ���������һ��͹�����,�Ҷ���
���밴˳��˳ʱ�롢��ʱ����ɣ��������롣�����벻��͹�����
����һ��㼯,��Ҫ������͹���� ��������������Ŷ�,Ȼ�����ÿ
���ŶԵľ���,ȡ����ߡ�����Ҫ����5��*/
void Diameter(POINT_ ch[],int n,double &dia);


#endif //__CommMath_H