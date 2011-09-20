// Normalmapping - program fragmentow

#define LIGHTS_CNT 3

uniform sampler2D tex;
uniform sampler2D normalmap;
uniform vec4 ambient;
uniform float nmcontrast;

uniform float lradius[LIGHTS_CNT];
uniform vec4 lcolor[LIGHTS_CNT];
varying vec3 ldir[LIGHTS_CNT];
varying float ldist[LIGHTS_CNT];

void main()
{
	vec4 texColor = texture2D(tex, gl_TexCoord[0].st);
	vec3 normal = texture2D(normalmap, gl_TexCoord[0].st).xyz;
	normal = normalize(normal - 0.5);
	
	// Oblicz kolor swiatla diffuse (to 1 - min.... to zanik swiatla - interpolacja liniowa)
	float diffuseStrength = max( dot(normal, normalize(ldir[0])), 0.0 );
	vec4 diffuse = lcolor[0] * diffuseStrength * nmcontrast * (1.0 - min(ldist[0]/lradius[0], 1.0));
	
	for (int i = 1; i < LIGHTS_CNT; i++)
	{
		diffuseStrength = max( dot(normal, normalize(ldir[i])), 0.0 ); 
		diffuse += lcolor[i] * diffuseStrength * nmcontrast * (1.0 - min(ldist[i]/lradius[i], 1.0));
	}
	
	gl_FragColor = (diffuse + ambient) * texColor;
	gl_FragColor.a = texColor.a;
}