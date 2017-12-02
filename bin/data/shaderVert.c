#version 120
#extension GL_ARB_texture_rectangle : enable
//# extension GL_Ext_gpu_shader4: enable

uniform sampler2DRect mask;
uniform float distortAmount = 0.25;
uniform float phase = 0.0;

void main(){
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
  
	/*vec3 v = gl_TexCoord[0].xyz;
	//vec4 color1 = glColorPointer;

	float distort = distortAmount * sin( phase + 0.015 * v.y );

	  v.x /= 1.0 + distort;
	  v.y /= 1.0 + distort;
	  v.z /= 1.0 + distort;

  	vec4 posHomog = vec4(v, 1.0);

  	gl_Position = gl_ModelViewProjectionMatrix * posHomog;
    gl_TexCoord[0] = gl_MultiTexCoord0;*/
	gl_FrontColor = gl_Color;

	 

}