uniform sampler2D tex;

uniform vec2 uTexSize;
uniform float uImageSize;

void main()
{
	vec4 color = texture2D(tex, gl_TexCoord[0].st);
	
	if (color.a == 0.0)
		color.rgb = vec3(0.0, 0.0, 0.0);	// nie wiedziec czemu przy alfie 100% reszta skladnikow ma 1.0
	
	gl_FragColor = color;
	
	// lod
	vec4 texCoord = gl_TexCoord[0];
	vec2 imageSize = vec2(uImageSize / uTexSize.x, uImageSize / uTexSize.y);
	vec2 delta = texCoord.xy;
	float iceSize = 0.1;
	
	// modf dla ubogich
	delta.x = mod(delta.x, imageSize.x);
	delta.y = mod(delta.y, imageSize.y);
	
	// lewy gorny rog klatki
	texCoord.x -= delta.x;
	texCoord.y -= delta.y;
	
	// lerp miedzy lewym gornym a prawym dolnym rogiem klatki, z powiekszeniem
	texCoord.x = mix(texCoord.x + imageSize.x * iceSize, texCoord.x + imageSize.x * (1.0 - iceSize), delta.x / imageSize.x);
	texCoord.y = mix(texCoord.y + imageSize.y * iceSize, texCoord.y + imageSize.y * (1.0 - iceSize), delta.y / imageSize.y);
	
	vec4 colorOutside = texture2D(tex, texCoord.xy);
	
	if (colorOutside.a > 0.0) // nie ma alphy - jestesmy w powiekszeniu
		gl_FragColor += vec4(0.6, 0.7, 1.0, 0.5);
}
	