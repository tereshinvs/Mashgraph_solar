#include <cmath>
#include <vector>
#include <tuple>
#include <map>

#include "figures.h"

#include "maths.h"

std::tuple<unsigned int, VertexDataDouble *, unsigned int, unsigned int *> get_tetrahedron(const double rad) {
	unsigned int dataCount = 4;
	VertexDataDouble *pData = new VertexDataDouble[dataCount];
	pData[0].pos = glm::dvec3(1, 1, 1);
	pData[1].pos = glm::dvec3(-1, -1, 1);
	pData[2].pos = glm::dvec3(-1, 1, -1);
	pData[3].pos = glm::dvec3(1, -1, -1);
	double coef = rad / std::sqrt(3.0);
	for (unsigned i = 0; i < dataCount; i++) {
		pData[i].pos = glm::dvec3(pData[i].pos[0] * coef, pData[i].pos[1] * coef, pData[i].pos[2] * coef);
		pData[i].tex = glm::dvec2(i % 2, i > 1);
	}
	unsigned int indexesCount = 4 * 3;
	unsigned int *pIndexes = new unsigned int[indexesCount];
	pIndexes[0] = 0; pIndexes[1] = 2; pIndexes[2] = 1;
	pIndexes[3] = 0; pIndexes[4] = 1; pIndexes[5] = 3;
	pIndexes[6] = 2; pIndexes[7] = 3; pIndexes[8] = 1;
	pIndexes[9] = 0; pIndexes[10] = 3; pIndexes[11] = 2;
	return std::make_tuple(dataCount, pData, indexesCount, pIndexes);
}

