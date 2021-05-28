#version 330

in vec3 a_color;
in vec3 a_vertex;
in vec2 a_uv;
in vec3 a_normal;

out vec3 v_color;
out vec2 v_uv;
out vec3 v_un;
out vec3 v_world_vertex_pos;

uniform mat4 u_model;
uniform mat3 u_normal;

out vec3 v_normal;


uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_color;

void main(){

	v_uv = a_uv;
	v_world_vertex_pos = (u_model * vec4(a_vertex,1.0)).xyz;

	vec3 position = (u_model * vec4(a_vertex,1.0)).xyz; 

	v_un = u_normal * a_normal;

	gl_Position = u_projection * u_view * u_model * vec4( a_vertex , 1.0 );

}
