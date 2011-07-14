uniform sampler2D tex;
uniform sampler2D normalmap;
uniform vec3 lpos;
uniform vec4 lcolor;

void main()
{
	vec4 pixel = texture2D(tex, gl_TexCoord[0].st);
	vec3 normal = texture2D(normalmap, gl_TexCoord[0].st).xyz;
	normal = normalize(normal - 0.5);
	lpos = normalize(lpos);
	
	float diffuseStrength = max( dot(normal, lpos), 0.0 ) * 1.5;
	vec4 diffuse = (diffuseStrength * lcolor + 0.2) * pixel;
	gl_FragColor = vec4( diffuse.rgb, pixel.a );
}