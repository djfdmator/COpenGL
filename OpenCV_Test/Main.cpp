#include "pch.h"
#include <vector>

struct Planet {
	GLuint texture;
	GLfloat radius;
	GLfloat rotate = 0.0f;
	GLfloat* VertexArray;
	GLfloat* TextCoordArray;
	GLfloat* NormalArray;
	GLfloat* ColorArray;
	GLint elementCount;
	GLint stackCount;
	GLint sectorCount;
	vector<GLuint> indices;
	//GLint* indices;

	//vector<GLfloat> InterleavedArray;
	GLint indicesCount;

	Planet(GLfloat _radius, GLint _stackCount, GLint _sectorCount)
	{
		this->radius = _radius;
		elementCount = (_stackCount + 1) * (_sectorCount + 1);
		stackCount = _stackCount;
		sectorCount = _sectorCount;
		indicesCount = stackCount * sectorCount * 6;
		//indices = new GLint[indicesCount];
		VertexArray = new GLfloat[elementCount * 3];
		TextCoordArray = new GLfloat[elementCount * 2];
		NormalArray = new GLfloat[elementCount * 3];
		ColorArray = new GLfloat[elementCount * 3];
		CaculateSphere();
		BuiltWireFrameSphere();
	};

	void DrawWireframeSphere()
	{

	}

	void BuiltWireFrameSphere()
	{
		// indices
		//  k1--k1+1
		//  |  / |
		//  | /  |
		//  k2--k2+1
		//unsigned int k1, k2;
		//for (int i = 0; i < sectorCount; ++i)
		//{
		//	k1 = i * (stackCount + 1);
		//	k2 = k1 + stackCount + 1;

		//	for (int j = 0; j < stackCount; ++j, ++k1, ++k2)
		//	{
		//		if (i != 0)
		//		{
		//			addIndices(k1, k2, k1 + 1);
		//		}

		//		if (i != (stackCount - 1))
		//		{
		//			addIndices(k1 + 1, k2, k2 + 1);
		//		}
		//	}
		//}
	}

	void DrawSphere()
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, VertexArray);
		glTexCoordPointer(2, GL_FLOAT, 0, TextCoordArray);
		glNormalPointer(GL_FLOAT, 0, NormalArray);
		glColorPointer(3, GL_FLOAT, 0, ColorArray);

		//glDrawArrays(GL_POLYGON, 0, indicesCount);
		glDrawElements(GL_POLYGON, indices.size(), GL_UNSIGNED_BYTE, indices.data());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	void CaculateSphere()
	{
		const float PI = 3.1415926f;

		vector<GLuint>().swap(indices);

		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
		float s, t;                                     // vertex texCoord

		float sectorStep = 2 * PI / sectorCount;
		float stackStep = PI / stackCount;
		float sectorAngle, stackAngle;

		int temp_index = 0, temp_index_TCA = 0;
		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = radius * cosf(stackAngle);             // r * cos(u)
			z = radius * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
				VertexArray[temp_index] = x;
				VertexArray[temp_index + 1] = y;
				VertexArray[temp_index + 2] = z;

				// normalized vertex normal (nx, ny, nz)
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				ColorArray[temp_index] = nx;
				ColorArray[temp_index + 1] = ny;
				ColorArray[temp_index + 2] = nz;

				NormalArray[temp_index++] = nx;
				NormalArray[temp_index++] = ny;
				NormalArray[temp_index++] = nz;

				// vertex tex coord (s, t) range between [0, 1]
				s = (float)j / sectorCount;
				t = (float)i / stackCount;
				TextCoordArray[temp_index_TCA++] = s;
				TextCoordArray[temp_index_TCA++] = t;
			}
		}

		// indices
