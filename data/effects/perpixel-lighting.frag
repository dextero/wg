// Swiatlo perpixel - program fragmentow

uniform sampler2D tex;
uniform vec4 ambient;

uniform float lradius1;
uniform float lradius2;
uniform float lradius3;

uniform vec4 lcolor1;
uniform vec4 lcolor2;
uniform vec4 lcolor3;

varying float ldist1;
varying float ldist2;
varying float ldist3;

void main()
{	
	vec4 texColor = texture2D(tex, gl_TexCoord[0].st);
	vec4 diffuse = mix(lcolor1, 0.0, min(ldist1/lradius1, 1.0));
	diffuse = diffuse + mix(lcolor2, 0.0, min(ldist2/lradius2, 1.0));
	diffuse = diffuse + mix(lcolor3, 0.0, min(ldist3/lradius3, 1.0));
	gl_FragColor = (diffuse + ambient) * texColor; 
}