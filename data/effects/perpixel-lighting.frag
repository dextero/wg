// Swiatlo perpixel - program fragmentow

#define LIGHTS_CNT 5

uniform sampler2D tex;
uniform vec4 ambient;

uniform float lradius[LIGHTS_CNT];
uniform vec4 lcolor[LIGHTS_CNT];
varying float ldist[LIGHTS_CNT];

void main()
{	
	vec4 texColor = texture2D(tex, gl_TexCoord[0].st);
	
	// Oblicz kolor swiatla diffuse (to 1 - min.... to zanik swiatla - interpolacja liniowa)
	vec4 diffuse = lcolor[0] * (1.0 - min(ldist[0]/lradius[0], 1.0));
	for (int i = 1; i < LIGHTS_CNT; i++)
		diffuse = diffuse + lcolor[i] * (1.0 - min(ldist[i]/lradius[i], 1.0));
		
	gl_FragColor = (diffuse + ambient) * texColor;
	gl_FragColor.a = texColor.a;
}