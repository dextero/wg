// Swiatlo perpixel - program fragmentow

#define LIGHTS_CNT 5

uniform sampler2D tex;
uniform vec4 ambient;

uniform vec3 lpos[LIGHTS_CNT];
uniform float lradius[LIGHTS_CNT];
uniform vec4 lcolor[LIGHTS_CNT];

varying vec3 vertexPos;

void main()
{	
	vec4 texColor = texture2D(tex, gl_TexCoord[0].st) * gl_Color;
	
	// Oblicz kolor swiatla diffuse (to 1 - min.... to zanik swiatla - interpolacja liniowa)
	vec4 diffuse = lcolor[0] * (1.0 - min(length(vertexPos-lpos[0])/lradius[0], 1.0));
	for (int i = 1; i < LIGHTS_CNT; i++)
		diffuse = diffuse + lcolor[i] * (1.0 - min(length(vertexPos-lpos[i])/lradius[i], 1.0));
		
	gl_FragColor = (diffuse + ambient) * texColor;
	gl_FragColor.a = texColor.a;
}