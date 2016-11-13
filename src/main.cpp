#include <iostream>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

//opengl mathematics
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/matrix_inverse.hpp>

#include "object.h"
#include "observer.h"
#include "shader.h"
#include "shaderprogram.h"
#include "figures.h"
#include "textures.h"
#include "planet_orbit_params.h"
#include "physics.h"
#include "material.h"
#include "maths.h"

#include <libconfig/libconfig.h++>

libconfig::Config config;
libconfig::Config data;

int window_width;
int window_height;

ShaderProgram shader_program;

std::vector<Object *> planets;
std::vector<PlanetOrbitParams> orbits;
std::vector<Object *> orbit_traces;
std::vector<Material> mats;
std::string texture_path;
Observer camera;

glm::dvec3 *roots;

glm::mat4x4 modelViewMatrix;
glm::mat4x4 projectionMatrix;
glm::mat4x4 modelViewProjectionMatrix;
glm::mat4x4 normalMatrix;

glm::vec4 pl_position;
glm::vec4 pl_ambient;
glm::vec4 pl_diffuse;
glm::vec4 pl_specular;
glm::vec3 pl_attenuation;

double DT;
double SIZE_SCALE_FACTOR;
double DIST_SCALE_FACTOR;
bool ENABLE_ANIMATION;
bool ENABLE_LOG = false;
bool ENABLE_ORBITS = true;
double ROTATE_ALPHA;
double ZOOM_SCALE;
unsigned TIMESTAMP;

void init_config(const char *config_path) {
	config.readFile(config_path);

	window_width = config.lookup("application.window.width");
	window_height = config.lookup("application.window.height");

	camera = Observer(
		glm::dvec3(
			config.lookup("application.camera.eye.x"),
			config.lookup("application.camera.eye.y"),
			config.lookup("application.camera.eye.z")),
		glm::dvec3(
			config.lookup("application.camera.cen.x"),
			config.lookup("application.camera.cen.y"),
			config.lookup("application.camera.cen.z")),
		glm::dvec3(
			config.lookup("application.camera.up.x"),
			config.lookup("application.camera.up.y"),
			config.lookup("application.camera.up.z")));

	ENABLE_ANIMATION = config.lookup("application.animation.enable");
	ENABLE_ORBITS = config.lookup("application.animation.enable_orbits");
	ENABLE_LOG = config.lookup("application.animation.enable_log");
	SIZE_SCALE_FACTOR = config.lookup("application.animation.scale_factor.size");
	DIST_SCALE_FACTOR = config.lookup("application.animation.scale_factor.dist");
	ROTATE_ALPHA = config.lookup("application.camera.rotate_alpha");
	ZOOM_SCALE = config.lookup("application.camera.zoom_scale");
	TIMESTAMP = config.lookup("application.animation.timestamp");
	DT = config.lookup("application.animation.dt");

	float x, y, z, w;
	x = config.lookup("application.light.position.x"); y = config.lookup("application.light.position.y");
	z = config.lookup("application.light.position.z"); w = config.lookup("application.light.position.w");
	pl_position = glm::vec4(x * DIST_SCALE_FACTOR, y * DIST_SCALE_FACTOR, z * DIST_SCALE_FACTOR, w);
	x = config.lookup("application.light.ambient.x"); y = config.lookup("application.light.ambient.y");
	z = config.lookup("application.light.ambient.z"); w = config.lookup("application.light.ambient.w");
	pl_ambient = glm::vec4(x, y, z, w);
	x = config.lookup("application.light.diffuse.x"); y = config.lookup("application.light.diffuse.y");
	z = config.lookup("application.light.diffuse.z"); w = config.lookup("application.light.diffuse.w");
	pl_diffuse = glm::vec4(x, y, z, w);
	x = config.lookup("application.light.specular.x"); y = config.lookup("application.light.specular.y");
	z = config.lookup("application.light.specular.z"); w = config.lookup("application.light.specular.w");
	pl_specular = glm::vec4(x, y, z, w);
	float a, b, c;
	a = config.lookup("application.light.attenuation.a"); b = config.lookup("application.light.attenuation.b");
	c = config.lookup("application.light.attenuation.c");
	pl_attenuation = glm::vec3(a, b, c);

	texture_path = (const char *)config.lookup("application.texture_path");
}

void initAllGLBuffers() {
	for (unsigned i = 0; i < planets.size(); i++)
		planets[i]->initGLBuffers(shader_program.programObject, "pos", "nor", "tex");
}

