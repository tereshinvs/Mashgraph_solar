//shader version
#version 120

uniform float scale_size;
uniform float scale_dist;
uniform float cen_x;
uniform float cen_y;
uniform float cen_z;
uniform float rad;

uniform float o_cen_x;
uniform float o_cen_y;
uniform float o_cen_z;
uniform float o_cen_scl;

uniform vec4 pl_position;
uniform vec4 pl_ambient;
uniform vec4 pl_diffuse;
uniform vec4 pl_specular;
uniform vec3 pl_attenuation;

uniform vec3 view_position;

uniform mat4 modelViewMatrix;

//inverse and transpose matrix for normals
uniform mat4 normalMatrix;

//projectionMatrix*modelViewMatrix
uniform mat4 modelViewProjectionMatrix;

//input vertex: position, normal, texture coordinates
attribute vec3 pos;
attribute vec3 nor;
attribute vec2 tex;

//output vertex to fragment shader
varying vec3 position;
varying vec3 normal;
varying vec2 texcoord;
varying vec3 lightdir;
varying vec3 viewdir;
varying float distance;

void main()
{
	vec3 orb_cen = vec3(o_cen_x, o_cen_y, o_cen_z);
	vec3 draw_cen = (orb_cen + (vec3(cen_x, cen_y, cen_z) - orb_cen) * o_cen_scl) * scale_dist;
	vec3 real_point = pos * rad * scale_size;
	vec3 tmp = real_point + draw_cen;

	//vec4 vertex = modelViewMatrix * vec4(tmp, 1.0);
	vec4 vertex = vec4(tmp, 1.0);
	vec4 _lightdir = pl_position - vertex;

	position = vec3(vertex);
	normal = nor.xyz;
	texcoord = vec2(tex.xy);
	lightdir = vec3(_lightdir);
	viewdir = view_position - vec3(vertex);
	distance = length(lightdir);
	
	gl_Position = modelViewProjectionMatrix * vertex;
}
