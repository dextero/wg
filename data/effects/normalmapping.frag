// Normalmapping - program fragmentow

uniform sampler2D tex;
uniform sampler2D normalmap;
uniform vec4 ambient;
uniform float nmcontrast;

uniform float lradius1;
uniform float lradius2;
uniform float lradius3;

uniform vec4 lcolor1;
uniform vec4 lcolor2;
uniform vec4 lcolor3;

varying vec3 ldir1;
varying vec3 ldir2;
varying vec3 ldir3;

varying float ldist1;
varying float ldist2;
varying float ldist3;

void main()
{
	vec4 texColor = texture2D(tex, gl_TexCoord[0].st);
	vec3 normal = texture2D(normalmap, gl_TexCoord[0].st).xyz;
	normal = normalize(normal - 0.5);
	
	// Oblicz kolor swiatla diffuse (to 1 - min.... to zanik swiatla - interpolacja liniowa)
	float diffuseStrength = max( dot(normal, normalize(ldir1)), 0.0 );
	vec4 diffuse = lcolor1 * diffuseStrength * nmcontrast * (1.0 - min(ldist1/lradius1, 1.0));
	
	diffuseStrength = max( dot(normal, normalize(ldir2)), 0.0 ); 
	diffuse += lcolor2 * diffuseStrength * nmcontrast * (1.0 - min(ldist2/lradius2, 1.0));
	
	diffuseStrength = max( dot(normal, normalize(ldir3)), 0.0 ); 
	diffuse += lcolor3 * diffuseStrength * nmcontrast * (1.0 - min(ldist3/lradius3, 1.0));
	
	gl_FragColor = (diffuse + ambient) * texColor;
	gl_FragColor.a = texColor.a;
}