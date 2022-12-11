#pragma once

#include <QVector3D>
#include <QVector>

class Cube
{
private:
	Cube() = default;
	Cube(const QVector3D &position, float size);

public:
	~Cube();

public:
	QVector3D position; // 位置
	float size{1.0f};	// 大小

public:
	/* 着色器路径 */
	static constexpr char VERT_PATH[] = ":/shader/cube.vert";
	static constexpr char FRAG_PATH[] = ":/shader/cube.frag";
	/* 顶点信息 */
	static void GetVertices(QVector<float> &cubeVertices);		// VBO
	static void GetIndices(QVector<unsigned int> &cubeIndices); // EBO
	/* 容器实例 */
	static Cube *container;
	static Cube *GetContainer();
};
