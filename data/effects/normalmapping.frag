// Normalmapping - program fragmentow

#define LIGHTS_CNT 3

uniform sampler2D tex;
uniform sampler2D normalmap;
uniform vec4 ambient;
uniform float nmcontrast;

uniform vec3 lpos[LIGHTS_CNT];
uniform float lradius[LIGHTS_CNT];
uniform vec4 lcolor[LIGHTS_CNT];

varying vec3 vertexPos;

void main()
{
	vec4 texColor = texture2D(tex, gl_TexCoord[0].st);
	vec3 normal = texture2D(normalmap, gl_TexCoord[0].st).xyz;
	normal = gl_NormalMatrix * normalize(normal - 0.5);
	
	// Oblicz kolor swiatla diffuse (to 1 - min.... to zanik swiatla - interpolacja liniowa)
	vec3	lightDir = lpos[0] - vertexPos;
	float 	diffuseStrength = max( dot(normal, normalize(lightDir)), 0.0 );
	vec4 	diffuse = diffuseStrength * nmcontrast * lcolor[0] * (1.0 - min(length(lightDir)/lradius[0], 1.0));
	
	for (int i = 1; i < LIGHTS_CNT; i++)
	{
		lightDir = lpos[i] - vertexPos;
		diffuseStrength = max( dot(normal, normalize(lightDir)), 0.0 );
		diffuse += diffuseStrength * nmcontrast * lcolor[i] * (1.0 - min(length(lightDir)/lradius[i], 1.0));
	}
	
	gl_FragColor = (diffuse + ambient) * texColor;
	gl_FragColor.a = texColor.a;
}