int find_object_by_name(const std::string &name, unsigned max_n) {
	for (unsigned i = 0; i < max_n; i++)
		if (planets[i]->get_name() == name)
			return i;
	return -1;
}

void init() {
	shader_program.init(config.lookup("application.shaders.vertex_shader"),
		config.lookup("application.shaders.fragment_shader"));
	
	data.readFile(config.lookup("application.data_file"));
	unsigned planets_n = data.lookup("data.planets_n");

	planets.resize(planets_n);
	roots = new glm::dvec3[planets_n];
	for (unsigned q = 0; q < planets_n; q++) {
		std::string curpath = std::string("data.planets.[") + std::to_string(q) + std::string("].");
		double cen_x = data.lookup(curpath + "cen.x");
		double cen_y = data.lookup(curpath + "cen.y");
		double cen_z = data.lookup(curpath + "cen.z");
		double vel_x = data.lookup(curpath + "vel.x");
		double vel_y = data.lookup(curpath + "vel.y");
		double vel_z = data.lookup(curpath + "vel.z");
		double rad = data.lookup(curpath + "radius");
		double log_rad = data.lookup(curpath + "log_rad");
		double perigee = data.lookup(curpath + "perigee");
		double omega = data.lookup(curpath + "omega");
		double a = data.lookup(curpath + "a");
		double e = data.lookup(curpath + "e");
		double i = data.lookup(curpath + "i");
		double _w = data.lookup(curpath + "w");
		double alpha = data.lookup(curpath + "alpha");
		double spin_time = data.lookup(curpath + "spin_time");
		int ot_r = data.lookup(curpath + "orbit_colour.r");
		int ot_g = data.lookup(curpath + "orbit_colour.g");
		int ot_b = data.lookup(curpath + "orbit_colour.b");
		GLuint orbit_texture = get_flat_texture(glm::ivec3(ot_r, ot_g, ot_b));
		std::string texname = data.lookup(curpath + "texture");
		std::string name = data.lookup(curpath + "name");
		std::string orb_cen = data.lookup(curpath + "orbit_cen");
		double log_orb_rad_scale = data.lookup(curpath + "log_orb_rad_scale");

		roots[q] = glm::dvec3(cen_x, cen_y, cen_z);
		if (name == "Saturn rings") {
			double rad_in = data.lookup(curpath + "radius_in");
			double rad_out = data.lookup(curpath + "radius_out");
			planets[q] =
				new Object(
					name,
					roots[q],
					rad, log_rad,
					get_rings(glm::dvec3(0.0, 0.0, 0.0), rad_in, rad_out),
					loadTextureFromFile(texture_path + "/" + texname), GL_TRIANGLES
				);
		} else
			planets[q] =
				new Object(
					name,
					roots[q],
					rad, log_rad,
					get_sphere(glm::dvec3(0.0, 0.0, 0.0), 1.0),
					loadTextureFromFile(texture_path + "/" + texname), GL_TRIANGLES
				);
		orbits.push_back(
			PlanetOrbitParams(
				orb_cen,
				planets[find_object_by_name(orb_cen, q + 1)]->get_cen(),
				roots[q],
				glm::dvec3(vel_x, vel_y, vel_z),
				perigee, a, e, omega, i, _w
			)
		);
		orbit_traces.push_back(
			new Object(
				name,
				planets[find_object_by_name(orb_cen, q + 1)]->get_cen(),
				1.0, log_orb_rad_scale,
				get_orbit(orbits[q]),
				orbit_texture, GL_LINES
			)
		);
		float x, y, z, w;
		x = data.lookup(curpath + "mat.ambient.x");
		y = data.lookup(curpath + "mat.ambient.y");
		z = data.lookup(curpath + "mat.ambient.z");
		w = data.lookup(curpath + "mat.ambient.w");
		glm::vec4 ambient = glm::vec4(x, y, z, w);
		x = data.lookup(curpath + "mat.diffuse.x");
		y = data.lookup(curpath + "mat.diffuse.y");
		z = data.lookup(curpath + "mat.diffuse.z");
		w = data.lookup(curpath + "mat.diffuse.w");
		glm::vec4 diffuse = glm::vec4(x, y, z, w);
		x = data.lookup(curpath + "mat.specular.x");
		y = data.lookup(curpath + "mat.specular.y");
		z = data.lookup(curpath + "mat.specular.z");
		w = data.lookup(curpath + "mat.specular.w");
		glm::vec4 specular = glm::vec4(x, y, z, w);
		x = data.lookup(curpath + "mat.emission.x");
		y = data.lookup(curpath + "mat.emission.y");
		z = data.lookup(curpath + "mat.emission.z");
		w = data.lookup(curpath + "mat.emission.w");
		glm::vec4 emission = glm::vec4(x, y, z, w);
		float shininess = data.lookup(curpath + "mat.shininess");
		mats.push_back(Material(ambient, diffuse, specular, emission, shininess));

		orbit_traces[q]->initGLBuffers(shader_program.programObject, "pos", "nor", "tex");
		planets[q]->set_spin_axe(orbits[q], alpha);
		planets[q]->set_spin_time(spin_time * 60.0 * 60.0 * 24.0);
	}
	initAllGLBuffers();
}

