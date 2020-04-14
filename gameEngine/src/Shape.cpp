
#include "Shape.h"
#include <iostream>
#include <cassert>

#include "GLSL.h"
#include "Program.h"

using namespace std;


// copy the data from the shape to this object
void Shape::createShape(tinyobj::shape_t & shape)
{
	posBuf = shape.mesh.positions;
	norBuf = shape.mesh.normals;
	texBuf = shape.mesh.texcoords;
	eleBuf = shape.mesh.indices;
}

void Shape::measure()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	minX = minY = minZ = std::numeric_limits<float>::infinity();
	maxX = maxY = maxZ = -std::numeric_limits<float>::infinity();

	//Go through all vertices to determine min and max of each dimension
	for (size_t v = 0; v < posBuf.size() / 3; v++)
	{
		if (posBuf[3*v+0] < minX) minX = posBuf[3 * v + 0];
		if (posBuf[3*v+0] > maxX) maxX = posBuf[3 * v + 0];

		if (posBuf[3*v+1] < minY) minY = posBuf[3 * v + 1];
		if (posBuf[3*v+1] > maxY) maxY = posBuf[3 * v + 1];

		if (posBuf[3*v+2] < minZ) minZ = posBuf[3 * v + 2];
		if (posBuf[3*v+2] > maxZ) maxZ = posBuf[3 * v + 2];
	}

	min.x = minX;
	min.y = minY;
	min.z = minZ;
	max.x = maxX;
	max.y = maxY;
	max.z = maxZ;
}

void Shape::init()
{
	// Initialize the vertex array object
	CHECKED_GL_CALL(glGenVertexArrays(1, &vaoID));
	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Send the position array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &posBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW));

	// Send the normal array to the GPU
	if (norBuf.empty())
	{
		norBufID = 0;
	}
	else
	{
		CHECKED_GL_CALL(glGenBuffers(1, &norBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW));
	}

	// Send the texture array to the GPU
	if (texBuf.empty())
	{
		texBufID = 0;
	}
	else
	{
		CHECKED_GL_CALL(glGenBuffers(1, &texBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW));
	}

	// Send the element array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &eleBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW));

	// Unbind the arrays
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));

	// Bind normal buffer
	h_nor = prog->getAttribute("vertNor");
	if (h_nor != -1 && norBufID != 0)
	{
		GLSL::enableVertexAttribArray(h_nor);
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
		CHECKED_GL_CALL(glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));
	}

	if (texBufID != 0)
	{
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");

		if (h_tex != -1 && texBufID != 0)
		{
			GLSL::enableVertexAttribArray(h_tex);
			CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
			CHECKED_GL_CALL(glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0));
		}
	}

	// Bind element buffer
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));

	// Draw
	CHECKED_GL_CALL(glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0));

	// Disable and unbind
	if (h_tex != -1)
	{
		GLSL::disableVertexAttribArray(h_tex);
	}
	if (h_nor != -1)
	{
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Shape::resize(glm::vec3 gTrans, float gScale) {
	for (size_t v = 0; v < posBuf.size() / 3; v++) {
		posBuf[3 * v + 0] = (posBuf[3 * v + 0] - gTrans.x) * gScale;
		posBuf[3 * v + 1] = (posBuf[3 * v + 1] - gTrans.y) * gScale;
		posBuf[3 * v + 2] = (posBuf[3 * v + 2] - gTrans.z) * gScale;
	}
}

void Shape::setCylinder() {
	//number of vertieces per layer;
	int numV = 30;
	float theta = 2 * 3.14159 / numV;
	int count = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < numV; j++) {
			posBuf.push_back(cos(theta*j));
			posBuf.push_back(sin(theta*j) - i);
			posBuf.push_back(sin(theta*j));

			texBuf.push_back(0.5 + 0.5*cos(theta*j));
			texBuf.push_back(0.5 + 0.5*(sin(theta*j) - i));
			texBuf.push_back(0.5 + 0.5*sin(theta*j));
		}
	}

	for (int i = 0; i < numV * 9; i++) {
		if ((i + 1) % numV == 0) {
			eleBuf.push_back(i);
			eleBuf.push_back(i + 1 - numV);
			eleBuf.push_back(i + 1);

			eleBuf.push_back(i);
			eleBuf.push_back(i + numV);
			eleBuf.push_back(i + 1);
		}
		else {
			eleBuf.push_back(i);
			eleBuf.push_back(i + 1);
			eleBuf.push_back(i + numV);

			eleBuf.push_back(i + 1);
			eleBuf.push_back(i + numV);
			eleBuf.push_back(i + 1 + numV);
		}

	}

	setNormals();

}

void Shape::setNormals() {
	float ax, ay, az, bx, by, bz, cx, cy, cz, nx, ny, nz;
	for (size_t v = 0; v < posBuf.size() / 3; v++) {
		norBuf.push_back(0);
		norBuf.push_back(0);
		norBuf.push_back(0);
	}
	for (size_t v = 0; v < eleBuf.size() / 3; v++) {
		ax = posBuf[3 * eleBuf[3 * v]];
		ay = posBuf[3 * eleBuf[3 * v] + 1];
		az = posBuf[3 * eleBuf[3 * v] + 2];

		bx = posBuf[3 * eleBuf[3 * v + 1]];
		by = posBuf[3 * eleBuf[3 * v + 1] + 1];
		bz = posBuf[3 * eleBuf[3 * v + 1] + 2];

		cx = posBuf[3 * eleBuf[3 * v + 2]];
		cy = posBuf[3 * eleBuf[3 * v + 2] + 1];
		cz = posBuf[3 * eleBuf[3 * v + 2] + 2];

		nx = (by - ay)*(cz - az) - (bz - az)*(cy - ay);
		ny = (bz - az)*(cx - ax) - (bx - ax)*(cz - az);
		nz = (bx - ax)*(cy - ay) - (by - ay)*(cx - ax);

		norBuf[3 * eleBuf[3 * v]] += nx;
		norBuf[3 * eleBuf[3 * v] + 1] += ny;
		norBuf[3 * eleBuf[3 * v] + 2] += nz;

		norBuf[3 * eleBuf[3 * v + 1]] += nx;
		norBuf[3 * eleBuf[3 * v + 1] + 1] += ny;
		norBuf[3 * eleBuf[3 * v + 1] + 2] += nz;

		norBuf[3 * eleBuf[3 * v + 2]] += nx;
		norBuf[3 * eleBuf[3 * v + 2] + 1] += ny;
		norBuf[3 * eleBuf[3 * v + 2] + 2] += nz;
	}
}