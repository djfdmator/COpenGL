#pragma once

#define DEGREES_TO_RADIANS(angle) ( angle * 3.141592 / 180.0 )   

struct Vec2
{
	Vec2() {}
	Vec2(GLfloat x, GLfloat y) : x(x), y(y) {}

	void operator -=(const Vec2& v)
	{
		x -= v.x; y -= v.y;
	}

	void operator *= (GLfloat a)
	{
		x *= a; y *= a;
	}

	void Print()
	{
		printf("(%f, %f)\n", x, y);
	}
	GLfloat x, y;
};

struct Vec4
{
	Vec4() {}
	Vec4(GLfloat x, GLfloat y, GLfloat z, GLfloat w) : x(x), y(y) ,z(z), w(w) {}

	void Set(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }

	Vec4 operator -() { return Vec4(-x, -y, -z, -w); }

	void operator += (const Vec4& v)
	{
		x += v.x; y += v.y; z += v.z; w += v.w;
	}

	void operator -= (const Vec4& v)
	{
		x -= v.x; y -= v.y; z -= v.z; w -= v.w;
	}

	void operator *= (GLfloat a)
	{
		x *= a; y *= a; z *= a; w *= a;
	}

	void operator /= (GLfloat a)
	{
		x /= a; y /= a; z /= a; w /= a;
	}

	void Print()
	{
		printf("(%f, %f, %f, %f)\n", x, y, z ,w);
	}

	GLfloat x, y, z, w;
};

struct Mat44
{
	Mat44() 
	{
		col1.Set(1, 0, 0, 0);
		col2.Set(0, 1, 0, 0);
		col3.Set(0, 0, 1, 0);
		col4.Set(0, 0, 0, 1);
	}

	Mat44(const Vec4& col1, const Vec4& col2, const Vec4& col3, const Vec4& col4) : col1(col1), col2(col2), col3(col3), col4(col4) {}

	void Set(const Vec4& col1, const Vec4& col2, const Vec4& col3, const Vec4& col4)
	{
		this->col1.Set(col1.x, col1.y, col1.z, col1.w);
		this->col2.Set(col2.x, col2.y, col2.z, col2.w);
		this->col3.Set(col3.x, col3.y, col3.z, col3.w);
		this->col4.Set(col4.x, col4.y, col4.z, col4.w);
	}

	Mat44 Transpose() const
	{
		return Mat44(Vec4(col1.x, col2.x, col3.x, col4.x), 
					 Vec4(col1.y, col2.y, col3.y, col4.y), 
					 Vec4(col1.z, col2.z, col3.z, col4.z),
				     Vec4(col1.w, col2.w, col3.w, col4.w));
	}

	void Print()
	{
		Mat44 temp = this->Transpose();
		temp.col1.Print();
		temp.col2.Print();
		temp.col3.Print();
		temp.col4.Print();
	}

	void operator *= (GLfloat a)
	{
		col1 *= a;
		col2 *= a;
		col3 *= a;
		col4 *= a;
	}

	Vec4 col1, col2, col3, col4;
};

Mat44 identityMatrix;

inline Vec2 operator - (const Vec2& v1, const Vec2& v2)
{
	return Vec2(v1.x - v2.x, v1.y - v2.y);
}

inline GLfloat operator * (const Vec4& v1, const Vec4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline Vec4 operator * (const Mat44& m, const Vec4& v)
{
	Mat44 m_trans = m.Transpose();
	Vec4 result(m_trans.col1 * v, m_trans.col2 * v, m_trans.col3 * v, m_trans.col4 * v);
	return result;
}

inline Vec4 operator * (const Vec4& v, const Mat44& m)
{
	return Vec4(v * m.col1, v * m.col2, v * m.col3, v * m.col4);
}

inline Mat44 operator * (const Mat44& m1, const Mat44& m2)
{
	Mat44 m_trans = m1.Transpose();
	Mat44 result(m_trans.col1 * m2, m_trans.col2 * m2, m_trans.col3 * m2, m_trans.col4 * m2);
	return result.Transpose();
}