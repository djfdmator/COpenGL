#include "pch.h"
#include <stack>
#include "MathUtils.h"

using namespace std;

Mat44 curViewportMatrix;
Mat44 curModelViewMatrix;
Mat44 curProjectionMatrix;

stack<Mat44> ModelViewStack;
stack<Mat44> ProjectionStack;

Mat44* currentMatrix = &curModelViewMatrix;
stack<Mat44>* currentMatrixStack = &ModelViewStack;
GLenum matrixMode = GL_MODELVIEW;

struct VertexArray
{
	GLint size;
	GLenum type;
	GLsizei stride;
	const void *pointer;
};
bool isOnVertexArray = false, isOnNormalArray = false, isOnColorArray = false, isOnTextureCoord = false;
VertexArray vertex, normal, color, texture_coord;

Mat display = Mat(720, 1280, CV_8UC3, Scalar(0, 0, 0));

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	curViewportMatrix.col1.Set(width / 2, 0, 0, 0);
	curViewportMatrix.col2.Set(0, -height / 2, 0, 0);
	curViewportMatrix.col3.Set(0, 0, 0, 0);
	curViewportMatrix.col4.Set(x + (width / 2), y + (height / 2), 1, 1);
}

void glMatrixMode(GLenum mode)
{
	matrixMode = mode;
	if (mode == GL_MODELVIEW)
	{
		currentMatrix = &curModelViewMatrix;
		currentMatrixStack = &ModelViewStack;
	}
	else if (mode == GL_PROJECTION)
	{
		currentMatrix = &curProjectionMatrix;
		currentMatrixStack = &ProjectionStack;
	}
}

void glPushMatrix(void)
{
	//if (matrixMode == GL_MODELVIEW)
	//{
	//	ModelViewStack.push(curModelViewMatrix);
	//}
	//else if (matrixMode == GL_PROJECTION)
	//{
	//	ProjectionStack.push(curProjectionMatrix);
	//}
	currentMatrixStack->push(*currentMatrix);
}

void glPopMatrix(void)
{
	//if (matrixMode == GL_MODELVIEW)
	//{
	//	//memcpy(currentMatrix, ModelViewStack.pop());
	//}
	//else if (matrixMode == GL_PROJECTION)
	//{
	//	//memcpy(currentMatrix, ProjectionStack.pop());
	//}

	if (currentMatrixStack->empty())
	{
		memcpy(currentMatrix, &identityMatrix, sizeof(struct Mat44));
		return;
	}

	memcpy(currentMatrix, &(currentMatrixStack->top()), sizeof(struct Mat44));
	currentMatrixStack->pop();
}

