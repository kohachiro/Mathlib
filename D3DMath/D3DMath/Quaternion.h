#ifndef _QUATERNION_H
#define _QUATERNION_H

class Quaternion
{
public:
	__m128 qua;
	Quaternion() {}
	Quaternion(__m128 q) { qua=q; }
	Quaternion(const Quaternion& q){ qua=q.qua; }
	Quaternion(float x, float y, float z,float w=1){qua=_mm_set_ps( w, z , y , x );}
	//Quaternion(const Vector &v,float w=1)
	//{
	//	qua = _mm_shuffle_ps( v.vec , v.vec , _MM_SHUFFLE( 0 , 2 , 1 , 3 ));
	//	qua = _mm_move_ss( m3 , _mm_set_ss( w ));
	//	qua = _mm_shuffle_ps( m3 , m3 , _MM_SHUFFLE( 0 , 2 , 1 , 3 ));
	//}
	inline void identity(){	qua = _mm_set_ps( 1 , 0 , 0 , 0 ); }
	inline void setToRotateAboutX(float theta)
	{

	}
}

#endif