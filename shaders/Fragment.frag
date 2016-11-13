//shader version
#version 120

//texture object
uniform sampler2D texture;

uniform bool need_light;

uniform vec4 pl_position;
uniform vec4 pl_ambient;
uniform vec4 pl_diffuse;
uniform vec4 pl_specular;
uniform vec3 pl_attenuation;

uniform vec4 mt_ambient;
uniform vec4 mt_diffuse;
uniform vec4 mt_specular;
uniform vec4 mt_emission;
uniform float mt_shininess;

//retrieve this data form vertex shader
varying vec3 position;
varying vec3 normal;
varying vec2 texcoord;
varying vec3 lightdir;
varying vec3 viewdir;
varying float distance;

//varying vec4 gl_FragColor;

//main function
void main(void)
{
	vec3 _normal = normalize(normal);
	vec3 _lightdir = normalize(lightdir);
	vec3 _viewdir = normalize(viewdir);

	float attenuation = 1.0 / (pl_attenuation[0] + pl_attenuation[1] * distance + pl_attenuation[2] * distance * distance);

	gl_FragColor = mt_emission;
	gl_FragColor += mt_ambient * pl_ambient * attenuation;

	float nor_dot_ldir = max(dot(_normal, _lightdir), 0.0);
	gl_FragColor += mt_diffuse * pl_diffuse * nor_dot_ldir * attenuation;

	if (dot(_normal, _lightdir) > 0.0) {
		float ref_dot_view = pow(max(dot(reflect(-_lightdir, _normal), _viewdir), 0.0), mt_shininess);
		gl_FragColor += mt_specular * pl_specular * ref_dot_view * attenuation;
	}

//	gl_FragColor *= vec4(texture2D(texture, texcoord).rgb, 1);
	gl_FragColor *= vec4(texture2D(texture, texcoord).rgb, 0);
	//take color from texture using texture2D

	if (!need_light)
		gl_FragColor = vec4(texture2D(texture,texcoord.xy).rgb,1);
}