void glOrthof(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{
	GLfloat x = -(r + l) / (r - l);
	GLfloat y = -(t + b) / (t - b);
	GLfloat z = -(f + n) / (f - n);
	Mat44 Prj(Vec4(2.0 / (r - l), 0, 0, 0),
		Vec4(0, 2.0 / (t - b), 0, 0),
		Vec4(0, 0, -2.0 / (f - n), 0),
		Vec4(x, y, z, 1));

	Mat44 temp = (*currentMatrix * Prj);
	currentMatrix->Set(temp.col1, temp.col2, temp.col3, temp.col4);

	//memcpy(&currentMatrix, &(*currentMatrix * Prj), sizeof(struct Mat44));
}

void glFrustumf(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{
	GLfloat A = (r + l) / (r - l);
	GLfloat B = (t + b) / (t - b);
	GLfloat C = -(f + n) / (f - n);
	GLfloat D = -(2 * f * n) / (f - n);
	Mat44 Prj(Vec4(2 * n / (r - l), 0, 0, 0),
		Vec4(0, 2 * n / (t - b), 0, 0),
		Vec4(A, B, C, -1),
		Vec4(0, 0, D, 0));
	Mat44 temp = (*currentMatrix * Prj);
	currentMatrix->Set(temp.col1, temp.col2, temp.col3, temp.col4);
	//currentMatrix = &(*currentMatrix * Prj);
	//memcpy(&currentMatrix, &(*currentMatrix * Prj), sizeof(struct Mat44));
}

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat _near, GLfloat _far)
{
	GLfloat h = 2 * _near * tanf(DEGREES_TO_RADIANS(fovy / 2));
	GLfloat top = h / 2;
	GLfloat bottom = -h / 2;
	GLfloat left = aspect * bottom;
	GLfloat right = aspect * top;
	glFrustumf(left, right, bottom, top, _near, _far);
}

void glLoadIdentity(void)
{
	memcpy(currentMatrix, &identityMatrix, sizeof(struct Mat44));
	//if (matrixMode == GL_MODELVIEW)
	//{
	//	memcpy(&curModelViewMatrix, &identityMatrix, sizeof(struct Mat44));
	//}
	//else if (matrixMode == GL_PROJECTION)
	//{
	//	memcpy(&curProjectionMatrix, &identityMatrix, sizeof(struct Mat44));
	//}
}

void glLoadMatrixf(const GLfloat *m)
{
	//if (matrixMode == GL_MODELVIEW)
	//{
	//	curModelViewMatrix.col1.Set(m[0], m[1], m[2], m[3]);
	//	curModelViewMatrix.col2.Set(m[4], m[5], m[6], m[7]);
	//	curModelViewMatrix.col3.Set(m[8], m[9], m[10], m[11]);
	//	curModelViewMatrix.col4.Set(m[12], m[13], m[14], m[15]);
	//}
	//else if (matrixMode == GL_PROJECTION)
	//{
	//	curProjectionMatrix.col1.Set(m[0], m[1], m[2], m[3]);
	//	curProjectionMatrix.col2.Set(m[4], m[5], m[6], m[7]);
	//	curProjectionMatrix.col3.Set(m[8], m[9], m[10], m[11]);
	//	curProjectionMatrix.col4.Set(m[12], m[13], m[14], m[15]);
	//}
	currentMatrix->col1.Set(m[0], m[1], m[2], m[3]);
	currentMatrix->col2.Set(m[4], m[5], m[6], m[7]);
	currentMatrix->col3.Set(m[8], m[9], m[10], m[11]);
	currentMatrix->col4.Set(m[12], m[13], m[14], m[15]);
}

void glMultMatrixf(const GLfloat *m)
{
	Mat44 matrix(Vec4(m[0], m[1], m[2], m[3]),
		Vec4(m[4], m[5], m[6], m[7]),
		Vec4(m[8], m[9], m[10], m[11]),
		Vec4(m[12], m[13], m[14], m[15]));

	Mat44 temp = (*currentMatrix * matrix);
	currentMatrix->Set(temp.col1, temp.col2, temp.col3, temp.col4);

	//memcpy(&currentMatrix, &(*currentMatrix * matrix), sizeof(struct Mat44));
	//if (matrixMode == GL_MODELVIEW)
	//{
	//	memcpy(&curModelViewMatrix, &(curModelViewMatrix * matrix), sizeof(struct Mat44));
	//}
	//else if (matrixMode == GL_PROJECTION)
	//{
	//	memcpy(&curProjectionMatrix, &(curProjectionMatrix * matrix), sizeof(struct Mat44));
	//}
}

void glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	Mat44 translate;
	translate.col4.Set(x, y, z, 1);

	Mat44 temp = (*currentMatrix * translate);
	currentMatrix->Set(temp.col1, temp.col2, temp.col3, temp.col4);

	//memcpy(&currentMatrix, &(*currentMatrix * translate), sizeof(struct Mat44));
	//curModelViewMatrix = curModelViewMatrix * translate;
}

void glScalef(GLfloat x, GLfloat y, GLfloat z)
{
	Mat44 scale;
	scale.col1.Set(x, 0, 0, 0);
	scale.col2.Set(0, y, 0, 0);
	scale.col3.Set(0, 0, z, 0);
	scale.col4.Set(0, 0, 0, 1);

	Mat44 temp = (*currentMatrix * scale);
	currentMatrix->Set(temp.col1, temp.col2, temp.col3, temp.col4);

	//memcpy(&currentMatrix, &(*currentMatrix * scale), sizeof(struct Mat44));
	//curModelViewMatrix = curModelViewMatrix * scale;
}

void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	const GLfloat PI = 3.1415926f;

	GLfloat normalValue = sqrtf(x * x + y * y + z * z);
	x /= normalValue;
	y /= normalValue;
	z /= normalValue;

	angle *= PI / 180;
	GLfloat c = cosf(angle), s = sinf(angle);
	Mat44 rotate(Vec4(x * x * (1 - c) + c, x * y * (1 - c) - z * s, x * z * (1 - c) + y * s, 0.0f),
		Vec4(y * x * (1 - c) + z * s, y * y * (1 - c) + c, y * z * (1 - c) - x * s, 0.0f),
		Vec4(x * z * (1 - c) - y * s, y * z * (1 - c) + x * s, z * z * (1 - c) + c, 0.0f),
		Vec4(0.0f, 0.0f, 0.0f, 1.0f));

	Mat44 temp = (*currentMatrix * rotate.Transpose());
	currentMatrix->Set(temp.col1, temp.col2, temp.col3, temp.col4);

	//memcpy(&currentMatrix, &(*currentMatrix * rotate.Transpose()), sizeof(struct Mat44));
	//curModelViewMatrix = curModelViewMatrix * rotate.Transpose();
}