std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_sphere_by_extension(
	const glm::dvec3 &cent, const double radius) {

	typedef bool (*compare_t)(const glm::dvec3 &, const glm::dvec3 &);
	compare_t cmp = compare_vec3;

	unsigned int dataCount, indexesCount, *pIndexes;
	VertexDataDouble *pData;
	std::tie(dataCount, pData, indexesCount, pIndexes) = get_tetrahedron(1.0);

	auto get_middle_point = [](const glm::dvec3 &p1, const glm::dvec3 &p2) {
		glm::vec3 mid((p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0, (p1[2] + p2[2]) / 2.0);
		double rad = std::sqrt(mid[0] * mid[0] + mid[1] * mid[1] + mid[2] * mid[2]);
		return glm::dvec3(mid[0] / rad, mid[1] / rad, mid[2] / rad);
	};
	auto get_middle_tex = [](const glm::dvec2 &t1, const glm::dvec2 &t2) {
		return glm::dvec2((t1[0] + t2[0]) / 2.0, (t1[1] + t2[1]) / 2.0);
	};
	auto put_into_map = [](const glm::dvec3 &pos, const glm::dvec2 &tex,
		std::vector<VertexDataDouble> &points_vec, std::map<glm::dvec3, unsigned, compare_t> &points_map) {
		unsigned ind;
		if (points_map.count(pos) == 0) {
			points_vec.push_back({ pos, glm::dvec3(0, 0, 0), tex });
			ind = points_map[pos] = points_vec.size() - 1;
		} else
			ind = points_map[pos];
		return ind;
	};

	for (unsigned step = 0; step < 5; step++) {
		std::map<glm::dvec3, unsigned, compare_t> points_map(cmp);
		std::vector<VertexDataDouble> points_vec;
		std::vector<unsigned> triangle_vec;
		for (unsigned i = 0; i < indexesCount; i += 3) {
			glm::dvec3 &pos1 = pData[pIndexes[i]].pos, &pos2 = pData[pIndexes[i + 1]].pos, &pos3 = pData[pIndexes[i + 2]].pos;
			glm::dvec2 &tex1 = pData[pIndexes[i]].tex, &tex2 = pData[pIndexes[i + 1]].tex, &tex3 = pData[pIndexes[i + 2]].tex;
			unsigned ind1 = put_into_map(pos1, tex1, points_vec, points_map);
			unsigned ind2 = put_into_map(pos2, tex2, points_vec, points_map);
			unsigned ind3 = put_into_map(pos3, tex3, points_vec, points_map);
			unsigned ind4 = put_into_map(get_middle_point(pos1, pos2), get_middle_tex(tex1, tex2), points_vec, points_map);
			unsigned ind5 = put_into_map(get_middle_point(pos1, pos3), get_middle_tex(tex1, tex3), points_vec, points_map);
			unsigned ind6 = put_into_map(get_middle_point(pos2, pos3), get_middle_tex(tex2, tex3), points_vec, points_map);
			triangle_vec.push_back(ind1); triangle_vec.push_back(ind5); triangle_vec.push_back(ind4);
			triangle_vec.push_back(ind5); triangle_vec.push_back(ind6); triangle_vec.push_back(ind4);
			triangle_vec.push_back(ind5); triangle_vec.push_back(ind3); triangle_vec.push_back(ind6);
			triangle_vec.push_back(ind4); triangle_vec.push_back(ind6); triangle_vec.push_back(ind2);
		}
		if (pData) {
			delete[] pData;
			delete[] pIndexes;
		}
		dataCount = points_vec.size();
		indexesCount = triangle_vec.size();
		pData = new VertexDataDouble[dataCount];
		pIndexes = new unsigned int[indexesCount];
		for (unsigned i = 0; i < dataCount; i++)
			pData[i] = points_vec[i];
		for (unsigned i = 0; i < indexesCount; i++)
			pIndexes[i] = triangle_vec[i];
	}

	for (unsigned i = 0; i < dataCount; i++) {
		double _rad, theta, phi;
		std::tie(_rad, theta, phi) = get_sphere_coord(pData[i].pos);
		pData[i].tex = glm::dvec2(0.5 + phi / (2.0 * PI), 1.0 - theta / PI);
		pData[i].pos = glm::dvec3(
			cent[0] + pData[i].pos[0] * radius,
			cent[1] + pData[i].pos[1] * radius,
			cent[2] + pData[i].pos[2] * radius);
		pData[i].nor = pData[i].pos;
	}

	VertexData *_pData = new VertexData[dataCount];
	for (unsigned i = 0; i < dataCount; i++) {
		_pData[i].pos = glm::vec3(float(pData[i].pos[0]), float(pData[i].pos[1]), float(pData[i].pos[2]));
		_pData[i].nor = glm::vec3(float(pData[i].nor[0]), float(pData[i].nor[1]), float(pData[i].nor[2]));
		_pData[i].tex = glm::vec2(float(pData[i].tex[0]), float(pData[i].tex[1]));
	}
	return std::make_tuple(dataCount, _pData, indexesCount, pIndexes);
}

std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_sphere_by_coords(
	const glm::dvec3 &cent, const double radius) {

	double log_step = 1e-1, lat_step = 1e-1;
	unsigned n = unsigned(std::floor(2.0 * PI / log_step) + 1), m = unsigned(std::floor(PI / lat_step) + 1);
	unsigned int dataCount = (n + 1) * (m + 1), indexesCount = n * m * 2 * 3;
	unsigned int *pIndexes = new unsigned int[indexesCount];
	VertexData *pData = new VertexData[dataCount];

	unsigned k = 0;
	for (unsigned i = 0; i <= n; i++)
		for (unsigned j = 0; j <= m; j++) {
			double phi = log_step * i;
			double theta = lat_step * j;
			pData[i * (m + 1) + j].pos = glm::vec3(
				float(radius * std::sin(theta) * std::cos(phi)),
				float(radius * std::sin(theta) * std::sin(phi)),
				float(radius * std::cos(theta)));
			pData[i * (m + 1) + j].tex = glm::vec2(phi / (2.0 * PI), 1.0 - theta / PI);
			pData[i * (m + 1) + j].nor = pData[i * (m + 1) + j].pos;

			if (i > 0 && j > 0) {
				pIndexes[k++] = i * (m + 1) + j;
				pIndexes[k++] = (i - 1) * (m + 1) + (j - 1);
				pIndexes[k++] = i * (m + 1) + (j - 1);

				pIndexes[k++] = i * (m + 1) + j;
				pIndexes[k++] = (i - 1) * (m + 1) + j;
				pIndexes[k++] = (i - 1) * (m + 1) + (j - 1);
			}
		}
	return std::make_tuple(dataCount, pData, indexesCount, pIndexes);
}

std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_sphere(const glm::dvec3 &cent, const double radius) {
	return get_sphere_by_coords(cent, radius);
}

std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_orbit(const PlanetOrbitParams &orbit) {
	unsigned dataCount;
	VertexData *pData;
	unsigned indexesCount;
	unsigned *pIndexes;

	double phi = 1e-3;
	dataCount = unsigned(std::floor(2.0 * PI / phi));
	pData = new VertexData[dataCount];
	indexesCount = dataCount * 2;
	pIndexes = new unsigned[indexesCount];

	unsigned k = 0;
	for (double i = 0.0; i < 2.0 * PI; i += phi) {
		if (k == dataCount)
			break;
		double po = orbit.P / (1.0 - orbit.e * std::cos(i));
		glm::dvec3 p = orbit.c_mat_inv * glm::dvec3(po * std::cos(i), po * std::sin(i), 0.0);

		pData[k].pos = glm::vec3(float(p[0]), float(p[1]), float(p[2]));
		pData[k].nor = glm::vec3(0.0, 0.0, 0.0);
		pData[k].tex = glm::vec2(0.0, 0.0);
		k++;
	}

	for (unsigned i = 0; i < dataCount; i++) {
		pIndexes[i * 2] = i;
		pIndexes[i * 2 + 1] = i + 1;
	}
	pIndexes[indexesCount - 1] = 0;
	return std::make_tuple(dataCount, pData, indexesCount, pIndexes);
}

std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_rings(
	const glm::dvec3 &cent, const double radius1, const double radius2) {
	
	double phi_step = 1.0e-1;
	double rad_step = 1.0e4;

	unsigned n = unsigned(std::floor((radius2 - radius1) / rad_step) + 1), m = unsigned(std::floor(2.0 * PI / phi_step) + 1);
	unsigned dataCount = (n + 1) * (m + 1);
	VertexData *pData = new VertexData[dataCount];
	unsigned indexesCount = n * m * 2 * 3;
	unsigned *pIndexes = new unsigned[indexesCount];

	unsigned k = 0;
	for (unsigned i = 0; i <= n; i++)
		for (unsigned j = 0; j <= m; j++) {
			double rad = radius1 + rad_step * i;
			double phi = phi_step * j;
			pData[i * (m + 1) + j].pos = glm::vec3(float(rad * std::cos(phi)), float(rad * std::sin(phi)), 0.0f);
			pData[i * (m + 1) + j].tex = glm::vec2(float((rad - radius1) / (radius2 - radius1)), float(phi / (2.0 * PI)));
			pData[i * (m + 1) + j].nor = glm::vec3(0.0, 0.0, 1.0);

			if (i > 0 && j > 0) {
				pIndexes[k++] = i * (m + 1) + j;
				pIndexes[k++] = (i - 1) * (m + 1) + (j - 1);
				pIndexes[k++] = i * (m + 1) + (j - 1);

				pIndexes[k++] = i * (m + 1) + j;
				pIndexes[k++] = (i - 1) * (m + 1) + j;
				pIndexes[k++] = (i - 1) * (m + 1) + (j - 1);
			}
		}
	return std::make_tuple(dataCount, pData, indexesCount, pIndexes);
}