void display() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(shader_program.programObject);

	glm::mat4x4 viewMatrix = glm::lookAt(camera.get_eye_f(), camera.get_cen_f(), camera.get_up_f());
	//modelMatrix is connected with current object. move square to scene's center
  	glm::mat4x4 modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));
	//modelViewMatrix consists of viewMatrix and modelMatrix
	modelViewMatrix = viewMatrix * modelMatrix;
	//calculate normal matrix 
	normalMatrix = glm::inverseTranspose(modelViewMatrix);
	//finally calculate modelViewProjectionMatrix
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	int locMV = glGetUniformLocation(shader_program.programObject, "modelViewMatrix");
	int locN = glGetUniformLocation(shader_program.programObject, "normalMatrix");
	int locP = glGetUniformLocation(shader_program.programObject, "modelViewProjectionMatrix");
	int texLoc = glGetUniformLocation(shader_program.programObject, "texture");
	int locSclDst = glGetUniformLocation(shader_program.programObject, "scale_dist");
	int locSclSz = glGetUniformLocation(shader_program.programObject, "scale_size");
	int locCenx = glGetUniformLocation(shader_program.programObject, "cen_x");
	int locCeny = glGetUniformLocation(shader_program.programObject, "cen_y");
	int locCenz = glGetUniformLocation(shader_program.programObject, "cen_z");
	int locRad = glGetUniformLocation(shader_program.programObject, "rad");
	int locPlPos = glGetUniformLocation(shader_program.programObject, "pl_position");
	int locPlAmb = glGetUniformLocation(shader_program.programObject, "pl_ambient");
	int locPlDif = glGetUniformLocation(shader_program.programObject, "pl_diffuse");
	int locPlSpc = glGetUniformLocation(shader_program.programObject, "pl_specular");
	int locPlAtt = glGetUniformLocation(shader_program.programObject, "pl_attenuation");
	int locMtAmb = glGetUniformLocation(shader_program.programObject, "mt_ambient");
	int locMtDif = glGetUniformLocation(shader_program.programObject, "mt_diffuse");
	int locMtSpc = glGetUniformLocation(shader_program.programObject, "mt_specular");
	int locMtEmi = glGetUniformLocation(shader_program.programObject, "mt_emission");
	int locMtShn = glGetUniformLocation(shader_program.programObject, "mt_shininess");
	int locViewPst = glGetUniformLocation(shader_program.programObject, "view_position");
	int locNeedL = glGetUniformLocation(shader_program.programObject, "need_light");
	int locOCenx = glGetUniformLocation(shader_program.programObject, "o_cen_x");
	int locOCeny = glGetUniformLocation(shader_program.programObject, "o_cen_y");
	int locOCenz = glGetUniformLocation(shader_program.programObject, "o_cen_z");
	int locOCenScl = glGetUniformLocation(shader_program.programObject, "o_cen_scl");

	glUniformMatrix4fv(locMV, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniformMatrix4fv(locN, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(locP, 1, 0, glm::value_ptr(modelViewProjectionMatrix));
	glUniform1ui(texLoc, 0);
	glUniform1f(locSclDst, DIST_SCALE_FACTOR);
	glUniform1f(locSclSz, SIZE_SCALE_FACTOR);

	glUniform4fv(locPlPos, 1, glm::value_ptr(pl_position));
	glUniform4fv(locPlAmb, 1, glm::value_ptr(pl_ambient));
	glUniform4fv(locPlDif, 1, glm::value_ptr(pl_diffuse));
	glUniform4fv(locPlSpc, 1, glm::value_ptr(pl_specular));
	glUniform3fv(locPlAtt, 1, glm::value_ptr(pl_attenuation));

	glm::vec3 eyef = glm::vec3(float(camera.eye[0]), float(camera.eye[1]), float(camera.eye[2]));
	glUniform3fv(locViewPst, 1, glm::value_ptr(eyef));

	//draw object
	if (ENABLE_ORBITS) {
		glUniform1i(locNeedL, 0);
		glUniform1f(locSclSz, DIST_SCALE_FACTOR);
		for (unsigned i = 1; i < orbit_traces.size(); i++) {
			glm::dvec3 _cen = orbit_traces[i]->get_cen();
			glm::vec3 cen = glm::vec3(float(_cen[0]), float(_cen[1]), float(_cen[2]));
			glUniform1f(locCenx, cen[0]);
			glUniform1f(locCeny, cen[1]);
			glUniform1f(locCenz, cen[2]);
			glUniform1f(locRad, orbit_traces[i]->get_radius(ENABLE_LOG));

			glm::dvec3 orb_cen = orbits[i].O;
			glUniform1f(locOCenx, float(orb_cen[0]));
			glUniform1f(locOCeny, float(orb_cen[1]));
			glUniform1f(locOCenz, float(orb_cen[2]));
			glUniform1f(locOCenScl, orbit_traces[i]->get_radius(ENABLE_LOG));

			orbit_traces[i]->draw();
		}
	}

	glUniform1i(locNeedL, 1);
	for (unsigned i = 0; i < planets.size(); i++) {
		if (i == 13)
			glEnable(GL_BLEND);
		glUniform4fv(locMtAmb, 1, glm::value_ptr(mats[i].ambient));
		glUniform4fv(locMtDif, 1, glm::value_ptr(mats[i].diffuse));
		glUniform4fv(locMtSpc, 1, glm::value_ptr(mats[i].specular));
		glUniform4fv(locMtEmi, 1, glm::value_ptr(mats[i].emission));
		glUniform1f(locMtShn, mats[i].shininess);

		glm::dvec3 cen = planets[i]->get_cen();
		glUniform1f(locCenx, float(cen[0]));
		glUniform1f(locCeny, float(cen[1]));
		glUniform1f(locCenz, float(cen[2]));
		glUniform1f(locRad, planets[i]->get_radius(ENABLE_LOG));

		glm::dvec3 orb_cen = orbits[i].O;
		glUniform1f(locOCenx, float(orb_cen[0]));
		glUniform1f(locOCeny, float(orb_cen[1]));
		glUniform1f(locOCenz, float(orb_cen[2]));
		glUniform1f(locOCenScl, orbit_traces[i]->get_radius(ENABLE_LOG));

		planets[i]->draw();
	}
	glDisable(GL_BLEND);
	glutSwapBuffers();
}

void reshape(int width, int height) {
	window_width = width;
	window_height = height;
	//set viewport to match window size
	glViewport(0, 0, width, height);
  
	float fieldOfView = config.lookup("application.view_angle");
	float aspectRatio = float(width) / float(height);
	float zNear = config.lookup("application.znear");
	float zFar = config.lookup("application.zfar");
	//set projection matrix
	projectionMatrix = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

void update() {
	glutPostRedisplay();
}

void update_position(int val) {
	if (ENABLE_ANIMATION) {
		for (unsigned i = 1; i < planets.size(); i++) {
			glm::dvec3 last = planets[i]->get_cen();
			planets[i]->set_center(get_next_position_by_Kepler(orbits[i], planets[i]->get_cen(), DT));
			for (unsigned j = 1; j < planets.size(); j++)
				if (orbits[j].orb_cen == planets[i]->get_name())
					planets[j]->set_center(planets[j]->get_cen() + planets[i]->get_cen() - last);
		}
		for (unsigned i = 0; i < planets.size(); i++)
			planets[i]->self_rotate(DT);
		initAllGLBuffers();
	}
	glutTimerFunc(TIMESTAMP, update_position, 0);
}

void empty_display() {
}

void special_keys(int key, int x, int y) {
	if (key == GLUT_KEY_HOME)
		camera = Observer(
			glm::dvec3(
				config.lookup("application.camera.eye.x"),
				config.lookup("application.camera.eye.y"),
				config.lookup("application.camera.eye.z")),
			glm::dvec3(
				config.lookup("application.camera.cen.x"),
				config.lookup("application.camera.cen.y"),
				config.lookup("application.camera.cen.z")),
			glm::dvec3(
				config.lookup("application.camera.up.x"),
				config.lookup("application.camera.up.y"),
				config.lookup("application.camera.up.z")));	
	if (key == GLUT_KEY_UP)
		camera.rotate_vert(ROTATE_ALPHA);
	if (key == GLUT_KEY_DOWN)
		camera.rotate_vert(-ROTATE_ALPHA);
	if (key == GLUT_KEY_LEFT)
		camera.rotate_horz(-ROTATE_ALPHA);
	if (key == GLUT_KEY_RIGHT)
		camera.rotate_horz(ROTATE_ALPHA);
}

double get_min_distance_to_object() {
	double res = 1e7;
	for (unsigned i = 0; i < planets.size(); i++)
		res = std::min(res, glm::length(camera.eye - planets[i]->get_cen()) - planets[i]->get_radius(true));
	return std::max(res, 0.0);
}

void keyboard(unsigned char key, int mx, int my) {
	if (key == '+')
		DT *= 2.0;
	if (key == '-')
		DT /= 2.0;
	if (key == ' ')
		ENABLE_ANIMATION = !ENABLE_ANIMATION;

	if (key == 13) // Enter key
		ENABLE_LOG = !ENABLE_LOG;

	Observer last = camera;

	if (key == 'i' || key == 'I')
		camera.zoom(ZOOM_SCALE);
	if (key == 'o' || key == 'O')
		camera.zoom(1.0 / ZOOM_SCALE);
	if (key == 't' || key == 'T')
		ENABLE_ORBITS = !ENABLE_ORBITS;

	if (key == 'w' || key == 'W') {
		double mindist = get_min_distance_to_object();
		camera.forward(std::pow(mindist, 1.0 / 2.0) * 1e3);
	}
	if (key == 's' || key == 'S') {
		double mindist = get_min_distance_to_object();
		camera.backward(std::pow(mindist, 1.0 / 2.0) * 1e3);
	}
	if (key == 'a' || key == 'A')
		camera.rotate_up(ROTATE_ALPHA);
	if (key == 'd' || key == 'D')
		camera.rotate_up(-ROTATE_ALPHA);

	if (get_min_distance_to_object() < EPS)
		camera = last;
}

int mouseX = -1, mouseY = -1;

void motion_mouse(int posx, int posy) {
	if (mouseX == -1 && mouseY == -1) {
		mouseX = posx;
		mouseY = posy;
	} else {
		camera.follow_by_mouse(mouseX, mouseY, posx, posy);
		mouseX = posx;
		mouseY = posy;
	}
}

void mouse(int button, int mode, int posx, int posy) {
	#ifndef GLUT_WHEEL_UP
		#define GLUT_WHEEL_UP 3
		#define GLUT_WHEEL_DOWN 4
	#endif
	if (mode == GLUT_UP)
		mouseX = mouseY = -1;
	if (button == GLUT_WHEEL_UP)
		camera.zoom(ZOOM_SCALE);
	if (button == GLUT_WHEEL_DOWN)
		camera.zoom(1.0 / ZOOM_SCALE);
}

void release_memory() {
	for (unsigned i = 0; i < planets.size(); i++)
		delete planets[i];
	for (unsigned i = 0; i < orbit_traces.size(); i++)
		delete orbit_traces[i];
	delete[] roots;
}

int main (int argc, char* argv[]) {
	glutInit(&argc, argv);

	init_config("solar.cfg");

	#ifdef __APPLE__
		glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE);
	#else
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
		glutInitContextVersion(3, 1);  
		glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
		glewExperimental = GL_TRUE;
	#endif
	glutCreateWindow(config.lookup("application.window.caption"));
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(window_width, window_height);
	glutIdleFunc(update);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion_mouse);
	glutSpecialFunc(special_keys);
	glutTimerFunc(TIMESTAMP, update_position, 0);

	glewInit(); 
	const char *slVer = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::cout << "GLSL Version: " << slVer << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,  GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	try {
		init();
	} catch (const char *str) {
		std::cout << "Error During Initialiation: " << str << std::endl;
		release_memory();
		//start main loop with empty screen
		glutDisplayFunc(empty_display);
		glutMainLoop();
		return -1;
	}

	try {
		glutMainLoop();
	} catch (const char *str) {
		std::cout << "Error During Main Loop: " << str << std::endl;
	}
	//release memory
	release_memory();
	return 0;
}