void glGetFloatv(GLenum pname, GLfloat* params)
{
	Mat44 m;
	switch (pname)
	{
	case GL_MODELVIEW_MATRIX:
		m = curModelViewMatrix;
		break;
	case GL_PROJECTION_MATRIX:
		m = curProjectionMatrix;
		break;
	}
	m.Print();
	GLfloat p[16] = { m.col1.x, m.col1.y, m.col1.z, m.col1.w,
					  m.col2.x, m.col2.y, m.col2.z, m.col2.w,
					  m.col3.x, m.col3.y, m.col3.z, m.col3.w,
					  m.col4.x, m.col4.y, m.col4.z, m.col4.w };

	for (int i = 0; i < 16; i++)
	{
		params[i] = p[i];
	}
}

void glEnableClientState(GLenum array)
{
	switch (array)
	{
	case GL_VERTEX_ARRAY:
		isOnVertexArray = true;
		break;
	case GL_NORMAL_ARRAY:
		isOnNormalArray = true;
		break;
	case GL_COLOR_ARRAY:
		isOnColorArray = true;
		break;
	case GL_TEXTURE_COORD_ARRAY:
		isOnTextureCoord = true;
		break;
	default:
		break;
	}
}

void glDisableClientState(GLenum array)
{
	switch (array)
	{
	case GL_VERTEX_ARRAY:
		isOnVertexArray = false;
		break;
	case GL_NORMAL_ARRAY:
		isOnNormalArray = false;
		break;
	case GL_COLOR_ARRAY:
		isOnColorArray = false;
		break;
	case GL_TEXTURE_COORD_ARRAY:
		isOnTextureCoord = false;
		break;
	default:
		break;
	}
}

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	if (isOnVertexArray)
	{
		if (size <= 0) size = 3;
		vertex.size = size;
		vertex.type = type;
		if (stride <= 0) stride = size * sizeof(type);
		vertex.stride = stride;
		vertex.pointer = pointer;
	}
}

void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	if (isOnTextureCoord)
	{
		if (size <= 0) size = 2;
		texture_coord.size = size;
		texture_coord.type = type;
		if (stride <= 0) stride = size * sizeof(type);
		texture_coord.stride = stride;
		texture_coord.pointer = pointer;
	}
}

void glColorPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	if (isOnColorArray)
	{
		if (size <= 0) size = 3;
		color.size = size;
		color.type = type;
		if (stride <= 0) stride = size * sizeof(type);
		color.stride = stride;
		color.pointer = pointer;
	}
}

void glNormalPointer(GLenum type, GLsizei stride, const void *pointer)
{
	if (isOnNormalArray)
	{
		normal.size = 3;
		normal.type = type;
		if (stride <= 0) stride = 3 * sizeof(type);
		normal.stride = stride;
		normal.pointer = pointer;
	}
}

struct TexBuffer {
	GLuint texture = 0;
	GLubyte* data;
	GLsizei width;
	GLsizei height;
	GLenum glTextureMinFilter = NULL;
	GLenum glTextureMagFilter = NULL;
	GLenum glTextureWrapS = NULL;
	GLenum glTextureWrapT = NULL;
};

TexBuffer texBuffer[1];
GLuint texCount = 0;

GLuint bindedTex = 0;

void glGenTexture(GLsizei n, GLuint *texture)
{
	for (int i = 0; i < n; i++)
	{
		GLuint temp = texCount++;
		*texture = temp;
		//texBuffer = (TexBuffer*)malloc(sizeof(TexBuffer) * texCount);
		texBuffer[texCount - 1].texture = *texture;
	}
}

void glBindTexture(GLenum target, GLuint texture)
{
	bindedTex = texture;
}

void glDeleteTextures(GLsizei n, const GLuint *textures) {}

void glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	if (bindedTex == 0) return;
	TexBuffer* texture = texBuffer + bindedTex;
	switch (pname)
	{
	case GL_TEXTURE_MAG_FILTER:
		texture->glTextureMagFilter = param;
		break;
	case GL_TEXTURE_MIN_FILTER:
		texture->glTextureMinFilter = param;
		break;
	case GL_TEXTURE_WRAP_S:
		texture->glTextureWrapS = param;
		break;
	case GL_TEXTURE_WRAP_T:
		texture->glTextureWrapT = param;
		break;
	default:
		break;
	}
}
void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
{
	//if (bindedTex == 0) return;
	//TexBuffer* texture = texBuffer + bindedTex;
	texBuffer[0].width = width;
	texBuffer[0].height = height;
	texBuffer[0].data = (GLubyte*)pixels;
	//memcpy(texBuffer[0].data, pixels, width * height * 3);
	//texture->data = (GLubyte*)pixels;
}

void Line2(Mat& img, const Point& start, const Point& end, const Scalar& c1, const Scalar& c2)
{
	LineIterator iter(display, start, end, LINE_8);

	double alpha = 0;
	for (int i = 0; i < iter.count; i++, iter++)
	{
		alpha = double(i) / iter.count;
		Scalar a(c1 * (1.0 - alpha) + c2 * alpha);
		display(Rect(iter.pos(), Size(1, 1))) = a * 255;
	}
}

void CPoint(const Point& point, const Scalar& color)
{
	line(display, point, point, color, 1, LINE_8);
}

void DrawPoint(Point* points, GLsizei count)
{
	if (isOnColorArray)
	{
		Scalar c1, c2;
		for (int i = 0; i < count; i++)
		{
			c1 = Scalar(((float*)color.pointer)[i], ((float*)color.pointer)[i + 1], ((float*)color.pointer)[i + 2]);
			CPoint(points[i], c1);
		}
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			CPoint(points[i], Scalar(255, 255, 255));
			//line(display, points[i], points[i], Scalar(255, 255, 255), 1, LINE_8);
		}
	}
}

void DrawLine(Point* points, GLsizei count)
{
	if (isOnColorArray)
	{
		Scalar c1, c2;
		for (int i = 0; i < count; i += 2)
		{
			c1 = Scalar(((float*)color.pointer)[i], ((float*)color.pointer)[i + 1], ((float*)color.pointer)[i + 2]);
			c2 = Scalar(((float*)color.pointer)[i + 3], ((float*)color.pointer)[i + 4], ((float*)color.pointer)[i + 5]);
			if (i + 1 >= count) break;
			Line2(display, points[i], points[i + 1], c1, c2);
		}
	}
	else
	{
		for (int i = 0; i < count; i += 2)
		{
			if (i + 1 >= count) break;
			line(display, points[i], points[i + 1], Scalar(255, 255, 255), 1, LINE_8);
		}
	}
}

void DrawTriangle(Point* points, GLsizei count)
{
	if (isOnColorArray)
	{
		Scalar c1, c2, c3;
		int j = 0;
		for (int i = 0; i < count; i += 3, j++)
		{
			c1 = Scalar(((float*)color.pointer)[j], ((float*)color.pointer)[j + 1], ((float*)color.pointer)[j + 2]);
			c2 = Scalar(((float*)color.pointer)[j + 3], ((float*)color.pointer)[j + 4], ((float*)color.pointer)[j + 5]);
			c3 = Scalar(((float*)color.pointer)[j + 6], ((float*)color.pointer)[j + 7], ((float*)color.pointer)[j + 8]);

			Line2(display, points[i], points[i + 1], c1, c2);
			Line2(display, points[i + 1], points[i + 2], c2, c3);
			Line2(display, points[i + 2], points[i], c3, c1);

			//line(display, points[i], points[i + 1], Scalar(255, 255, 255), 1, LINE_8);
			//line(display, points[i + 1], points[i + 2], Scalar(255, 255, 255), 1, LINE_8);
			//line(display, points[i + 2], points[i], Scalar(255, 255, 255), 1, LINE_8);
		}
	}
	else
	{
		for (int i = 0; i < count; i += 3)
		{
			line(display, points[i], points[i + 1], Scalar(255, 255, 255), 1, LINE_8);
			line(display, points[i + 1], points[i + 2], Scalar(255, 255, 255), 1, LINE_8);
			line(display, points[i + 2], points[i], Scalar(255, 255, 255), 1, LINE_8);
		}
	}
}

