#version 330

in vec3 v_color;
in vec2 v_uv;
in vec3 v_un;
in vec3 v_world_vertex_pos;
out vec4 fragColor;

uniform vec3 u_color;
uniform float u_ambient;
uniform vec3 u_light;
uniform vec3 u_light_color;
uniform vec3 u_cam_pos;
uniform float u_shiniess;
uniform sampler2D u_texture;

void main(void){

		
	vec3 texture_color = texture(u_texture, v_uv).xyz;
	fragColor = vec4(texture_color,1.0);
	


}