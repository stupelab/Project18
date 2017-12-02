#version 120
#extension GL_ARB_texture_rectangle : enable
//# extension GL_Ext_gpu_shader4: enable

uniform sampler2DRect mask;
uniform sampler2DRect texture1;
uniform sampler2DRect lines;
uniform sampler2DRect grayBg;

uniform float time;
uniform int counter;
uniform int num_lines;

float rand(vec2 co)
{
     float a = 12.9898;
     float b = 78.233;
     float c = 43758.5453;
     float dt= dot(co.xy ,vec2(a,b));
     float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float hash(float n){
    return fract(sin(n)*758.5453);
} 

float noise1d(float x){
    float n = floor(x); 
    return mix(hash(n+0.0),hash(n+1.0),smoothstep(0.0, 1.0, fract(x)));
}



void main(){
	
	vec2 pos = gl_TexCoord[0].xy;
	vec4 color0 = texture2DRect( mask , pos );
	vec4 color1 = texture2DRect( texture1 , pos );
	vec4 color2 = texture2DRect( lines , pos );
	vec4 color3 = texture2DRect( grayBg , pos );
	

  //random color
  float colorNoised =noise1d(pos.x * 101.0);

	//Compute resulted color

	vec4 color;
	vec4 colorGreen = vec4(0 , 1.0 , 0 , 0);

if(   color0.g < 0.5 ){

     float random= rand(pos);


     //if( random >0.15 ){

        if( color2.g >0.5 ) 
        pos.x = pos.x *noise1d(pos.x) +  30.0 * (sin(time*0.3) );
        else
        pos.x = pos.x  -  30.0 * (sin(time*0.3) );

       color = texture2DRect(  texture1 , pos );  //mode lines

    
}else{

  float random = rand(pos);
	float amp = sin(pos.y*0.6 );
	
//	pos.x = pos.x  - 30 * sin(time*0.02)*amp* *50.0;
  color = texture2DRect(  texture1 , pos );

}

//	Output of the shader
	gl_FragColor = color;
}