void DrawPolygon(const Point* points, const Scalar* colors, const Point2f* texCoords,GLsizei count)
{
	//Scalar c1, c2, c3;
	//int j = 0;
	//for (int i = 0; i < count; i += 3, j++)
	//{
	//	c1 = Scalar(((float*)color.pointer)[j], ((float*)color.pointer)[j + 1], ((float*)color.pointer)[j + 2]);
	//	c2 = Scalar(((float*)color.pointer)[j + 3], ((float*)color.pointer)[j + 4], ((float*)color.pointer)[j + 5]);
	//	c3 = Scalar(((float*)color.pointer)[j + 6], ((float*)color.pointer)[j + 7], ((float*)color.pointer)[j + 8]);
	//	c1 = (c1 + c2 + c2) * 255 / 3;
	//	//c1 *= 255; n 

	//	Point pt[1][3];
	//	pt[0][0] = Point(points[i]);
	//	pt[0][1] = Point(points[i + 1]);
	//	pt[0][2] = Point(points[i + 2]);
	//	printf("[%d, %d, %d, %d, %d, %d]\n", pt[0][0].x, pt[0][0].y, pt[0][1].x, pt[0][1].y, pt[0][2].x, pt[0][2].y);
	//	const Point* ppt[1] = { pt[0] };
	//	int npt[] = { 3 };
	//	fillPoly(display, ppt, npt, 1, c1, LINE_8);
	//}
	TexBuffer texbuf = texBuffer[0];
	GLubyte* texturePtr = texbuf.data;
	GLsizei width = texbuf.width;
	GLsizei height = texbuf.height;

	for (int i = 0; i < count; i += 3)
	{
		Point pt[3];
		pt[0] = Point(points[i]);
		pt[1] = Point(points[i + 1]);
		pt[2] = Point(points[i + 2]);

		Point2f tex[3];
		tex[0] = Point2f(texCoords[i]);
		tex[1] = Point2f(texCoords[i + 1]);
		tex[2] = Point2f(texCoords[i + 2]);

		// 폴리곤 y 높이에 따라 분할
		for (int j = 0; j < 3 - 1; j++)
		{
			for (int k = 0; k < 3 - j - 1; k++)
			{
				if (pt[k].y < pt[k + 1].y)
				{
					Point temp = pt[k];
					pt[k] = pt[k + 1];
					pt[k + 1] = temp;

					Point2f tempf = tex[k];
					tex[k] = tex[k + 1];
					tex[k + 1] = tempf;
				}
			}
		}

		int AC_Height = pt[0].y - pt[1].y;
		int CB_Height = pt[1].y - pt[2].y;
		int AB_Height = pt[0].y - pt[2].y;

		Point vecAC = pt[0] - pt[1];
		Point vecCB = pt[1] - pt[2];
		Point vecAB = pt[0] - pt[2];

		Point2f uvVecAC = tex[0] - tex[1];
		Point2f uvVecCB = tex[1] - tex[2];
		Point2f uvVecAB = tex[0] - tex[2];

		//높이에 따른 선의 점을 구한 후 같은 높이의 점끼리 선으로 연결한다.
		Point point1, point2, p;
		Point2f uv1, uv2, uv;
		int factor;
		for (int j = 0; j <= AB_Height; j++)
		{
			if (j > CB_Height)
			{
				if (AC_Height != 0)
				{
					factor = (j - CB_Height) / AC_Height;
					point1 = pt[1] + vecAC * factor;
					uv1 = tex[1] + uvVecAC * factor;
				}
				else
				{
					factor = j - CB_Height;
					point1 = pt[1] + vecAC * factor;
					uv1 = tex[1] + uvVecAC * factor;
				}
				//point2 = pt[2] + (vecAC * j / (AB_Height + 1));
				//uv1 = tex[1] + uvVecAC * j / (AB_Height + 1);
			}
			else
			{
				if (CB_Height != 0)
				{
					factor = j / CB_Height;
					point1 = pt[2] + vecCB * factor;
					uv1 = tex[2] + uvVecCB * factor;
				}
				else
				{
					point1 = pt[2] + vecCB * j;
					uv1 = tex[2] + uvVecCB * j;
				}
				//point1 = pt[2] + (vecCB * j / (CB_Height + 1));
				//uv1 = tex[2] + uvVecCB * j / (CB_Height + 1);
			}

			if (AB_Height != 0)
			{
				factor = j / AB_Height;
				point2 = pt[2] + vecAB * factor;
				uv2 = tex[2] + uvVecAB * factor;
			}
			else
			{
				point2 = pt[2] + vecAB;
				uv2 = tex[2] + uvVecAB * j;
			}
			Point2f uvline = uv1 - uv2;

			Point line = point1 - point2;
			bool isMinus = false;
			int absx = line.x;
			if (line.x < 0)
			{
				isMinus = true;
				absx = -absx;
			}
			
			for (int k = 0; k <= absx; k++)
			{
				p = point2;
				if (isMinus) p.x -= k;
				else p.x += k;

				if(absx !=0 ) uv = uv2 + uvline * k / absx;
				else uv = uv2 + uvline;
				int img_u = width * uv.x;
				int img_v = height * uv.y;
				int uvindex = (img_v * width + img_u) * 3;
				//printf("%d %d %d", texBuffer[0].data[uvindex], texBuffer[0].data[uvindex + 1], texBuffer[0].data[uvindex + 2]);
				CPoint(p, Scalar(texbuf.data[uvindex], texbuf.data[uvindex + 1], texbuf.data[uvindex + 2]));
				//CPoint(p, Scalar(255, 255, 255));
			}
		}
	}

}

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLuint* indices)
{
	display = Mat(720, 1280, CV_8UC3, Scalar(0, 0, 0));

	Mat44 transMat44 = (curViewportMatrix * (curProjectionMatrix * curModelViewMatrix));

	if (!isOnVertexArray) return;

	Point* points = new Point[count];

	float* vertexPtr = (float*)vertex.pointer;
	Vec4 point;
	int j = 0;
	for (int i = 0; i < count; i++)
	{
		j = indices[i] * 3;
		point.Set(vertexPtr[j], vertexPtr[j + 1], vertexPtr[j + 2], 1.0f);
		point = (curProjectionMatrix * curModelViewMatrix) * point;
		point /= point.w;
		point = curViewportMatrix * point;
		points[i].x = point.x;
		points[i].y = point.y; 
	}

	Scalar* colors = new Scalar[count];
	if (isOnColorArray)
	{
		float* colorPtr = (float*)color.pointer;
		for (int i = 0; i < count; i++)
		{
			j = indices[i];
			colors[i] = Scalar(colorPtr[j], colorPtr[j + 1], colorPtr[j + 2]);
		}
	}

	Point2f* texCoord = new Point2f[count];
	if (isOnTextureCoord)
	{
		float* texCoordPtr = (float*)texture_coord.pointer;
		for (int i = 0; i < count; i++)
		{
			j = indices[i];
			//printf("%f %f\n", texCoordPtr[j], texCoordPtr[j + 1]);
			texCoord[i] = Point2f(texCoordPtr[j], texCoordPtr[j + 1]);
		}
	}

	if (isOnNormalArray)
	{

	}

	switch (mode)
	{
	case GL_POINTS:
		DrawPoint(points, count);
		break;
	case GL_LINES:
		DrawLine(points, count);
		break;
	case GL_TRIANGLES:
		DrawTriangle(points, count);
		break;
	case GL_POLYGON:
		DrawPolygon(points, colors, texCoord, count);
		break;
	default:
		break;
	}

	imshow("Display", display);
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	display = Mat(720, 1280, CV_8UC3, Scalar(0, 0, 0));

	//Packed array
	printf("[curModelViewMatrix]\n");
	curModelViewMatrix.Print();
	printf("[curProjectionMatrix]\n");
	curProjectionMatrix.Print();
	printf("[curViewportMatrix]\n");
	curViewportMatrix.Print();
	Mat44 transMat44 = (curViewportMatrix * (curProjectionMatrix * curModelViewMatrix));
	printf("[transMat44]\n");
	transMat44.Print();
	Point* points = new Point[count];
	if (isOnVertexArray)
	{
		Vec4 point;
		int j = 0;
		for (int i = first; i < first + count; i++, j += vertex.size)
		{
			point.Set(((float*)vertex.pointer)[j], ((float*)vertex.pointer)[j + 1], ((float*)vertex.pointer)[j + 2], 1.0f);
			point = (curProjectionMatrix * curModelViewMatrix) * point;
			point /= point.w;
			point = curViewportMatrix * point;
			points[i].x = point.x;
			points[i].y = point.y;
			//printf("[%.2f, %.2f, %.2f, %.2f]\n", point.x, point.y, point.z, point.w);
		}
	}

	switch (mode)
	{
	case GL_POINTS:
		DrawPoint(points, count);
		break;
	case GL_LINES:
		DrawLine(points, count);
		break;
	case GL_TRIANGLES:
		DrawTriangle(points, count);
		break;
	case GL_POLYGON:
		//DrawPolygon(points, count);
		break;
	default:
		break;
	}

	imshow("Display", display);
}


