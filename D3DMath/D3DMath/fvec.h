#include <stdio.h>


class Vec
{
public:
	float x,y,z;
	Vec() {}
	Vec(float x1,float y1,float z1) {x=x1;y=y1;z=z1;}
	/* Assignment operations */
	friend float operator *(const Vec &a, const Vec &b) {	return a.x*b.x+a.y*b.y+a.z*b.z;	}
	float operator *=(const Vec &b) {	return x*b.x+y*b.y+z*b.z;	}
	float distance(const Vec & b)  //legnth form v1
	{
		float dx=x -b.x;
		float dy=y -b.y;
		float dz=z -b.z;
		return sqrt(dx*dx+dy*dy+dz*dz);
	}
	float sqrDistance(const Vec & b) 
	{
		float dx=x -b.x;
		float dy=y -b.y;
		float dz=z -b.z;
		return dx*dx+dy*dy+dz*dz;
	}
	inline void normalize()                   // normalize
	{
		float f = (float)sqrt(x*x + y*y + z*z);
		x/=f; y/=f; z/=f;
	}
	inline float length()
	{
		return sqrt(x*x + y*y + z*z);
	}
	inline float sqrLength()           // square length
	{
		return (x*x + y*y + z*z);
	}
	inline void  negate()                      // vector mult -1
	{
		x=-x;
		y=-y;
		z=-z;
	}
	inline Vec cross(const Vec &a,const Vec &b)//²æ³Ë
	{
		return Vec(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);
	}
	inline void  print()
	{
		printf("%.6f\t%.6f\t%.6f\n",x,y,z);
	}
	inline float angleWith( Vec &v )          // angle in rad
	{
		return (float)acos( ((*this) * v) / (this->length()*v.length()) );	
	}
};  // class

class Mat
{
public:
	float m11,m12,m13,m21,m22,m23,m31,m32,m33;
	Mat(){
		m11=0.0f;
		m12=0.1f;
		m13=0.2f;
		m21=1.0f;
		m22=1.1f;
		m23=1.2f;
		m31=2.0f;
		m32=2.1f;
		m33=2.2f;
	}
	void rota(const Vec &vc)
	{
		float cx,cy,cz,sx,sy,sz;
		cx=cosf(vc.x);
		cy=cosf(vc.y);
		cz=cosf(vc.z);
		sx=sinf(vc.x);
		sy=sinf(vc.y);
		sz=sinf(vc.z);
		m11=cy*cz;
		m12=cy*sz;
		m13=-sy;
		m21=sx*sy*cz-cx*sz;
		m22=sx*sy*sz+cx*cz;
		m23=sx*cy;
		m31=cx*sy *cz+sx*sz;
		m32=cx*sy *sz-sx*cz;
		m33=cx*cy;
	//cy*cz,cy*sz,-sy,0
	//sx*sy *cz-cx*sz,sx*sy *sz+cx*cz,sx*cy,0
	//cx*sy *cz+sx*sz,cx*sy *sz-sx*cz,cx*cy,0

	}
	void print()
	{
		printf("%.6f\t%.6f\t%.6f\n",m11,m12,m13);
		printf("%.6f\t%.6f\t%.6f\n",m21,m22,m23);
		printf("%.6f\t%.6f\t%.6f\n",m31,m32,m33);
	}
};