//  k1--k1+1
//  |  / |
//  | /  |
//  k2--k2+1
		unsigned int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);     // beginning of current stack
			k2 = k1 + sectorCount + 1;      // beginning of next stack

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding 1st and last stacks
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
					//InterleavedArray.push_back(VertexArray[3 * k1]);
					//InterleavedArray.push_back(VertexArray[3 * k1 + 1]);
					//InterleavedArray.push_back(VertexArray[3 * k1 + 2]);

					//InterleavedArray.push_back(VertexArray[3 * k2]);
					//InterleavedArray.push_back(VertexArray[3 * k2 + 1]);
					//InterleavedArray.push_back(VertexArray[3 * k2 + 2]);

					//InterleavedArray.push_back(VertexArray[3 * (k1 + 1)]);
					//InterleavedArray.push_back(VertexArray[3 * (k1 + 1) + 1]);
					//InterleavedArray.push_back(VertexArray[3 * (k1 + 1) + 2]);
				}

				if (i != (stackCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
					//InterleavedArray.push_back(VertexArray[3 * (k1 + 1)]);
					//InterleavedArray.push_back(VertexArray[3 * (k1 + 1) + 1]);
					//InterleavedArray.push_back(VertexArray[3 * (k1 + 1) + 2]);

					//InterleavedArray.push_back(VertexArray[3 * k2]);
					//InterleavedArray.push_back(VertexArray[3 * k2 + 1]);
					//InterleavedArray.push_back(VertexArray[3 * k2 + 2]);

					//InterleavedArray.push_back(VertexArray[3 * (k2 + 1)]);
					//InterleavedArray.push_back(VertexArray[3 * (k2 + 1) + 1]);
					//InterleavedArray.push_back(VertexArray[3 * (k2 + 1) + 2]);
				}
			}
		}


		//const float PI = 3.1415926f;

		//float x, y, z, xz;                              // vertex position
		//float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
		//float s, t;                                     // texCoord

		//float sectorStep = 2 * PI / sectorCount;
		//float stackStep = PI / stackCount;
		//float sectorAngle, stackAngle;

		//int temp_index = 0, temp_index_TCA = 0;
		//for (int i = 0; i <= sectorCount; ++i)
		//{
		//	//temp_index = i * (stackCount + 1);
		//	//temp_index_TCA = i * (stackCount + 1);

		//	sectorAngle = i * sectorStep;
		//	xz = radius * cosf(sectorAngle);             // r * cos(u)
		//	y = radius * sinf(sectorAngle);              // r * sin(u)

		//	for (int j = 0; j <= stackCount; ++j)
		//	{
		//		stackAngle = PI / 2 - j * stackStep;

		//		x = xz * cosf(stackAngle);             // r * cos(u) * cos(v)
		//		z = xz * sinf(stackAngle);             // r * cos(u) * sin(v)
		//		VertexArray[temp_index] = x;
		//		VertexArray[temp_index + 1] = y;
		//		VertexArray[temp_index + 2] = z;
		//		//printf("Vertex[%f, %f, %f]\n", x, y, z);

		//		nx = x * lengthInv;
		//		ny = y * lengthInv;
		//		nz = z * lengthInv;

		//		ColorArray[temp_index] = nx;
		//		ColorArray[temp_index + 1] = ny;
		//		ColorArray[temp_index + 2] = nz;

		//		NormalArray[temp_index++] = nx;
		//		NormalArray[temp_index++] = ny;
		//		NormalArray[temp_index++] = nz;

		//		s = (float)j / sectorCount;
		//		t = (float)i / stackCount;
		//		TextCoordArray[temp_index_TCA++] = s;
		//		TextCoordArray[temp_index_TCA++] = t;
		//	}
		//}


	}
};

GLuint LoadTexture(const char* filename)
{
	GLuint texture = 0;
	//int width, height;
	//GLubyte* data = NULL;
	//FILE* file = NULL;

	//fopen_s(&file, filename, "rb");

	//if (&file == NULL) return 0;

	//width = 2048;
	//height = 1024;

	//data = (GLubyte*)malloc(sizeof(GLubyte) * width * height * 3);

	//fread(data, width * height * 3, 1, file);
	//fclose(file);

	//unsigned char tmp;
	//for (int i = 0; i < width * height * 3; i += 3)
	//{
	//	tmp = data[i];
	//	data[i] = data[i + 2];
	//	data[i + 2] = tmp;
	//}
	unsigned char header[54]; // BMP 파일 헤더
	unsigned int dataPos;     // 데이터가 시작되는 위치
	unsigned int width, height;
	unsigned int imageSize;   // width * height * 3
	unsigned char * data;     // 데이터 첫 주소

	// Open file
	FILE * file;
	fopen_s(&file, filename, "rb");

	//파일의 형식이 맞지 않으면 일 안함
	if (!file) return 0;
	if (fread(header, 1, 54, file) != 54) return false;
	if (header[0] != 'B' || header[1] != 'M') return 0; //첫 문자 두개가 아스키 코드로 B,M

	// 바이트 배열에서 int 변수를 읽습니다. 
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// 누락 데이터 정리
	if (imageSize == 0)    imageSize = width * height * 3;
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];  // 버퍼 생성

	fread(data, 1, imageSize, file); // 파일에서 버퍼로 실제 데이터 넣기. 
	fclose(file); //이제 모두 메모리 안에 있으니까, 파일을 닫습니다.

		//bmp파일 BGR로 되어있습니다. RGB로 바꿉니다. 
	unsigned char tmp;
	for (int i = 0; i < imageSize; i += 3)
	{
		tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}

	glGenTexture(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

Planet Earth(1.0f, 36, 18);

int main()
{
	Earth.texture = LoadTexture("Earth.bmp");
	glViewport(0, 0, 1280, 720);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);

	int a = 90;
	while (true)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -3.0f);
		glRotatef(a, 1, 0, 0);
		//glScalef(0.005f, 0.005f, 0.005f);
		glBindTexture(GL_TEXTURE_2D, Earth.texture);
		Earth.DrawSphere();
		waitKey(0);
		a++;
	}

	return 0;
}